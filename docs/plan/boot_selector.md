# Boot Selector and Operating Mode Management

## Overview
The Boot Selector subsystem manages device boot-time mode selection, user input prompting, USB configuration flash drive access, and service dispatch without requiring a serial terminal or external flasher.

---

## Boot Sequence & Hardware Guarding

1. **Early Hardware Brownout Guard**:
   - `esp_brownout_disable()` is invoked at the very beginning of `app_main()` to prevent spurious hardware resets during USB connection transients.
   - Display and power manager initialize early.
   - `power_mgr_early_battery_guard()` verifies power rails:
     - If running on battery alone with depleted voltage (≤ 3.25V), displays a low-battery alert and enters deep sleep for 60 seconds (or until USER button press) to protect the lithium cell.
     - If USB power is present or battery connector is unpopulated, boot proceeds unconditionally.

2. **2.5-Second Interactive Prompt**:
   - At power-on or reset, the display renders an interactive 2.5-second countdown prompt (`Hold PRG for Config Mode`).
   - The ESP32-S3 polls `PIN_USER_BTN` (GPIO 0, active LOW with internal pull-up) every 100 ms.
   - **Button Pressed**: Device diverts into **USB Flash Drive Configuration Mode**.
   - **Timeout (No Press)**: Device proceeds directly to **Stratum-1 Time Server Mode**.

---

## Operating Modes

### 1. Stratum-1 Time Server Mode (Default)
When the countdown expires without button intervention:
- Initializes configuration manager (`config_mgr_init()`) and parses `/storage/config.ini` from the internal FATFS wear-levelled flash partition.
- Applies user display orientation (`cfg->display_flip`).
- Suppresses system logs if raw NMEA streaming is enabled (`cfg->usb_nmea_output`).
- Sets status LED peak pulse brightness (`cfg->led_peak_percent`).
- Launches core time discipline services:
  - GNSS receiver UART driver (`gnss_driver_init()`).
  - Microsecond time synchronization engine (`time_sync_init()`).
  - Dedicated GNSS background parsing task (`gnss_task_start()`).
- Selectively dispatches network and radio services based on `config.ini`:
  - **Wi-Fi Station Service**: Connects to configured SSID with DHCP or static IP (`cfg->enable_wifi`).
  - **UDP NTP Server**: Listens on UDP port 123 (`cfg->enable_wifi && cfg->enable_ntp`).
  - **BLE CTS Service**: Advertises Current Time Service and battery telemetry (`cfg->enable_ble`).
  - **LoRa Meshtastic Beacons**: Transmits periodic time, nodeinfo, and telemetry beacons (`cfg->enable_meshtastic`).

### 2. USB Flash Drive Configuration Mode
Triggered by holding the `USER` button during the 2.5-second boot prompt:
- **Display UI**: Renders `USB CONFIG MODE` with instructions to edit `config.ini` on the host PC and press PRG to exit.
- **Mass Storage Device (MSC)**:
  - Mounts the internal wear-levelled FATFS partition (`/storage`) as a USB Mass Storage drive using ESP TinyUSB.
  - If `config.ini` does not exist, automatically creates a default template with documented parameters.
  - The drive mounts natively on Windows, macOS, and Linux without requiring custom drivers.
- **Clean Exit & PHY Handoff**:
  - The loop monitors `PIN_USER_BTN` for exit request or host software unmount.
  - On exit, `usb_msc` cleanly flushes and unmounts FATFS (`esp_vfs_fat_spiflash_unmount_rw_wl`).
  - Uninstalls TinyUSB stack (`tinyusb_driver_uninstall`).
  - Restores USB Serial/JTAG PHY routing to prevent bus hang or lost debug output.
  - Executes `esp_restart()` to boot cleanly into Stratum-1 Time Server Mode with updated configuration.

---

## Configuration Parameter Summary (`config.ini`)

The canonical `config.ini` template auto-generated on first boot by `config_mgr_write_default_ini()` uses categorized sections:

```ini
[device]
node_name = TimeBeacon        ; Advertised BLE and Meshtastic name (max 32 chars)
# country = US                ; Regulatory ISO country code (WiFi channels & default LoRa freq)
# led_peak_percent = 50       ; Peak brightness of status LED breathing pulse (0-100%)
# usb_nmea_output = false     ; Stream raw NMEA sentences over USB console instead of logs
# enable_pps = true           ; Hardware 1PPS discipline (true = < 1us accuracy, false = NMEA)
# nmea_offset_ms = 60         ; Software NMEA delay offset in ms when 1PPS absent/disabled

[services]
# wifi = true                 ; Enable Wi-Fi network connection
# ntp = true                  ; Enable Stratum-1 UDP NTP server on port 123
# bluetooth = true            ; Enable BLE Current Time Service (CTS 0x1805)
# meshtastic = false          ; Enable LoRa Meshtastic POSITION & TELEMETRY broadcasts
# position_precision = 18     ; Meshtastic position precision (18 = ~90m fuzzy privacy, 32 = exact)
# ble_battery = true          ; Standard BLE Battery Service (BAS 0x180F) telemetry
# lora_power = true           ; LoRa power telemetry broadcast (battery & solar bus mV)

[lora]
# lora_freq = 906875000       ; LoRa carrier frequency in Hz (commented out = auto by country)
# tx_power = 22               ; LoRa RF transmit power in dBm (0 to 22 dBm, default: 22)

[display]
# flip = false                ; Invert display 180 degrees (true / false / 180 / 0)

[wifi]
ssid = MyNetwork              ; 2.4 GHz Wi-Fi network SSID
password = SecretKey          ; Wi-Fi WPA2/WPA3 passphrase
# country = US                ; Regulatory ISO country code for Wi-Fi channels

[network]
dhcp = true                   ; Set true for dynamic DHCP IP; false to use static IP below
# ip = 192.168.1.200          ; Static IP address (active when dhcp = false)
# gateway = 192.168.1.1       ; Static gateway
# netmask = 255.255.255.0     ; Static subnet mask
# dns = 1.1.1.1               ; Static DNS server
```

