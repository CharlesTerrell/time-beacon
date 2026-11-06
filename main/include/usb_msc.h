#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start TinyUSB Mass Storage Class mode.
 * Exposes the internal FATFS 'storage' partition as a USB Flash Drive to PC.
 */
esp_err_t usb_msc_start(void);

/**
 * Blocking loop for USB MSC mode.
 * Monitors USB status and USER button press.
 * Exits / reboots when user presses button to exit config mode.
 */
void usb_msc_run_loop(void);

#ifdef __cplusplus
}
#endif
