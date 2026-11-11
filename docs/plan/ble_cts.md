# Bluetooth Low Energy (BLE) Current Time Service (CTS)

## Overview
The BLE subsystem provides standard Bluetooth SIG Current Time Service (CTS) broadcasts and custom telemetry over Bluetooth Low Energy (BLE) using the NimBLE stack. This enables smart watches, mobile phones, sensors, and microcontrollers to synchronize their clocks to Stratum-1 time.

## BLE Profile & Services

### 1. Current Time Service (CTS) - Standard SIG
- **Service UUID**: `0x1805`
- **Current Time Characteristic**: `0x2A2B`
  - **Permissions**: Read, Notify
  - **Payload Structure (10 bytes)**:
    - Bytes 0-1: `Year` (`uint16_t` little-endian, e.g. 2026 = `0x07EA`)
    - Byte 2: `Month` (`uint8_t`, 1–12)
    - Byte 3: `Day` (`uint8_t`, 1–31)
    - Byte 4: `Hours` (`uint8_t`, 0–23)
    - Byte 5: `Minutes` (`uint8_t`, 0–59)
    - Byte 6: `Seconds` (`uint8_t`, 0–59)
    - Byte 7: `Day of Week` (`uint8_t`, 1=Monday .. 7=Sunday)
    - Byte 8: `Fractions256` (`uint8_t`, 1/256th of a second)
    - Byte 9: `Adjust Reason` (`uint8_t` bitmask: `0x02` = External reference time update / GPS PPS)

### 2. Device Information Service (DIS) - Standard SIG
- **Service UUID**: `0x180A`
- **Characteristics**:
  - `0x2A24`: Model Number String (`"Wireless Tracker"`)
  - `0x2A29`: Manufacturer Name String
  - `0x2A26`: Firmware Revision String

### 3. Power & Battery Telemetry Service - Custom
- **Service UUID**: `0xFFE0`
- **Telemetry Characteristic**: `0xFFE1`
  - **Permissions**: Read, Notify
  - **Payload Structure (4 bytes packed)**:
    - Byte 0: `percentage` (`uint8_t`, 0–100%)
    - Bytes 1-2: `voltage_mv` (`uint16_t` little-endian, millivolts)
    - Byte 3: `usb_connected` (`uint8_t`, 1 = USB powered, 0 = Battery)

## Operation
1. **Advertising**:
   - Device advertises as `Heltec-Time` (or configured device name).
   - Advertises CTS UUID `0x1805` in the advertising payload.
2. **Read Requests**:
   - When a client reads `0x2A2B`, the callback reads the system time disciplined by GPS PPS, breaks down UTC year/month/day/hour/min/sec, computes the 1/256-second sub-second fraction, and formats the 10-byte struct.
3. **Notifications**:
   - Optional periodic notifications push updated timestamps to subscribed clients.
