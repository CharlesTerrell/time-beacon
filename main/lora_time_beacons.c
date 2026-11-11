#include "lora_time_beacons.h"
#include "sx1262.h"
#include "config_mgr.h"
#include "time_sync.h"
#include "power_mgr.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "aes/esp_aes.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>

static const char *TAG = "lora_beacon";

// Meshtastic default primary channel key: "1PG7OiApB1nwvP-rz05pAQ=="
static const uint8_t s_meshtastic_default_key[16] = {
    0xd4, 0xf1, 0xbb, 0x3a, 0x20, 0x29, 0x07, 0x59,
    0xf0, 0xbc, 0xff, 0xab, 0xcf, 0x4e, 0x69, 0x01
};

typedef struct __attribute__((packed)) {
    uint32_t to;
    uint32_t from;
    uint32_t id;
    uint8_t  flags;
    uint8_t  channel;
    uint8_t  next_hop;
    uint8_t  relay_node;
} meshtastic_hdr_t;

static uint32_t s_node_num = 0;
static uint32_t s_packet_id = 0x1000;
static uint32_t s_beacon_count = 0;
static char s_last_mode[16] = "IDLE";
static uint8_t s_tx_buf[128];

static void init_node_num(void) {
    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    s_node_num = ((uint32_t)mac[2] << 24) |
                 ((uint32_t)mac[3] << 16) |
                 ((uint32_t)mac[4] << 8)  |
                 ((uint32_t)mac[5]);
    if (s_node_num == 0) {
        s_node_num = 0x12345678;
    }
}

static void encrypt_meshtastic_payload(uint32_t packet_id, uint32_t from_node,
                                       const uint8_t *plaintext, size_t len, uint8_t *out_ciphertext) {
    uint8_t nonce[16] = {0};
    uint64_t packet_id64 = (uint64_t)packet_id;
    memcpy(&nonce[0], &packet_id64, sizeof(uint64_t));
    memcpy(&nonce[8], &from_node, sizeof(uint32_t));

    uint8_t stream_block[16] = {0};
    size_t nc_off = 0;
    esp_aes_context aes;
    esp_aes_init(&aes);
    esp_aes_setkey(&aes, s_meshtastic_default_key, 128);
    esp_aes_crypt_ctr(&aes, len, &nc_off, nonce, stream_block, plaintext, out_ciphertext);
    esp_aes_free(&aes);
}

static inline uint32_t get_meshtastic_freq(const app_config_t *cfg, bool is_long_fast) {
    if (is_long_fast || !cfg) {
        return cfg ? cfg->lora_freq_hz : 906875000;
    }
    // US915 default preset mapping:
    // LongFast = 906.875 MHz (Slot 20)
    // MediumFast = 913.125 MHz (Slot 45)
    if (cfg->lora_freq_hz == 906875000) {
        return 913125000;
    }
    return cfg->lora_freq_hz;
}

