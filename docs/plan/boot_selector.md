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
