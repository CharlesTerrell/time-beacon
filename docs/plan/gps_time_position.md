# GPS Time and Position Subsystem

## Overview
The GPS subsystem provides Stratum-1 time synchronization and geographic coordinates using an onboard GNSS receiver (such as Unicore UC6580 or equivalent) coupled with a hardware Pulse-Per-Second (PPS) output.

## Hardware Interface
- **UART Port**: Dedicated UART interface connecting ESP32-S3 to GNSS module TX/RX.
- **Baud Rate**: Typically 115200 bps (configured in `board_pins.h`).
- **PPS Pin**: Connected to an interrupt-capable GPIO configured with a positive-edge trigger.
- **Power / Reset Control**: GPIO pin used to assert reset or toggle power to the GNSS receiver during cold/warm start.

## Time Discipline Architecture
1. **NMEA Stream Parsing**:
   - Parses standard sentences: `$GPRMC`, `$GPGGA`, `$GNRMC`, `$GNGGA`.
   - Extracts UTC date, UTC time (hours, minutes, seconds), latitude, longitude, altitude, fix quality, and satellite count.
   - Converts UTC timestamp to Unix epoch seconds (`time_t`).

2. **PPS Hardware Discipline**:
   - The PPS interrupt fires at the exact top-of-second edge.
   - On each PPS pulse, the driver captures `esp_timer_get_time()` (microsecond hardware counter).
   - Correlates the parsed second from NMEA with the microsecond timer edge to calculate clock offset.
   - Sets the ESP32-S3 internal system clock (`settimeofday()`) and provides microsecond-accurate timebase offsets for NTP and BLE CTS.

3. **Stratum-1 State Machine**:
   - `NO_FIX`: GNSS receiver searching; system relies on free-running internal timer or RTC.
   - `ACQUIRING`: Valid NMEA time received, waiting for stable PPS pulse train.
   - `LOCKED`: PPS pulse train active, jitter within bounds, Stratum-1 status declared.
   - `HOLDOVER`: PPS signal lost temporarily; internal high-resolution timer maintains clock until fix re-acquired.

## Public APIs
Defined in `main/include/gps_mgr.h`:
- `gps_mgr_init()`: Configures UART, PPS GPIO ISR, and parses NMEA task.
- `gps_mgr_get_time()`: Returns current disciplined UTC time with sub-second precision.
- `gps_mgr_get_fix()`: Returns current fix status, coordinates, altitude, and satellite metrics.
