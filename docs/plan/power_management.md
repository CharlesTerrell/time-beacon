# Power Management and Battery Telemetry Subsystem

## Overview
The power management subsystem handles battery voltage monitoring, USB power presence detection, power rail gating, dynamic RF power scaling, and brownout protection. It balances maximum wireless broadcast range against strict power-budget limits imposed by the onboard lithium battery charging circuit.

---

## Hardware Interface & Electrical Architecture
1. **Battery ADC**:
   - Monitored via ESP32-S3 internal ADC1 Channel 0 (GPIO 1) through a 4.9:1 resistive divider circuit.
   - Calibrated using factory eFuse curve-fitting calibration (`esp_adc_cal`).
   - Software averaging filter (8 samples with delay) eliminates transient conversion noise.
2. **ADC Power Gate Pin**:
   - Controlled via `PIN_VBAT_CTRL` (GPIO 2, active LOW).
   - Only asserted during measurement (~1 ms) to prevent continuous parasitic drain through the divider resistors.
3. **Vext / Sensor Power Rail**:
   - Hardware pin (`PIN_VEXT_CTRL` / GPIO 3) gates power to the GNSS receiver and display backlight. Cut during deep sleep to minimize quiescent current.
4. **Onboard Battery Charger (TP4054 / TP4056)**:
   - Linear constant-current / constant-voltage (CC/CV) LiPo charger powered directly from the USB 5V bus.
   - Draws up to **500 mA** during bulk constant-current charging.
   - Shares the 5V input rail with the 3.3V LDO supplying the ESP32-S3.

---

## USB Detection: Host PC vs. Wall Charger
Because the Heltec board lacks a dedicated GPIO sensing USB VBUS (5V), the firmware detects external power through two complementary mechanisms:
1. **Host PC / Data Connection**:
   - Sensed via `usb_serial_jtag_is_connected()`.
   - Returns `true` when active USB SOF/data traffic is present from a host computer.
2. **Wall Adapter / Dumb Power Bank**:
   - Lacks data lines; `usb_serial_jtag_is_connected()` returns `false`.
   - Detected by reading battery terminal voltage under charging load ($\Delta V = I_{\text{chg}} \cdot R_i$) and sensing variance.
   - When running on a wall charger with a depleted battery ($\le 3.25\text{V}$), the device enters **60-second deep sleep recovery cycles**, allowing the charger to bulk-charge the battery at full speed without competing with CPU or RF current.

---

## Battery State Calculation
- **Chemistry**: 1S LiPo / Li-ion (3.0V cut-off to 4.2V fully charged).
- **Capacity Curve Mapping**:
  - $\ge 4.15\text{ V} \to 100\%$
  - $3.85\text{ V} \to 50\%$
  - $3.65\text{ V} \to 25\%$
  - $3.35\text{ V} \to 0\%$ (`VBAT_CRITICAL_V`)
- **Open Battery Port Detection**:
  - Sample variance $> 200$ or measured voltage $< 1.0\text{V}$ identifies an unpopulated battery connector (running on pure USB power).

---

## Dynamic RF Power Scaling
To prevent brownout resets caused by simultaneous Wi-Fi RF power spikes (~400 mA) and battery charging (~500 mA), Wi-Fi transmit power scales dynamically:
- **Battery health takes precedence over USB status**:
  - **Depleted Battery ($< 3.65\text{ V}$)**: Wi-Fi TX power is **throttled to 13 dBm (~20 mW / 52 units)** even when plugged into USB, leaving maximum current budget for the TP4054 charger.
  - **Healthy Battery ($\ge 3.75\text{ V}$) or Pure USB (No Battery)**: Wi-Fi TX power boosts to **18 dBm (~63 mW / 72 units)** for maximum home/outdoor range.
  - **Intermediate Battery ($3.65\text{ V}$ to $3.75\text{ V}$)**: Wi-Fi operates at **15 dBm (~31 mW / 60 units)**.
- **LoRa Transmit Power**:
  - Remains at full **22 dBm (160 mW)** because packet emissions are brief (< 100 ms) and duty-cycled once every 60 seconds.

---

## Emergency Sleep & Countdown State Machine
1. **Early Boot Brownout Guard (`power_mgr_early_battery_guard`)**:
   - Executed in `app_main()` before Wi-Fi, BLE, or LoRa initialization.
   - If a physical battery is present and $\le 3.25\text{V}$, the device displays a brief warning, cuts peripheral power (`Vext`), and enters Deep Sleep for 60 seconds.
2. **Runtime Critical Battery Shutdown**:
   - Triggers when battery voltage drops to $\le 3.35\text{V}$ on battery power.
   - Starts a **30-second shutdown countdown** (`critical_countdown_sec = 30`), flashing the red status LED and displaying `LOW BAT! SLEEP:Xs`.
3. **Plugging into USB during Countdown**:
   - If USB power is connected during the countdown, the shutdown timer is **immediately disarmed and cancelled** (`critical_countdown_sec = -1`).
   - The display updates to **`[CHG] X.XV`**, and normal time services continue while Wi-Fi stays throttled to 13 dBm.
4. **Unplugging from USB while still Critical**:
   - If disconnected from USB while voltage remains $\le 3.35\text{V}$, the countdown **re-arms to 30 seconds immediately**, preventing battery over-discharge.
