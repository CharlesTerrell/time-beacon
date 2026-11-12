# Display and UI Management Subsystem

## Overview
The display subsystem provides real-time status visualization using an onboard TFT color display (ST7735 0.96" 80x160 or ST7789 depending on board revision) driven over SPI. It renders clock status, GPS fix quality, satellite count, radio telemetry, and battery levels.

## Hardware Interface
- **Bus**: Dedicated SPI bus (MOSI, SCLK).
- **Control Pins**: CS (Chip Select), DC (Data/Command), RST (Reset), and BL / LED (Backlight PWM or GPIO).
- **Power Gating**: Backlight and display power rail (VDD) controlled via power management GPIOs to allow complete shutdown in low-power modes.
- **Orientation Control**: ST7735 `MADCTL` register (`0x36`):
  - `0x68`: Standard landscape orientation (0°).
  - `0xA8`: Inverted landscape orientation (180°).

## Screen Layout & UI Pages

### 1. Main Dashboard
- **Header**: Device status banner, Stratum indicator (`Stratum-1` or `Acquiring`), battery percentage icon.
- **Clock Line**: Large UTC timestamp (`HH:MM:SS`) updated synchronously with GPS PPS.
- **Date Line**: `YYYY-MM-DD` and day of week.
- **GPS Metrics**: Fix status (`3D-FIX`, `2D-FIX`, `NO-FIX`), satellite count (`Sat: 12`), HDOP.
- **Position**: Latitude and Longitude (degrees/minutes or decimal).
- **Service Status Indicators**:
  - `NTP`: Active client count or listening state (`N:<count>`).
  - `BLE`: Advertising or Connected.
  - `LoRa`: Last packet phase (`LF` = LongFast, `MF` = MedFast) and seconds until next pulse (`L:<count>`).
- **Power & Battery Telemetry Line**:
  - `POWER_MODE_USB`: Displays `USB` indicator (`N:<count> L:<count> <ble> USB`).
  - `POWER_MODE_BATTERY_NORMAL` / `LOW`: Displays terminal voltage (`... <ble> X.XV`).
  - `POWER_MODE_BATTERY_CRITICAL` on USB: Displays charging indicator `[CHG] X.XV`. Countdown is disarmed.
  - `POWER_MODE_BATTERY_CRITICAL` on Battery: Displays emergency shutdown warning `LOW BAT! SLEEP:<sec>s`.

### 2. Boot Countdown Page
- Displays countdown timer (e.g., `Booting in 3s...`) and highlights selectable boot profiles.

## Orientation & User Button Gestures
The `PIN_USER_BTN` GPIO controls screen interaction dynamically after boot:
1. **Screen Asleep (Backlight Off)**:
   - A short press on `USER` wakes the backlight immediately and schedules a redraw.
2. **Screen Awake (Backlight On)**:
   - A short press on `USER` toggles the display orientation 180° (`0x68` $\leftrightarrow$ `0xA8`).
   - The orientation state is written to persistent storage (`config_mgr_set_display_flip()`) so orientation persists across reboots.
3. **Configuration Setting**:
   - `[display] flip = true` in `config.ini` sets the default 180° inverted orientation at initial display initialization.

## Power Saving & Blanking Policies
1. **Always-On Mode**: Backlight remains active at configured brightness (suitable for USB-powered bench clocks).
2. **Auto-Dimming**: Backlight drops to 20% PWM after 30 seconds of inactivity.
3. **Auto-Blanking**: Display shuts down completely after 120 seconds of user button inactivity on battery power.
4. **Emergency Wakeup**: When battery transitions to `POWER_MODE_BATTERY_CRITICAL`, the display immediately forces backlight ON and renders the emergency countdown or `[CHG]` banner.
