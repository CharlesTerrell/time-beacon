#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start the FreeRTOS GNSS reader and parser background task.
 * Task is pinned to Core 1 at priority 12.
 * Reads UART byte stream, reassembles NMEA lines, and feeds time_sync.
 */
esp_err_t gnss_task_start(void);

#ifdef __cplusplus
}
#endif
