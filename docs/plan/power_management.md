# Power Management and Battery Telemetry Subsystem

## Overview
The power management subsystem handles battery voltage monitoring, USB power presence detection, power rail gating, and low-power sleep state transitions to optimize battery longevity for mobile and field deployments.

## Hardware Interface
1. **Battery ADC**:
   - Monitored via ESP32-S3 internal ADC1 with voltage divider circuit.
   - Calibrated using factory ADC eFuse calibration curves (`esp_adc_cal`).
   - Software averaging filter eliminates noise and instantaneous load drops.
2. **ADC Power Gate Pin**:
   - A dedicated GPIO controls a P-channel MOSFET to enable the battery divider only during measurement, preventing passive resistor drain.
3. **Vext / Sensor Power Rail**:
   - Hardware pin (`Vext_Ctrl` / `GPIO3`) gates power to GNSS receiver, display, and external sensors when powering down.

## Battery State Calculation
- **Chemistry**: 1S LiPo / Li-ion (3.0V cut-off to 4.2V fully charged).
- **Percentage Mapping**: Non-linear lookup table / curve mapping millivolts to remaining capacity percentage:
  - $\ge 4.20\text{ V} \to 100\%$
  - $3.85\text{ V} \to 50\%$
  - $3.70\text{ V} \to 20\%$
  - $3.30\text{ V} \to 0\%$ (Critical low-battery trigger)
- **USB Detection**:
  - Voltage $> 4.35\text{ V}$ or dedicated USB VBUS sense GPIO identifies continuous external power.

## Telemetry Reporting
Power status is reported across all interfaces:
1. **BLE Custom Service (`0xFFE0` / `0xFFE1`)**: Broadcasts battery percentage, raw millivolts, and USB status flag.
2. **TFT Display**: Renders battery status icon and voltage readout.
3. **Logs**: Periodic battery health logging.

## Power Saving Profiles
1. **USB Powered**: All services run continuously with maximum radio responsiveness and active display.
2. **Battery Standard**: Wi-Fi duty-cycled or disabled; BLE CTS and LoRa beacons active; display auto-blanks.
3. **Critical Battery Shutdown**:
   - Triggers when battery drops below 3.2V.
   - Saves state to NVS, deactivates GNSS and radios, turns off display power rail, and enters ESP32-S3 Deep Sleep to protect battery from over-discharge.
