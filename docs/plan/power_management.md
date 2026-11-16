# Power Management and Battery Telemetry Subsystem

## Overview
The power management subsystem handles battery voltage monitoring, USB power presence detection, power rail gating, dynamic RF power scaling, hardware and runtime brownout protection, PWM status LED signaling, adaptive UI theming, and multi-tier emergency sleep safeguards. It ensures continuous Stratum-1 time synchronization while strictly protecting battery chemistry and preventing brownouts on current-limited USB sources (such as 500 mA host ports).

---

## Hardware Interface & Electrical Architecture

### 1. Supported Hardware Platforms
- **Heltec Wireless Tracker v1.1**: ESP32-S3, ST7735 0.96" TFT, UC6580 GNSS, SX1262 LoRa.
- **Heltec Wireless Tracker v2**: Adds active GNSS PPS routing and RF Front-End Module (FEM) PA (`PIN_PA_CSD` GPIO 4, `PIN_PA_CPS` GPIO 46).
- **Heltec WiFi LoRa 32 V4**: ESP32-S3, SSD1306 0.96" OLED (I2C), SX1262 LoRa with FEM PA (`PIN_PA_CSD` GPIO 7), external GNSS connector with PPS.

### 2. Battery ADC & Gated Voltage Divider
- **ADC Input Pin**: ESP32-S3 internal ADC1 Channel 0 (`PIN_VBAT_ADC` / GPIO 1) connected to a 4.9:1 resistive divider (390 kΩ / 100 kΩ).
- **Power Gate Pin (`PIN_VBAT_CTRL`)**:
  - GPIO 2 on Tracker v1.1 and v2; GPIO 37 on WiFi LoRa 32 V4.
  - **Active HIGH**: Driving `PIN_VBAT_CTRL` HIGH turns on NPN transistor Q3, pulling P-MOSFET Q2 gate LOW to connect the divider to battery positive.
  - Gated sampling: The gate pin is asserted only during measurement (~500 µs settle delay + 8 consecutive samples with 100 µs spacing) and driven LOW immediately after to eliminate continuous quiescent parasitic drain.
- **Factory Calibration**:
  - Calibrated using factory eFuse curve-fitting calibration (`adc_cali_create_scheme_curve_fitting`), falling back to linear approximation if calibration is unavailable.
- **Filtering**:
  - Software exponential moving average filter (`s_status.voltage = 0.25f * measured_v + 0.75f * s_status.voltage`) smooths analog noise and transient RF load dips.

### 3. Vext / Peripheral Power Rail
- Controlled via `PIN_VEXT_CTRL` (GPIO 3 on Tracker v1.1/v2; GPIO 36 on WiFi LoRa 32 V4), active HIGH.
- Supplies power to the GNSS receiver and display backlight. Cut during deep sleep to reduce sleep current to single-digit microamps.

### 4. Onboard Battery Charger (TP4054 / TP4056)
- Linear constant-current / constant-voltage (CC/CV) LiPo charger powered directly from the USB 5V rail.
- Pulls up to **500 mA** during bulk constant-current charging.
- Shares the 5V bus with the 3.3V LDO supplying the ESP32-S3, GNSS receiver, and display. Simultaneous high Wi-Fi transmit power (~400 mA peaks) and charger current (~500 mA) will sag weak USB rails (500 mA USB 2.0 host ports) unless dynamically managed.

### 5. Brownout Detector Safeguards
- **Kconfig Configuration (`sdkconfig.defaults`)**:
  - `# CONFIG_ESP_BROWNOUT_DET is not set` prevents aggressive hardware brownout resets caused by brief transient voltage dips during USB plugging, battery charging start, and Wi-Fi RF calibration.
  - `CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE=y` stores Wi-Fi PHY calibration data in NVS, eliminating large analog recalibration current spikes on boot.
- **Runtime Early Brownout Disable**:
  - `esp_brownout_disable()` is invoked at the very beginning of `app_main()` in `main/main.c` before initializing any hardware peripherals or radio stacks.

### 6. Status LED (LEDC PWM)
- Controlled via `PIN_STATUS_LED` (GPIO 18 on Tracker v1.1/v2; GPIO 35 on WiFi LoRa 32 V4) using the ESP32-S3 LEDC PWM peripheral (1 kHz, 10-bit resolution).
- **Behaviors**:
  - **Normal Operation (Display Awake)**: LED is turned OFF.
  - **Display Sleeping (Backlight Off after 120s inactivity)**: 3-second breathing pulse cycle (0.5s fade-in, 0.5s fade-out using quadratic gamma correction, followed by 2.0s dark period). Peak brightness is user-configurable from 0% to 100% (default 50%) via `led_peak_percent` in `config.ini`.
  - **Battery Critical Mode**: 100 ms full-brightness (100% duty) flash every 1 second (20 ticks @ 50 ms) to alert user.

---

