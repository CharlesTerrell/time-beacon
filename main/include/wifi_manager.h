#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize Wi-Fi station mode:
 * 1. Initialize NVS (required by Wi-Fi).
 * 2. Initialize default event loop and netif.
 * 3. Configure static IP if CONFIG_NTP_USE_STATIC_IP is set (stop DHCP, set IP info).
 * 4. Register event handlers for Wi-Fi and IP events.
 * 5. Start Wi-Fi station and connect to AP.
 * 6. CRITICAL FOR NTP: Disable power save mode with esp_wifi_set_ps(WIFI_PS_NONE).
 *
 * Blocks until connected or starts background reconnect loop.
 * Returns ESP_OK on initialization success.
 */
esp_err_t wifi_manager_init(void);

/**
 * Check if Wi-Fi is currently connected and has an IP address.
 */
bool wifi_manager_is_connected(void);

/**
 * Get current assigned IP address string (e.g. "192.168.1.200").
 * Returns ESP_OK if connected and copied, ESP_FAIL if disconnected.
 */
esp_err_t wifi_manager_get_ip(char *ip_str, size_t max_len);

#ifdef __cplusplus
}
#endif
