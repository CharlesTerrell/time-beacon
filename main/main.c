#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "board_pins.h"
#include "config_mgr.h"
#include "usb_msc.h"
#include "gnss_driver.h"
#include "time_sync.h"
#include "gnss_task.h"
#include "wifi_manager.h"
#include "ntp_server.h"
#include "ble_cts_service.h"
#include "lora_time_beacons.h"
#include "display.h"
#include "power_mgr.h"
#include "esp_private/brownout.h"

static const char *TAG = "main";

void app_main(void) {
    // Disable hardware brownout reset immediately to avoid bootloops on weak USB power supplies
    esp_brownout_disable();

    ESP_LOGI(TAG, "%s initializing...", BOARD_HARDWARE_NAME);

    // Initialize display and power management early
    ESP_ERROR_CHECK(display_init());
    ESP_ERROR_CHECK(power_mgr_init());

    // Brownout guard: Check battery voltage BEFORE starting countdown or radios
    ESP_ERROR_CHECK(power_mgr_early_battery_guard());

    // 2.5s prompt: Check if USER button is pressed to enter Config Mode
    bool enter_config = false;
    for (int i = 25; i > 0; i--) {
        if (i % 10 == 0 || i == 25) {
            display_show_boot_prompt((i + 9) / 10);
        }
        if (gpio_get_level(PIN_USER_BTN) == 0) {
            enter_config = true;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (enter_config) {
        ESP_LOGI(TAG, "USER button pressed: Entering USB Flash Drive Config Mode");
        display_show_config_mode();
        ESP_ERROR_CHECK(usb_msc_start());
        usb_msc_run_loop();
        return;
    }

    // Normal Stratum-1 Time Server Mode
    ESP_LOGI(TAG, "Booting Stratum-1 Time Server Mode...");
    ESP_ERROR_CHECK(config_mgr_init());
    const app_config_t *cfg = config_mgr_get();
    display_set_flip(cfg->display_flip);
    if (cfg->usb_nmea_output) {
        esp_log_level_set("*", ESP_LOG_NONE);
    }
    power_mgr_set_led_peak_percent(cfg->led_peak_percent);

    ESP_ERROR_CHECK(gnss_driver_init());
    ESP_ERROR_CHECK(time_sync_init());
    ESP_ERROR_CHECK(gnss_task_start());

    // Wi-Fi service
    if (cfg->enable_wifi) {
        ESP_LOGI(TAG, "Starting Wi-Fi service...");
        ESP_ERROR_CHECK(wifi_manager_init(cfg));
    } else {
        ESP_LOGI(TAG, "Wi-Fi service is disabled in config");
    }

    // NTP server service
    if (cfg->enable_wifi && cfg->enable_ntp) {
        ESP_LOGI(TAG, "Starting NTP server service...");
        ESP_ERROR_CHECK(ntp_server_start());
    } else if (!cfg->enable_wifi) {
        ESP_LOGI(TAG, "NTP server service disabled (requires Wi-Fi)");
    } else {
        ESP_LOGI(TAG, "NTP server service is disabled in config");
    }

    // BLE CTS service
    if (cfg->enable_ble) {
        ESP_LOGI(TAG, "Starting BLE CTS service (name: %s)...", cfg->node_name);
        ESP_ERROR_CHECK(ble_cts_service_start(cfg->node_name));
    } else {
        ESP_LOGI(TAG, "BLE CTS service is disabled in config");
    }

    // LoRa time beacon service (Meshtastic)
    if (cfg->enable_meshtastic) {
        ESP_LOGI(TAG, "Starting LoRa time beacon service...");
        ESP_ERROR_CHECK(lora_time_beacons_start());
    } else {
        ESP_LOGI(TAG, "LoRa time beacon service is disabled in config");
    }

    ESP_ERROR_CHECK(display_task_start());
}