## USB Power Presence & Battery Port Detection

Heltec hardware lacks a dedicated hardware GPIO sensing USB VBUS (5V). The firmware infers power state through three complementary checks in `power_mgr_update()`:

1. **Host PC / Data Connection**:
   - Evaluated via `usb_serial_jtag_is_connected()`.
   - Returns `true` when active USB SOF/data packet exchanges occur with a host computer.
2. **Open Battery Connector Detection**:
   - When no physical battery is attached, the TP4054 charger rapidly cycles output voltage ("hunting" between ~2.5V and 4.2V).
   - The driver calculates sample variance across 8 consecutive ADC readings:
     $$\sigma^2 = \frac{1}{8}\sum_{i=0}^{7} (V_i - \bar{V})^2$$
   - A variance $> 200$ or measured voltage $< 1.0\text{V}$ identifies an unpopulated battery connector.
   - Sets `battery_present = false`, forces `POWER_MODE_USB`, and clears critical alerts.
3. **Wall Charger / Dumb Power Bank Detection**:
   - Wall adapters provide no USB data lines (`usb_serial_jtag_is_connected()` returns `false`).
   - If a battery is present and measured terminal voltage exceeds **4.18 V** (the float/cutoff voltage established by the TP4054 charger), the subsystem classifies power as `POWER_MODE_USB` (`usb_connected = true`).

---

## Battery State & Percentage Calculation

- **Chemistry**: Single-cell lithium-polymer / lithium-ion ($3.0\text{V}$ cut-off to $4.2\text{V}$ float).
- **Linear Capacity Mapping**:
  - Formula:
    $$\text{Percentage} = \operatorname{clamp}\left( \frac{V_{\text{bat}} - 3.30\text{V}}{4.15\text{V} - 3.30\text{V}} \times 100,\; 0,\; 100 \right)$$
  - $\ge 4.15\text{ V} \to 100\%$
  - $3.85\text{ V} \to 65\%$
  - $3.65\text{ V} \to 41\%$
  - $3.30\text{ V} \to 0\%$
- **Operating Thresholds**:
  - `VBAT_MAX_V`: $4.10\text{V}$ (upper bound for UI color gradient)
  - `VBAT_LOW_V`: $3.45\text{V}$ (lower bound for UI color gradient)
  - `VBAT_CRITICAL_V`: $3.35\text{V}$ (runtime critical threshold triggering emergency shutdown countdown)
  - `VBAT_CRITICAL_RECOVERY_V`: $3.50\text{V}$ (hysteresis threshold required to exit critical mode on battery)
  - Early boot brownout cut-off: $\le 3.25\text{V}$ on battery

---

## Dynamic RF Power Scaling

To prevent USB rail collapse and brownouts caused by simultaneous charging current (~500 mA) and Wi-Fi transmit bursts (~400 mA), Wi-Fi power dynamically scales according to power source and battery charge level:

1. **Initial Startup Cap**:
   - Wi-Fi initialization in `wifi_manager_init()` enforces an initial TX power cap of **15 dBm (60 units)** via `esp_wifi_set_max_tx_power(60)` before connecting to the AP.
   - Modem sleep is enabled (`esp_wifi_set_ps(WIFI_PS_MIN_MODEM)`).
2. **Runtime Dynamic Scaling (`wifi_manager_update_power`)**:
   - **Low Battery ($V_{\text{bat}} < 3.65\text{V}$)**:
     - Wi-Fi TX power throttles to **13 dBm (~20 mW / 52 units)** even when connected to USB. Leaves full current budget for the TP4054 charger.
   - **Actively Charging on USB ($3.65\text{V} \le V_{\text{bat}} < 4.10\text{V}$ with USB connected)**:
     - Wi-Fi TX power is capped at **15 dBm (~31 mW / 60 units)**. Prevents overloading 500 mA USB host ports and weak wall adapters while the TP4054 draws 500 mA.
   - **Pure USB (No Battery attached) OR Fully Charged on USB ($V_{\text{bat}} \ge 4.10\text{V}$) OR Healthy Battery ($V_{\text{bat}} \ge 3.75\text{V}$ on battery)**:
     - Wi-Fi TX power boosts to full **18 dBm (~63 mW / 72 units)** for maximum wireless broadcast range.
   - **Discharging Battery Mid-Range ($3.65\text{V} \le V_{\text{bat}} < 3.75\text{V}$ on battery)**:
     - Wi-Fi operates at **15 dBm (~31 mW / 60 units)**.
3. **LoRa Transmit Power**:
   - SX1262 LoRa emissions remain at full **22 dBm (160 mW)**. Time beacon packets are small (< 64 bytes), brief (< 100 ms airtime), and strictly duty-cycled (once every 60 seconds), keeping average power drain negligible.

---

## Early Boot Brownout Guard & Emergency Sleep State Machine

