#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TFT_WIDTH   160
#define TFT_HEIGHT  80

/**
 * Initialize ST7735 SPI display and backlight on Heltec v1.1 pins.
 */
esp_err_t display_init(void);

/**
 * Turn display backlight ON or OFF.
 */
void display_set_backlight(bool enable);

/**
 * Draw dashboard frame:
 * IP address, Satellites, Lock status, UTC time, NTP request count.
 */
void display_update(const char *ip, uint8_t sats, bool locked, const char *utc_time, uint32_t packets);

/**
 * Show boot countdown prompt to allow user to enter Config Mode.
 */
void display_show_boot_prompt(int seconds_left);

/**
 * Show USB Config Mode screen.
 */
void display_show_config_mode(void);

/**
 * Set display orientation flip (true = 180 deg upside-down, false = 0 deg normal).
 */
void display_set_flip(bool flip);

/**
 * Toggle display orientation flip 180 degrees.
 */
void display_toggle_flip(void);

/**
 * Start the background display refresh task (refreshes once per second,
 * handles 120s inactivity auto-sleep, and configures GPIO 0 user button wake).
 */
esp_err_t display_task_start(void);

#ifdef __cplusplus
}
#endif
