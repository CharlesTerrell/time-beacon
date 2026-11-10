#include "display.h"
#include "font5x7.h"
#include "board_pins.h"
#include "time_sync.h"
#include "wifi_manager.h"
#include "ntp_server.h"
#include "power_mgr.h"
#include "config_mgr.h"
#include "ble_cts_service.h"
#include "lora_time_beacons.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

static const char *TAG = "display";
static spi_device_handle_t s_spi = NULL;
static bool s_backlight_on = true;
static int64_t s_last_activity_us = 0;
static uint16_t s_framebuf[TFT_WIDTH * TFT_HEIGHT];

static void tft_cmd(uint8_t cmd) {
    gpio_set_level(PIN_TFT_DC, 0);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi, &t));
}

static void tft_data(const uint8_t *data, size_t len) {
    if (len == 0) return;
    gpio_set_level(PIN_TFT_DC, 1);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi, &t));
}

static void tft_data_byte(uint8_t val) {
    tft_data(&val, 1);
}

static void draw_pixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < TFT_WIDTH && y >= 0 && y < TFT_HEIGHT) {
        s_framebuf[y * TFT_WIDTH + x] = (color >> 8) | (color << 8); // Swap endian for SPI
    }
}

static void draw_char(int x, int y, char c, uint16_t fg, uint16_t bg) {
    if (c < 32 || c > 126) c = ' ';
    const uint8_t *glyph = font5x7[c - 32];
    for (int col = 0; col < 5; col++) {
        uint8_t line = glyph[col];
        for (int row = 0; row < 7; row++) {
            draw_pixel(x + col, y + row, (line & (1 << row)) ? fg : bg);
        }
    }
    for (int row = 0; row < 7; row++) {
        draw_pixel(x + 5, y + row, bg); // Character spacing column
    }
}

static void draw_string(int x, int y, const char *s, uint16_t fg, uint16_t bg) {
    while (*s) {
        draw_char(x, y, *s, fg, bg);
        x += 6;
        s++;
    }
}

void display_set_backlight(bool enable) {
    gpio_set_level(PIN_TFT_BL, enable ? 1 : 0);
    s_backlight_on = enable;
}

static void display_flush(void) {
    // ST7735 80x160 panel in landscape mode offset: col 1..160, row 26..105
    uint8_t caset[] = {0x00, 0x01, 0x00, 0xA0};
    uint8_t raset[] = {0x00, 0x1A, 0x00, 0x69};

    tft_cmd(0x2A); // CASET
    tft_data(caset, sizeof(caset));

    tft_cmd(0x2B); // RASET
    tft_data(raset, sizeof(raset));

    tft_cmd(0x2C); // RAMWR
    tft_data((const uint8_t *)s_framebuf, sizeof(s_framebuf));
}