static uint8_t build_meshtastic_packet(uint8_t *out_buf, uint32_t epoch_sec, const gnss_status_t *gnss, bool is_long_fast) {
    meshtastic_hdr_t *hdr = (meshtastic_hdr_t *)out_buf;
    hdr->to = 0xFFFFFFFF; // Broadcast
    hdr->from = s_node_num;
    hdr->id = ++s_packet_id;
    hdr->flags = 0x63;    // hop_limit = 3 (bits 0-2), hop_start = 3 (bits 5-7)
    hdr->channel = is_long_fast ? 0x08 : 0x1F;
    hdr->next_hop = 0x00;
    hdr->relay_node = 0x00;

    // Build Position protobuf payload:
    uint8_t pos_buf[48];
    uint8_t pos_idx = 0;

    if (gnss && gnss->has_pos) {
        int32_t lat_i = gnss->latitude_i;
        int32_t lon_i = gnss->longitude_i;

        const app_config_t *cfg = config_mgr_get();
        uint8_t prec = cfg ? cfg->position_precision : 18;
        if (prec > 0 && prec < 32) {
            uint32_t mask = 0xFFFFFFFFUL << (32 - prec);
            lat_i = (int32_t)((uint32_t)lat_i & mask);
            lon_i = (int32_t)((uint32_t)lon_i & mask);
        }

        // field 1: latitude_i (sfixed32 -> tag 0x0D)
        pos_buf[pos_idx++] = 0x0D;
        memcpy(&pos_buf[pos_idx], &lat_i, 4);
        pos_idx += 4;

        // field 2: longitude_i (sfixed32 -> tag 0x15)
        pos_buf[pos_idx++] = 0x15;
        memcpy(&pos_buf[pos_idx], &lon_i, 4);
        pos_idx += 4;

        // field 3: altitude (int32 -> tag 0x18) if available
        if (gnss->altitude_m != 0) {
            pos_buf[pos_idx++] = 0x18;
            int32_t alt = gnss->altitude_m;
            uint32_t v = (uint32_t)alt;
            while (v >= 0x80) {
                pos_buf[pos_idx++] = (uint8_t)((v & 0x7F) | 0x80);
                v >>= 7;
            }
            pos_buf[pos_idx++] = (uint8_t)(v & 0x7F);
        }

        // field 23: precision_bits (uint32 -> tag (23 << 3) | 0 = 184 = 0xB8, 0x01)
        if (prec > 0) {
            pos_buf[pos_idx++] = 0xB8;
            pos_buf[pos_idx++] = 0x01;
            pos_buf[pos_idx++] = prec;
        }
    }

    // field 4: time (fixed32 -> tag (4 << 3) | 5 = 0x25)
    pos_buf[pos_idx++] = 0x25;
    memcpy(&pos_buf[pos_idx], &epoch_sec, 4);
    pos_idx += 4;

    // field 5: location_source = 2 (LOC_INTERNAL -> tag (5 << 3) | 0 = 0x28)
    pos_buf[pos_idx++] = 0x28;
    pos_buf[pos_idx++] = 0x02;

    // field 6: altitude_source = 2 (ALT_INTERNAL -> tag (6 << 3) | 0 = 0x30)
    pos_buf[pos_idx++] = 0x30;
    pos_buf[pos_idx++] = 0x02;

    // field 7: timestamp (fixed32 -> tag (7 << 3) | 5 = 0x3D)
    pos_buf[pos_idx++] = 0x3D;
    memcpy(&pos_buf[pos_idx], &epoch_sec, 4);
    pos_idx += 4;

    // Build Data protobuf wrapper:
    // field 1: portnum = 3 (POSITION_APP -> tag 0x08, 0x03)
    // field 2: payload (length-delimited -> tag 0x12, len, data)
    uint8_t plaintext[64];
    plaintext[0] = 0x08;
    plaintext[1] = 0x03; // POSITION_APP
    plaintext[2] = 0x12; // payload tag
    plaintext[3] = pos_idx;
    memcpy(&plaintext[4], pos_buf, pos_idx);
    uint8_t pt_len = 4 + pos_idx;

    encrypt_meshtastic_payload(hdr->id, hdr->from, plaintext, pt_len, &out_buf[sizeof(meshtastic_hdr_t)]);
    return sizeof(meshtastic_hdr_t) + pt_len;
}

