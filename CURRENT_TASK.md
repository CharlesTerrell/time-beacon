# Current Task: Implement RFC 5905 NTP Server

## Objective
Implement `main/ntp_server.c` to satisfy the UDP time server interface in `main/include/ntp_server.h`.

## Files in Scope
- `main/ntp_server.c` (implement)
- `main/include/ntp_server.h` (read-only reference)
- `main/include/ntp_packet.h` (read-only reference)
- `main/include/time_sync.h` (read-only reference)
- `main/include/wifi_manager.h` (read-only reference)

## Specific Implementation Rules
1. Required Includes:
   - `"ntp_server.h"`
   - `"ntp_packet.h"`
   - `"time_sync.h"`
   - `"wifi_manager.h"`
   - `"esp_log.h"`
   - `"esp_timer.h"`
   - `"freertos/FreeRTOS.h"`
   - `"freertos/task.h"`
   - `"lwip/sockets.h"`
   - `"lwip/netdb.h"`
   - `<string.h>`
   - `<sys/time.h>`
2. Tag & Statistics:
   ```c
   static const char *TAG = "ntp_server";
   static uint32_t s_packets_served = 0;
   ```
3. UDP Server Task `static void ntp_server_task(void *pvParameters)`:
   - Wait for Wi-Fi connection:
     ```c
     while (!wifi_manager_is_connected()) {
         vTaskDelay(pdMS_TO_TICKS(500));
     }
     ```
   - Create UDP socket on `AF_INET, SOCK_DGRAM, IPPROTO_IP`. Check `< 0`.
   - Set socket receive timeout `SO_RCVTIMEO` to 1000ms.
   - Bind socket to `INADDR_ANY` on `NTP_PORT` (`123`). Check `< 0`.
   - Receive loop:
     - `int len = recvfrom(sock, &req, sizeof(req), 0, (struct sockaddr *)&client_addr, &addr_len);`
     - If `len < (int)sizeof(ntp_packet_t)`, continue.
     - Capture `gettimeofday(&now, NULL)` and convert `now_ntp = timeval_to_ntp(&now);`
     - Check lock status: `bool locked = time_sync_get_status(&gnss_stat);`
     - Build response:
       - Mode/LI: `uint8_t li = locked ? 0 : 3; resp.li_vn_mode = (li << 6) | (4 << 3) | 4;`
       - Stratum: `resp.stratum = locked ? 1 : 16;`
       - Poll: `resp.poll = req.poll;`
       - Precision: `resp.precision = -18;`
       - Root dispersion: `resp.root_dispersion = locked ? htonl(0x00000080) : htonl(0x00010000);`
       - Ref ID: `resp.ref_id = locked ? htonl((uint32_t)NTP_REF_ID_GPS) : htonl((uint32_t)NTP_REF_ID_INIT);`
       - Timestamps:
         - `resp.origin_time = req.transmit_time;` (T1 from client)
         - `resp.receive_time = now_ntp;`          (T2 server receive)
         - `resp.transmit_time = now_ntp;`         (T3 server transmit)
     - `sendto(sock, &resp, sizeof(resp), 0, (struct sockaddr *)&client_addr, addr_len);`
     - Increment `s_packets_served++;`
4. Implement `esp_err_t ntp_server_start(void)`:
   - Call `xTaskCreatePinnedToCore(ntp_server_task, "ntp_server", 4096, NULL, 15, NULL, 0);`
   - Pinned to Core 0 at priority 15.
   - Return `ret == pdPASS ? ESP_OK : ESP_FAIL`.
5. Implement `uint32_t ntp_server_get_packet_count(void)`:
   - Return `s_packets_served`.

## Verification Command
```bash
idf.py build
```
When this command succeeds with zero errors and zero warnings under `-Wall`, mark task `[x]` below:

- [x] Task Complete: RFC 5905 NTP server implemented and verified by idf.py build.
