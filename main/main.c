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

    // Check if USER button (GPIO 0) is held during boot for Config Mode
    gpio_reset_pin(PIN_USER_BTN);
    gpio_set_direction(PIN_USER_BTN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_USER_BTN, GPIO_PULLUP_ONLY);
    vTaskDelay(pdMS_TO_TICKS(50));

    if (gpio_get_level(PIN_USER_BTN) == 0) {
        ESP_LOGI(TAG, "USER button held on boot: Entering USB Flash Drive Config Mode");
        ESP_ERROR_CHECK(display_init());
        display_show_config_mode();
        ESP_ERROR_CHECK(usb_msc_start());
        usb_msc_run_loop();
        return;
    }

    // Normal NTP Server Mode
    ESP_LOGI(TAG, "Booting Stratum-1 NTP Server Mode...");
    ESP_ERROR_CHECK(config_mgr_init());
    ESP_ERROR_CHECK(gnss_driver_init());
    ESP_ERROR_CHECK(time_sync_init());
    ESP_ERROR_CHECK(gnss_task_start());
    ESP_ERROR_CHECK(wifi_manager_init(config_mgr_get()));
    ESP_ERROR_CHECK(ntp_server_start());
    ESP_ERROR_CHECK(display_init());
    ESP_ERROR_CHECK(display_task_start());
}