static uint8_t build_meshtastic_nodeinfo_packet(uint8_t *out_buf, const char *node_name, bool is_long_fast) {
    meshtastic_hdr_t *hdr = (meshtastic_hdr_t *)out_buf;
    hdr->to = 0xFFFFFFFF; // Broadcast
    hdr->from = s_node_num;
    hdr->id = ++s_packet_id;
    hdr->flags = 0x63;    // hop_limit = 3 (bits 0-2), hop_start = 3 (bits 5-7)
    hdr->channel = is_long_fast ? 0x08 : 0x1F;
    hdr->next_hop = 0x00;
    hdr->relay_node = 0x00;

    char id_str[16];
    snprintf(id_str, sizeof(id_str), "!%08lx", (unsigned long)s_node_num);
    uint8_t id_len = (uint8_t)strlen(id_str);

    const char *lname = (node_name && node_name[0] != '\0') ? node_name : "Time-Beacon";
    uint8_t lname_len = (uint8_t)strlen(lname);
    if (lname_len > 24) lname_len = 24;

    char sname[5] = {0};
    for (int i = 0; i < 4 && lname[i] != '\0'; i++) {
        sname[i] = (char)toupper((unsigned char)lname[i]);
    }
    uint8_t sname_len = (uint8_t)strlen(sname);

    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    uint8_t user_buf[64];
    uint8_t u_idx = 0;

    // field 1: id (string tag 0x0A)
    user_buf[u_idx++] = 0x0A;
    user_buf[u_idx++] = id_len;
    memcpy(&user_buf[u_idx], id_str, id_len);
    u_idx += id_len;

    // field 2: long_name (string tag 0x12)
    user_buf[u_idx++] = 0x12;
    user_buf[u_idx++] = lname_len;
    memcpy(&user_buf[u_idx], lname, lname_len);
    u_idx += lname_len;

    // field 3: short_name (string tag 0x1A)
    user_buf[u_idx++] = 0x1A;
    user_buf[u_idx++] = sname_len;
    memcpy(&user_buf[u_idx], sname, sname_len);
    u_idx += sname_len;

    // field 4: macaddr (bytes tag 0x22)
    user_buf[u_idx++] = 0x22;
    user_buf[u_idx++] = 6;
    memcpy(&user_buf[u_idx], mac, 6);
    u_idx += 6;

    // field 5: hw_model (varint tag 0x28, 48 = HELTEC_WIRELESS_TRACKER)
    user_buf[u_idx++] = 0x28;
    user_buf[u_idx++] = 48;

    // Data wrapper:
    // field 1: portnum = 4 (NODEINFO_APP -> tag 0x08, 0x04)
    // field 2: payload (tag 0x12, u_idx, user_buf)
    uint8_t plaintext[80];
    plaintext[0] = 0x08;
    plaintext[1] = 0x04; // NODEINFO_APP
    plaintext[2] = 0x12;
    plaintext[3] = u_idx;
    memcpy(&plaintext[4], user_buf, u_idx);
    uint8_t pt_len = 4 + u_idx;

    encrypt_meshtastic_payload(hdr->id, hdr->from, plaintext, pt_len, &out_buf[sizeof(meshtastic_hdr_t)]);
    return sizeof(meshtastic_hdr_t) + pt_len;
}

static uint8_t build_meshtastic_telemetry_packet(uint8_t *out_buf, uint32_t epoch_sec, const power_status_t *pwr, bool is_long_fast) {
    meshtastic_hdr_t *hdr = (meshtastic_hdr_t *)out_buf;
    hdr->to = 0xFFFFFFFF; // Broadcast
    hdr->from = s_node_num;
    hdr->id = ++s_packet_id;
    hdr->flags = 0x63;    // hop_limit = 3 (bits 0-2), hop_start = 3 (bits 5-7)
    hdr->channel = is_long_fast ? 0x08 : 0x1F;
    hdr->next_hop = 0x00;
    hdr->relay_node = 0x00;

    // DeviceMetrics:
    // field 1: battery_level (tag 0x08): 101 if USB powered, otherwise 0..100%
    // field 2: voltage (tag 0x15): float
    uint32_t bat_val = pwr->usb_connected ? 101 : (uint32_t)pwr->percentage;
    float volt = pwr->voltage;
    uint32_t volt_raw;
    memcpy(&volt_raw, &volt, sizeof(float));

    uint8_t dm_buf[7];
    dm_buf[0] = 0x08; // battery_level tag
    dm_buf[1] = (uint8_t)(bat_val & 0x7F);
    dm_buf[2] = 0x15; // voltage tag (fixed32 float)
    dm_buf[3] = (uint8_t)(volt_raw & 0xFF);
    dm_buf[4] = (uint8_t)((volt_raw >> 8) & 0xFF);
    dm_buf[5] = (uint8_t)((volt_raw >> 16) & 0xFF);
    dm_buf[6] = (uint8_t)((volt_raw >> 24) & 0xFF);

    // Telemetry message:
    // field 1: time (fixed32 tag 0x0d)
    // field 2: device_metrics (length-delimited tag 0x12, len, data)
    uint8_t telem_buf[14];
    telem_buf[0] = 0x0d;
    telem_buf[1] = (uint8_t)(epoch_sec & 0xFF);
    telem_buf[2] = (uint8_t)((epoch_sec >> 8) & 0xFF);
    telem_buf[3] = (uint8_t)((epoch_sec >> 16) & 0xFF);
    telem_buf[4] = (uint8_t)((epoch_sec >> 24) & 0xFF);
    telem_buf[5] = 0x12; // device_metrics tag
    telem_buf[6] = sizeof(dm_buf);
    memcpy(&telem_buf[7], dm_buf, sizeof(dm_buf));

    // Data wrapper:
    // field 1: portnum = 67 (TELEMETRY_APP -> tag 0x08, 0x43)
    // field 2: payload (tag 0x12, len, data)
    uint8_t plaintext[18];
    plaintext[0] = 0x08;
    plaintext[1] = 0x43; // 67 TELEMETRY_APP
    plaintext[2] = 0x12;
    plaintext[3] = sizeof(telem_buf);
    memcpy(&plaintext[4], telem_buf, sizeof(telem_buf));
    uint8_t pt_len = 4 + sizeof(telem_buf);

    encrypt_meshtastic_payload(hdr->id, hdr->from, plaintext, pt_len, &out_buf[sizeof(meshtastic_hdr_t)]);
    return sizeof(meshtastic_hdr_t) + pt_len;
}

