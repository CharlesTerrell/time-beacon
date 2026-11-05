# Current Task: Implement GNSS FreeRTOS Streaming Task

## Objective
Implement `main/gnss_task.c` to satisfy the background parser task interface in `main/include/gnss_task.h`.

## Files in Scope
- `main/gnss_task.c` (implement)
- `main/include/gnss_task.h` (read-only reference)
- `main/include/gnss_driver.h` (read-only reference)
- `main/include/nmea_parser.h` (read-only reference)
- `main/include/time_sync.h` (read-only reference)
- `CURRENT_TASK.md` (mark [x] when verified)

## Specific Implementation Rules
1. Required Includes:
   - `"gnss_task.h"`
   - `"gnss_driver.h"`
   - `"nmea_parser.h"`
   - `"time_sync.h"`
   - `"esp_log.h"`
   - `"esp_timer.h"`
   - `"freertos/FreeRTOS.h"`
   - `"freertos/task.h"`
   - `<string.h>`
2. Logging:
   ```c
   static const char *TAG = "gnss_task";
   ```
3. Background Task `static void gnss_stream_task(void *pvParameters)`:
   - Allocate `char line_buf[128];` and `size_t idx = 0;` and `int64_t anchor_us = 0;`
   - In infinite loop:
     - Read 1 byte using `gnss_driver_read(&byte, 1, 100);`
     - If read returns `<= 0`, continue.
     - If `byte == '$'`:
       - Record timestamp: `anchor_us = esp_timer_get_time();`
       - Reset line index: `idx = 0;`
       - Store `line_buf[idx++] = '$';`
     - Else if `idx > 0`:
       - If `byte == '\r' || byte == '\n'`:
         - Null-terminate: `line_buf[idx] = '\0';`
         - Verify checksum:
           ```c
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
           ```
         - Reset `idx = 0;`
       - Else:
         - If `idx < sizeof(line_buf) - 1`, append `line_buf[idx++] = (char)byte;`
         - Else buffer overflow: reset `idx = 0;`
4. Implement `esp_err_t gnss_task_start(void)`:
   - Call `xTaskCreatePinnedToCore`:
     - Task func: `gnss_stream_task`
     - Name: `"gnss_stream"`
     - Stack size: `4096`
     - Param: `NULL`
     - Priority: `12`
     - Task handle: `NULL`
     - Core ID: `1` (run on Core 1)
   - Return `BaseType_t ret == pdPASS ? ESP_OK : ESP_FAIL`.

## Verification Command
```bash
idf.py build
```
When this command succeeds with zero errors and zero warnings under `-Wall`, mark task `[x]` below:

- [x] Task Complete: GNSS background streaming task implemented and verified by idf.py build.
