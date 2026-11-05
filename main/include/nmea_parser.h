#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t year;       /* e.g. 2026 */
    uint8_t  month;      /* 1 - 12 */
    uint8_t  day;        /* 1 - 31 */
    uint8_t  hour;       /* 0 - 23 */
    uint8_t  minute;     /* 0 - 59 */
    uint8_t  second;     /* 0 - 59 */
    uint32_t microseconds; /* 0 - 999999 */
    bool     fix_valid;  /* true if fix status is active ('A') */
    uint8_t  satellites; /* satellite count if present */
} nmea_time_t;

/**
 * Validate standard NMEA XOR checksum (*XX\r\n).
 * Returns true if valid, false if corrupted or missing.
 */
bool nmea_verify_checksum(const char *sentence);

/**
 * Parse $GNRMC / $GPRMC sentence into nmea_time_t.
 * Updates out_time on successful parse.
 * Returns true if sentence was valid RMC and fix was valid ('A').
 */
bool nmea_parse_rmc(const char *sentence, nmea_time_t *out_time);

/**
 * Parse $GNZDA / $GPZDA sentence into nmea_time_t.
 * Updates out_time on successful parse.
 * Returns true if sentence was valid ZDA.
 */
bool nmea_parse_zda(const char *sentence, nmea_time_t *out_time);

#ifdef __cplusplus
}
#endif
