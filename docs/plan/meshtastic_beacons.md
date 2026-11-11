# Meshtastic LoRa Time Beacons

## Overview
The Meshtastic beacon subsystem broadcasts over-the-air time synchronization and position packets over LoRa using the Semtech SX1262 transceiver. This enables off-grid Meshtastic nodes (especially clock-less sensors and repeaters) to synchronize their internal Real-Time Clocks without Internet access or onboard GPS.

## Radio Parameters & Channels
The beacon rotates transmissions between two primary Meshtastic channel presets:
1. **LongFast (Default Mesh)**:
   - Frequency: 906.875 MHz (US915)
   - Bandwidth: 250 kHz
   - Spreading Factor: SF11
   - Coding Rate: 4/5 (CR5)
   - Preamble: 16 symbols
   - Sync Word: `0x2B` (SX1262 LoRa default `0x1424` / `0x2B44`)
2. **MediumFast**:
   - Frequency: 906.875 MHz (US915)
   - Bandwidth: 250 kHz
   - Spreading Factor: SF9
   - Coding Rate: 4/5 (CR5)
   - Preamble: 16 symbols

## Rotation Schedule
- Transmissions alternate automatically on minute boundaries:
  - **Even minutes (0, 2, 4, ...)**: LongFast
  - **Odd minutes (1, 3, 5, ...)**: MediumFast
- A staggered startup pulse emits both profiles shortly after boot to verify transceiver health.

## Packet Wire Format
Packets adhere strictly to Meshtastic protobuf wire format:
1. **Packet Header (16 bytes)**:
   - `to`: `0xFFFFFFFF` (Broadcast)
   - `from`: Node 32-bit ID (derived from ESP32 MAC address)
   - `id`: Monotonically incrementing 32-bit packet counter
   - `flags`: Hop limit set to 3
   - `channel`: Channel hash for Primary channel (`0x00`)
2. **Payload Type**: `POSITION_APP` (PortNum 3)
   - Serialized Protobuf containing:
     - `time`: 32-bit unsigned Unix epoch seconds (Stratum-1 GPS time)
     - `latitude_i`: Latitude × 1e7
     - `longitude_i`: Longitude × 1e7
     - `altitude`: Altitude in meters above sea level
     - `location_source`: `LOC_INTERNAL` (enum value `2`)

## OTA Clock Sync Mechanics on Client Nodes
Meshtastic client nodes running stock firmware inspect incoming `POSITION_APP` packets in `PositionModule::trySetRtc()`:
- Condition 1: Packet received on Primary channel (`channel == 0`).
- Condition 2: `p.time != 0`.
- Condition 3: `p.location_source >= LOC_INTERNAL`.
- Condition 4: Receiving node does not currently possess a high-quality timesource (`!hasQualityTimesource()`).
When all conditions are met, the receiving node updates its RTC directly to the beacon's timestamp.
