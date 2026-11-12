#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    POWER_MODE_USB,              /* Powered via USB (charging battery or running without battery) */
    POWER_MODE_BATTERY,          /* Running on battery (voltage normal, background light blue -> yellow) */
    POWER_MODE_BATTERY_CRITICAL  /* Battery critically low (<= 3.35V) -> Red display, flash LED */
} power_mode_t;

typedef struct {
    power_mode_t mode;
    float voltage;               /* Measured battery voltage in Volts */
    int percentage;              /* Estimated remaining percentage (0 - 100%) */
    bool battery_present;        /* True if physical battery detected */
    bool usb_connected;          /* True if USB connection detected */
    int critical_countdown_sec;  /* Seconds remaining before emergency sleep (< 0 if not critical) */
} power_status_t;

/**
 * Initialize ADC for battery measurement and LEDC for status LED control.
 */
esp_err_t power_mgr_init(void);

/**
 * Periodic update of battery measurement and power mode determination.
 * Call every ~500ms to 1s.
 */
void power_mgr_update(void);

/**
 * Retrieve the current power status structure.
 */
void power_mgr_get_status(power_status_t *out_status);

/**
 * Set the peak brightness percentage (0 - 100%) of the status LED pulse
 * during screen-off sleep mode.
 */
void power_mgr_set_led_peak_percent(uint8_t percent);

/**
 * Drive the status LED based on backlight state and power mode.
 * Call from periodic display loop (~50ms intervals).
 */
void power_mgr_tick_led(bool display_backlight_on);

/**
 * Get current adaptive display background color (RGB565).
 */
uint16_t power_mgr_get_bg_color(void);

/**
 * Get current adaptive display foreground text color (RGB565).
 */
uint16_t power_mgr_get_fg_color(void);

/**
 * Check if critical battery countdown expired and trigger emergency sleep.
 */
void power_mgr_check_emergency_sleep(void);

/**
 * Early boot low-battery check.
 * If running on battery and voltage is <= 3.30V, displays low-battery alert
 * and puts the device into deep sleep to prevent brownout boot loops.
 */
esp_err_t power_mgr_early_battery_guard(void);

#ifdef __cplusplus
}
#endif
