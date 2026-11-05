# Project Status: Complete

All phases and tasks from `TODO.md` are implemented, compiled, and verified:
- [x] Phase 1: Scaffolding, pin mapping, host-tested NMEA parser.
- [x] Phase 2: Hardware driver (`GPIO 3` VEXT, UART1), FreeRTOS background stream task on Core 1, clock discipline with 5s watchdog.
- [x] Phase 3: Wi-Fi manager with static IP and modem-sleep disabled (`WIFI_PS_NONE`), RFC 5905 NTP server daemon on Core 0.
- [x] Phase 4: ST7735 SPI display driver, dashboard view, 120s auto-dim, and `GPIO 0` wake button.
- [x] Phase 5: Build verification, `deploy/chrony.conf`, and `deploy/test_ntp.sh`.

## Flashing the Hardware
```bash
. $HOME/esp-idf/export.sh
idf.py -p /dev/ttyACM0 flash monitor
```
