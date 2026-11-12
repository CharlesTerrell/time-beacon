#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/ip_addr.h"
#include <string.h>

static const char *TAG = "wifi_manager";
static bool s_connected = false;
static char s_ip_str[16] = {0};
static esp_netif_t *s_sta_netif = NULL;
static app_config_t s_active_cfg = {0};

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wi-Fi station started, connecting to %s...", s_active_cfg.wifi_ssid);
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        if (!s_active_cfg.use_dhcp) {
            s_connected = true;
            strncpy(s_ip_str, s_active_cfg.static_ip, sizeof(s_ip_str) - 1);
            s_ip_str[sizeof(s_ip_str) - 1] = '\0';
            ESP_LOGI(TAG, "Wi-Fi connected! Static IP: %s", s_ip_str);
        } else {
            ESP_LOGI(TAG, "Wi-Fi connected to AP, waiting for DHCP lease...");
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *disconn = (wifi_event_sta_disconnected_t *)event_data;
        s_connected = false;
        s_ip_str[0] = '\0';
        ESP_LOGW(TAG, "Wi-Fi disconnected (reason %d), reconnecting...", disconn ? disconn->reason : 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        esp_ip4addr_ntoa(&event->ip_info.ip, s_ip_str, sizeof(s_ip_str));
        s_connected = true;
        ESP_LOGI(TAG, "Wi-Fi Got IP: %s", s_ip_str);
    }
}

esp_err_t wifi_manager_init(const app_config_t *cfg) {
    if (cfg != NULL) {
        s_active_cfg = *cfg;
    } else {
        const app_config_t *loaded = config_mgr_get();
        if (loaded) s_active_cfg = *loaded;
    }

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) return ret;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    s_sta_netif = esp_netif_create_default_wifi_sta();

    if (!s_active_cfg.use_dhcp) {
        esp_netif_dhcpc_stop(s_sta_netif);
        esp_netif_ip_info_t ip_info;
        memset(&ip_info, 0, sizeof(ip_info));
        ip_info.ip.addr = esp_ip4addr_aton(s_active_cfg.static_ip);
        ip_info.gw.addr = esp_ip4addr_aton(s_active_cfg.static_gateway);
        ip_info.netmask.addr = esp_ip4addr_aton(s_active_cfg.static_netmask);
        esp_netif_set_ip_info(s_sta_netif, &ip_info);

        esp_netif_dns_info_t dns_info;
        dns_info.ip.u_addr.ip4.addr = esp_ip4addr_aton(s_active_cfg.static_dns);
        dns_info.ip.type = ESP_IPADDR_TYPE_V4;
        esp_netif_set_dns_info(s_sta_netif, ESP_NETIF_DNS_MAIN, &dns_info);
        ESP_LOGI(TAG, "Configured static IP: %s (GW: %s)", s_active_cfg.static_ip, s_active_cfg.static_gateway);
    } else {
        ESP_LOGI(TAG, "Configured DHCP client mode");
    }

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    if (strlen(s_active_cfg.country_code) == 2) {
        esp_err_t country_err = esp_wifi_set_country_code(s_active_cfg.country_code, true);
        if (country_err == ESP_OK) {
            ESP_LOGI(TAG, "Wi-Fi country code set to: %s", s_active_cfg.country_code);
        } else {
            ESP_LOGW(TAG, "Failed to set country code %s: %s", s_active_cfg.country_code, esp_err_to_name(country_err));
        }
    }

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, s_active_cfg.wifi_ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, s_active_cfg.wifi_password, sizeof(wifi_config.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Cap Wi-Fi TX power to ~15 dBm (60 * 0.25 dBm) to prevent peak RF current
    // spikes from sagging the power rail and triggering the brownout detector.
    esp_err_t pwr_err = esp_wifi_set_max_tx_power(60);
    if (pwr_err == ESP_OK) {
        ESP_LOGI(TAG, "Wi-Fi max TX power capped to 15 dBm to protect power rail");
    }

    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MIN_MODEM));
    ESP_LOGI(TAG, "Wi-Fi modem sleep enabled (WIFI_PS_MIN_MODEM)");

    return ESP_OK;
}

bool wifi_manager_is_connected(void) {
    return s_connected;
}

esp_err_t wifi_manager_get_ip(char *ip_str, size_t max_len) {
    if (!s_connected || ip_str == NULL || max_len == 0) {
        return ESP_FAIL;
    }
    strncpy(ip_str, s_ip_str, max_len);
    ip_str[max_len - 1] = '\0';
    return ESP_OK;
}
