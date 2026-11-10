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

static bool parse_bool(const char *val, bool default_val) {
    if (strcasecmp(val, "true") == 0 || strcmp(val, "1") == 0 ||
        strcasecmp(val, "yes") == 0 || strcasecmp(val, "on") == 0 ||
        strcasecmp(val, "enable") == 0 || strcasecmp(val, "enabled") == 0) {
        return true;
    }
    if (strcasecmp(val, "false") == 0 || strcmp(val, "0") == 0 ||
        strcasecmp(val, "no") == 0 || strcasecmp(val, "off") == 0 ||
        strcasecmp(val, "disable") == 0 || strcasecmp(val, "disabled") == 0) {
        return false;
    }
    return default_val;
}

static uint32_t get_default_lora_freq_by_country(const char *country) {
    if (!country || country[0] == '\0') {
        return 906875000; // US915 default
    }
    if (strcasecmp(country, "US") == 0 || strcasecmp(country, "CA") == 0 ||
        strcasecmp(country, "MX") == 0) {
        return 906875000; // US915
    }
    if (strcasecmp(country, "EU") == 0 || strcasecmp(country, "GB") == 0 ||
        strcasecmp(country, "UK") == 0 || strcasecmp(country, "DE") == 0 ||
        strcasecmp(country, "FR") == 0 || strcasecmp(country, "IT") == 0 ||
        strcasecmp(country, "ES") == 0 || strcasecmp(country, "NL") == 0 ||
        strcasecmp(country, "PL") == 0 || strcasecmp(country, "SE") == 0 ||
        strcasecmp(country, "CH") == 0 || strcasecmp(country, "AT") == 0 ||
        strcasecmp(country, "BE") == 0 || strcasecmp(country, "DK") == 0 ||
        strcasecmp(country, "FI") == 0 || strcasecmp(country, "NO") == 0 ||
        strcasecmp(country, "CZ") == 0 || strcasecmp(country, "IE") == 0 ||
        strcasecmp(country, "PT") == 0 || strcasecmp(country, "GR") == 0) {
        return 869525000; // EU868
    }
    if (strcasecmp(country, "AU") == 0 || strcasecmp(country, "NZ") == 0) {
        return 916875000; // AU915
    }
    if (strcasecmp(country, "IN") == 0) {
        return 865200000; // IN865
    }
    if (strcasecmp(country, "JP") == 0) {
        return 921875000; // JP920
    }
    if (strcasecmp(country, "KR") == 0) {
        return 921900000; // KR920
    }
    if (strcasecmp(country, "CN") == 0) {
        return 470000000; // CN470
    }
    return 906875000; // Default US915
}

