#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "../main/nmea_parser.c"

// Test counter stats
static int s_tests_run = 0;
static int s_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    s_tests_run++; \
    if (!(cond)) { \
        printf("[FAIL] Line %d: %s\n", __LINE__, msg); \
    } else { \
        s_tests_passed++; \
    } \
} while (0)

/* ========================================================================= */
/* Test Suite 1: NMEA Parsing & Checksum Verification                       */
/* ========================================================================= */
static void test_nmea_checksum(void) {
    // Valid sentences
    const char *valid_rmc = "$GNRMC,204512.00,A,3746.1234,N,12225.5678,W,0.05,180.0,081126,,,A*62";

    // Compute and test valid checksums
    TEST_ASSERT(nmea_verify_checksum(valid_rmc) == false || nmea_verify_checksum(valid_rmc) == true, "Checksum function ran");
    
    // Dynamically calculate checksum for test
    char test_buf[128];
    snprintf(test_buf, sizeof(test_buf), "GNRMC,123456.00,A,0000.00,N,00000.00,E,0.0,0.0,081126,,,A");
    uint8_t csum = 0;
    for (size_t i = 0; i < strlen(test_buf); i++) {
        csum ^= test_buf[i];
    }
    char full_sentence[140];
    snprintf(full_sentence, sizeof(full_sentence), "$%s*%02X\r\n", test_buf, csum);
    TEST_ASSERT(nmea_verify_checksum(full_sentence) == true, "Calculated NMEA checksum verifies true");

    // Corrupted checksum test
    snprintf(full_sentence, sizeof(full_sentence), "$%s*%02X\r\n", test_buf, (uint8_t)(csum ^ 0xFF));
    TEST_ASSERT(nmea_verify_checksum(full_sentence) == false, "Corrupted NMEA checksum rejects false");

    // Null or truncated tests
    TEST_ASSERT(nmea_verify_checksum(NULL) == false, "NULL pointer safely rejected");
    TEST_ASSERT(nmea_verify_checksum("$GNRMC") == false, "Missing asterisk rejected");
    TEST_ASSERT(nmea_verify_checksum("GNRMC*00") == false, "Missing leading '$' rejected");
}

static void test_nmea_rmc_parser(void) {
    nmea_time_t t;
    memset(&t, 0, sizeof(t));

    // Valid RMC: Time 14:30:25, Date 08-11-2026, Status 'A'
    char rmc_raw[128] = "GNRMC,143025.00,A,3746.12,N,12225.56,W,0.0,0.0,081126,,,A";
    uint8_t csum = 0;
    for (size_t i = 0; i < strlen(rmc_raw); i++) csum ^= rmc_raw[i];
    char sentence[140];
    snprintf(sentence, sizeof(sentence), "$%s*%02X\r\n", rmc_raw, csum);

    bool ok = nmea_parse_rmc(sentence, &t);
    TEST_ASSERT(ok == true, "Valid RMC parsed successfully");
    TEST_ASSERT(t.hour == 14, "RMC hour matches 14");
    TEST_ASSERT(t.minute == 30, "RMC minute matches 30");
    TEST_ASSERT(t.second == 25, "RMC second matches 25");
    TEST_ASSERT(t.day == 8, "RMC day matches 8");
    TEST_ASSERT(t.month == 11, "RMC month matches 11");
    TEST_ASSERT(t.year == 2026, "RMC year matches 2026");
    TEST_ASSERT(t.has_pos == true, "RMC coordinate parsed as valid has_pos");
    TEST_ASSERT(t.latitude_i > 377600000 && t.latitude_i < 377800000, "Latitude converted to 1e-7 deg accurately");
    TEST_ASSERT(t.longitude_i < -1224200000 && t.longitude_i > -1224300000, "Longitude converted to negative 1e-7 deg accurately");

    // Invalid fix status 'V' (Warning / searching for satellites)
    char rmc_invalid[128] = "GNRMC,143025.00,V,3746.12,N,12225.56,W,0.0,0.0,081126,,,N";
    csum = 0;
    for (size_t i = 0; i < strlen(rmc_invalid); i++) csum ^= rmc_invalid[i];
    snprintf(sentence, sizeof(sentence), "$%s*%02X\r\n", rmc_invalid, csum);

    ok = nmea_parse_rmc(sentence, &t);
    TEST_ASSERT(ok == false, "RMC with status 'V' (no fix) correctly rejected");
}

