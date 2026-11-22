# Time Beacon: Multi-Protocol Stratum-1 Time Server

[![CI](https://github.com/tachyon/time_beacon/actions/workflows/ci.yml/badge.svg)](https://github.com/tachyon/time_beacon/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

Time Beacon is an ESP32-S3 battery- or USB-powered **Stratum-1 Reference Clock**. Disciplined by GNSS/GPS and a Pulse-Per-Second (PPS) interrupt, it broadcasts precision time over three wireless protocols:

1. **Wi-Fi NTP Server**: RFC 5905 NTPv4 / RFC 4330 SNTP server on UDP port 123 (DHCP or static IP, modem sleep, dynamic TX power scaling).
2. **Bluetooth Low Energy (BLE)**: SIG Current Time Service (CTS `0x1805` / `0x2A2B`, 1/256s precision), Battery Service (BAS `0x180F`), Device Information Service (DIS `0x180A`), and 4-byte power telemetry (`0xFFE0`).
3. **Meshtastic LoRa Beacons**: Alternating 2-phase time sync (even minutes: LongFast at 906.875 MHz; odd minutes: MediumFast at 913.125 MHz). Broadcasts a 3-packet suite (`POSITION_APP` with privacy fuzzing, `NODEINFO_APP`, and `TELEMETRY_APP`), letting off-grid nodes sync clocks without GPS or internet.
4. **USB MSC Configuration**: Hold `USER` at boot to mount internal FATFS storage and edit `config.ini` directly without serial drivers.

---

## Supported Hardware

- **Heltec Wireless Tracker v1.1**: ESP32-S3FN8, UC6580 GNSS, SX1262 LoRa, ST7735 0.96" SPI TFT (80x160), NMEA rollover discipline (unrouted PCB PPS).
- **Heltec Wireless Tracker v2**: Adds active GNSS 1PPS GPIO interrupt discipline and RF Front-End Module (FEM PA).
- **Heltec WiFi LoRa 32 V4** *(Experimental)*: ESP32-S3, SSD1306 0.96" I2C OLED (128x64), SX1262 LoRa with FEM PA, external GNSS header with 1PPS interrupt (select in `idf.py menuconfig`).

---

## Quick Start

### 1. Prerequisites
- Linux / macOS development environment
- ESP-IDF **v5.x** or **v6.x**
- Standard tools: Python 3, `git`, `cmake`, `ninja`

### 2. Activate Build Environment
Source the helper script to activate ESP-IDF:
```bash
source tools/idf_env.sh
```

### 3. Build Firmware
Build both targets or a single target:
```bash
./build.sh all
# or individual target:
./build.sh v1.1
./build.sh v2
```

### 4. Run Host Unit Tests
Run native host tests (NMEA parsing, time discipline, INI parsing, Wi-Fi power scaling, packet encoders):
```bash
./build.sh test
# or directly:
./tests/run_tests.sh
```

### 5. Memory & Footprint Audit
Audit flash, DRAM, and IRAM usage:
```bash
./build.sh size v1.1
./build.sh size v2
# With component breakdown:
./build.sh size v1.1 components
```

### 6. Flash to Device
Flash over USB-C:
```bash
./build.sh flash v1.1
```
Monitor serial output:
```bash
tio -b 115200 /dev/ttyACM0
# or
idf.py -B build_v1 monitor
```

---

## Configuration (`config.ini`)

Hold **USER / PRG** during the 2.5-second boot prompt to mount the device as a USB drive. Edit `config.ini`, then eject or press PRG to reboot with updated settings:

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
enable = false                ; Enable LoRa 2-phase Meshtastic time beacons
freq_hz = 906875000           ; Base frequency (906.875 MHz US915 LongFast)

[display]
flip = false                  ; Invert display 180 degrees
led_peak_percent = 50         ; Peak brightness of status LED breathing pulse (0-100%)

[debug]
usb_nmea_output = false       ; Stream raw NMEA sentences over USB Serial/JTAG
```

### Runtime Gestures
- **Short Press (Screen Sleeping)**: Wakes backlight.
- **Short Press (Screen Awake)**: Toggles display orientation 180° and saves state.

---

## Architecture & Hardware Safeguards

### Power Management & Brownout Protection
- **Brownout Safeguards**: Hardware brownout reset disabled in Kconfig (`# CONFIG_ESP_BROWNOUT_DET is not set`) and early runtime (`esp_brownout_disable()`) to prevent boot loops on weak 500 mA USB ports and charging transients. Wi-Fi PHY calibration cached in NVS.
- **Dynamic Wi-Fi TX Scaling**:
  - Depleted battery (< 3.65V): Throttled to **13 dBm** (~20 mW) to preserve TP4054 charge current.
  - Actively charging (< 4.10V): Capped at **15 dBm** (~31 mW) to avoid 5V rail sag on 500 mA ports.
  - Pure USB or fully charged (≥ 4.10V): Full **18 dBm** (~63 mW) for maximum range.
- **Gated Battery Divider**: Active-high `PIN_VBAT_CTRL` gate eliminates parasitic drain; 8-sample variance filter detects open battery ports.
- **Multi-Tier Sleep Protection**:
  - Early boot: 60s deep sleep if battery ≤ 3.25V without USB power.
  - Runtime: 30s shutdown countdown at ≤ 3.35V, with 3.50V recovery hysteresis.
  - Plugging in USB cancels countdown; unplugging re-arms it.
- **Status LED PWM**: Breathing pulse during display sleep (0–100% configurable peak duty); 100 ms flash on critical battery.

### Resource Footprint (ESP32-S3)
- **Flash Storage**: **1.16 MB** (~1,185 KB) in 3.0 MB app partition (**38.6% used**).
- **Static RAM**: **~187 KB** total (68 KB DRAM + 117 KB IRAM).
- **Runtime Free Heap**: **~325 KB free** of 512 KB internal SRAM for network sockets and buffers.
- **Static Allocation**: Zero runtime `malloc` in loops, packet engines, or ISRs.

---

## Subsystem Architecture Documentation

Design specifications and wire formats in [`docs/plan/`](docs/plan/):
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
