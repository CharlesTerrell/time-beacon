# Current Task: Implement ST7735 TFT Display & Power Management

## Objective
Implement `main/display.c` to satisfy the display interface declared in `main/include/display.h`.

## Files in Scope
- `main/display.c` (implement)
- `main/include/display.h` (read-only reference)
- `main/include/font5x7.h` (read-only reference)
- `main/include/board_pins.h` (read-only reference)

## Specific Implementation Rules
1. Required Includes:
   - `"display.h"`
   - `"font5x7.h"`
   - `"board_pins.h"`
   - `"time_sync.h"`
   - `"wifi_manager.h"`
   - `"ntp_server.h"`
   - `"esp_log.h"`
   - `"esp_timer.h"`
   - `"driver/gpio.h"`
   - `"driver/spi_master.h"`
   - `"freertos/FreeRTOS.h"`
   - `"freertos/task.h"`
   - `<string.h>`
   - `<stdio.h>`
   - `<time.h>`

2. Static State & SPI Handles:
   ```c
   static const char *TAG = "display";
   static spi_device_handle_t s_spi = NULL;
   static bool s_backlight_on = true;
   static int64_t s_last_activity_us = 0;
   static uint16_t s_framebuf[TFT_WIDTH * TFT_HEIGHT];
   ```

3. SPI & GPIO Initialization (`display_init`):
   - Configure Backlight `PIN_TFT_BL` and Reset `PIN_TFT_RST` and DC `PIN_TFT_DC` as GPIO outputs.
   - Configure Button `PIN_USER_BTN` as GPIO input with pull-up enabled.
   - Hardware Reset pulse:
     `gpio_set_level(PIN_TFT_RST, 0); vTaskDelay(pdMS_TO_TICKS(20)); gpio_set_level(PIN_TFT_RST, 1); vTaskDelay(pdMS_TO_TICKS(50));`
   - Initialize SPI Bus `SPI2_HOST`:
     ```c
     spi_bus_config_t buscfg = {
         .mosi_io_num = PIN_TFT_SDIN,
         .miso_io_num = -1,
         .sclk_io_num = PIN_TFT_SCLK,
         .quadwp_io_num = -1,
         .quadhd_io_num = -1,
         .max_transfer_sz = TFT_WIDTH * TFT_HEIGHT * 2,
     };
     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
     ```
   - Add Device on SPI:
     ```c
     spi_device_interface_config_t devcfg = {
         .clock_speed_hz = 20 * 1000 * 1000, // 20 MHz
         .mode = 0,
         .spics_io_num = PIN_TFT_CS,
         .queue_size = 1,
     };
     ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &s_spi));
     ```
   - Send ST7735 init commands (0x01 SWRESET, 0x11 SLPOUT, 0x3A COLMOD 0x05 for RGB565, 0x36 MADCTL 0x68 landscape, 0x29 DISPON).
   - Set `display_set_backlight(true)`.
   - Initialize `s_last_activity_us = esp_timer_get_time();`

4. Implement `void display_set_backlight(bool enable)`:
   - `gpio_set_level(PIN_TFT_BL, enable ? 1 : 0);`
   - `s_backlight_on = enable;`

5. Drawing Helpers:
   - Write pixel to `s_framebuf`:
     ```c
     static void draw_pixel(int x, int y, uint16_t color) {
         if (x >= 0 && x < TFT_WIDTH && y >= 0 && y < TFT_HEIGHT) {
             s_framebuf[y * TFT_WIDTH + x] = (color >> 8) | (color << 8); // Swap endian for SPI
         }
     }
     ```
   - Text rendering using `font5x7`:
     ```c
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
     ```
   - Flush `s_framebuf` to panel via window commands (0x2A CASET 1..160, 0x2B RASET 26..105, 0x2C RAMWR) and `spi_device_polling_transmit`.

6. Implement `void display_update(...)`:
   - Clear framebuf to 0x0000 (black).
   - Render lines:
     - (y=4): `draw_string(4, 4, "HELTEC NTP STRATUM 1", 0xFFE0, 0x0000);` (Yellow)
     - (y=18): `IP: <ip>` (White: 0xFFFF)
     - (y=32): `SATS: <sats> [LOCK / WAIT]` (Green 0x07E0 if locked, Red 0xF800 if searching)
     - (y=46): `UTC: <time_str>` (White: 0xFFFF)
     - (y=60): `REQS: <packets>` (Cyan: 0x07FF)
   - Flush framebuf.

7. Implement `display_task_start(void)`:
   - Create task `display_refresh_task` pinned to Core 1 at priority 5 (stack 4096 bytes).
   - Task loop:
     - Check `gpio_get_level(PIN_USER_BTN) == 0`: if pressed, `s_last_activity_us = esp_timer_get_time(); display_set_backlight(true);`
     - Auto-dim check: if `(esp_timer_get_time() - s_last_activity_us) > 120000000LL` (120s), call `display_set_backlight(false);`
     - If backlight is on:
       - Fetch current IP with `wifi_manager_get_ip()`.
       - Fetch GNSS lock with `time_sync_get_status()`.
       - Format UTC string with `strftime()`.
       - Fetch packets with `ntp_server_get_packet_count()`.
       - Call `display_update()`.
     - `vTaskDelay(pdMS_TO_TICKS(1000));`
   - Return `ESP_OK`.

## Verification Command
```bash
idf.py build
```
When this command succeeds with zero errors and zero warnings under `-Wall`, mark task `[x]` below:

- [x] Task Complete: ST7735 display driver implemented and verified by idf.py build.
