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
    if (token == NULL || (strcmp(token, "$GNRMC") != 0 && strcmp(token, "$GPRMC") != 0)) {
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

    int hour, minute, second;
    sscanf(token, "%02d%02d%02d.%*f", &hour, &minute, &second);
    out_time->hour = (uint8_t)hour;
    out_time->minute = (uint8_t)minute;
    out_time->second = (uint8_t)second;

    for (int i = 0; i < 8; i++) {
        token = strtok(NULL, ",");
        if (token == NULL) {
            return false;
        }
    }

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    int day, month, year;
    sscanf(token, "%02d%02d%02d", &day, &month, &year);
    out_time->day = (uint8_t)day;
    out_time->month = (uint8_t)month;
    out_time->year = (uint16_t)(year + 2000);

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
    if (token == NULL || strcmp(token, "$GNZDA") != 0) {
        return false;
    }

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    int hour, minute, second;
    sscanf(token, "%02d%02d%02d.%*f", &hour, &minute, &second);
    out_time->hour = (uint8_t)hour;
    out_time->minute = (uint8_t)minute;
    out_time->second = (uint8_t)second;

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    int day;
    sscanf(token, "%02d", &day);
    out_time->day = (uint8_t)day;

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    int month;
    sscanf(token, "%02d", &month);
    out_time->month = (uint8_t)month;

    token = strtok(NULL, ",");
    if (token == NULL) {
        return false;
    }

    int year;
    sscanf(token, "%04d", &year);
    out_time->year = (uint16_t)year;

    return true;
}
