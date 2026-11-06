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
#include "display.h"

static const char *TAG = "main";

void app_main(void) {
    ESP_LOGI(TAG, "Heltec Tracker v1.1 initializing...");

    // Initialize display early
    ESP_ERROR_CHECK(display_init());

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

    // Normal Stratum-1 NTP Server Mode
    ESP_LOGI(TAG, "Booting Stratum-1 NTP Server Mode...");
    ESP_ERROR_CHECK(config_mgr_init());
    ESP_ERROR_CHECK(gnss_driver_init());
    ESP_ERROR_CHECK(time_sync_init());
    ESP_ERROR_CHECK(gnss_task_start());
    ESP_ERROR_CHECK(wifi_manager_init(config_mgr_get()));
    ESP_ERROR_CHECK(ntp_server_start());
    ESP_ERROR_CHECK(display_task_start());
}