static void load_defaults(app_config_t *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    snprintf(cfg->node_name, sizeof(cfg->node_name), "%s", "Heltec-Time");
    cfg->led_peak_percent = 50;
    cfg->usb_nmea_output = false;
    cfg->enable_pps = true;
    cfg->nmea_offset_ms = 60;
    cfg->enable_wifi = true;
    cfg->enable_ntp = true;
    cfg->enable_ble = true;
    cfg->enable_meshcore = true;
    cfg->enable_meshtastic = true;
    cfg->enable_ble_battery = true;
    cfg->enable_lora_power = true;
    cfg->position_precision = 18; // Default ~90m fuzzy precision for privacy
    snprintf(cfg->country_code, sizeof(cfg->country_code), "%s", "US");
    cfg->lora_freq_hz = get_default_lora_freq_by_country(cfg->country_code);
    cfg->lora_tx_power = 22;
    snprintf(cfg->wifi_ssid, sizeof(cfg->wifi_ssid), "%s", WIFI_SSID);
    snprintf(cfg->wifi_password, sizeof(cfg->wifi_password), "%s", WIFI_PASSWORD);
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

    fprintf(f, "# ==============================================================================\n");
    fprintf(f, "# Heltec Stratum-1 Time Server Configuration\n");
    fprintf(f, "# ==============================================================================\n");
    fprintf(f, "# Edit this file, save, and eject/reboot device to apply changes.\n");
    fprintf(f, "# Lines starting with '#' or ';' are comments.\n");
    fprintf(f, "# Built-in defaults are used for commented options; uncomment to customize.\n\n");

    fprintf(f, "[device]\n");
    fprintf(f, "# Custom device node name broadcast in beacons and telemetry (max 32 chars):\n");
    fprintf(f, "node_name = %s\n\n", s_config.node_name);
    fprintf(f, "# Regulatory ISO country code (controls Wi-Fi channels and default LoRa frequency plan):\n");
    fprintf(f, "#   US = 906.875 MHz (US915)      EU = 869.525 MHz (EU868)      AU = 916.875 MHz (AU915)\n");
    fprintf(f, "#   IN = 865.200 MHz (IN865)      JP = 921.875 MHz (JP920)      KR = 921.900 MHz (KR920)\n");
    fprintf(f, "#   CN = 470.000 MHz (CN470)\n");
    fprintf(f, "# country = %s\n\n", s_config.country_code);
    fprintf(f, "# Status LED peak pulse intensity when screen is in powersave/off (0 to 100 percent):\n");
    fprintf(f, "# led_peak_percent = %u\n\n", (unsigned int)s_config.led_peak_percent);
    fprintf(f, "# Stream raw NMEA sentences ($GNRMC, $GNZDA, etc.) out USB console instead of logs:\n");
    fprintf(f, "# Useful for GPS passthrough to PC apps like GPSD, Chrony, or QGIS (true/false).\n");
    fprintf(f, "# usb_nmea_output = %s\n\n", s_config.usb_nmea_output ? "true" : "false");
    fprintf(f, "# Hardware 1PPS discipline (GPIO 36 on v1.1/v2, GPIO 41 on v4):\n");
    fprintf(f, "# true  = Lock top-of-second to hardware 1PPS rising edge (< 1 us accuracy).\n");
    fprintf(f, "# false = Force software NMEA delay compensation (useful for fallback testing).\n");
    fprintf(f, "# enable_pps = %s\n\n", s_config.enable_pps ? "true" : "false");
    fprintf(f, "# Software NMEA delay compensation in ms when 1PPS is absent or disabled (default: 60):\n");
    fprintf(f, "# nmea_offset_ms = %d\n\n", (int)s_config.nmea_offset_ms);

    fprintf(f, "[services]\n");
    fprintf(f, "# Service toggles (true = enabled, false = disabled)\n");
    fprintf(f, "# Wi-Fi network connection:\n");
    fprintf(f, "# wifi = %s\n\n", s_config.enable_wifi ? "true" : "false");
    fprintf(f, "# UDP Stratum-1 NTP server on port 123:\n");
    fprintf(f, "# ntp = %s\n\n", s_config.enable_ntp ? "true" : "false");
    fprintf(f, "# Bluetooth Low Energy (CTS 0x1805 Current Time Service & Power Telemetry):\n");
    fprintf(f, "# bluetooth = %s\n\n", s_config.enable_ble ? "true" : "false");
    fprintf(f, "# Periodic MeshCore binary time beacon broadcasts over LoRa:\n");
    fprintf(f, "# meshcore = %s\n\n", s_config.enable_meshcore ? "true" : "false");
    fprintf(f, "# Periodic Meshtastic POSITION_APP and TELEMETRY_APP broadcasts over LoRa:\n");
    fprintf(f, "# meshtastic = %s\n\n", s_config.enable_meshtastic ? "true" : "false");
    fprintf(f, "# Location precision in bits for Meshtastic POSITION_APP (default: 18):\n");
    fprintf(f, "# 18 = ~90m fuzzy precision (protects exact base/home privacy on public mesh)\n");
    fprintf(f, "# 32 = exact GPS precision (< 1m)\n");
    fprintf(f, "# position_precision = %u\n\n", (unsigned int)s_config.position_precision);
    fprintf(f, "# BLE Standard Battery Service (BAS 0x180F) battery level telemetry:\n");
    fprintf(f, "# ble_battery = %s\n\n", s_config.enable_ble_battery ? "true" : "false");
    fprintf(f, "# LoRa packet power telemetry (battery mV and solar panel bus mV):\n");
    fprintf(f, "# lora_power = %s\n\n", s_config.enable_lora_power ? "true" : "false");

    fprintf(f, "[lora]\n");
    fprintf(f, "# LoRa carrier frequency in Hz.\n");
    fprintf(f, "# Leave commented out to automatically use regional frequency for 'country' above.\n");
    fprintf(f, "# To override with an explicit custom frequency, uncomment and set:\n");
    fprintf(f, "# lora_freq = %lu\n\n", (unsigned long)s_config.lora_freq_hz);
    fprintf(f, "# LoRa RF transmit power in dBm (0 to 22 dBm, default: 22):\n");
    fprintf(f, "# tx_power = %d\n\n", (int)s_config.lora_tx_power);

    fprintf(f, "[wifi]\n");
    fprintf(f, "# 2.4 GHz Wi-Fi credentials for local network NTP server:\n");
    fprintf(f, "ssid = %s\n", s_config.wifi_ssid);
    fprintf(f, "password = %s\n\n", s_config.wifi_password);
    fprintf(f, "# Regulatory ISO country code for Wi-Fi channels and default LoRa frequency:\n");
    fprintf(f, "# country = %s\n\n", s_config.country_code);

    fprintf(f, "[network]\n");
    fprintf(f, "# IP address configuration mode:\n");
    fprintf(f, "# Set dhcp = true to obtain dynamic IP from router (default).\n");
    fprintf(f, "# Set dhcp = false to use static IP configuration below.\n");
    fprintf(f, "dhcp = %s\n\n", s_config.use_dhcp ? "true" : "false");
    fprintf(f, "# Static IP settings (active only when dhcp = false):\n");
    fprintf(f, "# ip = %s\n", s_config.static_ip[0] ? s_config.static_ip : "192.168.1.200");
    fprintf(f, "# gateway = %s\n", s_config.static_gateway[0] ? s_config.static_gateway : "192.168.1.1");
    fprintf(f, "# netmask = %s\n", s_config.static_netmask[0] ? s_config.static_netmask : "255.255.255.0");
    fprintf(f, "# dns = %s\n", s_config.static_dns[0] ? s_config.static_dns : "1.1.1.1");

    fclose(f);
    ESP_LOGI(TAG, "Created default template: %s", path);
    return ESP_OK;
}