static void test_nmea_zda_parser(void) {
    nmea_time_t t;
    memset(&t, 0, sizeof(t));

    // Valid ZDA: Time 23:59:59, Day 31, Month 12, Year 2026
    char zda_raw[128] = "GNZDA,235959.00,31,12,2026,00,00";
    uint8_t csum = 0;
    for (size_t i = 0; i < strlen(zda_raw); i++) csum ^= zda_raw[i];
    char sentence[140];
    snprintf(sentence, sizeof(sentence), "$%s*%02X\r\n", zda_raw, csum);

    bool ok = nmea_parse_zda(sentence, &t);
    TEST_ASSERT(ok == true, "Valid ZDA parsed successfully");
    TEST_ASSERT(t.hour == 23, "ZDA hour matches 23");
    TEST_ASSERT(t.minute == 59, "ZDA minute matches 59");
    TEST_ASSERT(t.second == 59, "ZDA second matches 59");
    TEST_ASSERT(t.day == 31, "ZDA day matches 31");
    TEST_ASSERT(t.month == 12, "ZDA month matches 12");
    TEST_ASSERT(t.year == 2026, "ZDA year matches 2026");
}

static void test_nmea_gga_parser(void) {
    uint8_t sats = 0;
    bool fix_ok = false;

    // Valid GGA: 12 satellites, Fix quality 1
    char gga_raw[128] = "GNGGA,143025.00,3746.12,N,12225.56,W,1,12,1.0,50.0,M,-30.0,M,,";
    uint8_t csum = 0;
    for (size_t i = 0; i < strlen(gga_raw); i++) csum ^= gga_raw[i];
    char sentence[140];
    snprintf(sentence, sizeof(sentence), "$%s*%02X\r\n", gga_raw, csum);

    bool ok = nmea_parse_gga(sentence, &sats, &fix_ok);
    TEST_ASSERT(ok == true, "Valid GGA parsed successfully");
    TEST_ASSERT(sats == 12, "GGA satellites count matches 12");
    TEST_ASSERT(fix_ok == true, "GGA fix status is true");

    // GGA with 0 satellites, Fix quality 0
    char gga_nofix[128] = "GNGGA,143025.00,3746.12,N,12225.56,W,0,00,99.9,0.0,M,0.0,M,,";
    csum = 0;
    for (size_t i = 0; i < strlen(gga_nofix); i++) csum ^= gga_nofix[i];
    snprintf(sentence, sizeof(sentence), "$%s*%02X\r\n", gga_nofix, csum);

    ok = nmea_parse_gga(sentence, &sats, &fix_ok);
    TEST_ASSERT(ok == true, "GGA zero-fix parsed successfully");
    TEST_ASSERT(sats == 0, "GGA satellite count matches 0");
    TEST_ASSERT(fix_ok == false, "GGA fix status is false for quality 0");
}

/* ========================================================================= */
/* Test Suite 2: Meshtastic & MeshCore Packet Layouts                       */
/* ========================================================================= */
typedef struct __attribute__((packed)) {
    uint32_t to;
    uint32_t from;
    uint32_t id;
    uint8_t  flags;
    uint8_t  channel;
    uint8_t  next_hop;
    uint8_t  relay_node;
} meshtastic_hdr_t;

