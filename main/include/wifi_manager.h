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

#ifdef __cplusplus
}
#endif
