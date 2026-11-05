#include "nmea_parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static bool nmea_verify_checksum(const char *sentence) {
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

static uint8_t hex_to_byte(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

bool nmea_parse_rmc(const char *sentence, nmea_time_t *out_time) {
    if (!nmea_verify_checksum(sentence)) {
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

    char buffer[end - start + 1];
    strncpy(buffer, start, end - start);
    buffer[end - start] = '\0';

    char *token = strtok(buffer, ",");
    if (token == NULL || strcmp(token, "$GNRMC") != 0 && strcmp(token, "$GPRMC") != 0) {
        return false;
    }

    token = strtok(NULL, ",");
    if (token == NULL || token[0] != 'A') {
        return false;
    }

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    sscanf(token, "%02d%02d%02d.%*f", &out_time->hour, &out_time->minute, &out_time->second);

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    sscanf(token, "%02d%02d%02d", &out_time->day, &out_time->month, &out_time->year);
    out_time->year += 2000;

    return true;
}

bool nmea_parse_zda(const char *sentence, nmea_time_t *out_time) {
    if (!nmea_verify_checksum(sentence)) {
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

    char buffer[end - start + 1];
    strncpy(buffer, start, end - start);
    buffer[end - start] = '\0';

    char *token = strtok(buffer, ",");
    if (token == NULL || strcmp(token, "$GZDA") != 0) {
        return false;
    }

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    sscanf(token, "%02d%02d%02d.%*f", &out_time->hour, &out_time->minute, &out_time->second);

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    sscanf(token, "%02d", &out_time->day);

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    sscanf(token, "%02d", &out_time->month);

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    sscanf(token, "%04d", &out_time->year);

    return true;
}
