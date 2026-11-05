# Current Task: Implement Time Synchronization Discipline

## Objective
Implement `main/time_sync.c` to satisfy the state and clock discipline interface in `main/include/time_sync.h`.

## Files in Scope
- `main/time_sync.c` (implement)
- `main/include/time_sync.h` (read-only reference)
- `CURRENT_TASK.md` (mark [x] when verified)

## Specific Implementation Rules
1. Required Includes:
   - `"time_sync.h"`
   - `"esp_log.h"`
   - `"esp_timer.h"`
   - `"freertos/FreeRTOS.h"`
   - `"freertos/semphr.h"`
   - `<sys/time.h>`
   - `<time.h>`
   - `<string.h>`
2. Static State & Mutex:
   ```c
   static const char *TAG = "time_sync";
   static SemaphoreHandle_t s_sync_mutex = NULL;
   static gnss_status_t s_gnss_status = {0};
   ```
3. Implement `esp_err_t time_sync_init(void)`:
   - Create mutex: `s_sync_mutex = xSemaphoreCreateMutex();`
   - Return `s_sync_mutex != NULL ? ESP_OK : ESP_ERR_NO_MEM`.
4. Implement `void time_sync_update_gnss(const nmea_time_t *time_data, int64_t anchor_esp_time_us)`:
   - If `time_data == NULL || !time_data->fix_valid`, return early.
   - Populate `struct tm`:
     - `tm.tm_year = time_data->year - 1900;`
     - `tm.tm_mon = time_data->month - 1;`
     - `tm.tm_mday = time_data->day;`
     - `tm.tm_hour = time_data->hour;`
     - `tm.tm_min = time_data->minute;`
     - `tm.tm_sec = time_data->second;`
     - `tm.tm_isdst = 0;`
   - Compute epoch seconds: `time_t epoch_sec = timegm(&tm);`
   - Compute elapsed microseconds since anchor:
     ```c
     int64_t now_us = esp_timer_get_time();
     int64_t elapsed_us = now_us - anchor_esp_time_us;
     if (elapsed_us < 0) elapsed_us = 0;
     ```
   - Update system clock via POSIX `settimeofday()`:
     ```c
     struct timeval tv;
     tv.tv_sec = epoch_sec + (time_t)(elapsed_us / 1000000LL);
     tv.tv_usec = (suseconds_t)((time_data->microseconds + (elapsed_us % 1000000LL)) % 1000000LL);
     settimeofday(&tv, NULL);
     ```
   - Protect with `xSemaphoreTake(s_sync_mutex, portMAX_DELAY)` / `xSemaphoreGive(s_sync_mutex)`:
     - `s_gnss_status.fix_valid = true;`
     - `s_gnss_status.last_fix_time = *time_data;`
     - `s_gnss_status.last_fix_esp_time_us = now_us;`
     - `s_gnss_status.sentences_parsed++;`
5. Implement `void time_sync_record_error(void)`:
   - Acquire mutex, increment `s_gnss_status.checksum_errors++`, release mutex.
6. Implement `bool time_sync_get_status(gnss_status_t *out_status)`:
   - Return false if `out_status == NULL`.
   - Under mutex, copy `*out_status = s_gnss_status;`
   - Lock timeout check: fix is valid ONLY IF `s_gnss_status.fix_valid` is true AND `s_gnss_status.last_fix_esp_time_us > 0` AND `(esp_timer_get_time() - s_gnss_status.last_fix_esp_time_us) < 5000000LL` (5 seconds).
   - Return true if active fix valid within 5 seconds, false otherwise.
7. Implement `bool time_sync_is_synchronized(void)`:
   - Call `time_sync_get_status(&dummy)` and return its return value.

## Verification Command
```bash
idf.py build
```
When this command succeeds with zero errors and zero warnings under `-Wall`, mark task `[x]` below:

- [x] Task Complete: Time synchronization discipline implemented and verified by idf.py build.
