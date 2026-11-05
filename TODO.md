# Roadmap & Task Backlog

## Phase 1: Project Scaffolding & Pin Definitions
- [x] Task 1.1: Root & Component CMake, `sdkconfig.defaults`, `board_pins.h`.
- [x] Task 1.2: Host-testable NMEA sentence parser (`$GNRMC`, `$GNZDA`).
- [x] Task 1.3: Host test runner with test vectors (`tests/test_nmea.c`).

## Phase 2: Hardware Driver & FreeRTOS Time Sync
- [x] Task 2.1: GNSS power control (`GPIO 3` VEXT) & UART1 init (`GPIO 33` RX, `GPIO 34` TX).
- [ ] Task 2.2: Time synchronization discipline & state (`main/time_sync.c`).
- [ ] Task 2.3: FreeRTOS GNSS UART streaming task (`main/gnss_task.c`).

## Phase 3: Wi-Fi & RFC 5905 NTP Server
- [ ] Task 3.1: Wi-Fi manager with static IP and modem-sleep disabled (`esp_wifi_set_ps(WIFI_PS_NONE)`).
- [ ] Task 3.2: RFC 5905 packed NTPv4 struct and packet encoder in `main/include/ntp_packet.h`.
- [ ] Task 3.3: UDP socket server on port 123 (non-blocking, high priority task on Core 0).

## Phase 4: TFT Status Display & Power Management
- [ ] Task 4.1: ST7735 SPI display driver init (`GPIO 38-42`, Backlight `GPIO 21`).
- [ ] Task 4.2: Dashboard view (IP, satellites, sync status, UTC time, packet count).
- [ ] Task 4.3: 120s inactivity screen dim / button wake on `GPIO 0`.

## Phase 5: Verification & Host Integration
- [ ] Task 5.1: Clean build check `idf.py build`.
- [ ] Task 5.2: Generate `deploy/chrony.conf` for Proxmox.
- [ ] Task 5.3: Generate `deploy/test_ntp.sh` test script.