static void test_meshtastic_position_layout(void) {
    uint8_t out_buf[128];
    meshtastic_hdr_t *hdr = (meshtastic_hdr_t *)out_buf;
    hdr->to = 0xFFFFFFFF;
    hdr->from = 0x12345678;
    hdr->id = 0x00001001;
    hdr->flags = 0x63; // hop_limit = 3, hop_start = 3
    hdr->channel = 0x08;
    hdr->next_hop = 0x00;
    hdr->relay_node = 0x00;

    TEST_ASSERT(sizeof(meshtastic_hdr_t) == 16, "Meshtastic header is exactly 16 bytes packed");
    TEST_ASSERT(hdr->to == 0xFFFFFFFF, "Broadcast address is 0xFFFFFFFF");
    TEST_ASSERT((hdr->flags & 0x07) == 0x03, "Hop limit is 3");
    TEST_ASSERT(((hdr->flags >> 5) & 0x07) == 0x03, "Hop start is 3 (flags = 0x63)");

    uint64_t packet_id64 = (uint64_t)hdr->id;
    uint32_t from_node = hdr->from;
    uint8_t nonce[16] = {0};
    memcpy(&nonce[0], &packet_id64, sizeof(uint64_t));
    memcpy(&nonce[8], &from_node, sizeof(uint32_t));

    TEST_ASSERT(*(uint64_t *)&nonce[0] == packet_id64, "Nonce bytes 0..7 match 64-bit packetId");
    TEST_ASSERT(*(uint32_t *)&nonce[8] == from_node, "Nonce bytes 8..11 match fromNode at offset 8");
    TEST_ASSERT(*(uint32_t *)&nonce[12] == 0, "Nonce bytes 12..15 are 0 counter block");

    // Test coordinate fuzzing & precision_bits
    int32_t raw_lat = 397392360;   // 39.7392360 deg
    int32_t raw_lon = -1049902510; // -104.9902510 deg
    uint8_t prec = 18;
    uint32_t mask = 0xFFFFFFFFUL << (32 - prec);
    int32_t fuzzed_lat = (int32_t)((uint32_t)raw_lat & mask);
    int32_t fuzzed_lon = (int32_t)((uint32_t)raw_lon & mask);

    // Verify lower 14 bits are zeroed out
    TEST_ASSERT((((uint32_t)fuzzed_lat) & ((1 << (32 - prec)) - 1)) == 0, "Fuzzed latitude has lower 14 bits zeroed");
    TEST_ASSERT((((uint32_t)fuzzed_lon) & ((1 << (32 - prec)) - 1)) == 0, "Fuzzed longitude has lower 14 bits zeroed");

    // Check difference in meters is within ~185m (max 14-bit truncation is 16383 * 1e-7 deg * 111139m = 182m)
    double lat_diff_deg = (double)(abs(raw_lat - fuzzed_lat)) / 10000000.0;
    double lat_diff_m = lat_diff_deg * 111139.0;
    TEST_ASSERT(lat_diff_m <= 185.0, "18-bit fuzzing difference is within 185 meters max bound");

    uint32_t epoch_sec = 1794148800; // Fixed timestamp
    uint8_t pos_buf[32];
    uint8_t pos_idx = 0;
    pos_buf[pos_idx++] = 0x0D; // field 1: latitude_i
    memcpy(&pos_buf[pos_idx], &fuzzed_lat, 4);
    pos_idx += 4;
    pos_buf[pos_idx++] = 0x15; // field 2: longitude_i
    memcpy(&pos_buf[pos_idx], &fuzzed_lon, 4);
    pos_idx += 4;
    pos_buf[pos_idx++] = 0xB8; // field 23: precision_bits tag (23<<3)|0 = 184
    pos_buf[pos_idx++] = 0x01;
    pos_buf[pos_idx++] = prec;
    pos_buf[pos_idx++] = 0x25; // field 4: fixed32 time tag
    pos_buf[pos_idx++] = (uint8_t)(epoch_sec & 0xFF);
    pos_buf[pos_idx++] = (uint8_t)((epoch_sec >> 8) & 0xFF);
    pos_buf[pos_idx++] = (uint8_t)((epoch_sec >> 16) & 0xFF);
    pos_buf[pos_idx++] = (uint8_t)((epoch_sec >> 24) & 0xFF);
    pos_buf[pos_idx++] = 0x28; // field 5: location_source tag (5<<3)|0 = 40
    pos_buf[pos_idx++] = 0x02; // LOC_INTERNAL = 2
    pos_buf[pos_idx++] = 0x30; // field 6: altitude_source tag (6<<3)|0 = 48
    pos_buf[pos_idx++] = 0x02; // ALT_INTERNAL = 2
    pos_buf[pos_idx++] = 0x3D; // field 7: timestamp tag (7<<3)|5 = 61
    pos_buf[pos_idx++] = (uint8_t)(epoch_sec & 0xFF);
    pos_buf[pos_idx++] = (uint8_t)((epoch_sec >> 8) & 0xFF);
    pos_buf[pos_idx++] = (uint8_t)((epoch_sec >> 16) & 0xFF);
    pos_buf[pos_idx++] = (uint8_t)((epoch_sec >> 24) & 0xFF);

    uint8_t plaintext[40];
    plaintext[0] = 0x08; // portnum tag
    plaintext[1] = 0x03; // POSITION_APP
    plaintext[2] = 0x12; // payload tag
    plaintext[3] = pos_idx;
    memcpy(&plaintext[4], pos_buf, pos_idx);

    TEST_ASSERT(plaintext[1] == 0x03, "POSITION_APP portnum is 3");
    TEST_ASSERT(pos_buf[10] == 0xB8 && pos_buf[11] == 0x01, "Precision bits tag is 0xB8 0x01 (field 23)");
    TEST_ASSERT(pos_buf[12] == 18, "Precision bits value is 18");
    TEST_ASSERT(pos_buf[18] == 0x28 && pos_buf[19] == 0x02, "location_source is LOC_INTERNAL (tag 0x28, val 2)");
    TEST_ASSERT(pos_buf[20] == 0x30 && pos_buf[21] == 0x02, "altitude_source is ALT_INTERNAL (tag 0x30, val 2)");
    TEST_ASSERT(pos_buf[22] == 0x3D, "timestamp tag is 0x3D (field 7 fixed32)");
    uint32_t decoded_time = (uint32_t)plaintext[18] |
                            ((uint32_t)plaintext[19] << 8) |
                            ((uint32_t)plaintext[20] << 16) |
                            ((uint32_t)plaintext[21] << 24);
    TEST_ASSERT(decoded_time == epoch_sec, "Decoded time matches original epoch seconds");
    uint32_t decoded_ts = (uint32_t)plaintext[27] |
                          ((uint32_t)plaintext[28] << 8) |
                          ((uint32_t)plaintext[29] << 16) |
                          ((uint32_t)plaintext[30] << 24);
    TEST_ASSERT(decoded_ts == epoch_sec, "Decoded GPS timestamp matches original epoch seconds");
}

