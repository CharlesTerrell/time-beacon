# Current Task: Implement Wi-Fi Manager

## Objective
Implement `main/wifi_manager.c` to satisfy the station interface declared in `main/include/wifi_manager.h`.

## Files in Scope
- `main/wifi_manager.c` (implement)
- `main/include/wifi_manager.h` (read-only reference)
- `main/Kconfig.projbuild` (read-only reference)

## Specific Implementation Rules
1. Required Includes:
   - `"wifi_manager.h"`
   - `"esp_log.h"`
   - `"esp_wifi.h"`
   - `"esp_event.h"`
   - `"nvs_flash.h"`
   - `"esp_netif.h"`
   - `"lwip/ip_addr.h"`
   - `<string.h>`
2. Logging Tag & Static State:
   ```c
   static const char *TAG = "wifi_manager";
   static bool s_connected = false;
   static char s_ip_str[16] = {0};
   static esp_netif_t *s_sta_netif = NULL;
   ```
3. Event Handler `static void wifi_event_handler(...)`:
   - On `WIFI_EVENT_STA_START`: call `esp_wifi_connect()`.
   - On `WIFI_EVENT_STA_DISCONNECTED`:
     - Set `s_connected = false;`
     - Clear `s_ip_str[0] = '\0';`
     - Log warning and call `esp_wifi_connect()` to reconnect.
   - On `IP_EVENT_STA_GOT_IP`:
     - Cast `event_data` to `ip_event_got_ip_t *event`.
     - Convert IP with `esp_ip4addr_ntoa(&event->ip_info.ip, s_ip_str, sizeof(s_ip_str));`
     - Set `s_connected = true;`
     - Log `ESP_LOGI(TAG, "Wi-Fi connected! IP: %s", s_ip_str);`
4. Implement `esp_err_t wifi_manager_init(void)`:
   - Initialize NVS:
     ```c
     esp_err_t ret = nvs_flash_init();
     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
         ESP_ERROR_CHECK(nvs_flash_erase());
         ret = nvs_flash_init();
     }
     if (ret != ESP_OK) return ret;
     ```
   - Initialize network interface: `ESP_ERROR_CHECK(esp_netif_init());`
   - Create default event loop: `ESP_ERROR_CHECK(esp_event_loop_create_default());`
   - Create station netif: `s_sta_netif = esp_netif_create_default_wifi_sta();`
   - Static IP configuration (if `CONFIG_NTP_USE_STATIC_IP` is defined):
     ```c
     #if CONFIG_NTP_USE_STATIC_IP
     esp_netif_dhcpc_stop(s_sta_netif);
     esp_netif_ip_info_t ip_info;
     memset(&ip_info, 0, sizeof(ip_info));
     ip_info.ip.addr = esp_ip4addr_aton(CONFIG_NTP_STATIC_IP);
     ip_info.gw.addr = esp_ip4addr_aton(CONFIG_NTP_STATIC_GW);
     ip_info.netmask.addr = esp_ip4addr_aton(CONFIG_NTP_STATIC_NETMASK);
     esp_netif_set_ip_info(s_sta_netif, &ip_info);

     esp_netif_dns_info_t dns_info;
     dns_info.ip.u_addr.ip4.addr = esp_ip4addr_aton(CONFIG_NTP_STATIC_DNS);
     dns_info.ip.type = ESP_IPADDR_TYPE_V4;
     esp_netif_set_dns_info(s_sta_netif, ESP_NETIF_DNS_MAIN, &dns_info);
     #endif
     ```
   - Initialize Wi-Fi driver:
     ```c
     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
     ```
   - Register event handlers for `WIFI_EVENT` (any ID) and `IP_EVENT` (`IP_EVENT_STA_GOT_IP`).
   - Configure credentials and start:
     ```c
     wifi_config_t wifi_config = {0};
     strncpy((char *)wifi_config.sta.ssid, CONFIG_NTP_WIFI_SSID, sizeof(wifi_config.sta.ssid));
     strncpy((char *)wifi_config.sta.password, CONFIG_NTP_WIFI_PASSWORD, sizeof(wifi_config.sta.password));
     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
     ESP_ERROR_CHECK(esp_wifi_start());
     ```
   - **CRITICAL FOR NTP**: Disable Wi-Fi modem-sleep:
     ```c
     ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
     ESP_LOGI(TAG, "Wi-Fi modem sleep disabled (WIFI_PS_NONE)");
     ```
   - Return `ESP_OK`.
5. Implement `bool wifi_manager_is_connected(void)`:
   - Return `s_connected`.
6. Implement `esp_err_t wifi_manager_get_ip(char *ip_str, size_t max_len)`:
   - If `!s_connected || ip_str == NULL || max_len == 0`, return `ESP_FAIL`.
   - `strncpy(ip_str, s_ip_str, max_len);`
   - `ip_str[max_len - 1] = '\0';`
   - Return `ESP_OK`.

## Verification Command
```bash
idf.py build
```
When this command succeeds with zero errors and zero warnings under `-Wall`, mark task `[x]` below:

- [x] Task Complete: Wi-Fi manager implemented and verified by idf.py build.
