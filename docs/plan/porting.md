# Multi-Architecture PlatformIO Porting Plan: Heltec Tracker (ESP32-S3) & Mesh Node T096 (nRF52840)

## 1. Executive Summary & Objective

Migrate the Stratum-1 GNSS Time Server and LoRa Beacon project from a pure ESP-IDF environment to a multi-architecture **PlatformIO** project supporting:
1. **ESP32-S3 Targets**: Heltec Wireless Tracker v1.1, Tracker v2, and WiFi LoRa 32 V4.
2. **nRF52840 Targets**: Heltec Mesh Node T096 (HT-n5262G).

The goal is to maintain a single codebase sharing core synchronization and protocol logic while abstracting peripherals, radio, and transport layers without runtime performance penalty.

---

## 2. Technical Roadmap: What Needs to Be Done

### Step 1: PlatformIO Directory Structure & Configuration
Create a unified `platformio.ini` declaring distinct target environments:

```ini
[platformio]
default_envs = tracker_v1_1, tracker_v2, mesh_node_t096

[common]
build_flags = 
    -std=gnu17
    -Wall -Wextra
    -DCORE_TIME_SERVER

[env:tracker_v1_1]
platform = espressif32 @ ^6.5.0
board = esp32-s3-devkitc-1
framework = espidf
board_build.partitions = partitions.csv
build_flags = ${common.build_flags} -DTARGET_TRACKER_V1_1

[env:tracker_v2]
platform = espressif32 @ ^6.5.0
board = esp32-s3-devkitc-1
framework = espidf
board_build.partitions = partitions.csv
build_flags = ${common.build_flags} -DTARGET_TRACKER_V2

[env:wifi_lora_v4]
platform = espressif32 @ ^6.5.0
board = esp32-s3-devkitc-1
framework = espidf
build_flags = ${common.build_flags} -DTARGET_WIFI_LORA_V4

[env:mesh_node_t096]
platform = nordicnrf52 @ ^10.4.0
board = generic_nrf52840
framework = zephyr   ; (Or arduino with Adafruit/Heltec nRF52 core)
build_flags = ${common.build_flags} -DTARGET_MESH_NODE_T096
```

### Step 2: Define the Hardware Abstraction Layer (HAL)
Isolate all silicon-specific drivers behind clean, compile-time C APIs (`include/hal/`):

1. **`hal_gpio.h`**:
   - `hal_gpio_init_input(pin, pull_up)`, `hal_gpio_init_output(pin, initial_state)`
   - `hal_gpio_set(pin, level)`, `hal_gpio_get(pin)`
   - `hal_gpio_attach_irq(pin, edge, isr_fn, arg)` (Used for sub-microsecond PPS interrupt).
2. **`hal_spi.h`**:
   - Polled & DMA SPI bus initialization, device registration, and bidirectional block transfers for SX1262 LoRa and ST7735 LCD.
3. **`hal_uart.h`**:
   - GNSS serial port configuration (baud rate, pins, ring buffer, interrupt reception).
4. **`hal_timer.h`**:
   - High-resolution monotonic microsecond timestamping: `hal_get_time_us()`.
   - On ESP32: backed by `esp_timer_get_time()`.
   - On nRF52: backed by `nrfx_timer` / RTC counter.
5. **`hal_crypto.h`**:
   - AES-128 CTR mode encryption (for Meshtastic packet payloads).
   - On ESP32: backed by hardware cryptographic accelerator (`esp_aes`).
   - On nRF52: backed by ARM CryptoCell-310 or mbedTLS software fallback.
6. **`hal_ble.h`**:
   - Generic GAP advertisement, Current Time Service (CTS 0x1805), Battery Service (BAS 0x180F), and custom telemetry notifications.

### Step 3: Separate Architecture-Independent Core Logic
Move ~75% of the codebase into a pure C core library (`src/core/`):
* **`gnss_parser.c`**: NMEA-0183 & Unicore binary sentence parsing and satellite tracking state.
* **`time_discipline.c`**: PPS jitter filtering, software PLL, leap-second management, and stratum-1 timestamp discipline.
* **`lora_beacons.c`**: Protobuf encoding for Meshtastic `POSITION_APP` / `TELEMETRY_APP` and MeshCore flood frame generation.
* **`ini_config.c`**: INI token parsing and configuration schema validation.
* **`display_ui.c`**: Framebuffer graphics primitives, 5x7 font rendering, and dashboard status layout.