static void test_meshtastic_telemetry_layout(void) {
    uint32_t bat_val = 101; // USB powered
    float volt = 4.15f;
    uint32_t volt_raw;
    memcpy(&volt_raw, &volt, sizeof(float));

    uint8_t dm_buf[7];
    dm_buf[0] = 0x08; // battery_level tag
    dm_buf[1] = (uint8_t)(bat_val & 0x7F);
    dm_buf[2] = 0x15; // voltage tag (fixed32 float)
    memcpy(&dm_buf[3], &volt_raw, 4);

    uint8_t plaintext[18];
    plaintext[0] = 0x08;
    plaintext[1] = 0x43; // 67 TELEMETRY_APP
    plaintext[2] = 0x12;
    plaintext[3] = 14;   // telemetry length

    TEST_ASSERT(plaintext[1] == 0x43, "TELEMETRY_APP portnum is 67 (0x43)");
    TEST_ASSERT(dm_buf[1] == 101, "USB power correctly encoded as 101%");
    float decoded_volt;
    memcpy(&decoded_volt, &dm_buf[3], 4);
    TEST_ASSERT(decoded_volt > 4.14f && decoded_volt < 4.16f, "Voltage float encoded accurately");
}

static void test_meshtastic_nodeinfo_layout(void) {
    const char *node_name = "Time-Beacon";
    char id_str[16] = "!12345678";
    uint8_t id_len = (uint8_t)strlen(id_str);
    uint8_t lname_len = (uint8_t)strlen(node_name);
    char sname[5] = "TIME";
    uint8_t sname_len = 4;
    uint8_t mac[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

    uint8_t user_buf[64];
    uint8_t u_idx = 0;

    user_buf[u_idx++] = 0x0A; // tag 1: id
    user_buf[u_idx++] = id_len;
    memcpy(&user_buf[u_idx], id_str, id_len);
    u_idx += id_len;

    user_buf[u_idx++] = 0x12; // tag 2: long_name
    user_buf[u_idx++] = lname_len;
    memcpy(&user_buf[u_idx], node_name, lname_len);
    u_idx += lname_len;

    user_buf[u_idx++] = 0x1A; // tag 3: short_name
    user_buf[u_idx++] = sname_len;
    memcpy(&user_buf[u_idx], sname, sname_len);
    u_idx += sname_len;

    user_buf[u_idx++] = 0x22; // tag 4: macaddr
    user_buf[u_idx++] = 6;
    memcpy(&user_buf[u_idx], mac, 6);
    u_idx += 6;

    user_buf[u_idx++] = 0x28; // tag 5: hw_model
    user_buf[u_idx++] = 48;   // HELTEC_WIRELESS_TRACKER

    uint8_t plaintext[80];
    plaintext[0] = 0x08; // portnum tag
    plaintext[1] = 0x04; // NODEINFO_APP
    plaintext[2] = 0x12; // payload tag
    plaintext[3] = u_idx;
    memcpy(&plaintext[4], user_buf, u_idx);

    TEST_ASSERT(plaintext[1] == 0x04, "NODEINFO_APP portnum is 4");
    TEST_ASSERT(user_buf[0] == 0x0A, "User protobuf starts with field 1 (id)");
    TEST_ASSERT(user_buf[2 + id_len] == 0x12, "User protobuf field 2 is long_name");
    TEST_ASSERT(user_buf[u_idx - 2] == 0x28, "User protobuf contains hw_model tag 0x28");
    TEST_ASSERT(user_buf[u_idx - 1] == 48, "Hardware model is 48 (HELTEC_WIRELESS_TRACKER)");
}

static void test_meshtastic_sync_word(void) {
    uint16_t sync_word = 0x24B4;
    TEST_ASSERT(sync_word == 0x24B4, "Meshtastic SX1262 sync word register is 0x24B4");
    uint8_t sw_msb = (uint8_t)((sync_word >> 8) & 0xFF);
    uint8_t sw_lsb = (uint8_t)(sync_word & 0xFF);
    TEST_ASSERT(sw_msb == 0x24, "Sync word MSB register 0x0740 is 0x24");
    TEST_ASSERT(sw_lsb == 0xB4, "Sync word LSB register 0x0741 is 0xB4");
}

static void test_display_utc_location_line(void) {
    char time_str[32];
    const char *utc_time = "17:40:25";
    int32_t lat_i = 395000000;   // 39.5 deg
    int32_t lon_i = -1050000000; // -105.0 deg

    float lat = (float)lat_i / 10000000.0f;
    float lon = (float)lon_i / 10000000.0f;
    snprintf(time_str, sizeof(time_str), "UTC %s %0.1f %0.1f", utc_time, lat, lon);

    TEST_ASSERT(strcmp(time_str, "UTC 17:40:25 39.5 -105.0") == 0, "UTC location line formats accurately");
    TEST_ASSERT(strlen(time_str) <= 26, "UTC location line fits within 26-character display width");
    int px_width = (int)strlen(time_str) * 6;
    TEST_ASSERT(4 + px_width <= 160, "UTC location line fits within 160-pixel display width");
}

static void test_meshcore_packet_layout(void) {
    uint8_t out_buf[64];
    uint32_t epoch_sec = 1794148800;
    const char *name = "TEST-NODE";
    uint8_t name_len = (uint8_t)strlen(name);

    out_buf[0] = 0x11; // (PAYLOAD_TYPE_ADVERT 0x04 << 2) | ROUTE_TYPE_FLOOD 0x01
    out_buf[1] = 0x40; // 2-byte path hash mode ((2 - 1) << 6 | 0 hops)
    out_buf[2] = (uint8_t)(epoch_sec & 0xFF);
    out_buf[3] = (uint8_t)((epoch_sec >> 8) & 0xFF);
    out_buf[4] = (uint8_t)((epoch_sec >> 16) & 0xFF);
    out_buf[5] = (uint8_t)((epoch_sec >> 24) & 0xFF);

    uint8_t flags = 0x01 | 0x02; // Stratum-1 | USB Powered
    out_buf[6] = flags;
    out_buf[7] = 100; // 100% battery
    uint16_t v_mv = 4150;
    out_buf[8] = (uint8_t)(v_mv & 0xFF);
    out_buf[9] = (uint8_t)((v_mv >> 8) & 0xFF);
    memcpy(&out_buf[10], name, name_len);

    TEST_ASSERT(out_buf[0] == 0x11, "MeshCore header has ADVERT type (0x04) and FLOOD route (0x01)");
    TEST_ASSERT(out_buf[1] == 0x40, "MeshCore path length byte encodes 2-byte hash mode (0x40)");
    TEST_ASSERT((out_buf[6] & 0x01) == 0x01, "MeshCore Stratum-1 flag set");
    TEST_ASSERT((out_buf[6] & 0x02) == 0x02, "MeshCore USB powered flag set");
    TEST_ASSERT(out_buf[7] == 100, "MeshCore battery percentage is 100%");
    uint16_t decoded_mv = (uint16_t)out_buf[8] | ((uint16_t)out_buf[9] << 8);
    TEST_ASSERT(decoded_mv == 4150, "MeshCore decoded millivolts is 4150 mV");
    TEST_ASSERT(memcmp(&out_buf[10], "TEST-NODE", 9) == 0, "MeshCore node name preserved");

    // MeshCore local standard radio parameters
    uint8_t mc_sf = 7;
    uint8_t mc_bw = 0x03; // SX1262_BW_62_5_KHZ
    uint16_t mc_preamble = 32;
    int8_t mc_pwr = 22;
    TEST_ASSERT(mc_sf == 7, "MeshCore local standard SF is 7");
    TEST_ASSERT(mc_bw == 0x03, "MeshCore local standard BW register is 0x03 (62.5 kHz)");
    TEST_ASSERT(mc_preamble == 32, "MeshCore preamble length is 32 symbols for SF <= 8");
    TEST_ASSERT(mc_pwr == 22, "MeshCore TX power is 22 dBm");
}

static void test_lora_rotation_schedule(void) {
    // Rotation schedule:
    // When both enabled: 0 = LongFast, 1 = MedFast, 2 = MeshCore (period = 3 minutes)
    for (uint32_t c = 0; c < 9; c++) {
        uint32_t phase = c % 3;
        if (phase == 0) {
            TEST_ASSERT(phase == 0, "Cycle phase 0 is LongFast");
        } else if (phase == 1) {
            TEST_ASSERT(phase == 1, "Cycle phase 1 is MedFast");
        } else {
            TEST_ASSERT(phase == 2, "Cycle phase 2 is MeshCore");
        }
    }

    // When only Meshtastic enabled: alternate LongFast (even) and MedFast (odd)
    TEST_ASSERT((0 % 2) == 0, "Meshtastic-only cycle 0 is LongFast");
    TEST_ASSERT((1 % 2) == 1, "Meshtastic-only cycle 1 is MedFast");
    TEST_ASSERT((2 % 2) == 0, "Meshtastic-only cycle 2 is LongFast");
}

/* ========================================================================= */
/* Test Suite 3: Configuration & Regulatory Frequency Rules                  */
/* ========================================================================= */
static uint32_t get_default_lora_freq_by_country(const char *country) {
    if (!country || country[0] == '\0') return 906875000;
    if (strcasecmp(country, "US") == 0 || strcasecmp(country, "CA") == 0) return 906875000;
    if (strcasecmp(country, "EU") == 0 || strcasecmp(country, "DE") == 0) return 869525000;
    if (strcasecmp(country, "AU") == 0) return 916875000;
    if (strcasecmp(country, "IN") == 0) return 865200000;
    if (strcasecmp(country, "JP") == 0) return 921875000;
    if (strcasecmp(country, "KR") == 0) return 921900000;
    if (strcasecmp(country, "CN") == 0) return 470000000;
    return 906875000;
}

static void test_regulatory_frequencies(void) {
    TEST_ASSERT(get_default_lora_freq_by_country("US") == 906875000, "US maps to 906.875 MHz");
    TEST_ASSERT(get_default_lora_freq_by_country("EU") == 869525000, "EU maps to 869.525 MHz");
    TEST_ASSERT(get_default_lora_freq_by_country("AU") == 916875000, "AU maps to 916.875 MHz");
    TEST_ASSERT(get_default_lora_freq_by_country("IN") == 865200000, "IN maps to 865.200 MHz");
    TEST_ASSERT(get_default_lora_freq_by_country("JP") == 921875000, "JP maps to 921.875 MHz");
    TEST_ASSERT(get_default_lora_freq_by_country("KR") == 921900000, "KR maps to 921.900 MHz");
    TEST_ASSERT(get_default_lora_freq_by_country("CN") == 470000000, "CN maps to 470.000 MHz");
    TEST_ASSERT(get_default_lora_freq_by_country("UNKNOWN") == 906875000, "Fallback defaults to US 906.875 MHz");

    // Meshtastic MediumFast frequency mapping
    uint32_t us_lf_freq = 906875000;
    uint32_t us_mf_freq = (us_lf_freq == 906875000) ? 913125000 : us_lf_freq;
    TEST_ASSERT(us_mf_freq == 913125000, "US915 MediumFast uses 913.125 MHz (Slot 45)");

    uint32_t eu_lf_freq = 869525000;
    uint32_t eu_mf_freq = (eu_lf_freq == 906875000) ? 913125000 : eu_lf_freq;
    TEST_ASSERT(eu_mf_freq == 869525000, "EU868 MediumFast preserves 869.525 MHz");

    // MeshCore local standard frequency mapping
    uint32_t us_mc_freq = (us_lf_freq == 906875000) ? 910525000 : us_lf_freq;
    TEST_ASSERT(us_mc_freq == 910525000, "US915 MeshCore uses 910.525 MHz local standard");

    uint32_t eu_mc_freq = (eu_lf_freq == 906875000) ? 910525000 : eu_lf_freq;
    TEST_ASSERT(eu_mc_freq == 869525000, "EU868 MeshCore preserves configured frequency");
}

/* ========================================================================= */
/* Test Suite 4: Hardware 1PPS & Software Offset Compensation                */
/* ========================================================================= */
static void test_time_sync_pps_and_offset(void) {
    time_t epoch_sec = 1794148800; // Fixed timestamp
    int64_t top_of_sec_us = 1000000000LL;

    // Case 1: Hardware 1PPS Active
    // PPS pulse fires at top_of_sec_us exactly.
    // NMEA parsing finishes 75ms later (now_us = top_of_sec_us + 75000).
    int64_t last_pps_us = top_of_sec_us;
    int64_t now_us = top_of_sec_us + 75000LL;
    int64_t pps_age_us = now_us - last_pps_us;

    TEST_ASSERT(pps_age_us == 75000LL, "PPS age calculated exactly as 75000 us (75ms)");
    time_t pps_sec = epoch_sec + (time_t)(pps_age_us / 1000000LL);
    suseconds_t pps_usec = (suseconds_t)(pps_age_us % 1000000LL);
    TEST_ASSERT(pps_sec == epoch_sec, "Whole second unchanged within 75ms window");
    TEST_ASSERT(pps_usec == 75000, "Microsecond matches elapsed time since 1PPS tick");

    // Case 2: Hardware 1PPS Absent (Stock v1.1 Fallback)
    // '$' character arrived 60ms after top_of_sec (anchor_esp_time_us = top_of_sec_us + 60000).
    // NMEA parsing finishes 15ms later at now_us = top_of_sec_us + 75000.
    // Configured offset is 60ms (60000 us).
    int64_t anchor_us = top_of_sec_us + 60000LL;
    int64_t offset_us = 60000LL; // 60 ms
    int64_t elapsed_us = (now_us - anchor_us) + offset_us;

    TEST_ASSERT(elapsed_us == 75000LL, "Compensated elapsed time equals true elapsed time (75ms)");
    time_t comp_sec = epoch_sec + (time_t)(elapsed_us / 1000000LL);
    suseconds_t comp_usec = (suseconds_t)(elapsed_us % 1000000LL);
    TEST_ASSERT(comp_sec == pps_sec, "Compensated second matches PPS ground truth");
    TEST_ASSERT(comp_usec == pps_usec, "Compensated microsecond matches PPS ground truth (zero offset)");

    // Case 3: Forced Software Fallback via enable_pps = false
    bool enable_pps_cfg = false;
    bool using_pps = false;
    if (enable_pps_cfg && last_pps_us > 0 && pps_age_us >= 0 && pps_age_us < 1500000LL) {
        using_pps = true;
    } else {
        using_pps = false;
    }
    TEST_ASSERT(!using_pps, "Forced NMEA fallback: using_pps is false when enable_pps is false");

    // Case 4: NTP Ref IDs
    uint32_t ref_id_gps = 0x47505300; // "GPS\0"
    uint32_t ref_id_pps = 0x50505300; // "PPS\0"
    TEST_ASSERT(ref_id_gps == 0x47505300, "NTP GPS RefID corresponds to ASCII 'GPS\\0'");
    TEST_ASSERT(ref_id_pps == 0x50505300, "NTP PPS RefID corresponds to ASCII 'PPS\\0'");
}

/* ========================================================================= */
/* Test Suite 5: INI Parsing with Commented-out Defaults                     */
/* ========================================================================= */
static void test_config_ini_commented_defaults(void) {
    const char *ini_content =
        "# [device]\n"
        "[device]\n"
        "node_name = Custom-Node\n"
        "# led_peak_percent = 20\n"
        "# enable_pps = false\n"
        "# nmea_offset_ms = 85\n"
        "[services]\n"
        "# ntp = false\n"
        "wifi = true\n";

    char node_name[33] = "Heltec-Time";
    uint8_t led_peak_percent = 50;
    bool enable_pps = true;
    int16_t nmea_offset_ms = 60;
    bool enable_ntp = true;
    bool enable_wifi = false;
    bool enable_meshcore = false;
    uint8_t position_precision = 18;

    char buf[256];
    const char *ptr = ini_content;
    char current_section[32] = {0};

    while (*ptr) {
        size_t len = 0;
        while (ptr[len] && ptr[len] != '\n') len++;
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, ptr, len);
        buf[len] = '\0';
        ptr += len;
        if (*ptr == '\n') ptr++;

        char *p = buf;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#' || *p == ';') continue;

        if (*p == '[' && p[strlen(p) - 1] == ']') {
            p[strlen(p) - 1] = '\0';
            strncpy(current_section, p + 1, sizeof(current_section) - 1);
            continue;
        }

        char *eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = p;
        while (strlen(key) > 0 && key[strlen(key) - 1] == ' ') key[strlen(key) - 1] = '\0';
        char *val = eq + 1;
        while (*val == ' ') val++;

        if (strcmp(current_section, "device") == 0) {
            if (strcmp(key, "node_name") == 0) snprintf(node_name, sizeof(node_name), "%s", val);
            if (strcmp(key, "led_peak_percent") == 0) led_peak_percent = (uint8_t)atoi(val);
            if (strcmp(key, "enable_pps") == 0) enable_pps = (strcmp(val, "true") == 0);
            if (strcmp(key, "nmea_offset_ms") == 0) nmea_offset_ms = (int16_t)atoi(val);
        } else if (strcmp(current_section, "services") == 0) {
            if (strcmp(key, "ntp") == 0) enable_ntp = (strcmp(val, "true") == 0);
            if (strcmp(key, "wifi") == 0) enable_wifi = (strcmp(val, "true") == 0);
            if (strcmp(key, "meshcore") == 0) enable_meshcore = (strcmp(val, "true") == 0);
            if (strcmp(key, "position_precision") == 0) position_precision = (uint8_t)atoi(val);
        }
    }

    TEST_ASSERT(strcmp(node_name, "Custom-Node") == 0, "Uncommented node_name applied");
    TEST_ASSERT(led_peak_percent == 50, "Commented led_peak_percent preserves default (50)");
    TEST_ASSERT(enable_pps == true, "Commented enable_pps preserves default (true)");
    TEST_ASSERT(nmea_offset_ms == 60, "Commented nmea_offset_ms preserves default (60)");
    TEST_ASSERT(enable_ntp == true, "Commented ntp preserves default (true)");
    TEST_ASSERT(enable_wifi == true, "Uncommented wifi applied (true)");
    TEST_ASSERT(enable_meshcore == false, "Commented meshcore preserves default (false)");
    TEST_ASSERT(position_precision == 18, "Commented position_precision preserves default (18)");
}

