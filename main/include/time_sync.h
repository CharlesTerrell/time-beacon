#pragma once

#include "esp_err.h"
#include "nmea_parser.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool        fix_valid;               /* True if GNSS reports valid fix */
    uint8_t     satellites;              /* Count of satellites in use */
    nmea_time_t last_fix_time;           /* Last parsed UTC timestamp */
    int64_t     last_fix_esp_time_us;    /* esp_timer_get_time() when last fix arrived */
    uint32_t    sentences_parsed;        /* Count of valid NMEA sentences */
    uint32_t    checksum_errors;         /* Count of corrupt sentences */
} gnss_status_t;

/**
 * Initialize time sync module (creates mutexes, initializes state).
 */
esp_err_t time_sync_init(void);

/**
 * Update system time and internal state with fresh GNSS fix.
 * Calls settimeofday() disciplined to the esp_timer anchor.
 * Thread-safe.
 */
void time_sync_update_gnss(const nmea_time_t *time_data, int64_t anchor_esp_time_us);

/**
 * Record a checksum error event. Thread-safe.
 */
void time_sync_record_error(void);

/**
 * Copy current GNSS status snapshot into out_status. Thread-safe.
 * Returns true if GNSS currently has a valid active fix (lock < 5 seconds old).
 */
bool time_sync_get_status(gnss_status_t *out_status);

/**
 * Check if system clock is currently synchronized to GNSS.
 * Returns false if fix lost or no fix received in > 5 seconds.
 */
bool time_sync_is_synchronized(void);

#ifdef __cplusplus
}
#endif
