#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize and start the LoRa time beacon task.
 * Periodically transmits:
 *   - Meshtastic POSITION_APP time pulses alternating between LongFast (SF11)
 *     and MediumFast (SF9) every minute.
 *   - MeshCore time beacon advertisement packets with Stratum-1 Unix epoch timestamps.
 */
esp_err_t lora_time_beacons_start(void);

/**
 * Get total number of transmitted LoRa time beacon packets.
 */
uint32_t lora_time_beacons_get_count(void);

/**
 * Get the name of the last transmitted LoRa beacon mode.
 */
const char *lora_time_beacons_get_last_mode(void);

#ifdef __cplusplus
}
#endif
