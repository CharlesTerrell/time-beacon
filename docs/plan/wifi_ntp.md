# Wi-Fi Station & NTP/SNTP Server Subsystem

## Overview
The Wi-Fi NTP subsystem implements a high-precision Stratum-1 Network Time Protocol (NTP / SNTPv4, RFC 5905 / RFC 4330) server over UDP port 123. Coupled with hardware GNSS time discipline, it delivers sub-millisecond accurate timestamps to local network clients (PCs, routers, servers, Chrony/NTP daemons, and IoT devices).

---

## Wi-Fi Station Configuration & Power Architecture

The Wi-Fi interface operates as an 802.11 b/g/n Station (`WIFI_MODE_STA`) connecting to existing local network infrastructure.

### 1. Network Configuration (`config.ini`)
Configured under the `[wifi]` block:
- **SSID / Password**: WPA2/WPA3 credentials.
- **Addressing**:
  - **DHCP Client Mode (Default)**: Automatically negotiates IP, netmask, gateway, and DNS servers.
  - **Static IP Mode**: Enforced when `use_static_ip = true`, configuring `static_ip`, `static_gw`, `static_netmask`, and `static_dns`.
- **Regulatory Domain**: Configures two-letter country code (e.g. `country_code = US`) via `esp_wifi_set_country_code()`.

### 2. Wi-Fi Power Management & Dynamic RF Scaling
High-power Wi-Fi RF transmissions can draw 300–400 mA peaks, which risks sagging power rails and triggering brownouts when simultaneous battery charging (~500 mA) is active. The driver implements multi-tiered power protection:
- **Modem Sleep**: Enables automatic light-sleep between DTIM beacons (`WIFI_PS_MIN_MODEM`), cutting idle radio power consumption.
- **Initial Boot Cap**: Caps TX power to **15 dBm (60 units)** during driver startup before network join.
- **Dynamic TX Scaling (`wifi_manager_update_power`)**:
  - **Depleted Battery ($V_{\text{bat}} < 3.65\text{V}$)**: Throttled to **13 dBm (~20 mW / 52 units)** even when plugged into USB, maximizing available current for the onboard LiPo charger.
  - **Actively Charging on USB ($3.65\text{V} \le V_{\text{bat}} < 4.10\text{V}$)**: Capped to **15 dBm (~31 mW / 60 units)** to prevent overloading 500 mA host USB ports.
  - **Pure USB (No Battery) OR Fully Charged ($V_{\text{bat}} \ge 4.10\text{V}$) OR Healthy Battery ($V_{\text{bat}} \ge 3.75\text{V}$)**: Scaled to full **18 dBm (~63 mW / 72 units)** for maximum indoor range.
  - **Intermediate Battery on Discharge ($3.65\text{V} \le V_{\text{bat}} < 3.75\text{V}$)**: Balanced at **15 dBm (~31 mW / 60 units)**.

---

## NTP Packet Engine & Precision Timing

- **Port**: UDP 123
- **Socket**: Non-blocking Berkeley UDP socket bound to `INADDR_ANY` with FreeRTOS worker task.
- **Packet Wire Format (48 bytes, RFC 5905)**:
  - Byte 0: `LI = 0` (No leap warning), `VN = 4` (NTPv4), `Mode = 4` (Server response) $\to$ `0x24`
  - Byte 1: `Stratum = 1` (Primary reference clock)
  - Byte 2: `Poll` (Echoed from client request)
  - Byte 3: `Precision = -20` (~1 microsecond resolution)
  - Bytes 4–7: `Root Delay` (`0`)
  - Bytes 8–11: `Root Dispersion` (< 1 ms)
  - Bytes 12–15: `Reference Identifier`: `"GPS "` (`0x47505320`)
  - Bytes 16–23: `Reference Timestamp` (Time of most recent GNSS PPS lock)
  - Bytes 24–31: `Origin Timestamp` (Copied verbatim from client Transmit Timestamp)
  - Bytes 32–39: `Receive Timestamp` (System time at socket ingress)
  - Bytes 40–47: `Transmit Timestamp` (System time at socket egress)

---

## Timestamp Conversion & Network Byte Order

NTP timestamps are 64-bit fixed-point numbers counting seconds since January 1, 1900.
- **Epoch Conversion**:
  - Unix Epoch starts January 1, 1970 ($2,208,988,800$ seconds after NTP epoch).
  - $\text{NTP Seconds} = \text{Unix Seconds} + 2208988800\text{UL}$.
  - $\text{NTP Fraction} = \frac{\mu\text{s} \times 4294967296}{1000000}$.
- **Endianness**: All 32-bit timestamp integer fields are encoded in big-endian network byte order using `htonl()`.
