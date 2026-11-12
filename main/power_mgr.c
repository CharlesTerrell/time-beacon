#include "power_mgr.h"
#include "board_pins.h"
#include "display.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/ledc.h"
#include "driver/usb_serial_jtag.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "power_mgr";

#define LEDC_PWM_TIMER          LEDC_TIMER_0
#define LEDC_PWM_CHANNEL        LEDC_CHANNEL_0
#define LEDC_PWM_FREQ           1000
#define LEDC_PWM_RES            LEDC_TIMER_10_BIT
#define LEDC_MAX_DUTY           1023

#define VBAT_DIVIDER_RATIO      4.9f
#define VBAT_MAX_V              4.10f
#define VBAT_LOW_V              3.45f
#define VBAT_CRITICAL_V         3.35f

/* RGB565 Colors */
#define COLOR_WHITE             0xFFFF
#define COLOR_BLACK             0x0000
#define COLOR_RED               0xF800
#define COLOR_CYAN_BLUE         0x07FF  /* Electric Cyan / Sky Blue: R=0, G=63, B=31 */
#define COLOR_YELLOW            0xFFE0  /* Bright Yellow: R=31, G=63, B=0 */

static adc_oneshot_unit_handle_t s_adc_handle = NULL;
static adc_cali_handle_t s_cali_handle = NULL;
static power_status_t s_status = {
    .mode = POWER_MODE_USB,
    .voltage = 4.20f,
    .percentage = 100,
    .battery_present = false,
    .usb_connected = true,
    .critical_countdown_sec = -1
};

static uint32_t s_led_ticks = 0;
static uint8_t s_led_peak_percent = 50;

static uint16_t interpolate_rgb565(uint16_t c1, uint16_t c2, float t) {
    if (t <= 0.0f) return c1;
    if (t >= 1.0f) return c2;
    uint8_t r1 = (c1 >> 11) & 0x1F;
    uint8_t g1 = (c1 >> 5) & 0x3F;
    uint8_t b1 = c1 & 0x1F;
    uint8_t r2 = (c2 >> 11) & 0x1F;
    uint8_t g2 = (c2 >> 5) & 0x3F;
    uint8_t b2 = c2 & 0x1F;
    uint8_t r = (uint8_t)(r1 + t * (r2 - r1));
    uint8_t g = (uint8_t)(g1 + t * (g2 - g1));
    uint8_t b = (uint8_t)(b1 + t * (b2 - b1));
    return (uint16_t)((r << 11) | (g << 5) | b);
}

esp_err_t power_mgr_init(void) {
    // 1. Configure Battery Divider Control Pin (GPIO 2)
    gpio_reset_pin(PIN_VBAT_CTRL);
    gpio_set_direction(PIN_VBAT_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_VBAT_CTRL, 1); // Disabled (Active LOW)

    // 2. Configure ADC1 Oneshot on GPIO 1 (ADC1_CHANNEL_0)
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    esp_err_t err = adc_oneshot_new_unit(&init_config1, &s_adc_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init ADC1 oneshot: %s", esp_err_to_name(err));
        return err;
    }

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    err = adc_oneshot_config_channel(s_adc_handle, ADC_CHANNEL_0, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to config ADC channel: %s", esp_err_to_name(err));
        return err;
    }

    // 3. Calibration scheme (curve fitting for ESP32-S3)
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .chan = ADC_CHANNEL_0,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_config, &s_cali_handle) != ESP_OK) {
        ESP_LOGW(TAG, "ADC calibration not available, using linear approximation");
        s_cali_handle = NULL;
    }

    // 4. Configure LEDC PWM for Status LED (GPIO 18)
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_PWM_TIMER,
        .duty_resolution  = LEDC_PWM_RES,
        .freq_hz          = LEDC_PWM_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_PWM_CHANNEL,
        .timer_sel      = LEDC_PWM_TIMER,
        .gpio_num       = PIN_STATUS_LED,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG, "Power manager initialized");
    power_mgr_update();
    return ESP_OK;
}

void power_mgr_update(void) {
    if (!s_adc_handle) return;

    // Enable voltage divider
    gpio_set_level(PIN_VBAT_CTRL, 0);
    // Allow brief settle time for RC filter
    esp_rom_delay_us(500);

    // Collect 8 samples
    int raw_sum = 0;
    int samples[8];
    for (int i = 0; i < 8; i++) {
        int raw = 0;
        adc_oneshot_read(s_adc_handle, ADC_CHANNEL_0, &raw);
        samples[i] = raw;
        raw_sum += raw;
        esp_rom_delay_us(100);
    }

    // Disable divider to prevent quiescent battery drain
    gpio_set_level(PIN_VBAT_CTRL, 1);

    int raw_avg = raw_sum / 8;
    int voltage_mv = 0;
    if (s_cali_handle) {
        adc_cali_raw_to_voltage(s_cali_handle, raw_avg, &voltage_mv);
    } else {
        voltage_mv = (raw_avg * 3100) / 4095;
    }

    float measured_v = ((float)voltage_mv * VBAT_DIVIDER_RATIO) / 1000.0f;

    // Smooth voltage reading with exponential filter
    s_status.voltage = (0.25f * measured_v) + (0.75f * s_status.voltage);

    // Calculate sample variance to detect open battery port (charger hunting)
    int diff_sum = 0;
    for (int i = 0; i < 8; i++) {
        int d = samples[i] - raw_avg;
        diff_sum += (d * d);
    }
    int variance = diff_sum / 8;

    bool usb_active = usb_serial_jtag_is_connected();
    bool battery_missing = (variance > 200) || (measured_v < 1.0f);
    s_status.battery_present = !battery_missing;

    // Calculate percentage (3.3V = 0%, 4.15V = 100%)
    float pct = (s_status.voltage - 3.30f) / (4.15f - 3.30f) * 100.0f;
    if (pct < 0.0f) pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;
    s_status.percentage = (int)pct;

    // Power mode determination
    if (usb_active || battery_missing || s_status.voltage > 4.18f) {
        s_status.mode = POWER_MODE_USB;
        s_status.usb_connected = true;
        s_status.critical_countdown_sec = -1;
    } else {
        s_status.usb_connected = false;
        if (s_status.voltage <= VBAT_CRITICAL_V) {
            s_status.mode = POWER_MODE_BATTERY_CRITICAL;
            if (s_status.critical_countdown_sec < 0) {
                s_status.critical_countdown_sec = 30; // 30s countdown before sleep
            }
        } else {
            s_status.mode = POWER_MODE_BATTERY;
            s_status.critical_countdown_sec = -1;
        }
    }
}

