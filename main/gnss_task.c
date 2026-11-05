#include "gnss_task.h"
#include "gnss_driver.h"
#include "nmea_parser.h"
#include "time_sync.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "gnss_task";

static void gnss_stream_task(void *pvParameters) {
    char line_buf[128];
    size_t idx = 0;
    int64_t anchor_us = 0;
    uint8_t byte;

    while (1) {
        int read = gnss_driver_read(&byte, 1, 100);
        if (read <= 0) {
            continue;
        }

        if (byte == '$') {
            anchor_us = esp_timer_get_time();
            idx = 0;
            line_buf[idx++] = '$';
        } else if (idx > 0) {
            if (byte == '\r' || byte == '\n') {
                line_buf[idx] = '\0';
                if (nmea_verify_checksum(line_buf)) {
                    nmea_time_t t;
                    if (strncmp(line_buf, "$GNRMC", 6) == 0 || strncmp(line_buf, "$GPRMC", 6) == 0) {
                        if (nmea_parse_rmc(line_buf, &t)) {
                            time_sync_update_gnss(&t, anchor_us);
                        }
                    } else if (strncmp(line_buf, "$GNZDA", 6) == 0 || strncmp(line_buf, "$GPZDA", 6) == 0) {
                        if (nmea_parse_zda(line_buf, &t)) {
                            time_sync_update_gnss(&t, anchor_us);
                        }
                    }
                } else {
                    time_sync_record_error();
                }
                idx = 0;
            } else {
                if (idx < sizeof(line_buf) - 1) {
                    line_buf[idx++] = (char)byte;
                } else {
                    idx = 0;
                }
            }
        }
    }
}

esp_err_t gnss_task_start(void) {
    BaseType_t ret = xTaskCreatePinnedToCore(gnss_stream_task, "gnss_stream", 4096, NULL, 12, NULL, 1);
    return ret == pdPASS ? ESP_OK : ESP_FAIL;
}
