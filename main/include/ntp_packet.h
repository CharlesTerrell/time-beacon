#pragma once

#include <stdint.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <assert.h>

#define NTP_PORT                123
#define NTP_EPOCH_OFFSET_SEC    2208988800ULL  /* Seconds between 1900 and 1970 */
#define NTP_REF_ID_GPS          0x47505300ULL  /* "GPS\0" */
#define NTP_REF_ID_INIT         0x494E4954ULL  /* "INIT" */

#pragma pack(push, 1)

typedef struct {
    uint32_t seconds;   /* Big-endian seconds since 1900 */
    uint32_t fraction;  /* Big-endian 32-bit fractional second (2^32 = 1 sec) */
} ntp_time_t;

typedef struct {
    uint8_t    li_vn_mode;      /* Leap (2 bits), Version (3 bits), Mode (3 bits) */
    uint8_t    stratum;         /* Stratum (1 = primary GPS, 16 = unsynchronized) */
    int8_t     poll;            /* Poll interval exponent */
    int8_t     precision;       /* Clock precision exponent (-18 ~ 3.8 us) */
    uint32_t   root_delay;      /* Root delay (16.16 fixed point) */
    uint32_t   root_dispersion; /* Root dispersion (16.16 fixed point) */
    uint32_t   ref_id;          /* Reference clock identifier (e.g. "GPS\0") */
    ntp_time_t ref_time;        /* Reference timestamp */
    ntp_time_t origin_time;     /* Origin timestamp (copied from client transmit) */
    ntp_time_t receive_time;    /* Server receive timestamp */
    ntp_time_t transmit_time;   /* Server transmit timestamp */
} ntp_packet_t;

#pragma pack(pop)

static_assert(sizeof(ntp_packet_t) == 48, "RFC 5905 NTP packet must be exactly 48 bytes");

/**
 * Convert POSIX struct timeval to NTP timestamp with network byte order.
 */
static inline ntp_time_t timeval_to_ntp(const struct timeval *tv) {
    ntp_time_t ntp;
    ntp.seconds = htonl((uint32_t)(tv->tv_sec + NTP_EPOCH_OFFSET_SEC));
    /* Convert microseconds to 32-bit fraction: fraction = (us * 2^32) / 1000000 */
    uint32_t frac = (uint32_t)(((uint64_t)tv->tv_usec << 32) / 1000000ULL);
    ntp.fraction = htonl(frac);
    return ntp;
}