static void parse_ini_file(FILE *f, app_config_t *cfg) {
    char line[128];
    char current_section[32] = {0};
    bool explicit_freq = false;

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

        if (strcmp(current_section, "device") == 0 || strcmp(current_section, "general") == 0 ||
            strcmp(current_section, "power") == 0) {
            if (strcasecmp(key, "node_name") == 0 || strcasecmp(key, "name") == 0) {
                snprintf(cfg->node_name, sizeof(cfg->node_name), "%s", val);
            } else if (strcasecmp(key, "led_peak_percent") == 0 ||
                       strcasecmp(key, "led_intensity") == 0 ||
                       strcasecmp(key, "led_brightness") == 0 ||
                       strcasecmp(key, "led_percent") == 0) {
                int p = atoi(val);
                if (p < 0) p = 0;
                if (p > 100) p = 100;
                cfg->led_peak_percent = (uint8_t)p;
            } else if (strcasecmp(key, "usb_nmea_output") == 0 ||
                       strcasecmp(key, "usb_nmea") == 0 ||
                       strcasecmp(key, "gps_passthrough") == 0) {
                cfg->usb_nmea_output = parse_bool(val, cfg->usb_nmea_output);
            } else if (strcasecmp(key, "enable_pps") == 0 ||
                       strcasecmp(key, "pps_enable") == 0 ||
                       strcasecmp(key, "pps") == 0) {
                cfg->enable_pps = parse_bool(val, cfg->enable_pps);
            } else if (strcasecmp(key, "nmea_offset_ms") == 0 ||
                       strcasecmp(key, "nmea_delay_ms") == 0 ||
                       strcasecmp(key, "gps_offset_ms") == 0) {
                cfg->nmea_offset_ms = (int16_t)atoi(val);
            } else if (strcasecmp(key, "country") == 0 || strcasecmp(key, "country_code") == 0) {
                snprintf(cfg->country_code, sizeof(cfg->country_code), "%s", val);
                for (int i = 0; cfg->country_code[i]; i++) {
                    cfg->country_code[i] = (char)toupper((unsigned char)cfg->country_code[i]);
                }
                if (!explicit_freq) {
                    cfg->lora_freq_hz = get_default_lora_freq_by_country(cfg->country_code);
                }
            }
        } else if (strcmp(current_section, "regulatory") == 0) {
            if (strcasecmp(key, "country") == 0 || strcasecmp(key, "country_code") == 0) {
                snprintf(cfg->country_code, sizeof(cfg->country_code), "%s", val);
                for (int i = 0; cfg->country_code[i]; i++) {
                    cfg->country_code[i] = (char)toupper((unsigned char)cfg->country_code[i]);
                }
                if (!explicit_freq) {
                    cfg->lora_freq_hz = get_default_lora_freq_by_country(cfg->country_code);
                }
            }
        } else if (strcmp(current_section, "services") == 0) {
            if (strcasecmp(key, "wifi") == 0) {
                cfg->enable_wifi = parse_bool(val, cfg->enable_wifi);
            } else if (strcasecmp(key, "ntp") == 0) {
                cfg->enable_ntp = parse_bool(val, cfg->enable_ntp);
            } else if (strcasecmp(key, "pps") == 0 || strcasecmp(key, "enable_pps") == 0) {
                cfg->enable_pps = parse_bool(val, cfg->enable_pps);
            } else if (strcasecmp(key, "bluetooth") == 0 || strcasecmp(key, "ble") == 0) {
                cfg->enable_ble = parse_bool(val, cfg->enable_ble);
            } else if (strcasecmp(key, "meshcore") == 0) {
                cfg->enable_meshcore = parse_bool(val, cfg->enable_meshcore);
            } else if (strcasecmp(key, "meshtastic") == 0) {
                cfg->enable_meshtastic = parse_bool(val, cfg->enable_meshtastic);
            } else if (strcasecmp(key, "position_precision") == 0 || strcasecmp(key, "precision_bits") == 0) {
                int prec = atoi(val);
                if (prec >= 0 && prec <= 32) cfg->position_precision = (uint8_t)prec;
            } else if (strcasecmp(key, "ble_battery") == 0 || strcasecmp(key, "battery_ble") == 0) {
                cfg->enable_ble_battery = parse_bool(val, cfg->enable_ble_battery);
            } else if (strcasecmp(key, "lora_power") == 0 || strcasecmp(key, "lora_telemetry") == 0) {
                cfg->enable_lora_power = parse_bool(val, cfg->enable_lora_power);
            }
        } else if (strcmp(current_section, "power") == 0) {
            if (strcasecmp(key, "ble_battery") == 0) {
                cfg->enable_ble_battery = parse_bool(val, cfg->enable_ble_battery);
            } else if (strcasecmp(key, "lora_power") == 0 || strcasecmp(key, "lora_telemetry") == 0) {
                cfg->enable_lora_power = parse_bool(val, cfg->enable_lora_power);
            }
        } else if (strcmp(current_section, "lora") == 0) {
            if (strcasecmp(key, "lora_freq") == 0 || strcasecmp(key, "lora_freq_hz") == 0 ||
                strcasecmp(key, "lora_frequency") == 0 || strcasecmp(key, "frequency") == 0 ||
                strcasecmp(key, "freq") == 0) {
                uint32_t f_val = (uint32_t)strtoul(val, NULL, 10);
                if (f_val > 100000000 && f_val < 1000000000) {
                    cfg->lora_freq_hz = f_val;
                    explicit_freq = true;
                }
            } else if (strcasecmp(key, "country") == 0 || strcasecmp(key, "country_code") == 0) {
                snprintf(cfg->country_code, sizeof(cfg->country_code), "%s", val);
                for (int i = 0; cfg->country_code[i]; i++) {
                    cfg->country_code[i] = (char)toupper((unsigned char)cfg->country_code[i]);
                }
                if (!explicit_freq) {
                    cfg->lora_freq_hz = get_default_lora_freq_by_country(cfg->country_code);
                }
            } else if (strcasecmp(key, "tx_power") == 0 || strcasecmp(key, "power") == 0) {
                int p_val = atoi(val);
                if (p_val >= 0 && p_val <= 22) cfg->lora_tx_power = (int8_t)p_val;
            }
        } else if (strcmp(current_section, "meshcore") == 0) {
            if (strcasecmp(key, "enable") == 0 || strcasecmp(key, "enabled") == 0) {
                cfg->enable_meshcore = parse_bool(val, cfg->enable_meshcore);
            }
        } else if (strcmp(current_section, "meshtastic") == 0) {
            if (strcasecmp(key, "enable") == 0 || strcasecmp(key, "enabled") == 0) {
                cfg->enable_meshtastic = parse_bool(val, cfg->enable_meshtastic);
            } else if (strcasecmp(key, "position_precision") == 0 || strcasecmp(key, "precision_bits") == 0) {
                int prec = atoi(val);
                if (prec >= 0 && prec <= 32) cfg->position_precision = (uint8_t)prec;
            }
        } else if (strcmp(current_section, "wifi") == 0) {
            if (strcasecmp(key, "enable") == 0 || strcasecmp(key, "enabled") == 0) {
                cfg->enable_wifi = parse_bool(val, cfg->enable_wifi);
            } else if (strcasecmp(key, "ssid") == 0) {
                snprintf(cfg->wifi_ssid, sizeof(cfg->wifi_ssid), "%s", val);
            } else if (strcasecmp(key, "password") == 0) {
                snprintf(cfg->wifi_password, sizeof(cfg->wifi_password), "%s", val);
            } else if (strcasecmp(key, "country") == 0) {
                snprintf(cfg->country_code, sizeof(cfg->country_code), "%s", val);
                for (int i = 0; cfg->country_code[i]; i++) {
                    cfg->country_code[i] = (char)toupper((unsigned char)cfg->country_code[i]);
                }
                if (!explicit_freq) {
                    cfg->lora_freq_hz = get_default_lora_freq_by_country(cfg->country_code);
                }
            }
        } else if (strcmp(current_section, "ntp") == 0) {
            if (strcasecmp(key, "enable") == 0 || strcasecmp(key, "enabled") == 0) {
                cfg->enable_ntp = parse_bool(val, cfg->enable_ntp);
            }
        } else if (strcmp(current_section, "bluetooth") == 0 || strcmp(current_section, "ble") == 0) {
            if (strcasecmp(key, "enable") == 0 || strcasecmp(key, "enabled") == 0) {
                cfg->enable_ble = parse_bool(val, cfg->enable_ble);
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
        ESP_LOGI(TAG, "Device: NodeName='%s', Country='%s', LEDPeak=%u%%",
                 s_config.node_name, s_config.country_code, (unsigned int)s_config.led_peak_percent);
        ESP_LOGI(TAG, "Services: WiFi=%d NTP=%d BLE=%d MeshCore=%d Meshtastic=%d (LoRa %luHz, %ddBm)",
                 s_config.enable_wifi, s_config.enable_ntp, s_config.enable_ble,
                 s_config.enable_meshcore, s_config.enable_meshtastic,
                 (unsigned long)s_config.lora_freq_hz, (int)s_config.lora_tx_power);
    }

    esp_vfs_fat_spiflash_unmount_rw_wl("/storage", s_wl_handle);
    s_wl_handle = WL_INVALID_HANDLE;
    return ESP_OK;
}

const app_config_t *config_mgr_get(void) {
    return &s_config;
}
