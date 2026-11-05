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
        printf("Checksum verification failed for RMC sentence.\n");
        return false;
    }

    char field[10];
    if (!get_field(sentence, 0, field, sizeof(field)) || 
        (strcmp(field, "$GNRMC") != 0 && strcmp(field, "$GPRMC") != 0)) {
        printf("Invalid sentence header for RMC: %s\n", field);
        return false;
    }

    if (!get_field(sentence, 2, field, sizeof(field)) || field[0] != 'A') {
        printf("Invalid status for RMC: %s\n", field);
        return false;
    }

    if (!get_field(sentence, 1, field, sizeof(field))) {
        printf("Failed to get time field for RMC.\n");
        return false;
    }
    printf("Time field for RMC: %s\n", field);

    int hour, minute, second;
    sscanf(field, "%02d%02d%02d.%*f", &hour, &minute, &second);
    out_time->hour = (uint8_t)hour;
    out_time->minute = (uint8_t)minute;
    out_time->second = (uint8_t)second;

    if (!get_field(sentence, 9, field, sizeof(field))) {
        printf("Failed to get date field for RMC.\n");
        return false;
    }
    printf("Date field for RMC: %s\n", field);

    int day, month, year;
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

    char field[10];
    if (!get_field(sentence, 0, field, sizeof(field)) || strcmp(field, "$GNZDA") != 0) {
        printf("Invalid sentence header for ZDA: %s\n", field);
        return false;
    }

    if (!get_field(sentence, 1, field, sizeof(field))) {
        printf("Failed to get time field for ZDA.\n");
        return false;
    }
    printf("Time field for ZDA: %s\n", field);

    int hour, minute, second;
    sscanf(field, "%02d%02d%02d.%*f", &hour, &minute, &second);
    out_time->hour = (uint8_t)hour;
    out_time->minute = (uint8_t)minute;
    out_time->second = (uint8_t)second;

    if (!get_field(sentence, 2, field, sizeof(field))) {
        printf("Failed to get day field for ZDA.\n");
        return false;
    }
    printf("Day field for ZDA: %s\n", field);

    if (!get_field(sentence, 3, field, sizeof(field))) {
        printf("Failed to get month field for ZDA.\n");
        return false;
    }
    printf("Month field for ZDA: %s\n", field);

    if (!get_field(sentence, 4, field, sizeof(field))) {
        printf("Failed to get year field for ZDA.\n");
        return false;
    }
    printf("Year field for ZDA: %s\n", field);

    int day, month, year;
    sscanf(field, "%02d", &day);
    out_time->day = (uint8_t)day;

    sscanf(field, "%02d", &month);
    out_time->month = (uint8_t)month;

    sscanf(field, "%04d", &year);
    out_time->year = (uint16_t)year;

    out_time->fix_valid = true;
    return true;
}
