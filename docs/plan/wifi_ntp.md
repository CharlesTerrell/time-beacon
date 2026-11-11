# Wi-Fi NTP/SNTP Server Subsystem

## Overview
The Wi-Fi NTP subsystem implements a high-precision Stratum-1 Network Time Protocol (NTP / SNTPv4, RFC 5905 / RFC 4330) server over UDP port 123. It provides microsecond-accurate time to local network clients (PCs, routers, Chrony/NTP daemons, IoT devices).

## Wi-Fi Modes
The subsystem supports flexible operating modes via `time_beacon.ini`:
1. **Access Point (AP Mode)**:
   - Default SSID: `TimeBeacon-NTP` (or configured)
   - Channel: 1 or 6
   - Static IP: `192.168.4.1`
   - Built-in DHCP server provides client IP, gateway, and advertises the AP IP as the local NTP server (DHCP Option 42).
   - Ideal for field deployments, off-grid networks, or air-gapped measurement rigs.
2. **Station (STA Mode)**:
   - Connects to existing local Wi-Fi infrastructure.
   - Obtains IP via DHCP or static IP.
   - Listens on UDP port 123 on the assigned interface IP.

## NTP Packet Engine & Precision Timing
- **Port**: UDP 123
- **Socket**: Non-blocking or FreeRTOS task loop listening on `INADDR_ANY`.
- **Packet Wire Format (48 bytes)**:
  - Byte 0: `LI = 0` (No warning), `VN = 4` (NTPv4), `Mode = 4` (Server response) $\to$ `0x24`
  - Byte 1: `Stratum = 1` (Primary reference)
  - Byte 2: `Poll` (Copied from client request)
  - Byte 3: `Precision = -20` (~1 microsecond resolution)
  - Bytes 4-7: `Root Delay` (0)
  - Bytes 8-11: `Root Dispersion` (< 1 ms)
  - Bytes 12-15: `Reference Identifier`: `"GPS "` (`0x47505320`) or `"PPS "` (`0x50505320`)
  - Bytes 16-23: `Reference Timestamp` (NTP format: 32-bit seconds since 1900, 32-bit fractional seconds)
  - Bytes 24-31: `Origin Timestamp` (Copied verbatim from client Transmit Timestamp)
  - Bytes 32-39: `Receive Timestamp` (Time request arrived at socket)
  - Bytes 40-47: `Transmit Timestamp` (Time response leaves socket)

## Timestamp Conversion
NTP epoch begins on January 1, 1900. Unix epoch begins on January 1, 1970.
- Offset: `2,208,988,800` seconds (`70 * 365 + 17` leap years).
- NTP Seconds = Unix Seconds + `2208988800UL`.
- NTP Fraction = `(microsecond_remainder * 4294967296ULL) / 1000000ULL`.
- All multi-byte integers are serialized in big-endian network byte order (`htonl()`).
