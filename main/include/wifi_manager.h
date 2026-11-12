#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "config_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_manager_init(const app_config_t *cfg);
bool wifi_manager_is_connected(void);
esp_err_t wifi_manager_get_ip(char *ip_str, size_t max_len);

/**
 * Dynamically scale Wi-Fi transmit power based on USB power and battery voltage:
 * - On USB or healthy battery (>= 3.70V): 18 dBm for max range.
 * - On low battery (< 3.60V): 14 dBm to protect against brownout voltage sags.
 */
void wifi_manager_update_power(bool usb_connected, float battery_voltage);

#ifdef __cplusplus
}
#endif
