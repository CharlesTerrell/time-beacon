#include "nmea_parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

uint8_t hex_to_byte(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

bool nmea_verify_checksum(const char *sentence) {
    if (sentence == NULL) {
        return false;
    }

    const char *start = strchr(sentence, '$');
    if (start == NULL) {
        return false;
    }

    const char *end = strchr(start, '*');
    if (end == NULL) {
        return false;
    }

    uint8_t checksum = 0;
    for (const char *p = start + 1; p < end; p++) {
        checksum ^= *p;
    }

    if (end + 2 >= sentence + strlen(sentence)) {
        return false;
    }

    uint8_t expected_checksum = 0;
    expected_checksum = (hex_to_byte(end[1]) << 4) | hex_to_byte(end[2]);

    return checksum == expected_checksum;
}

static bool get_field(const char *sentence, int field_idx, char *out, size_t max_len) {
    const char *p = sentence;
    if (*p == '$') p++;
    for (int i = 0; i < field_idx; i++) {
        p = strchr(p, ',');
        if (!p) return false;
        p++;
    }
    size_t len = 0;
    while (p[len] && p[len] != ',' && p[len] != '*' && p[len] != '\r' && p[len] != '\n') {
        if (len + 1 < max_len) {
            out[len] = p[len];
            len++;
        } else {
            break;
        }
    }
    out[len] = '\0';
    return true;
}

bool nmea_parse_rmc(const char *sentence, nmea_time_t *out_time) {
    if (!nmea_verify_checksum(sentence)) {
        return false;
    }

    char field[16];
    if (!get_field(sentence, 0, field, sizeof(field))) {
        return false;
    }
    if (strcmp(field, "GNRMC") != 0 && strcmp(field, "GPRMC") != 0 &&
        strcmp(field, "$GNRMC") != 0 && strcmp(field, "$GPRMC") != 0) {
        return false;
    }

    if (!get_field(sentence, 2, field, sizeof(field)) || field[0] != 'A') {
        return false;
    }

    if (!get_field(sentence, 1, field, sizeof(field))) {
        return false;
    }

    int hour = 0, minute = 0, second = 0;
    sscanf(field, "%02d%02d%02d.%*f", &hour, &minute, &second);
    out_time->hour = (uint8_t)hour;
    out_time->minute = (uint8_t)minute;
    out_time->second = (uint8_t)second;

    if (!get_field(sentence, 9, field, sizeof(field))) {
        return false;
    }

    int day = 0, month = 0, year = 0;
    sscanf(field, "%02d%02d%02d", &day, &month, &year);
    out_time->day = (uint8_t)day;
    out_time->month = (uint8_t)month;
    out_time->year = (uint16_t)(year + 2000);

    out_time->fix_valid = true;
    return true;
}

bool nmea_parse_zda(const char *sentence, nmea_time_t *out_time) {
    if (!nmea_verify_checksum(sentence)) {
        printf("Checksum verification failed for ZDA sentence.\n");
        return false;
    }

    char field[16];
    if (!get_field(sentence, 0, field, sizeof(field))) {
        return false;
    }
    if (strcmp(field, "GNZDA") != 0 && strcmp(field, "GPZDA") != 0 &&
        strcmp(field, "$GNZDA") != 0 && strcmp(field, "$GPZDA") != 0) {
        printf("Invalid sentence header for ZDA: %s\n", field);
        return false;
    }

    if (!get_field(sentence, 1, field, sizeof(field))) {
        return false;
    }
    int hour = 0, minute = 0, second = 0;
    sscanf(field, "%02d%02d%02d.%*f", &hour, &minute, &second);
    out_time->hour = (uint8_t)hour;
    out_time->minute = (uint8_t)minute;
    out_time->second = (uint8_t)second;

    if (!get_field(sentence, 2, field, sizeof(field))) {
        return false;
    }
    int day = 0;
    sscanf(field, "%02d", &day);
    out_time->day = (uint8_t)day;

    if (!get_field(sentence, 3, field, sizeof(field))) {
        return false;
    }
    int month = 0;
    sscanf(field, "%02d", &month);
    out_time->month = (uint8_t)month;

    if (!get_field(sentence, 4, field, sizeof(field))) {
        return false;
    }
    int year = 0;
    sscanf(field, "%04d", &year);
    out_time->year = (uint16_t)year;

    out_time->fix_valid = true;
    return true;
}

bool nmea_parse_gga(const char *sentence, uint8_t *out_sats, bool *out_fix_valid) {
    if (!nmea_verify_checksum(sentence)) {
        return false;
    }

    char field[16];
    if (!get_field(sentence, 0, field, sizeof(field))) {
        return false;
    }
    if (strcmp(field, "GNGGA") != 0 && strcmp(field, "GPGGA") != 0 &&
        strcmp(field, "$GNGGA") != 0 && strcmp(field, "$GPGGA") != 0) {
        return false;
    }

    if (get_field(sentence, 6, field, sizeof(field))) {
        int quality = 0;
        sscanf(field, "%d", &quality);
        if (out_fix_valid) {
            *out_fix_valid = (quality > 0);
        }
    }

    if (get_field(sentence, 7, field, sizeof(field))) {
        int sats = 0;
        sscanf(field, "%d", &sats);
        if (out_sats) {
            *out_sats = (uint8_t)(sats < 0 ? 0 : (sats > 255 ? 255 : sats));
        }
    }

    return true;
}