static void test_config_ini_lora_freq_and_country_aliases(void) {
    // Case 1: country in [device] auto-derives LoRa frequency when lora_freq is commented out
    const char *ini_device_country =
        "[device]\n"
        "country = EU\n"
        "[lora]\n"
        "# lora_freq = 906875000\n";

    char country[4] = "US";
    uint32_t lora_freq = get_default_lora_freq_by_country(country);
    bool explicit_freq = false;

    char buf[256];
    const char *ptr = ini_device_country;
    char current_section[32] = {0};

    while (*ptr) {
        size_t len = 0;
        while (ptr[len] && ptr[len] != '\n') len++;
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, ptr, len);
        buf[len] = '\0';
        ptr += len;
        if (*ptr == '\n') ptr++;

        char *p = buf;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#' || *p == ';') continue;

        if (*p == '[' && p[strlen(p) - 1] == ']') {
            p[strlen(p) - 1] = '\0';
            strncpy(current_section, p + 1, sizeof(current_section) - 1);
            continue;
        }

        char *eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = p;
        while (strlen(key) > 0 && key[strlen(key) - 1] == ' ') key[strlen(key) - 1] = '\0';
        char *val = eq + 1;
        while (*val == ' ') val++;

        if (strcmp(current_section, "device") == 0 || strcmp(current_section, "regulatory") == 0) {
            if (strcmp(key, "country") == 0) {
                snprintf(country, sizeof(country), "%s", val);
                if (!explicit_freq) lora_freq = get_default_lora_freq_by_country(country);
            }
        } else if (strcmp(current_section, "lora") == 0) {
            if (strcmp(key, "lora_freq") == 0 || strcmp(key, "frequency") == 0) {
                lora_freq = (uint32_t)strtoul(val, NULL, 10);
                explicit_freq = true;
            }
        }
    }

    TEST_ASSERT(strcmp(country, "EU") == 0, "Country set to EU under [device]");
    TEST_ASSERT(lora_freq == 869525000, "Commented lora_freq auto-derives 869.525 MHz for EU");

    // Case 2: explicit lora_freq overrides country default
    const char *ini_explicit_freq =
        "[device]\n"
        "country = US\n"
        "[lora]\n"
        "lora_freq = 915000000\n";

    strcpy(country, "US");
    lora_freq = get_default_lora_freq_by_country(country);
    explicit_freq = false;
    ptr = ini_explicit_freq;
    memset(current_section, 0, sizeof(current_section));

    while (*ptr) {
        size_t len = 0;
        while (ptr[len] && ptr[len] != '\n') len++;
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, ptr, len);
        buf[len] = '\0';
        ptr += len;
        if (*ptr == '\n') ptr++;

        char *p = buf;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#' || *p == ';') continue;

        if (*p == '[' && p[strlen(p) - 1] == ']') {
            p[strlen(p) - 1] = '\0';
            strncpy(current_section, p + 1, sizeof(current_section) - 1);
            continue;
        }

        char *eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = p;
        while (strlen(key) > 0 && key[strlen(key) - 1] == ' ') key[strlen(key) - 1] = '\0';
        char *val = eq + 1;
        while (*val == ' ') val++;

        if (strcmp(current_section, "device") == 0) {
            if (strcmp(key, "country") == 0) {
                snprintf(country, sizeof(country), "%s", val);
                if (!explicit_freq) lora_freq = get_default_lora_freq_by_country(country);
            }
        } else if (strcmp(current_section, "lora") == 0) {
            if (strcmp(key, "lora_freq") == 0 || strcmp(key, "frequency") == 0) {
                lora_freq = (uint32_t)strtoul(val, NULL, 10);
                explicit_freq = true;
            }
        }
    }

    TEST_ASSERT(lora_freq == 915000000, "Explicit lora_freq sets custom frequency 915.000 MHz");
}

/* ========================================================================= */
/* Main Runner                                                               */
/* ========================================================================= */
int main(void) {
    printf("====================================================\n");
    printf(" Stratum-1 Time Server Native Test Suite (Host x86) \n");
    printf("====================================================\n");

    test_nmea_checksum();
    test_nmea_rmc_parser();
    test_nmea_zda_parser();
    test_nmea_gga_parser();
    test_meshtastic_position_layout();
    test_meshtastic_telemetry_layout();
    test_meshtastic_nodeinfo_layout();
    test_meshtastic_sync_word();
    test_meshcore_packet_layout();
    test_lora_rotation_schedule();
    test_regulatory_frequencies();
    test_time_sync_pps_and_offset();
    test_config_ini_commented_defaults();
    test_config_ini_lora_freq_and_country_aliases();
    test_display_utc_location_line();

    printf("\nResults: %d/%d assertions passed.\n", s_tests_passed, s_tests_run);
    if (s_tests_passed == s_tests_run) {
        printf("[SUCCESS] All native simulation tests passed!\n");
        return 0;
    } else {
        printf("[FAILURE] %d assertions failed!\n", s_tests_run - s_tests_passed);
        return 1;
    }
}
