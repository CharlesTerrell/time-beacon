#include "config_mgr.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "wear_levelling.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if __has_include("wifi_credentials.h")
#include "wifi_credentials.h"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID CONFIG_NTP_WIFI_SSID
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD CONFIG_NTP_WIFI_PASSWORD
#endif

#ifndef STATIC_IP_ENABLE
#define STATIC_IP_ENABLE CONFIG_NTP_USE_STATIC_IP
#endif

#ifndef STATIC_IP_ADDR
#define STATIC_IP_ADDR CONFIG_NTP_STATIC_IP
#endif

#ifndef STATIC_IP_GATEWAY
#define STATIC_IP_GATEWAY CONFIG_NTP_STATIC_GW
#endif

#ifndef STATIC_IP_NETMASK
#define STATIC_IP_NETMASK CONFIG_NTP_STATIC_NETMASK
#endif

#ifndef STATIC_IP_DNS
#define STATIC_IP_DNS CONFIG_NTP_STATIC_DNS
#endif

#define CONFIG_FILE_PATH "/storage/config.ini"

static const char *TAG = "config_mgr";
static app_config_t s_config = {0};
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

static char *trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static void load_defaults(app_config_t *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    snprintf(cfg->wifi_ssid, sizeof(cfg->wifi_ssid), "%s", WIFI_SSID);
    snprintf(cfg->wifi_password, sizeof(cfg->wifi_password), "%s", WIFI_PASSWORD);
    snprintf(cfg->country_code, sizeof(cfg->country_code), "%s", "US");
    cfg->use_dhcp = !STATIC_IP_ENABLE;
    snprintf(cfg->static_ip, sizeof(cfg->static_ip), "%s", STATIC_IP_ADDR);
    snprintf(cfg->static_gateway, sizeof(cfg->static_gateway), "%s", STATIC_IP_GATEWAY);
    snprintf(cfg->static_netmask, sizeof(cfg->static_netmask), "%s", STATIC_IP_NETMASK);
    snprintf(cfg->static_dns, sizeof(cfg->static_dns), "%s", STATIC_IP_DNS);
}

esp_err_t config_mgr_write_default_ini(const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to create %s", path);
        return ESP_FAIL;
    }

    fprintf(f, "# Heltec Wireless Tracker NTP Server Configuration\n");
    fprintf(f, "# Edit this file and eject/reboot device to apply.\n\n");
    fprintf(f, "[wifi]\n");
    fprintf(f, "ssid = %s\n", s_config.wifi_ssid);
    fprintf(f, "password = %s\n", s_config.wifi_password);
    fprintf(f, "country = %s\n\n", s_config.country_code);
    fprintf(f, "[network]\n");
    fprintf(f, "dhcp = %s\n", s_config.use_dhcp ? "true" : "false");
    fprintf(f, "ip = %s\n", s_config.static_ip);
    fprintf(f, "gateway = %s\n", s_config.static_gateway);
    fprintf(f, "netmask = %s\n", s_config.static_netmask);
    fprintf(f, "dns = %s\n", s_config.static_dns);

    fclose(f);
    ESP_LOGI(TAG, "Created default template: %s", path);
    return ESP_OK;
}

static void parse_ini_file(FILE *f, app_config_t *cfg) {
    char line[128];
    char current_section[32] = {0};

    while (fgets(line, sizeof(line), f)) {
        char *p = trim_whitespace(line);
        if (*p == '\0' || *p == '#' || *p == ';') {
            continue;
        }

        if (*p == '[' && p[strlen(p) - 1] == ']') {
            p[strlen(p) - 1] = '\0';
            strncpy(current_section, p + 1, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\0';
            continue;
        }

        char *eq = strchr(p, '=');
        if (!eq) continue;

        *eq = '\0';
        char *key = trim_whitespace(p);
        char *val = trim_whitespace(eq + 1);

        if (strcmp(current_section, "wifi") == 0) {
            if (strcasecmp(key, "ssid") == 0) {
                snprintf(cfg->wifi_ssid, sizeof(cfg->wifi_ssid), "%s", val);
            } else if (strcasecmp(key, "password") == 0) {
                snprintf(cfg->wifi_password, sizeof(cfg->wifi_password), "%s", val);
            } else if (strcasecmp(key, "country") == 0) {
                snprintf(cfg->country_code, sizeof(cfg->country_code), "%s", val);
                for (int i = 0; cfg->country_code[i]; i++) {
                    cfg->country_code[i] = (char)toupper((unsigned char)cfg->country_code[i]);
                }
            }
        } else if (strcmp(current_section, "network") == 0) {
            if (strcasecmp(key, "dhcp") == 0) {
                if (strcasecmp(val, "true") == 0 || strcmp(val, "1") == 0 || strcasecmp(val, "yes") == 0) {
                    cfg->use_dhcp = true;
                } else {
                    cfg->use_dhcp = false;
                }
            } else if (strcasecmp(key, "ip") == 0) {
                snprintf(cfg->static_ip, sizeof(cfg->static_ip), "%s", val);
            } else if (strcasecmp(key, "gateway") == 0) {
                snprintf(cfg->static_gateway, sizeof(cfg->static_gateway), "%s", val);
            } else if (strcasecmp(key, "netmask") == 0) {
                snprintf(cfg->static_netmask, sizeof(cfg->static_netmask), "%s", val);
            } else if (strcasecmp(key, "dns") == 0) {
                snprintf(cfg->static_dns, sizeof(cfg->static_dns), "%s", val);
            }
        }
    }
}

esp_err_t config_mgr_init(void) {
    load_defaults(&s_config);

    esp_vfs_fat_mount_config_t mount_cfg = {
        .format_if_mount_failed = true,
        .max_files = 4,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl("/storage", "storage", &mount_cfg, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to mount FATFS storage (%s), using compiled defaults", esp_err_to_name(err));
        return err;
    }

    FILE *f = fopen(CONFIG_FILE_PATH, "r");
    if (!f) {
        ESP_LOGI(TAG, "Config file not found. Creating default: %s", CONFIG_FILE_PATH);
        config_mgr_write_default_ini(CONFIG_FILE_PATH);
    } else {
        parse_ini_file(f, &s_config);
        fclose(f);
        ESP_LOGI(TAG, "Loaded config: SSID='%s', Country='%s', DHCP=%s, IP='%s'",
                 s_config.wifi_ssid, s_config.country_code,
                 s_config.use_dhcp ? "true" : "false",
                 s_config.use_dhcp ? "auto" : s_config.static_ip);
    }

    esp_vfs_fat_spiflash_unmount_rw_wl("/storage", s_wl_handle);
    s_wl_handle = WL_INVALID_HANDLE;
    return ESP_OK;
}

const app_config_t *config_mgr_get(void) {
    return &s_config;
}