static uint8_t build_meshcore_packet(uint8_t *out_buf, uint32_t epoch_sec, const char *node_name,
                                     const power_status_t *pwr, bool include_power) {
    // MeshCore Packet Format:
    // Header (1 byte): (PAYLOAD_TYPE_ADVERT 0x04 << 2) | ROUTE_TYPE_FLOOD 0x00 = 0x10
    // Path length (1 byte): 0x00 (direct)
    // Timestamp (4 bytes little-endian)
    // Beacon flag (1 byte): 0x01 (Stratum-1 GPS Time Source)
    // Optional power telemetry: [battery_pct: 1B] [voltage_mv: 2B]
    // Node Name (variable length)
    const char *name = (node_name && node_name[0] != '\0') ? node_name : "STRATUM1-GPS";
    uint8_t name_len = (uint8_t)strlen(name);
    if (name_len > 24) name_len = 24;

    out_buf[0] = 0x10; // Advert | Flood
    out_buf[1] = 0x00; // Path length
    out_buf[2] = (uint8_t)(epoch_sec & 0xFF);
    out_buf[3] = (uint8_t)((epoch_sec >> 8) & 0xFF);
    out_buf[4] = (uint8_t)((epoch_sec >> 16) & 0xFF);
    out_buf[5] = (uint8_t)((epoch_sec >> 24) & 0xFF);

    uint8_t flags = 0x01; // Stratum-1 clock beacon
    if (include_power && pwr) {
        if (pwr->usb_connected) flags |= 0x02; // USB powered
        if (pwr->mode == POWER_MODE_BATTERY_CRITICAL) flags |= 0x04;
        out_buf[6] = flags;
        out_buf[7] = (uint8_t)pwr->percentage;
        uint16_t v_mv = (uint16_t)(pwr->voltage * 1000.0f);
        out_buf[8] = (uint8_t)(v_mv & 0xFF);
        out_buf[9] = (uint8_t)((v_mv >> 8) & 0xFF);
        memcpy(&out_buf[10], name, name_len);
        return 10 + name_len;
    } else {
        out_buf[6] = flags;
        memcpy(&out_buf[7], name, name_len);
        return 7 + name_len;
    }
}

static void send_meshtastic_burst(const app_config_t *cfg, bool is_long_fast) {
    if (!cfg) return;

    uint8_t sf = is_long_fast ? 11 : 9;
    uint32_t freq = get_meshtastic_freq(cfg, is_long_fast);
    const char *mode_name = is_long_fast ? "Mesh LongFast" : "Mesh MedFast";

    sx1262_set_sync_word(SX1262_SYNC_MESHTASTIC);
    sx1262_set_lora_params(sf, SX1262_BW_250_KHZ, SX1262_CR_4_5, false);
    sx1262_set_frequency(freq);
    sx1262_set_tx_power(cfg->lora_tx_power);

    // 1. NodeInfo packet
    uint8_t len = build_meshtastic_nodeinfo_packet(s_tx_buf, cfg->node_name, is_long_fast);
    esp_err_t err = sx1262_transmit(s_tx_buf, len, 3000);
    if (err == ESP_OK) {
        s_beacon_count++;
        ESP_LOGI(TAG, "Sent %s burst [1/3 NodeInfo] (SF%u, %luHz, %uB)",
                 mode_name, sf, (unsigned long)freq, len);
    } else {
        ESP_LOGW(TAG, "Failed %s burst [1/3 NodeInfo]: %s", mode_name, esp_err_to_name(err));
    }

    vTaskDelay(pdMS_TO_TICKS(500));

    // 2. Position + Time packet
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint32_t now_sec = (uint32_t)tv.tv_sec;
    gnss_status_t gnss;
    time_sync_get_status(&gnss);

    len = build_meshtastic_packet(s_tx_buf, now_sec, &gnss, is_long_fast);
    err = sx1262_transmit(s_tx_buf, len, 3000);
    if (err == ESP_OK) {
        s_beacon_count++;
        ESP_LOGI(TAG, "Sent %s burst [2/3 Pos] (SF%u, %luHz, %uB, time=%lu, %s)",
                 mode_name, sf, (unsigned long)freq, len, (unsigned long)now_sec,
                 gnss.has_pos ? "GPS" : "NoGPS");
    } else {
        ESP_LOGW(TAG, "Failed %s burst [2/3 Pos]: %s", mode_name, esp_err_to_name(err));
    }

    // 3. Telemetry packet (battery / power status)
    if (cfg->enable_lora_power) {
        vTaskDelay(pdMS_TO_TICKS(500));
        power_status_t pwr;
        power_mgr_get_status(&pwr);
        len = build_meshtastic_telemetry_packet(s_tx_buf, now_sec, &pwr, is_long_fast);
        err = sx1262_transmit(s_tx_buf, len, 3000);
        if (err == ESP_OK) {
            s_beacon_count++;
            ESP_LOGI(TAG, "Sent %s burst [3/3 Telem] (SF%u, %luHz, %uB)",
                     mode_name, sf, (unsigned long)freq, len);
        } else {
            ESP_LOGW(TAG, "Failed %s burst [3/3 Telem]: %s", mode_name, esp_err_to_name(err));
        }
    }

    strncpy(s_last_mode, is_long_fast ? "MESH-LF" : "MESH-MF", sizeof(s_last_mode) - 1);
}

