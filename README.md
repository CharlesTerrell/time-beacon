# Time Beacon: Multi-Protocol Stratum-1 Time Server

Time Beacon is a self-contained, battery- or USB-powered **Stratum-1 Reference Clock** built for ESP32-S3. Displined by an onboard GNSS/GPS receiver and hardware Pulse-Per-Second (PPS) interrupt, it simultaneously broadcasts precision time across three wireless protocols:

1. **Wi-Fi NTP Server**: RFC 5905 NTPv4 / RFC 4330 SNTP on UDP port 123 (AP or Station mode).
2. **Bluetooth Low Energy (BLE)**: Standard SIG Current Time Service (CTS `0x1805` / `0x2A2B`) with sub-second (1/256s) precision, plus battery telemetry.
3. **Meshtastic LoRa Beacons**: Alternating 2-phase over-the-air time sync (LongFast even minutes, MediumFast odd minutes) enabling off-grid nodes to sync clocks without GPS or Internet.

---

## Supported Hardware

- **Heltec Wireless Tracker v1.1** (ESP32-S3, UC6580 GNSS, SX1262 LoRa, ST7735 0.96" TFT)
- **Heltec Wireless Tracker v2** (ESP32-S3, upgraded GNSS/display routing)

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
./build_tracker.sh all
```
Or build an individual target:
```bash
./build_tracker.sh v1.1
./build_tracker.sh v2
```

### 4. Run Host Tests
The project includes a native x86 test harness simulating NMEA parsing, PPS jitter, NTP encoding, and BLE packet layout:
```bash
./tests/run_tests.sh
```

### 5. Flash to Device
Connect the board via USB-C and flash:
```bash
./build_tracker.sh flash v1.1
```
To monitor serial logs:
```bash
tio -b 115200 /dev/ttyACM0
# or
idf.py -B build_v1 monitor
```

---

## Configuration

Settings can be customized at runtime by placing a `time_beacon.ini` file in device storage (LittleFS/NVS) or editing `main/config_mgr.c`:

```ini
[wifi]
mode = ap            ; "ap" or "sta"
ssid = TimeBeacon-NTP
password = stratum1time
channel = 1

[lora]
enable_meshtastic = true
tx_power = 22

[ble]
enable_cts = true
device_name = TimeBeacon

[gps]
baud_rate = 115200
min_satellites = 4
```

---

## Subsystem Architecture Documentation

Detailed design specifications and wire formats are located in [`docs/plan/`](docs/plan/):
- [GPS Time & Position Subsystem](docs/plan/gps_time_position.md)
- [BLE Current Time Service (CTS)](docs/plan/ble_cts.md)
- [Meshtastic LoRa Beacons](docs/plan/meshtastic_beacons.md)
- [Wi-Fi NTP Server](docs/plan/wifi_ntp.md)
- [Boot Selector & Operating Profiles](docs/plan/boot_selector.md)
- [Display & UI Management](docs/plan/display_management.md)
- [Power Management & Battery Telemetry](docs/plan/power_management.md)
- [MeshCore Architecture Specification](docs/plan/meshcore_integration_spec.md)

---

## License
MIT License.
