#include "display.h"
#include "font5x7.h"
#include "board_pins.h"
#include "time_sync.h"
#include "wifi_manager.h"
#include "ntp_server.h"
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

static void display_set_backlight(bool enable) {
    gpio_set_level(PIN_TFT_BL, enable ? 1 : 0);
    s_backlight_on = enable;
}

static void display_flush(void) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = TFT_WIDTH * TFT_HEIGHT * 16;
    t.tx_buffer = s_framebuf;
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi, &t));
}

void display_update(const char *ip, uint8_t sats, bool locked, const char *utc_time, uint32_t packets) {
    memset(s_framebuf, 0, sizeof(s_framebuf));
    draw_string(4, 4, "HELTEC NTP STRATUM 1", 0xFFE0, 0x0000); // Yellow
    char ip_str[16];
    snprintf(ip_str, sizeof(ip_str), "IP: %s", ip);
    draw_string(4, 18, ip_str, 0xFFFF, 0x0000); // White
    char sats_str[16];
    snprintf(sats_str, sizeof(sats_str), "SATS: %d [%s]", sats, locked ? "LOCK" : "WAIT");
    draw_string(4, 32, sats_str, locked ? 0x07E0 : 0xF800, 0x0000); // Green if locked, Red if searching
    draw_string(4, 46, utc_time, 0xFFFF, 0x0000); // White
    char packets_str[16];
    snprintf(packets_str, sizeof(packets_str), "REQS: %u", packets);
    draw_string(4, 60, packets_str, 0x07FF, 0x0000); // Cyan
    display_flush();
}

static void display_refresh_task(void *pvParameters) {
    while (1) {
        if (gpio_get_level(PIN_USER_BTN) == 0) {
            s_last_activity_us = esp_timer_get_time();
            display_set_backlight(true);
        }
        if ((esp_timer_get_time() - s_last_activity_us) > 120000000LL) {
            display_set_backlight(false);
        }
        if (s_backlight_on) {
            char ip[16];
            if (wifi_manager_get_ip(ip, sizeof(ip)) == ESP_OK) {
                gnss_status_t status;
                if (time_sync_get_status(&status)) {
                    char utc_time[20];
                    strftime(utc_time, sizeof(utc_time), "%Y-%m-%d %H:%M:%S", &status.last_fix_time);
                    uint32_t packets = ntp_server_get_packet_count();
                    display_update(ip, status.satellites, status.fix_valid, utc_time, packets);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

esp_err_t display_task_start(void) {
    return xTaskCreatePinnedToCore(display_refresh_task, "display_refresh_task", 4096, NULL, 5, NULL, 1) == pdPASS ? ESP_OK : ESP_FAIL;
}

esp_err_t display_init(void) {
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

    // Send ST7735 init commands
    uint8_t init_cmds[] = {
        0x01, 0x00, // SWRESET
        0x11, 0x80, // SLPOUT
        0x3A, 0x05, // COLMOD 0x05 for RGB565
        0x36, 0x68, // MADCTL 0x68 landscape
        0x29, 0x80, // DISPON
    };
    for (int i = 0; i < sizeof(init_cmds); i += 2) {
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 16;
        t.tx_buffer = &init_cmds[i];
        ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi, &t));
    }

    display_set_backlight(true);
    s_last_activity_us = esp_timer_get_time();

    return ESP_OK;
}
