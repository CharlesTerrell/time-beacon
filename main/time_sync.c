#include "time_sync.h"
#include "board_pins.h"
#include "config_mgr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <sys/time.h>
#include <time.h>
#include <string.h>

static const char *TAG = "time_sync";
static SemaphoreHandle_t s_sync_mutex = NULL;
static gnss_status_t s_gnss_status = {0};

static volatile int64_t s_last_pps_esp_time_us = 0;
static volatile uint32_t s_pps_pulse_count = 0;

static void IRAM_ATTR pps_gpio_isr(void *arg) {
    (void)arg;
    s_last_pps_esp_time_us = esp_timer_get_time();
    s_pps_pulse_count++;
}

esp_err_t time_sync_init(void) {
    s_sync_mutex = xSemaphoreCreateMutex();
    if (s_sync_mutex == NULL) {
        return ESP_ERR_NO_MEM;
    }

    const app_config_t *cfg = config_mgr_get();
    if (cfg && !cfg->enable_pps) {
        ESP_LOGI(TAG, "Hardware 1PPS disabled by configuration; using software NMEA discipline");
        ESP_LOGI(TAG, "Time sync module initialized");
        return ESP_OK;
    }

    // Configure 1PPS GPIO interrupt
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_GNSS_PPS),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    esp_err_t err = gpio_config(&io_conf);
    if (err == ESP_OK) {
        err = gpio_install_isr_service(0);
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "gpio_install_isr_service note: %s", esp_err_to_name(err));
        }
        err = gpio_isr_handler_add(PIN_GNSS_PPS, pps_gpio_isr, NULL);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Hardware 1PPS interrupt initialized on GPIO %d", PIN_GNSS_PPS);
        } else {
            ESP_LOGW(TAG, "Failed to attach 1PPS ISR on GPIO %d: %s", PIN_GNSS_PPS, esp_err_to_name(err));
        }
    } else {
        ESP_LOGW(TAG, "Failed to configure 1PPS GPIO %d: %s", PIN_GNSS_PPS, esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "Time sync module initialized");
    return ESP_OK;
}

void time_sync_update_gnss(const nmea_time_t *time_data, int64_t anchor_esp_time_us) {
    if (time_data == NULL || !time_data->fix_valid) {
        return;
    }

    struct tm tm;
    tm.tm_year = time_data->year - 1900;
    tm.tm_mon = time_data->month - 1;
    tm.tm_mday = time_data->day;
    tm.tm_hour = time_data->hour;
    tm.tm_min = time_data->minute;
    tm.tm_sec = time_data->second;
    tm.tm_isdst = 0;

    time_t epoch_sec = timegm(&tm);
    int64_t now_us = esp_timer_get_time();
    int64_t last_pps_us = s_last_pps_esp_time_us;
    int64_t pps_age_us = now_us - last_pps_us;

    struct timeval tv;
    bool using_pps = false;
    const app_config_t *cfg = config_mgr_get();
    bool pps_enabled = (!cfg || cfg->enable_pps);

    // Check if 1PPS pulse is actively arriving (< 1.5s old and within reasonable phase)
    if (pps_enabled && last_pps_us > 0 && pps_age_us >= 0 && pps_age_us < 1500000LL) {
        // Precise discipline to hardware 1PPS rising edge
        tv.tv_sec = epoch_sec + (time_t)(pps_age_us / 1000000LL);
        tv.tv_usec = (suseconds_t)(pps_age_us % 1000000LL);
        using_pps = true;
    } else {
        // Fallback: Software NMEA delay compensation
        int64_t offset_us = cfg ? ((int64_t)cfg->nmea_offset_ms * 1000LL) : 60000LL;
        int64_t elapsed_us = (now_us - anchor_esp_time_us) + offset_us;
        if (elapsed_us < 0) elapsed_us = 0;

        tv.tv_sec = epoch_sec + (time_t)(elapsed_us / 1000000LL);
        tv.tv_usec = (suseconds_t)((time_data->microseconds + (elapsed_us % 1000000LL)) % 1000000LL);
        using_pps = false;
    }

    settimeofday(&tv, NULL);

    xSemaphoreTake(s_sync_mutex, portMAX_DELAY);
    s_gnss_status.fix_valid = true;
    s_gnss_status.pps_synced = using_pps;
    s_gnss_status.pps_pulse_count = s_pps_pulse_count;
    s_gnss_status.last_pps_esp_time_us = last_pps_us;
    s_gnss_status.last_fix_time = *time_data;
    if (time_data->has_pos) {
        s_gnss_status.has_pos = true;
        s_gnss_status.latitude_i = time_data->latitude_i;
        s_gnss_status.longitude_i = time_data->longitude_i;
        s_gnss_status.altitude_m = time_data->altitude_m;
    }
    s_gnss_status.last_fix_esp_time_us = now_us;
    s_gnss_status.sentences_parsed++;
    xSemaphoreGive(s_sync_mutex);
}

void time_sync_record_error(void) {
    if (s_sync_mutex == NULL) return;
    xSemaphoreTake(s_sync_mutex, portMAX_DELAY);
    s_gnss_status.checksum_errors++;
    xSemaphoreGive(s_sync_mutex);
}

void time_sync_update_sats(uint8_t satellites) {
    if (s_sync_mutex == NULL) return;
    xSemaphoreTake(s_sync_mutex, portMAX_DELAY);
    s_gnss_status.satellites = satellites;
    xSemaphoreGive(s_sync_mutex);
}

bool time_sync_get_status(gnss_status_t *out_status) {
    if (out_status == NULL || s_sync_mutex == NULL) {
        return false;
    }

    xSemaphoreTake(s_sync_mutex, portMAX_DELAY);
    *out_status = s_gnss_status;
    xSemaphoreGive(s_sync_mutex);

    if (s_gnss_status.fix_valid && s_gnss_status.last_fix_esp_time_us > 0 &&
        (esp_timer_get_time() - s_gnss_status.last_fix_esp_time_us) < 5000000LL) {
        return true;
    }

    return false;
}

bool time_sync_is_synchronized(void) {
    gnss_status_t status;
    return time_sync_get_status(&status);
}

bool time_sync_is_pps_locked(void) {
    gnss_status_t status;
    if (time_sync_get_status(&status)) {
        return status.pps_synced;
    }
    return false;
}
