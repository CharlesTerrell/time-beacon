# GNSS Time and Position Subsystem

## Overview
The GNSS subsystem provides Stratum-1 time synchronization and precision geographic coordinates using an onboard multi-constellation GNSS receiver (Unicore UC6580 or equivalent). It disciplines the ESP32-S3 internal system clock with sub-millisecond precision to support NTP, BLE CTS, and LoRa time beacon services.

---

## Hardware Interface & Board BSP Differences

### 1. Pin Assignments & UART Configuration
- **UART Port**: UART1 (`GNSS_UART_NUM`) at 115200 baud, 8N1.
- **Serial Pins**:
  - Tracker v1.1 / v2: RX on GPIO 33, TX on GPIO 34, Reset on GPIO 35.
  - WiFi LoRa 32 V4: RX on GPIO 39, TX on GPIO 38, Reset on GPIO 42, Wakeup on GPIO 40.
- **Power Rail Control (`PIN_VEXT_CTRL`)**: Active HIGH GPIO (GPIO 3 on Tracker v1.1/v2, GPIO 36 on WiFi LoRa 32 V4) gates external power to the GNSS module. Cut during deep sleep to eliminate quiescent draw.

### 2. PPS Hardware Routing Differences
- **Heltec Wireless Tracker v1.1 (`BOARD_HAS_GNSS_PPS 0`)**:
  - The hardware 1PPS trace is unrouted on the v1.1 PCB.
  - The time discipline engine relies on NMEA sentence arrival timing with calibrated UART transmission latency to lock system time.
- **Heltec Wireless Tracker v2 & WiFi LoRa 32 V4 (`BOARD_HAS_GNSS_PPS 1`)**:
  - Active hardware 1PPS trace routed to GPIO 36 (Tracker v2) or GPIO 41 (WiFi LoRa 32 V4).
  - Configured as a GPIO positive-edge interrupt (`gpio_isr_handler_add()`) capturing `esp_timer_get_time()` with sub-microsecond precision at the top of every second.

---

## Subsystem Architecture & Modular Components

The GNSS subsystem is partitioned into modular, decoupled layers:

### 1. NMEA Protocol Parser (`nmea_parser.c`)
- Stream-based parser validating NMEA-0183 checksums.
- Evaluates `$GNRMC`, `$GNGGA`, `$GPRMC`, and `$GPGGA` sentences.
- Extracts UTC timestamp (year, month, day, hour, minute, second), satellite count, 3D fix quality, altitude, and geographic coordinates ($\text{degrees} \times 10^7$).

### 2. Low-Level Serial Driver (`gnss_driver.c`)
- Manages the ESP-IDF UART ring buffer and reset sequencing.
- Handles optional raw NMEA USB pass-through: when `usb_nmea_output = true` in `config.ini`, system logs are muted (`esp_log_level_set("*", ESP_LOG_NONE)`) and incoming NMEA strings are streamed directly to `usb_serial_jtag` for diagnostic software (e.g. GPSD, u-center).

### 3. Dedicated Background Task (`gnss_task.c`)
- Runs as an independent FreeRTOS task pinned to Core 1.
- Continuously ingests UART stream bytes, feeds the NMEA parser, and notifies the time synchronization engine when a complete sentence arrives.

### 4. Precision Time Synchronization Engine (`time_sync.c`)
- Combines NMEA time with hardware PPS interrupts (or sentence rollover fallback).
- Disciplines system time (`settimeofday()`) with smoothed top-of-second epoch timestamps.
- Tracks lock state:
  - `has_time`: Valid UTC time received from GNSS constellation.
  - `has_pos`: Valid 2D/3D fix with geographic coordinates.
  - `locked`: Valid top-of-second discipline active (Stratum-1 status declared).

---

## Public APIs

- `gnss_driver_init(void)`: Configures UART, reset line, and PPS ISR (if routed).
- `gnss_task_start(void)`: Launches background GNSS parsing task.
- `time_sync_init(void)`: Initializes system clock discipline state.
- `time_sync_get_status(gnss_status_t *out_status)`: Retrieves current fix status, satellite count, coordinates, and lock condition.
- `time_sync_get_time(struct timeval *tv)`: Returns disciplined UTC time with microsecond resolution.
