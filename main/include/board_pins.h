#pragma once

#include "sdkconfig.h"

/**
 * Heltec Hardware Pin Definitions & Board Support Package (BSP) Selector
 */

#if defined(CONFIG_BOARD_HELTEC_V2)
#include "../boards/board_tracker_v2.h"
#elif defined(CONFIG_BOARD_HELTEC_WIFI_LORA_V4)
#include "../boards/board_wifi_lora_v4.h"
#else /* Default: Heltec Wireless Tracker v1.1 */
#include "../boards/board_tracker_v1_1.h"
#endif