void display_update(const char *ip, uint8_t sats, bool locked, const char *utc_time, uint32_t packets) {
    power_status_t pwr;
    power_mgr_get_status(&pwr);
    uint16_t bg_color = power_mgr_get_bg_color();
    uint16_t fg_color = power_mgr_get_fg_color();
    uint16_t bg_swapped = (uint16_t)((bg_color >> 8) | (bg_color << 8));

    for (size_t i = 0; i < TFT_WIDTH * TFT_HEIGHT; i++) {
        s_framebuf[i] = bg_swapped;
    }

    if (pwr.mode == POWER_MODE_BATTERY_CRITICAL) {
        draw_string(4, 4, "! CRITICAL BATTERY !", fg_color, bg_color);
        draw_string(4, 20, "CONNECT USB CHARGER", fg_color, bg_color);
        char sleep_str[32];
        snprintf(sleep_str, sizeof(sleep_str), "SHUTDOWN IN: %ds", pwr.critical_countdown_sec);
        draw_string(4, 38, sleep_str, fg_color, bg_color);
        char v_str[32];
        snprintf(v_str, sizeof(v_str), "VOLTAGE: %0.2fV", pwr.voltage);
        draw_string(4, 56, v_str, fg_color, bg_color);
        display_flush();
        return;
    }

    const app_config_t *cfg = config_mgr_get();
    char title_str[64];
    if (cfg && cfg->node_name[0] != '\0') {
        if (strlen(cfg->node_name) <= 15) {
            snprintf(title_str, sizeof(title_str), "%s STRATUM 1", cfg->node_name);
        } else {
            snprintf(title_str, sizeof(title_str), "%.24s", cfg->node_name);
        }
    } else {
        snprintf(title_str, sizeof(title_str), "HELTEC TIME STRATUM 1");
    }
    draw_string(4, 4, title_str, fg_color, bg_color);

    char ip_str[32];
    uint32_t lora_cnt = lora_time_beacons_get_count();
    if (cfg && !cfg->enable_wifi) {
        snprintf(ip_str, sizeof(ip_str), "LoRa:%s [%lu]", lora_time_beacons_get_last_mode(), (unsigned long)lora_cnt);
    } else {
        snprintf(ip_str, sizeof(ip_str), "IP: %s", ip ? ip : "---");
    }
    draw_string(4, 18, ip_str, fg_color, bg_color);

    char sats_str[32];
    const char *lock_mode = locked ? (time_sync_is_pps_locked() ? "PPS" : "NMEA") : "WAIT";
    snprintf(sats_str, sizeof(sats_str), "SATS: %u [%s]", (unsigned int)sats, lock_mode);
    uint16_t lock_color = locked ? 0x07E0 : 0xF800; // Bright green / bright red on dark bg
    draw_string(4, 32, sats_str, lock_color, bg_color);

    char time_str[32];
    gnss_status_t gnss;
    if (time_sync_get_status(&gnss) && gnss.has_pos) {
        float lat = (float)gnss.latitude_i / 10000000.0f;
        float lon = (float)gnss.longitude_i / 10000000.0f;
        snprintf(time_str, sizeof(time_str), "UTC %s %0.1f %0.1f", utc_time ? utc_time : "--:--:--", lat, lon);
    } else {
        snprintf(time_str, sizeof(time_str), "UTC: %s", utc_time ? utc_time : "--:--:--");
    }
    draw_string(4, 46, time_str, fg_color, bg_color);

    char status_str[32];
    const char *ble_status = "";
    if (cfg && cfg->enable_ble) {
        ble_status = ble_cts_service_is_connected() ? "[BT*]" : "[BT]";
    }
    if (pwr.mode == POWER_MODE_USB) {
        snprintf(status_str, sizeof(status_str), "N:%lu L:%lu %s USB",
                 (unsigned long)packets, (unsigned long)lora_cnt, ble_status);
    } else {
        snprintf(status_str, sizeof(status_str), "N:%lu L:%lu %s %0.1fV",
                 (unsigned long)packets, (unsigned long)lora_cnt, ble_status, pwr.voltage);
    }
    draw_string(4, 60, status_str, fg_color, bg_color);

    display_flush();
}

void display_show_boot_prompt(int seconds_left) {
    memset(s_framebuf, 0, sizeof(s_framebuf));
    draw_string(4, 4, "HELTEC NTP SERVER", 0xFFFF, 0x0000);   // White
    draw_string(4, 20, "Press USER btn", 0xFFFF, 0x0000);     // White
    draw_string(4, 34, "for USB Config...", 0xFFFF, 0x0000);  // White
    char sec_str[32];
    snprintf(sec_str, sizeof(sec_str), "Starting in %ds", seconds_left);
    draw_string(4, 52, sec_str, 0x07E0, 0x0000);             // Green
    display_flush();
    display_set_backlight(true);
}

void display_show_config_mode(void) {
    memset(s_framebuf, 0, sizeof(s_framebuf));
    draw_string(4, 4, "USB CONFIG MODE", 0x07FF, 0x0000);  // Cyan
    draw_string(4, 20, "Drive: NTP_CONFIG", 0xFFFF, 0x0000); // White
    draw_string(4, 34, "Edit: config.ini", 0xFFE0, 0x0000); // Yellow
    draw_string(4, 48, "Tap USER to exit", 0x07E0, 0x0000); // Green
    draw_string(4, 62, "& reboot server", 0x07E0, 0x0000);  // Green
    display_flush();
    display_set_backlight(true);
}

