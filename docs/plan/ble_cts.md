# Bluetooth Low Energy (BLE) Current Time & Telemetry Service

## Overview
The BLE subsystem provides standard Bluetooth SIG Current Time Service (CTS) broadcasts, standard Battery Service (BAS) telemetry, and custom device power metrics over Bluetooth Low Energy (BLE) using the Apache NimBLE stack. This allows smart watches, smartphones, sensors, and client microcontrollers to synchronize their real-time clocks to Stratum-1 time.

---

## BLE Profiles & Supported Services

### 1. Current Time Service (CTS) - Standard SIG
- **Service UUID**: `0x1805` (`BLE_SVC_CTS_UUID16`)
- **Current Time Characteristic**: `0x2A2B`
  - **Permissions**: Read, Notify
  - **Payload Structure (10 bytes packed)**:
    - Bytes 0–1: `Year` (`uint16_t` little-endian, e.g. 2026 = `0x07EA`)
    - Byte 2: `Month` (`uint8_t`, 1–12)
    - Byte 3: `Day` (`uint8_t`, 1–31)
    - Byte 4: `Hours` (`uint8_t`, 0–23)
    - Byte 5: `Minutes` (`uint8_t`, 0–59)
    - Byte 6: `Seconds` (`uint8_t`, 0–59)
    - Byte 7: `Day of Week` (`uint8_t`, 1 = Monday .. 7 = Sunday)
    - Byte 8: `Fractions256` (`uint8_t`, 1/256th of a second)
    - Byte 9: `Adjust Reason` (`uint8_t` bitmask: `0x02` = External reference time update / GNSS PPS discipline)

### 2. Device Information Service (DIS) - Standard SIG
- **Service UUID**: `0x180A` (`BLE_SVC_DIS_UUID16`)
- **Characteristics**:
  - `0x2A24`: Model Number String (`"Wireless Tracker"`)
  - `0x2A29`: Manufacturer Name String (`"Heltec"`)
  - `0x2A26`: Firmware Revision String

### 3. Battery Service (BAS) - Standard SIG
- **Service UUID**: `0x180F` (`BLE_SVC_BAS_UUID16`)
- **Battery Level Characteristic**: `0x2A19`
  - **Permissions**: Read, Notify
  - **Payload**: 1-byte battery percentage (0–100%)
  - **Configuration**: Enabled via `enable_battery = true` under `[ble]` in `config.ini`. When enabled, the BAS UUID is included in GAP advertising packets.

### 4. Power & Battery Telemetry Service - Custom
- **Service UUID**: `0xFFE0`
- **Telemetry Characteristic**: `0xFFE1`
  - **Permissions**: Read, Notify
  - **Payload Structure (4 bytes packed)**:
    - Byte 0: `percentage` (`uint8_t`, 0–100%)
    - Bytes 1–2: `voltage_mv` (`uint16_t` little-endian, millivolts)
    - Byte 3: `usb_connected` (`uint8_t`, 1 = USB powered, 0 = Battery)

---

## GAP Advertising & Operation

1. **Advertising Parameters**:
   - Device Name: Configurable via `node_name` in `config.ini` (default: `TimeBeacon`).
   - Flags: `BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP`.
   - Advertises CTS UUID `0x1805` (and BAS UUID `0x180F` if `enable_ble_battery` is active).
2. **Read Requests**:
   - When a client reads `0x2A2B`, the callback reads system time disciplined by GNSS, breaks down UTC year/month/day/hour/min/sec, computes the 1/256-second sub-second fraction, and responds with the 10-byte struct.
3. **Telemetry Notifications**:
   - Updates are pushed periodically or when connection states transition.
