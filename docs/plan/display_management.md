# Display and UI Management Subsystem

## Overview
The display subsystem provides real-time visualization of Stratum-1 time synchronization, GNSS fix quality, satellite count, NTP/LoRa packet metrics, BLE connection status, and battery health. It supports both SPI color TFT and I2C monochrome OLED displays across Heltec hardware revisions.

---

## Hardware Interface & Board Support

### 1. Supported Display Controllers
- **ST7735 0.96" SPI Color TFT (80x160)**:
  - Used on Heltec Wireless Tracker v1.1 and Tracker v2 (`BOARD_HAS_TFT_ST7735 1`).
  - Driven over dedicated SPI bus: MOSI (GPIO 42), SCLK (GPIO 41), DC (GPIO 40), RST (GPIO 39), CS (GPIO 38), and Backlight BL (GPIO 21).
  - Orientation control via ST7735 `MADCTL` register (`0x36`):
    - `0x68`: Standard landscape orientation (0°).
    - `0xA8`: Inverted landscape orientation (180°).
- **SSD1306 0.96" I2C Monochrome OLED (128x64)**:
  - Used on Heltec WiFi LoRa 32 V4 (`BOARD_HAS_OLED_SSD1306 1`).
  - Driven over I2C: SDA (GPIO 17), SCL (GPIO 18), RST (GPIO 21).

### 2. Power Gating & Backlight
- The display logic and backlight are powered via `PIN_VEXT_CTRL`.
- Deasserted during deep sleep to eliminate quiescent current.

---

## Screen Layout & UI Screens

### 1. Main Dashboard (80x160 ST7735 Layout)
- **Line 1 (y=4)**: Header banner displaying node name and Stratum level (e.g. `TIMEBEACON STRATUM 1`).
- **Line 2 (y=18)**: Wi-Fi status or assigned station IP (e.g. `192.168.1.200` or `DISCONNECTED`).
- **Line 3 (y=32)**: Satellite count and lock status (e.g. `SAT: 12 [LOCKED]` or `SAT: 03 ACQUIRING`).
- **Line 4 (y=46)**: UTC timestamp and optional coordinates (e.g. `UTC 18:20:05 37.7 -122.4`).
- **Line 5 (y=60)**: Telemetry and power status bar:
  - `N:<ntp> L:<lora> [BT] 3.9V*`: USB connected with battery charging (asterisk denotes external power).
  - `N:<ntp> L:<lora> [BT] USB`: USB connected without battery.
  - `N:<ntp> L:<lora> [CHG] 3.3V`: USB connected with depleted battery.
  - `LOW BAT! SLEEP:<sec>s`: Critical battery on battery power (countdown ticking).
  - `N:<ntp> L:<lora> [BT] 3.8V`: Normal battery operation.

### 2. Critical Battery Shutdown Alert Screen
When terminal voltage drops to ≤ 3.35V on battery power, the normal dashboard is replaced by a full-screen red warning:
```text
! CRITICAL BATTERY !
CONNECT USB CHARGER
SHUTDOWN IN: 28s
VOLTAGE: 3.32V
```

### 3. USB Configuration Mode Screen
When entering USB Mass Storage mode during boot:
```text
USB CONFIG MODE
Edit: config.ini
Eject / Press PRG
to restart
```

---

## User Button Interaction & Orientation Flip

The `PIN_USER_BTN` (GPIO 0) dynamically handles screen wake and orientation rotation during runtime:

1. **Screen Sleeping (Backlight Off)**:
   - Short button press immediately wakes the backlight and refreshes the dashboard without altering orientation.
2. **Screen Awake (Backlight On)**:
   - Short button press toggles screen orientation by 180° (`0x68` ↔ `0xA8`).
   - The new state is stored via `config_mgr_set_display_flip()` so orientation state persists across runtime operations.
3. **Configuration Setting**:
   - `[display] flip = true` in `config.ini` enforces the 180° inverted orientation upon boot.

---

## Inactivity Blanking & Power Policies

1. **Auto-Blanking**:
   - Backlight turns off automatically after **120 seconds** of user button inactivity to conserve battery power.
2. **Emergency Alert Wakeup**:
   - If the battery transitions to `POWER_MODE_BATTERY_CRITICAL` while the screen is asleep, the display immediately forces backlight ON and renders the emergency countdown.
