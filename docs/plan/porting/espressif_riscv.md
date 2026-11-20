# Future Scenario: Espressif RISC-V Chips (e.g. ESP32-C3 / C6 / P4)

> [!NOTE]
> **Status: Future Architectural Roadmap**
> Production firmware is implemented in pure ESP-IDF (v5.x/v6.x) for ESP32-S3. This document captures an analysis for migrating to Espressif's RISC-V microcontrollers if comparable GNSS + Wi-Fi boards become available.

## 1. Executive Summary

If a comparable GNSS and Wi-Fi enabled hardware platform is released using Espressif's newer RISC-V microcontrollers, porting effort is minimal (**~5% of the codebase**) compared to a non-Espressif migration. Because both Xtensa and RISC-V families share the official ESP-IDF software stack, the core protocol engines require zero rewrites.

---

## 2. Architecture-Independent Core (95%+ Unchanged)

Because the firmware is written against standard ESP-IDF HAL abstractions rather than raw Xtensa assembly or chip registers, the vast majority of subsystems compile and run without modification:
* **GNSS Engine & Parser**: `gnss_driver.c`, `gnss_task.c`, `nmea_parser.c` (standard UART driver).
* **NTP Server & Sockets**: `ntp_server.c`, `wifi_manager.c` (LwIP stack, FreeRTOS tasks, BSD sockets).
* **LoRa Beacons & Display**: `sx1262.c`, `display.c` (standard ESP-IDF SPI Master and I2C drivers).
* **Bluetooth Time Broadcast**: `ble_cts_service.c` (Apache NimBLE runs identically across Xtensa and RISC-V targets in ESP-IDF).
* **Time Discipline & Clocks**: `time_sync.c` (`esp_timer_get_time()` and 64-bit monotonic microsecond timers).

---

## 3. Required Adaptations (~5%)

### 1. Board Pin Mapping (`main/boards/board_<chip>.h`)
Create a dedicated pinout header defining GPIOs for GNSS UART, SX1262 SPI, display I2C/SPI, battery ADC divider gate, and status LED to match the new silicon package.

### 2. ADC Calibration Scheme (`main/power_mgr.c`)
- ESP32-S3 uses factory eFuse curve-fitting (`adc_cali_create_scheme_curve_fitting`).
- Single-core RISC-V chips (such as ESP32-C3 and ESP32-C6) use line-fitting calibration (`adc_cali_create_scheme_line_fitting`).
- Handled cleanly via conditional compilation:
  ```c
  #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
      // Curve fitting (ESP32-S3, etc.)
  #elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
      // Line fitting (ESP32-C3, ESP32-C6, etc.)
  #endif
  ```

### 3. USB Configuration Drive Peripheral Constraints
- **Chips with USB-OTG (e.g. ESP32-P4 or future S-series RISC-V)**: TinyUSB MSC (`usb_msc.c`) operates identically, mounting the internal FATFS partition as a mass storage drive over USB-C.
- **Chips with USB Serial/JTAG only (e.g. ESP32-C3, ESP32-C6, ESP32-C5)**: Lack hardware USB OTG peripheral support and cannot natively mount as a USB flash drive. Configuration storage (`config.ini`) would adapt to:
  - BLE GATT configuration characteristic.
  - Embedded Wi-Fi captive web portal.
  - USB Serial interactive CLI menu.

### 4. Build System & Toolchain
- ESP-IDF handles toolchain switching automatically (`riscv32-esp-elf-gcc`).
- Add target sdkconfig defaults (`sdkconfig.<target>`) and target selection in `build.sh` (`idf.py set-target <target>`).