### Step 4: Implement Architecture Drivers

#### For ESP32-S3 Backend (`src/hal/esp32/`):
* Map HAL calls to ESP-IDF v5/v6 drivers (`driver/gpio`, `driver/spi_master`, `driver/uart`).
* Use native Apache NimBLE port (`nimble/nimble_port.h`).
* Retain LwIP UDP socket for NTP time server.
* Retain ESP TinyUSB MSC stack for FATFS flash-drive configuration.

#### For nRF52840 Backend (`src/hal/nrf52/`):
* Map HAL calls to Nordic nrfx or Zephyr device tree bindings.
* BLE: Implement CTS/BAS using Nordic SoftDevice (if using Arduino-nRF52) or Zephyr Bluetooth LE subsystem.
* Display: Connect ST7735 driver to nRF SPI (SPIM3 @ 32 MHz).
* Power: Implement nRF low-power System-ON / System-OFF sleep states.
* Config Storage: Map INI storage to LittleFS on internal 1 MB flash or QSPI flash, exposed via TinyUSB MSC or BLE config characteristic.

---

## 3. Maintainability Consequences

### Pros:
1. **Shared Protocol & Algorithm Updates**:
   - Improvements to PPS synchronization, leap-second handling, or Meshtastic/MeshCore beacon specifications instantly apply to both platforms.
2. **Unified Toolchain**:
   - PlatformIO coordinates toolchains (`xtensa-esp32s3-elf-gcc` and `arm-none-eabi-gcc`) from a single command line or IDE.

### Cons & Overhead:
1. **Dual-Framework Complexity**:
   - You must maintain two framework lifecycles: ESP-IDF updates on one side, and Nordic SDK / Zephyr updates on the other. Framework API breaks on either side require HAL maintenance.
2. **Double Testing Matrix**:
   - Pull requests must be built and verified against both Xtensa and ARM toolchains. Hardware verification requires physical units of both ESP32-S3 and nRF52840.
3. **Bluetooth Stack Fragmentation**:
   - NimBLE (ESP-IDF) and Nordic SoftDevice / Zephyr BLE have different connection lifecycle paradigms and memory models. BLE code will require distinct platform glue.

---

## 4. End-User Interaction Consequences

### 1. Feature Disparity (The "No Wi-Fi" Factor)
* **ESP32-S3 Devices (Tracker v1.1/v2, WiFi LoRa 32 V4)**:
  * Full-featured network device: Acts as a **Local Network Wi-Fi NTP Server** + **BLE CTS Time Provider** + **LoRa Time Beacon**.
* **nRF52840 Device (Mesh Node T096)**:
  * **No Wi-Fi capability** (hardware does not have a 2.4 GHz 802.11 physical radio).
  * Serves time **only** over LoRa beacons, Bluetooth (BLE CTS), and direct USB connection (serial NMEA / PPS).
  * Users expecting a LAN NTP server will find the nRF52 variant unsuitable.

### 2. Battery Life & Field Deployment
* **ESP32-S3**:
  * High idle consumption (~40–80 mA with Wi-Fi/display active).
  * Requires larger battery (1000–3000 mAh) or continuous solar/USB power.
  * Suited for rooftop, base station, or desktop network time server deployments.
* **nRF52840 (T096)**:
  * Ultra-low power consumption (< 20 µA deep sleep, ~7 mA operating).
  * Operates for days or weeks on a compact 500 mAh LiPo battery.
  * Suited for remote, battery-constrained, off-grid mesh repeaters and portable field time beacons.

### 3. Firmware Flashing & Configuration Experience
* **Flashing**:
  * ESP32 requires `esptool.py` over USB-CDC/JTAG (holding BOOT button on certain revisions).
  * nRF52840 supports drag-and-drop UF2 bootloader flashing (appears as a flash drive to drop `.uf2` files) or `nrfutil`.
* **Configuration (config.ini)**:
  * ESP32 presents an emulated USB flash drive (TinyUSB MSC) on user button press.
  * nRF52840 can either mirror this via TinyUSB MSC or provide instant configuration via a mobile phone app using Bluetooth Low Energy (BLE GATT).