void power_mgr_get_status(power_status_t *out_status) {
    if (out_status) {
        *out_status = s_status;
    }
}

void power_mgr_set_led_peak_percent(uint8_t percent) {
    if (percent > 100) percent = 100;
    s_led_peak_percent = percent;
    ESP_LOGI(TAG, "Status LED pulse peak set to %u%%", (unsigned int)percent);
}

void power_mgr_tick_led(bool display_backlight_on) {
    s_led_ticks++;

    if (s_status.mode == POWER_MODE_BATTERY_CRITICAL) {
        // Flash status LED for a moment (100ms) every second (20 ticks @ 50ms)
        uint32_t phase = s_led_ticks % 20;
        uint32_t duty = (phase < 2) ? LEDC_MAX_DUTY : 0;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CHANNEL, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CHANNEL);
    } else if (!display_backlight_on) {
        // Slow pulse: 1s breathing pulse (0.5s up, 0.5s down), then 2s off (60 ticks @ 50ms = 3s total)
        uint32_t phase = s_led_ticks % 60;
        uint32_t duty = 0;
        uint32_t peak_duty = ((uint32_t)LEDC_MAX_DUTY * s_led_peak_percent) / 100;
        if (peak_duty > 0) {
            if (phase < 10) {
                uint32_t raw_duty = (phase * peak_duty) / 10;
                duty = (raw_duty * raw_duty) / peak_duty; // Quadratic gamma
            } else if (phase < 20) {
                uint32_t raw_duty = ((20 - phase) * peak_duty) / 10;
                duty = (raw_duty * raw_duty) / peak_duty;
            }
        }
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CHANNEL, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CHANNEL);
    } else {
        // Display is on and power is normal: LED off
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CHANNEL, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_PWM_CHANNEL);
    }
}

uint16_t power_mgr_get_bg_color(void) {
    return COLOR_BLACK;
}

uint16_t power_mgr_get_fg_color(void) {
    if (s_status.mode == POWER_MODE_USB) {
        return COLOR_WHITE;
    }
    if (s_status.mode == POWER_MODE_BATTERY_CRITICAL) {
        return COLOR_RED;
    }
    // Battery mode: Fade from Cyan Blue (at VBAT_MAX_V) to Yellow (at VBAT_LOW_V)
    float t = (VBAT_MAX_V - s_status.voltage) / (VBAT_MAX_V - VBAT_LOW_V);
    return interpolate_rgb565(COLOR_CYAN_BLUE, COLOR_YELLOW, t);
}

void power_mgr_check_emergency_sleep(void) {
    if (s_status.mode == POWER_MODE_BATTERY_CRITICAL && s_status.critical_countdown_sec > 0) {
        s_status.critical_countdown_sec--;
        if (s_status.critical_countdown_sec == 0) {
            ESP_LOGE(TAG, "Battery critically low (%0.2fV). Emergency deep sleep now.", s_status.voltage);
            display_set_backlight(false);
            esp_wifi_stop();
            esp_deep_sleep_start();
        }
    }
}

esp_err_t power_mgr_early_battery_guard(void) {
    // Refresh battery voltage status
    power_mgr_update();

    // If USB is connected or battery is absent, boot safely
    if (s_status.usb_connected || !s_status.battery_present) {
        return ESP_OK;
    }

    // Critical low-battery threshold for early boot: 3.30V
    // Booting RF radios (Wi-Fi/BLE/LoRa) causes 300+ mA sag, tripping brownout.
    if (s_status.voltage <= 3.30f) {
        ESP_LOGE(TAG, "Battery voltage too low to safely boot (%0.2fV <= 3.30V)", s_status.voltage);

        // Display brief warning before deep sleeping
        display_set_backlight(true);
        // Wakeup on button press or after 60 seconds to re-evaluate charging state
        esp_sleep_enable_timer_wakeup(60ULL * 1000ULL * 1000ULL);
        esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_USER_BTN, 0); // Active LOW button

        vTaskDelay(pdMS_TO_TICKS(1500));
        display_set_backlight(false);
        gpio_set_level(PIN_VEXT_CTRL, 0); // Cut peripheral rail
        esp_deep_sleep_start();
    }

    return ESP_OK;
}
