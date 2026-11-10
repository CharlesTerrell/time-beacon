#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // Device identity
    char node_name[33];
    uint8_t led_peak_percent; // 0 - 100%
    bool usb_nmea_output;     // Stream raw NMEA out USB console instead of logging
    bool enable_pps;          // Enable hardware 1PPS discipline (default: true; false forces NMEA fallback)
    int16_t nmea_offset_ms;   // Software delay compensation in ms when 1PPS is absent (default: 60)

    // Services enable/disable
    bool enable_wifi;
    bool enable_ntp;
    bool enable_ble;
    bool enable_meshcore;
    bool enable_meshtastic;
    bool enable_ble_battery;
    bool enable_lora_power;
    uint8_t position_precision; // Meshtastic precision bits: 18 = ~90m fuzzy/privacy, 32 = exact GPS

    // LoRa RF Settings
    uint32_t lora_freq_hz;
    int8_t lora_tx_power;

    // Wi-Fi network settings
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