static void send_meshcore_beacon(const app_config_t *cfg) {
    if (!cfg) return;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint32_t now_sec = (uint32_t)tv.tv_sec;

    sx1262_set_sync_word(SX1262_SYNC_PUBLIC);
    sx1262_set_lora_params(10, SX1262_BW_250_KHZ, SX1262_CR_4_5, false);
    sx1262_set_frequency(cfg->lora_freq_hz);
    sx1262_set_tx_power(cfg->lora_tx_power);

    power_status_t pwr;
    power_mgr_get_status(&pwr);
    uint8_t len = build_meshcore_packet(s_tx_buf, now_sec, cfg->node_name, &pwr, cfg->enable_lora_power);
    esp_err_t err = sx1262_transmit(s_tx_buf, len, 2500);
    if (err == ESP_OK) {
        s_beacon_count++;
        strncpy(s_last_mode, "MESHCORE", sizeof(s_last_mode) - 1);
        ESP_LOGI(TAG, "Sent MeshCore time beacon (SF10, %luHz, %uB, time=%lu, pwr=%s)",
                 (unsigned long)cfg->lora_freq_hz, len, (unsigned long)now_sec,
                 cfg->enable_lora_power ? (pwr.usb_connected ? "USB" : "BAT") : "OFF");
    } else {
        ESP_LOGW(TAG, "Failed to send MeshCore beacon: %s", esp_err_to_name(err));
    }
}

