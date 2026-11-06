# Task: Implement Virtual USB Flash Drive (MSC) Mode, DHCP, and Wi-Fi Regulatory Country

## Task Goal
1. Support Virtual USB Flash Drive (MSC) on native USB-C port via TinyUSB and Wear-Levelled FATFS (`storage` partition).
2. Holding `USER` button (`GPIO 0`) on boot enters USB Config Mode, mounting `/storage` as a USB flash drive to edit `config.ini`.
3. Support Wi-Fi regulatory country code (`country = US/EU/JP/CN`) for channel adaptation via `esp_wifi_set_country_code()`.
4. Support DHCP (`dhcp = true`) or static IP (`dhcp = false` with IP, gateway, netmask, DNS) in `config.ini`.
5. Display visual indicator on ST7735 LCD when in USB Config Mode.
6. Normal boot reads `config.ini` from `/storage` (or creates default if missing) and launches Stratum-1 NTP server.

## Allowed Files to Edit
- `CURRENT_TASK.md`
- `partitions.csv`
- `sdkconfig.defaults`
- `.gitignore`
- `main/idf_component.yml`
- `main/CMakeLists.txt`
- `main/include/config_mgr.h`
- `main/config_mgr.c`
- `main/include/usb_msc.h`
- `main/usb_msc.c`
- `main/include/wifi_manager.h`
- `main/wifi_manager.c`
- `main/include/display.h`
- `main/display.c`
- `main/main.c`

## Verification Command
```bash
. $HOME/esp-idf/export.sh && idf.py build
```

## Checklist
- [x] Create `partitions.csv` with custom FATFS `storage` partition.
- [x] Configure `sdkconfig.defaults` for custom partition table and TinyUSB.
- [x] Add `main/idf_component.yml` for `espressif/esp_tinyusb`.
- [x] Implement `main/include/config_mgr.h` and `main/config_mgr.c` (INI parser and defaults generator).
- [x] Implement `main/include/usb_msc.h` and `main/usb_msc.c` (TinyUSB MSC storage driver).
- [x] Update `main/include/wifi_manager.h` and `main/wifi_manager.c` with DHCP and country code support.
- [x] Update `main/include/display.h` and `main/display.c` with config mode dashboard.
- [x] Update `main/main.c` with boot mode selection (button held = USB drive, else normal NTP).
- [x] Run verification command with zero errors and zero warnings.
