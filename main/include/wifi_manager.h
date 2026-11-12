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
 * - When battery is present and low (< 3.65V), throttles to 13 dBm regardless of USB
 *   so the onboard LiPo charger has full current budget.
 * - On USB without battery, or with healthy battery (>= 3.75V): boosts to 18 dBm.
 */
void wifi_manager_update_power(bool usb_connected, bool battery_present, float battery_voltage);

#ifdef __cplusplus
}
#endif