---

## Supported Alternative Sections, Flags, and Formats

To ensure backwards compatibility with older configuration styles and intuitive manual editing, `config_mgr.c` accepts several alternative section names and key aliases:

### 1. Service Toggles: Unified `[services]` vs. Per-Service Sections
Services can be enabled or disabled either within the unified `[services]` block or inside dedicated per-service sections:

| Feature | Canonical (`[services]`) | Alternative Section Format | Accepted Values |
| :--- | :--- | :--- | :--- |
| **Wi-Fi Connection** | `[services]`<br>`wifi = true` | `[wifi]`<br>`enable = true` or `enabled = true` | Boolean |
| **NTP Server** | `[services]`<br>`ntp = true` | `[ntp]`<br>`enable = true` or `enabled = true` | Boolean |
| **Bluetooth LE** | `[services]`<br>`bluetooth = true` or `ble = true` | `[ble]` or `[bluetooth]`<br>`enable = true` or `enabled = true` | Boolean |
| **Meshtastic Beacons** | `[services]`<br>`meshtastic = true` | `[meshtastic]`<br>`enable = true` or `enabled = true` | Boolean |
| **1PPS Discipline** | `[services]`<br>`pps = true` or `enable_pps = true` | `[device]` or `[general]`<br>`enable_pps = true` or `pps_enable = true` | Boolean |
| **BLE Battery Service** | `[services]`<br>`ble_battery = true` or `battery_ble = true` | `[power]`<br>`ble_battery = true` | Boolean |
| **LoRa Power Telemetry**| `[services]`<br>`lora_power = true` or `lora_telemetry = true` | `[power]`<br>`lora_power = true` or `lora_telemetry = true` | Boolean |

### 2. Parameter Key Aliases

| Parameter | Canonical Section & Key | Supported Section & Key Aliases | Notes |
| :--- | :--- | :--- | :--- |
| **Node Name** | `[device]` `node_name` | `[general]` `name` | Maximum 32 characters |
| **LED Peak Brightness** | `[device]` `led_peak_percent` | `[device]` / `[general]` / `[power]`:<br>`led_intensity`, `led_brightness`, `led_percent` | Integer 0 to 100 (%) |
| **USB NMEA Stream** | `[device]` `usb_nmea_output` | `[device]` / `[general]` / `[power]`:<br>`usb_nmea`, `gps_passthrough` | Boolean |
| **NMEA Offset** | `[device]` `nmea_offset_ms` | `[device]` / `[general]` / `[power]`:<br>`nmea_delay_ms`, `gps_offset_ms` | Signed integer in ms |
| **Country Code** | `[device]` `country` | `[device]` / `[regulatory]` / `[lora]` / `[wifi]`:<br>`country_code` | 2-letter ISO code (e.g., `US`, `EU`, `AU`, `IN`, `JP`) |
| **LoRa Frequency** | `[lora]` `lora_freq` | `[lora]`:<br>`lora_freq_hz`, `lora_frequency`, `frequency`, `freq` | Integer in Hz (100 MHz – 1 GHz) |
| **LoRa TX Power** | `[lora]` `tx_power` | `[lora]`:<br>`power` | Integer 0 to 22 dBm |
| **Position Precision**| `[services]` `position_precision` | `[services]` / `[meshtastic]`:<br>`precision_bits` | Integer 0 to 32 bits (18 = ~90m fuzz) |
| **Display Flip** | `[display]` `flip` | `[display]`:<br>`invert`, `rotation` | Boolean or integer (`180` = true, `0` = false) |
| **Static IP Address** | `[network]` `ip` | `[network]` `ip` | IPv4 address string |
| **Static Gateway** | `[network]` `gateway` | `[network]` `gateway` | IPv4 gateway string |
| **Static Netmask** | `[network]` `netmask` | `[network]` `netmask` | IPv4 netmask string |
| **Static DNS** | `[network]` `dns` | `[network]` `dns` | IPv4 DNS server string |

### 3. Accepted Boolean Formats
All boolean flags parsed by `config_mgr.c` evaluate case-insensitively to:
- **True**: `true`, `1`, `yes`, `on`, `enable`, `enabled`
- **False**: `false`, `0`, `no`, `off`, `disable`, `disabled`