static void display_refresh_task(void *pvParameters) {
    (void)pvParameters;
    uint32_t tick_count = 0;
    while (1) {
        power_mgr_tick_led(s_backlight_on);

        if (gpio_get_level(PIN_USER_BTN) == 0) {
            s_last_activity_us = esp_timer_get_time();
            if (!s_backlight_on) {
                display_set_backlight(true);
                tick_count = 20; // Trigger immediate display refresh
            }
        }
        if ((esp_timer_get_time() - s_last_activity_us) > 120000000LL) {
            display_set_backlight(false);
        }
        if (++tick_count >= 20) {
            tick_count = 0;
            power_mgr_update();
            power_mgr_check_emergency_sleep();

            power_status_t pwr;
            power_mgr_get_status(&pwr);
            ble_cts_service_update_power((uint8_t)pwr.percentage, (uint16_t)(pwr.voltage * 1000.0f), pwr.usb_connected);
            if (pwr.mode == POWER_MODE_BATTERY_CRITICAL && !s_backlight_on) {
                display_set_backlight(true);
                s_last_activity_us = esp_timer_get_time();
            }

            if (s_backlight_on) {
                char ip[16] = "DISCONNECTED";
                wifi_manager_get_ip(ip, sizeof(ip));

                gnss_status_t status;
                bool locked = time_sync_get_status(&status);

                char utc_time[16] = "--:--:--";
                if (status.last_fix_esp_time_us > 0) {
                    snprintf(utc_time, sizeof(utc_time), "%02u:%02u:%02u",
                             (unsigned int)status.last_fix_time.hour,
                             (unsigned int)status.last_fix_time.minute,
                             (unsigned int)status.last_fix_time.second);
                }

                uint32_t packets = ntp_server_get_packet_count();
                display_update(ip, status.satellites, locked, utc_time, packets);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

esp_err_t display_task_start(void) {
    BaseType_t ret = xTaskCreatePinnedToCore(display_refresh_task, "display_task", 4096, NULL, 5, NULL, 1);
    return ret == pdPASS ? ESP_OK : ESP_FAIL;
}

esp_err_t display_init(void) {
    gpio_reset_pin(PIN_VEXT_CTRL);
    gpio_set_direction(PIN_VEXT_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_VEXT_CTRL, 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    gpio_reset_pin(PIN_TFT_BL);
    gpio_reset_pin(PIN_TFT_RST);
    gpio_reset_pin(PIN_TFT_DC);
    gpio_reset_pin(PIN_USER_BTN);

    gpio_set_direction(PIN_TFT_BL, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_TFT_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_TFT_DC, GPIO_MODE_OUTPUT);

    gpio_set_direction(PIN_USER_BTN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_USER_BTN, GPIO_PULLUP_ONLY);

    gpio_set_level(PIN_TFT_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PIN_TFT_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_TFT_SDIN,
        .miso_io_num = -1,
        .sclk_io_num = PIN_TFT_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = TFT_WIDTH * TFT_HEIGHT * 2,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20 * 1000 * 1000, // 20 MHz
        .mode = 0,
        .spics_io_num = PIN_TFT_CS,
        .queue_size = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &s_spi));

    // ST7735 Initialization sequence
    tft_cmd(0x01); // Software Reset
    vTaskDelay(pdMS_TO_TICKS(120));

    tft_cmd(0x11); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(120));

    tft_cmd(0x3A); // Color Mode
    tft_data_byte(0x05); // 16-bit RGB565

    tft_cmd(0x36); // Memory Access Control
    tft_data_byte(0x68); // Landscape orientation

    tft_cmd(0x21); // Display Inversion ON (INVON)

    tft_cmd(0x29); // Display ON
    vTaskDelay(pdMS_TO_TICKS(50));

    display_set_backlight(true);
    s_last_activity_us = esp_timer_get_time();

    ESP_LOGI(TAG, "ST7735 display initialized on SPI2");
    return ESP_OK;
}
