#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize GNSS hardware:
 * 1. Configure and drive PIN_VEXT_CTRL (GPIO 3) HIGH to power UC6580.
 * 2. Delay 100ms for power rail stabilization.
 * 3. Configure UART1 (RX = GPIO 33, TX = GPIO 34, 115200 baud, 8N1).
 * 4. Install UART driver with internal ring buffer.
 *
 * Returns ESP_OK on success, or standard esp_err_t on failure.
 */
esp_err_t gnss_driver_init(void);

/**
 * Read raw bytes from GNSS UART ring buffer.
 * Non-blocking / timeout bounded.
 *
 * @param buf Destination buffer.
 * @param max_len Maximum bytes to read.
 * @param timeout_ms Max milliseconds to wait for data.
 * @return Number of bytes read (>= 0), or negative on error.
 */
int gnss_driver_read(uint8_t *buf, size_t max_len, uint32_t timeout_ms);

/**
 * Send command string to GNSS module over UART.
 *
 * @param cmd Null-terminated NMEA command string (must end in \r\n).
 * @return ESP_OK on success, or esp_err_t on failure.
 */
esp_err_t gnss_driver_send_cmd(const char *cmd);

#ifdef __cplusplus
}
#endif
