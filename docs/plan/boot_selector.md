# Boot Selector and Operating Mode Management

## Overview
The Boot Selector subsystem manages device boot-time profile selection, startup countdowns, emergency fallback triggers, and configuration overrides without requiring a firmware reflash or serial terminal.

## Operating Modes
The device supports several operating profiles:
1. **Full Service Mode (Default)**:
   - Wi-Fi NTP Server active (AP or STA).
   - BLE Current Time Service active.
   - LoRa Meshtastic time beacons active.
   - GPS PPS discipline active.
   - TFT status display active.
2. **Low-Power / Field Beacon Mode**:
   - Wi-Fi radio disabled to conserve power.
   - BLE and LoRa time beacons active.
   - Display dims or blanks after timeout.
3. **Wi-Fi NTP Only Mode**:
   - LoRa and BLE disabled.
   - Dedicated high-throughput NTP server for bench / rackmount installations.
4. **Config / Recovery Mode**:
   - Device boots in AP mode with web configuration portal or mass-storage/serial mount.

## Boot Selector Mechanics
1. **Interactive Countdown**:
   - At power-on or reset, the display shows a 3 to 5 second boot countdown.
   - The user can press the user button (PRG / KEY) during the countdown to cycle through available boot profiles.
2. **Button Detection & Debouncing**:
   - Short press: Increments selected profile index.
   - Long press (> 2 seconds): Confirms selection immediately and skips remaining countdown.
   - Timeout (no press): Boots the default or previously saved mode from NVS / `time_beacon.ini`.
3. **Fallback Safety Mechanism**:
   - If a crash occurs within 10 seconds of boot repeatedly (tracked via NVS crash counter), the boot selector forces safe mode (AP recovery mode with full logging) to prevent boot loops.
