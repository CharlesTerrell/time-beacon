#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char wifi_ssid[33];
    char wifi_password[65];
    char country_code[3]; // e.g. "US", "EU", "JP", "CN"
    bool use_dhcp;
    char static_ip[16];
    char static_gateway[16];
    char static_netmask[16];
    char static_dns[16];
} app_config_t;

/**
 * Initialize config manager and load configuration.
 * Mounts FATFS partition, reads /storage/config.ini.
 * If file does not exist, formats/creates default template.
 * Leaves configuration accessible via config_mgr_get().
 */
esp_err_t config_mgr_init(void);

/**
 * Get pointer to active loaded configuration.
 */
const app_config_t *config_mgr_get(void);

/**
 * Write default config.ini to the given filesystem path.
 */
esp_err_t config_mgr_write_default_ini(const char *path);

#ifdef __cplusplus
}
#endif
