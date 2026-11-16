# Meshtastic LoRa Time Beacons

## Overview
The Meshtastic beacon subsystem broadcasts over-the-air Stratum-1 time synchronization, node identity, and battery telemetry over LoRa using the Semtech SX1262 transceiver. This enables off-grid Meshtastic nodes, field sensors, and repeaters to discipline their internal Real-Time Clocks without cellular, Internet, or onboard GNSS hardware.

---

## Radio Parameters & Multi-Preset Rotation

The beacon rotates transmissions on minute boundaries across two primary Meshtastic channel presets:

### 1. LongFast (Default Mesh Preset - Even Minutes)
- **Frequency**: `906.875 MHz` (US915 Channel Slot 20; configurable via `freq_hz`)
- **Bandwidth**: `250 kHz` (`SX1262_BW_250_KHZ`)
- **Spreading Factor**: `SF11`
- **Coding Rate**: `4/5` (`SX1262_CR_4_5`)
- **Preamble Length**: 16 symbols
- **Sync Word**: `0x2B44` (Meshtastic standard public sync word)
- **Channel XOR Hash**: `0x08` (Default "LongFast" channel hash)

### 2. MediumFast (High-Throughput Preset - Odd Minutes)
- **Frequency**: `913.125 MHz` (US915 Channel Slot 45)
- **Bandwidth**: `250 kHz` (`SX1262_BW_250_KHZ`)
- **Spreading Factor**: `SF9`
- **Coding Rate**: `4/5` (`SX1262_CR_4_5`)
- **Preamble Length**: 16 symbols
- **Sync Word**: `0x2B44`
- **Channel XOR Hash**: `0x1F` (Default "MediumFast" channel hash)

### 3. Rotation Schedule
- **Startup**: Sends a staggered beacon pulse shortly after boot to verify transceiver health.
- **Runtime Loop**:
  - **Even minutes (0, 2, 4, ...)**: SX1262 tunes to 906.875 MHz / SF11 (LongFast).
  - **Odd minutes (1, 3, 5, ...)**: SX1262 tunes to 913.125 MHz / SF9 (MediumFast).
- **Transmit Power**: +22 dBm (160 mW). Short duration emissions (< 100 ms) maintain negligible average current draw. High-power boards (Tracker v2, WiFi LoRa 32 V4) gate the external Front-End Module (FEM PA) via `PIN_PA_CSD`.

---

## Packet Wire Format & Cryptography

Packets adhere to Meshtastic Protobuf wire format and encryption standards:

### 1. Packet Header (16 bytes packed)
- `to`: `0xFFFFFFFF` (Broadcast address)
- `from`: 32-bit node number derived from the ESP32 Wi-Fi MAC address (`mac[2..5]`)
- `id`: Monotonically incrementing 32-bit packet sequence number
- `flags`: `0x63` (`hop_limit = 3`, `hop_start = 3`)
- `channel`: Preset channel hash (`0x08` for LongFast, `0x1F` for MediumFast)
- `next_hop` / `relay_node`: `0x00`

### 2. Payload Encryption (AES-128-CTR)
- Meshtastic default primary channel key:
  `1PG7OiApB1nwvP-rz05pAQ==` (`d4 f1 bb 3a 20 29 07 59 f0 bc ff ab cf 4e 69 01`).
- Cipher stream block generated with 128-bit Nonce:
  - Bytes 0–7: 64-bit packet sequence ID (little-endian)
  - Bytes 8–11: 32-bit transmitting node ID
  - Bytes 12–15: Zero padding (`0x00`)

---

## Broadcasted Message Suite

During each beacon period, the service broadcasts up to three coordinated Protobuf packets:

### 1. Position & Time Packet (`POSITION_APP`, PortNum 3)
Contains precision Stratum-1 time and geographic coordinates:
- `time`: 32-bit unsigned Unix epoch seconds (GPS top-of-second time)
- `timestamp`: 32-bit unsigned Unix epoch seconds
- `location_source`: `LOC_INTERNAL` (`2`)
- `altitude_source`: `ALT_INTERNAL` (`2`)
- `latitude_i` / `longitude_i`: Signed 32-bit integers ($\text{degrees} \times 10^7$).
  - Configurable fuzzing mask (`position_precision` in `config.ini`, default 18 bits $\approx 1.5\text{ km}$ privacy circle; 32 bits = raw precision).
- `altitude`: Altitude in meters above sea level (int32).
- `precision_bits`: Precision bit count (`0xB8 0x01 <prec>`).

### 2. Node Information Packet (`NODEINFO_APP`, PortNum 4)
Registers the time beacon in receiver mesh node routing tables:
- `id`: Formatted node string (e.g., `!12345678`)
- `long_name`: UTF-8 device name from `config.ini` (default `TimeBeacon`)
- `short_name`: 4-character uppercase identifier
- `macaddr`: 6-byte hardware MAC address
- `hw_model`: `48` (`HELTEC_WIRELESS_TRACKER`)

### 3. Device Metrics & Telemetry Packet (`TELEMETRY_APP`, PortNum 67)
Publishes power health and battery telemetry:
- `time`: Unix epoch seconds
- `battery_level`: Remaining battery percentage ($0\% \dots 100\%$), or **`101%`** when running on external USB power
- `voltage`: Current terminal voltage in Volts (IEEE 754 float)

---

## OTA Clock Sync Mechanics on Client Nodes

Meshtastic client nodes running official stock firmware evaluate incoming `POSITION_APP` packets in `PositionModule::trySetRtc()`:
1. Packet received on primary channel (`channel == 0` or default hash match).
2. Packet contains a non-zero timestamp (`p.time != 0`).
3. Position source qualifies as internal/GPS (`p.location_source >= LOC_INTERNAL`).
4. Client node currently lacks a trusted timesource (`!hasQualityTimesource()`).

When all conditions match, the client updates its internal RTC directly to the beacon's Stratum-1 timestamp.

---

## Note on MeshCore Protocol
*The legacy experimental MeshCore beacon implementation was removed from active firmware in commit `5cd5bec` to eliminate dead code and preserve radio airtime for 2-phase Meshtastic beacons. Full MeshCore protocol specifications and custom client patch details are archived in [meshcore_integration_spec.md](file:///home/charlie/Code/time_beacon/docs/plan/meshcore_integration_spec.md).*
