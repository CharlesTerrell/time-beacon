# Time Beacon: Multi-Protocol Stratum-1 Time Server

[![CI](https://github.com/tachyon/time_beacon/actions/workflows/ci.yml/badge.svg)](https://github.com/tachyon/time_beacon/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

Time Beacon is a self-contained, battery- or USB-powered **Stratum-1 Reference Clock** built for ESP32-S3. Disciplined by an onboard GNSS/GPS receiver and Pulse-Per-Second (PPS) interrupt, it simultaneously broadcasts precision time across three wireless protocols:

1. **Wi-Fi NTP Server**: High-throughput RFC 5905 NTPv4 / RFC 4330 SNTP server on UDP port 123 (Station mode with DHCP or Static IP, modem sleep, and dynamic RF power scaling to prevent USB brownouts).
2. **Bluetooth Low Energy (BLE)**: Standard SIG Current Time Service (CTS `0x1805` / `0x2A2B`) with sub-second (1/256s) precision, standard Battery Service (BAS `0x180F`), Device Information Service (DIS `0x180A`), and custom 4-byte power telemetry (`0xFFE0`).
3. **Meshtastic LoRa Beacons**: Alternating 2-phase over-the-air time sync (even minutes = LongFast at 906.875 MHz; odd minutes = MediumFast at 913.125 MHz). Broadcasts a 3-packet suite (`POSITION_APP` with privacy fuzzing, `NODEINFO_APP`, and `TELEMETRY_APP` with 101% USB encoding), enabling off-grid nodes to sync clocks without GPS or Internet.
4. **USB MSC Configuration Drive**: Plug into any computer and hold the `USER` button during boot to edit `config.ini` directly on an internal FATFS flash drive without serial drivers or special software.

---

## Supported Hardware

- **Heltec Wireless Tracker v1.1**: ESP32-S3FN8, UC6580 GNSS, SX1262 LoRa, ST7735 0.96" SPI TFT (80x160), NMEA rollover time discipline (unrouted PCB PPS).
- **Heltec Wireless Tracker v2**: Adds active GNSS 1PPS GPIO interrupt discipline and RF Front-End Module (FEM PA).
- **Heltec WiFi LoRa 32 V4** *(Experimental)*: ESP32-S3, SSD1306 0.96" I2C OLED (128x64), SX1262 LoRa with FEM PA, external GNSS connector with 1PPS interrupt discipline (selected via `idf.py menuconfig`).

---

## Quick Start

### 1. Prerequisites
- Linux / macOS development environment
- ESP-IDF **v5.x** or **v6.x** installed
- Standard tools: Python 3, `git`, `cmake`, `ninja`

### 2. Activate Build Environment
Source the helper script to locate and activate ESP-IDF in your current shell:
```bash
source tools/idf_env.sh
```

### 3. Build Firmware
Build both hardware targets (or specify `v1.1` or `v2`):
```bash
./build.sh all
```
Or build an individual target:
```bash
./build.sh v1.1
./build.sh v2
```

### 4. Run Host Unit Tests
The project includes a native x86 simulation test suite verifying NMEA parsing, time discipline, INI tokenization, Wi-Fi dynamic power scaling, and packet encoders:
```bash
./build.sh test
# or directly:
./tests/run_tests.sh
```

### 5. Memory & Footprint Audit
Check exact flash, DRAM, and IRAM allocations for built targets:
```bash
./build.sh size v1.1
./build.sh size v2
# With detailed per-component breakdown:
./build.sh size v1.1 components
```

### 6. Flash to Device
Connect the board via USB-C and flash:
```bash
./build.sh flash v1.1
```
To monitor serial logs:
```bash
tio -b 115200 /dev/ttyACM0
# or
idf.py -B build_v1 monitor
```

---

## Configuration (`config.ini`)

To configure the device, hold the **USER / PRG** button during the initial 2.5-second boot prompt. The device mounts as a standard USB flash drive. Edit `config.ini` in any text editor, eject or press PRG, and it reboots with new settings:

```ini
[device]
node_name = TimeBeacon        ; Advertised BLE and Meshtastic name
position_precision = 18       ; Masked position bits (18 ~= 1.5km fuzz, 32 = exact)

[wifi]
enable = true                 ; Enable Wi-Fi client connection
ssid = MyNetwork              ; Wi-Fi network SSID
password = SecretKey          ; Wi-Fi WPA2/WPA3 passphrase
use_static_ip = false         ; Use static IP configuration
static_ip = 192.168.1.200     ; Static IP address
static_gw = 192.168.1.1       ; Default gateway
static_netmask = 255.255.255.0; Subnet mask
static_dns = 1.1.1.1          ; Primary DNS server
country_code = US             ; 2-letter regulatory country code

[ntp]
enable = true                 ; Enable Stratum-1 UDP NTP time server

[ble]
enable = true                 ; Enable Bluetooth Low Energy Current Time Service
enable_battery = true         ; Expose standard BLE Battery Service (BAS)

[meshtastic]
enable = true                 ; Enable LoRa 2-phase Meshtastic time beacons
freq_hz = 906875000           ; Base frequency (906.875 MHz US915 LongFast)

[display]
flip = false                  ; Invert display 180 degrees
led_peak_percent = 50         ; Peak brightness of status LED breathing pulse (0-100%)

[debug]
usb_nmea_output = false       ; Stream raw NMEA sentences over USB Serial/JTAG
```

### Runtime Gestures
- **Short Press when Screen Sleeping**: Wakes display backlight.
- **Short Press when Screen Awake**: Inverts display orientation 180° and saves state.

---

## Architecture & Hardware Safeguards

### Power Management & Brownout Protection
- **Brownout Safeguards**: Hardware brownout detector reset disabled in Kconfig (`# CONFIG_ESP_BROWNOUT_DET is not set`) and early runtime (`esp_brownout_disable()`) to prevent boot loops on weak 500 mA USB ports and charging transients. Wi-Fi PHY calibration data cached in NVS.
- **Dynamic Wi-Fi TX Scaling**:
  - Depleted battery (< 3.65V): Throttled to **13 dBm** (~20 mW) to leave full current for TP4054 charger.
  - Actively charging on USB (< 4.10V): Capped to **15 dBm** (~31 mW) to avoid 5V rail sag on 500 mA USB ports.
  - Pure USB (no battery) or fully charged (≥ 4.10V): Full **18 dBm** (~63 mW) for maximum range.
- **Gated Battery Divider**: `PIN_VBAT_CTRL` active HIGH gate eliminates parasitic drain; 8-sample variance filter detects open battery ports.
- **Multi-Tier Sleep Protection**:
  - Early boot guard puts device into 60s deep sleep if battery is ≤ 3.25V without USB.
  - Runtime 30-second shutdown countdown triggers if battery drops to ≤ 3.35V, with 3.50V bounce recovery hysteresis latch.
  - Plugging in USB cancels countdown immediately; unplugging re-arms countdown.
- **Status LED PWM**: Breathing pulse during screen sleep (0–100% configurable peak duty), 100 ms flash on critical battery.

### Resource Footprint (ESP32-S3)
- **Flash Storage**: **1.16 MB** (~1,185 KB binary) fits within 3.0 MB app partition (**38.6% used**).
- **Static RAM**: **~187 KB** total (68 KB DRAM + 117 KB IRAM).
- **Runtime Free Heap**: **~325 KB free** out of 512 KB internal SRAM for network sockets and buffers.
- **Static Allocation**: Zero `malloc` calls in runtime loops, packet engines, or ISRs.

---

## Subsystem Architecture Documentation

Detailed design specifications and wire formats are located in [`docs/plan/`](docs/plan/):
- [Power Management & Battery Telemetry](docs/plan/power_management.md)
- [Boot Selector & Operating Modes](docs/plan/boot_selector.md)
- [Meshtastic LoRa Beacons](docs/plan/meshtastic_beacons.md)
- [Wi-Fi Station & NTP Server](docs/plan/wifi_ntp.md)
- [GNSS Time & Position Subsystem](docs/plan/gps_time_position.md)
- [Display & UI Management](docs/plan/display_management.md)
- [BLE Current Time Service (CTS)](docs/plan/ble_cts.md)
- [MeshCore Architecture Specification](docs/plan/meshcore_integration_spec.md)
- [Multi-Architecture Porting Roadmaps](docs/plan/porting/README.md)

---

## License

[MIT License](LICENSE)