### 1. Early Boot Brownout Guard (`power_mgr_early_battery_guard`)
- Invoked in `app_main()` before radios (Wi-Fi, BLE, LoRa) or the 2.5-second USB configuration prompt.
- Reads initial battery voltage via `power_mgr_update()`.
- **Pure USB Mode (`!battery_present`)**: Skips deep sleep and boots normally.
- **USB Connected (`usb_connected == true`)**: Continues boot even if battery is low, allowing the device to charge in throttled Wi-Fi mode.
- **Battery-Only with Depleted Cell ($V_{\text{bat}} \le 3.25\text{V}$)**:
  - Turns on display backlight for 1.5 seconds to display warning.
  - Turns off display backlight and deasserts `PIN_VEXT_CTRL` to cut peripheral power rails.
  - Waits for USER button release if held down to prevent an immediate wake loop.
  - Enables internal pull-up on `PIN_USER_BTN` (`gpio_pullup_en`).
  - Configures dual wake-up sources: 60-second timer (`esp_sleep_enable_timer_wakeup`) and ext0 USER button wake (`esp_sleep_enable_ext0_wakeup(PIN_USER_BTN, 0)`).
  - Enters deep sleep (`esp_deep_sleep_start()`) to protect the lithium cell from over-discharge.

### 2. Runtime Critical Battery Shutdown Countdown
- Evaluated every second in the display task loop (`power_mgr_check_emergency_sleep`).
- **Trigger**: Battery present and $V_{\text{bat}} \le 3.35\text{V}$ on battery power (`!usb_active`).
- **Action**:
  - Enters `POWER_MODE_BATTERY_CRITICAL`.
  - Arms a 30-second shutdown countdown (`s_status.critical_countdown_sec = 30`).
  - Wakes screen backlight if it was off.
  - Flashes status LED at 100 ms on / 900 ms off.
  - Display overrides normal time screens with full-screen emergency alert:
    ```
    ! CRITICAL BATTERY !
    CONNECT USB CHARGER
    SHUTDOWN IN: <sec>s
    VOLTAGE: <v>V
    ```
    rendered in red (`COLOR_RED`) on black.
- **Voltage Bounce Hysteresis**:
  - As current load drops or cell relaxes, battery voltage can bounce from 3.30V to ~3.40V.
  - If already in critical mode on battery, the state machine holds critical mode and keeps the countdown ticking as long as $V_{\text{bat}} < 3.50\text{V}$ (`VBAT_CRITICAL_RECOVERY_V`).
  - Only charging above 3.50V or connecting USB will clear the critical latch.
- **Plugging into USB during Countdown**:
  - The countdown timer is immediately disarmed (`critical_countdown_sec = -1`).
  - Normal UI resumes; status line displays `[CHG] <v>V`.
- **Unplugging from USB while still Critical**:
  - If USB power is removed and voltage remains $\le 3.35\text{V}$, the countdown immediately re-arms to 30 seconds.
- **Countdown Expiry (`critical_countdown_sec == 0`)**:
  - Turns off display backlight (`display_set_backlight(false)`).
  - Stops Wi-Fi radio (`esp_wifi_stop()`).
  - Enters deep sleep (`esp_deep_sleep_start()`).

---

## Telemetry Reporting & UI Indicators

### 1. Adaptive Display Theming
- **Background**: Black (`0x0000`).
- **Foreground Text Color (`power_mgr_get_fg_color`)**:
  - **USB Mode**: White (`0xFFFF`).
  - **Critical Battery Mode**: Red (`0xF800`).
  - **Battery Mode**: Dynamic RGB565 color gradient interpolating smoothly between Electric Cyan / Sky Blue (`0x07FF` at $V_{\text{bat}} \ge 4.10\text{V}$) and Bright Yellow (`0xFFE0` at $V_{\text{bat}} \le 3.45\text{V}$).

### 2. Status Line Indicators
- **USB Mode with Battery Present**: `N:<ntp> L:<lora> [BT] 3.9V*` (asterisk denotes external power / charging).
- **Pure USB Mode (No Battery)**: `N:<ntp> L:<lora> [BT] USB`.
- **USB Connected with Low Battery**: `N:<ntp> L:<lora> [CHG] 3.3V`.
- **Critical Battery on Battery Power**: `LOW BAT! SLEEP:<sec>s`.
- **Normal Battery Operation**: `N:<ntp> L:<lora> [BT] 3.8V`.

### 3. Remote Telemetry Broadcasting
- **Bluetooth Low Energy (BLE CTS)**:
  - Invokes `ble_cts_service_update_power(percentage, voltage_mv, usb_connected)` to publish battery level, terminal voltage in mV, and USB power status to connected BLE clients.
- **LoRa Meshtastic Telemetry Packet**:
  - `lora_time_beacons.c` queries `power_mgr_get_status()` every 60 seconds:
    - Encodes 0% to 100% when running on battery.
    - Encodes 101% when operating on external USB power.
