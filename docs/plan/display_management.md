# Display and UI Management Subsystem

## Overview
The display subsystem provides real-time status visualization using an onboard TFT color display (ST7735 0.96" 80x160 or ST7789 depending on board revision) driven over SPI. It renders clock status, GPS fix quality, satellite count, radio telemetry, and battery levels.

## Hardware Interface
- **Bus**: Dedicated SPI bus (MOSI, SCLK).
- **Control Pins**: CS (Chip Select), DC (Data/Command), RST (Reset), and BL / LED (Backlight PWM or GPIO).
- **Power Gating**: Backlight and display power rail (VDD) controlled via power management GPIOs to allow complete shutdown in low-power modes.

## Screen Layout & UI Pages

### 1. Main Dashboard
- **Header**: Device status banner, Stratum indicator (`Stratum-1` or `Acquiring`), battery percentage icon.
- **Clock Line**: Large UTC timestamp (`HH:MM:SS`) updated synchronously with GPS PPS.
- **Date Line**: `YYYY-MM-DD` and day of week.
- **GPS Metrics**: Fix status (`3D-FIX`, `2D-FIX`, `NO-FIX`), satellite count (`Sat: 12`), HDOP.
- **Position**: Latitude and Longitude (degrees/minutes or decimal).
- **Service Status Indicators**:
  - `NTP`: Active client count or listening state.
  - `BLE`: Advertising or Connected.
  - `LoRa`: Last packet phase (`LF` = LongFast, `MF` = MedFast) and seconds until next pulse.

### 2. Boot Countdown Page
- Displays countdown timer (e.g., `Booting in 3s...`) and highlights selectable boot profiles.

## Power Saving & Blanking Policies
1. **Always-On Mode**: Backlight remains active at configured brightness (suitable for USB-powered bench clocks).
2. **Auto-Dimming**: Backlight drops to 20% PWM after 30 seconds of inactivity.
3. **Auto-Blanking**: Display shuts down completely after 60 seconds on battery power. Any button press wakes the display for a temporary viewing window.
