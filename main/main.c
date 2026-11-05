#include <stdio.h>
#include "esp_log.h"
#include "board_pins.h"
#include "gnss_driver.h"
#include "time_sync.h"
#include "gnss_task.h"
#include "wifi_manager.h"
#include "ntp_server.h"
#include "display.h"

static const char *TAG = "main";

void app_main(void) {
    ESP_LOGI(TAG, "Heltec Tracker v1.1 NTP Server starting...");
    ESP_ERROR_CHECK(gnss_driver_init());
    ESP_ERROR_CHECK(time_sync_init());
    ESP_ERROR_CHECK(gnss_task_start());
    ESP_ERROR_CHECK(wifi_manager_init());
    ESP_ERROR_CHECK(ntp_server_start());
    ESP_ERROR_CHECK(display_init());
    ESP_ERROR_CHECK(display_task_start());
}
