#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize and start BLE GATT Current Time Service (CTS).
 * Sets up NimBLE host, registers GAP/GATT/DIS/CTS services,
 * and starts BLE advertising with the given device name.
 */
esp_err_t ble_cts_service_start(const char *device_name);

/**
 * Notify connected BLE clients that current time has been updated.
 */
void ble_cts_service_notify_time(void);

/**
 * Update BLE Battery Service (BAS) and power status telemetry.
 */
void ble_cts_service_update_power(uint8_t percentage, uint16_t voltage_mv, bool usb_connected);

/**
 * Check if a BLE client is currently connected.
 */
bool ble_cts_service_is_connected(void);

#ifdef __cplusplus
}
#endif
