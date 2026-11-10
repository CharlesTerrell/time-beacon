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
    bool        pps_synced;              /* True if hardware 1PPS pulse is actively driving time */
    uint32_t    pps_pulse_count;         /* Total 1PPS interrupt pulses received */
    uint8_t     satellites;              /* Count of satellites in use */
    bool        has_pos;                 /* True if coordinates are valid */
    int32_t     latitude_i;              /* Lat degrees * 1e7 */
    int32_t     longitude_i;             /* Lon degrees * 1e7 */
    int32_t     altitude_m;              /* Altitude in meters */
    nmea_time_t last_fix_time;           /* Last parsed UTC timestamp */
    int64_t     last_fix_esp_time_us;    /* esp_timer_get_time() when last fix arrived */
    int64_t     last_pps_esp_time_us;    /* esp_timer_get_time() of last 1PPS pulse */
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
 * Update current satellite count. Thread-safe.
 */
void time_sync_update_sats(uint8_t satellites);

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

/**
 * Check if system clock is currently disciplined directly to hardware 1PPS.
 */
bool time_sync_is_pps_locked(void);

#ifdef __cplusplus
}
#endif