static void lora_beacon_task(void *pvParameters) {
    (void)pvParameters;
    uint32_t cycle = 0;

    // Allow GNSS task time to start up
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Send immediate advertisements on startup across enabled modes so peers populate NodeDB quickly
    const app_config_t *init_cfg = config_mgr_get();
    if (init_cfg) {
        if (init_cfg->enable_meshtastic) {
            // 1. LongFast startup pulse
            sx1262_set_sync_word(SX1262_SYNC_MESHTASTIC);
            sx1262_set_lora_params(11, SX1262_BW_250_KHZ, SX1262_CR_4_5, false);
            sx1262_set_frequency(init_cfg->lora_freq_hz);
            sx1262_set_tx_power(init_cfg->lora_tx_power);
            uint8_t len = build_meshtastic_nodeinfo_packet(s_tx_buf, init_cfg->node_name, true);
            sx1262_transmit(s_tx_buf, len, 3000);
            s_beacon_count++;
            ESP_LOGI(TAG, "Sent initial Meshtastic LongFast NodeInfo (name='%s', node=!%08lx)",
                     init_cfg->node_name, (unsigned long)s_node_num);
            vTaskDelay(pdMS_TO_TICKS(1500));

            struct timeval tv_init;
            gettimeofday(&tv_init, NULL);
            gnss_status_t gnss_init;
            time_sync_get_status(&gnss_init);
            len = build_meshtastic_packet(s_tx_buf, (uint32_t)tv_init.tv_sec, &gnss_init, true);
            sx1262_transmit(s_tx_buf, len, 3000);
            s_beacon_count++;
            ESP_LOGI(TAG, "Sent initial Meshtastic LongFast Position");
            vTaskDelay(pdMS_TO_TICKS(1500));

            // 2. MediumFast startup pulse
            uint32_t mf_freq = get_meshtastic_freq(init_cfg, false);
            sx1262_set_lora_params(9, SX1262_BW_250_KHZ, SX1262_CR_4_5, false);
            sx1262_set_frequency(mf_freq);
            len = build_meshtastic_nodeinfo_packet(s_tx_buf, init_cfg->node_name, false);
            sx1262_transmit(s_tx_buf, len, 3000);
            s_beacon_count++;
            ESP_LOGI(TAG, "Sent initial Meshtastic MedFast NodeInfo (freq=%luHz)", (unsigned long)mf_freq);
            vTaskDelay(pdMS_TO_TICKS(1500));

            gettimeofday(&tv_init, NULL);
            time_sync_get_status(&gnss_init);
            len = build_meshtastic_packet(s_tx_buf, (uint32_t)tv_init.tv_sec, &gnss_init, false);
            sx1262_transmit(s_tx_buf, len, 3000);
            s_beacon_count++;
            ESP_LOGI(TAG, "Sent initial Meshtastic MedFast Position");
            vTaskDelay(pdMS_TO_TICKS(1500));
        }

        if (init_cfg->enable_meshcore) {
            struct timeval tv_init;
            gettimeofday(&tv_init, NULL);
            power_status_t pwr_init;
            power_mgr_get_status(&pwr_init);
            sx1262_set_sync_word(SX1262_SYNC_PUBLIC);
            sx1262_set_lora_params(10, SX1262_BW_250_KHZ, SX1262_CR_4_5, false);
            sx1262_set_frequency(init_cfg->lora_freq_hz);
            sx1262_set_tx_power(init_cfg->lora_tx_power);
            uint8_t len = build_meshcore_packet(s_tx_buf, (uint32_t)tv_init.tv_sec, init_cfg->node_name,
                                                &pwr_init, init_cfg->enable_lora_power);
            sx1262_transmit(s_tx_buf, len, 2500);
            s_beacon_count++;
            ESP_LOGI(TAG, "Sent initial MeshCore time beacon");
        }
    }

    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        const app_config_t *cfg = config_mgr_get();

        if (cfg) {
            if (cfg->enable_meshtastic && cfg->enable_meshcore) {
                // 3-phase rotation: 0=LongFast, 1=MedFast, 2=MeshCore
                uint32_t phase = cycle % 3;
                if (phase == 0) {
                    send_meshtastic_burst(cfg, true);
                } else if (phase == 1) {
                    send_meshtastic_burst(cfg, false);
                } else {
                    send_meshcore_beacon(cfg);
                }
            } else if (cfg->enable_meshtastic) {
                // Only Meshtastic enabled: alternate LongFast and MediumFast
                bool is_long_fast = ((cycle % 2) == 0);
                send_meshtastic_burst(cfg, is_long_fast);
            } else if (cfg->enable_meshcore) {
                // Only MeshCore enabled
                send_meshcore_beacon(cfg);
            }
        }

        cycle++;
        // 1 turn every minute
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(60000));
    }
}

esp_err_t lora_time_beacons_start(void) {
    const app_config_t *cfg = config_mgr_get();
    if (!cfg || (!cfg->enable_meshcore && !cfg->enable_meshtastic)) {
        ESP_LOGI(TAG, "LoRa time beacons disabled in config.ini");
        return ESP_OK;
    }

    init_node_num();

    esp_err_t err = sx1262_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SX1262: %s", esp_err_to_name(err));
        return err;
    }

    BaseType_t ret = xTaskCreatePinnedToCore(
        lora_beacon_task,
        "lora_beacon",
        4096,
        NULL,
        5,
        NULL,
        1 // Pinned to Core 1
    );

    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create lora_beacon task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "LoRa time beacon service started (Node: 0x%08lx, Freq: %luHz)",
             (unsigned long)s_node_num, (unsigned long)cfg->lora_freq_hz);
    return ESP_OK;
}

uint32_t lora_time_beacons_get_count(void) {
    return s_beacon_count;
}

const char *lora_time_beacons_get_last_mode(void) {
    return s_last_mode;
}
