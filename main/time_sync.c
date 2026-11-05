#include "time_sync.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <sys/time.h>
#include <time.h>
#include <string.h>

static const char *TAG = "time_sync";
static SemaphoreHandle_t s_sync_mutex = NULL;
static gnss_status_t s_gnss_status = {0};

esp_err_t time_sync_init(void) {
    s_sync_mutex = xSemaphoreCreateMutex();
    if (s_sync_mutex != NULL) {
        ESP_LOGI(TAG, "Time sync module initialized");
        return ESP_OK;
    }
    return ESP_ERR_NO_MEM;
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
    int64_t elapsed_us = now_us - anchor_esp_time_us;
    if (elapsed_us < 0) elapsed_us = 0;

    struct timeval tv;
    tv.tv_sec = epoch_sec + (time_t)(elapsed_us / 1000000LL);
    tv.tv_usec = (suseconds_t)((time_data->microseconds + (elapsed_us % 1000000LL)) % 1000000LL);
    settimeofday(&tv, NULL);

    xSemaphoreTake(s_sync_mutex, portMAX_DELAY);
    s_gnss_status.fix_valid = true;
    s_gnss_status.last_fix_time = *time_data;
    s_gnss_status.last_fix_esp_time_us = now_us;
    s_gnss_status.sentences_parsed++;
    xSemaphoreGive(s_sync_mutex);
}

void time_sync_record_error(void) {
    xSemaphoreTake(s_sync_mutex, portMAX_DELAY);
    s_gnss_status.checksum_errors++;
    xSemaphoreGive(s_sync_mutex);
}

bool time_sync_get_status(gnss_status_t *out_status) {
    if (out_status == NULL) {
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
    gnss_status_t dummy;
    return time_sync_get_status(&dummy);
}
