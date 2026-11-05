#pragma once

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start RFC 5905 NTP server FreeRTOS task on UDP port 123.
 * Task is pinned to Core 0 (alongside LwIP stack) at priority 15.
 * Returns ESP_OK on success.
 */
esp_err_t ntp_server_start(void);

/**
 * Get total number of valid NTP requests served since boot.
 */
uint32_t ntp_server_get_packet_count(void);

#ifdef __cplusplus
}
#endif
