#include "ble_cts_service.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/dis/ble_svc_dis.h"
#include "services/cts/ble_svc_cts.h"
#include "services/bas/ble_svc_bas.h"
#include "config_mgr.h"
#include "time_sync.h"
#include <sys/time.h>
#include <time.h>
#include <string.h>

static const char *TAG = "ble_cts";
static char s_device_name[33] = "Heltec-Time";
static uint8_t s_own_addr_type = 0;
static bool s_connected = false;
static bool s_started = false;

typedef struct __attribute__((packed)) {
    uint8_t percentage;
    uint16_t voltage_mv;
    uint8_t usb_connected;
} ble_pwr_telemetry_t;

static ble_pwr_telemetry_t s_pwr_data = {0};
static uint16_t s_pwr_val_handle = 0;
static const ble_uuid16_t s_pwr_svc_uuid = BLE_UUID16_INIT(0xFFE0);
static const ble_uuid16_t s_pwr_chr_uuid = BLE_UUID16_INIT(0xFFE1);

static int ble_pwr_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt, void *arg) {
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        int rc = os_mbuf_append(ctxt->om, &s_pwr_data, sizeof(s_pwr_data));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    return BLE_ATT_ERR_UNLIKELY;
}

static const struct ble_gatt_svc_def s_custom_pwr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &s_pwr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &s_pwr_chr_uuid.u,
                .access_cb = ble_pwr_access_cb,
                .val_handle = &s_pwr_val_handle,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
            },
            {
                0,
            }
        },
    },
    {
        0,
    },
};

static void ble_cts_advertise(void);

static int ble_cts_gap_event(struct ble_gap_event *event, void *arg) {
    (void)arg;
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            s_connected = true;
            ESP_LOGI(TAG, "BLE client connected");
        } else {
            s_connected = false;
            ESP_LOGW(TAG, "BLE connection failed (status %d), resuming adv", event->connect.status);
            ble_cts_advertise();
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        s_connected = false;
        ESP_LOGI(TAG, "BLE client disconnected (reason %d), resuming adv", event->disconnect.reason);
        ble_cts_advertise();
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "BLE adv complete, restarting adv");
        ble_cts_advertise();
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "BLE subscribe event: attr=%d, cur_notify=%d",
                 event->subscribe.attr_handle, event->subscribe.cur_notify);
        break;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "BLE MTU updated: handle=%d, mtu=%d",
                 event->mtu.conn_handle, event->mtu.value);
        break;

    default:
        break;
    }
    return 0;
}

static void ble_cts_advertise(void) {
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;

    memset(&fields, 0, sizeof(fields));

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)s_device_name;
    fields.name_len = strlen(s_device_name);
    fields.name_is_complete = 1;

    const app_config_t *cfg = config_mgr_get();
    ble_uuid16_t adv_uuids[2];
    uint8_t num_uuids = 0;
    adv_uuids[num_uuids++] = (ble_uuid16_t)BLE_UUID16_INIT(BLE_SVC_CTS_UUID16);
    if (cfg && cfg->enable_ble_battery) {
        adv_uuids[num_uuids++] = (ble_uuid16_t)BLE_UUID16_INIT(BLE_SVC_BAS_UUID16);
    }
    fields.uuids16 = adv_uuids;
    fields.num_uuids16 = num_uuids;
    fields.uuids16_is_complete = 1;

    int rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set adv fields (rc=%d)", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(s_own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_cts_gap_event, NULL);
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        ESP_LOGE(TAG, "Failed to start BLE advertising (rc=%d)", rc);
    } else {
        ESP_LOGI(TAG, "BLE advertising started: '%s' (CTS 0x1805)", s_device_name);
    }
}

static int fetch_current_time(struct ble_svc_cts_curr_time *ctime) {
    if (!ctime) return -1;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm tm_info;
    gmtime_r(&tv.tv_sec, &tm_info);

    ctime->et_256.d_d_t.d_t.year = (uint16_t)(tm_info.tm_year + 1900);
    ctime->et_256.d_d_t.d_t.month = (uint8_t)(tm_info.tm_mon + 1);
    ctime->et_256.d_d_t.d_t.day = (uint8_t)tm_info.tm_mday;
    ctime->et_256.d_d_t.d_t.hours = (uint8_t)tm_info.tm_hour;
    ctime->et_256.d_d_t.d_t.minutes = (uint8_t)tm_info.tm_min;
    ctime->et_256.d_d_t.d_t.seconds = (uint8_t)tm_info.tm_sec;

    // Day of week: 1 = Monday ... 7 = Sunday
    ctime->et_256.d_d_t.day_of_week = (uint8_t)(tm_info.tm_wday == 0 ? 7 : tm_info.tm_wday);

    // Fraction 256: 1/256th second resolution
    ctime->et_256.fractions_256 = (uint8_t)(((uint64_t)tv.tv_usec * 256ULL) / 1000000ULL);

    // Adjust reason: External reference if GNSS synchronized
    ctime->adjust_reason = time_sync_is_synchronized() ? EXTERNAL_REFERENCE_TIME_UPDATE_MASK : 0;

    return 0;
}

static int fetch_local_time_info(struct ble_svc_cts_local_time_info *info) {
    if (!info) return -1;
    info->timezone = 0;              // UTC: offset = 0
    info->dst_offset = TIME_STANDARD; // Standard time (no DST)
    return 0;
}

static int fetch_reference_time_info(struct ble_svc_cts_reference_time_info *info) {
    if (!info) return -1;
    info->time_source = TIME_SOURCE_GPS;
    info->time_accuracy = 0; // Highest accuracy (< 1/8s)
    info->days_since_update = 0;
    info->hours_since_update = 0;
    return 0;
}

static int set_current_time(struct ble_svc_cts_curr_time ctime) {
    (void)ctime;
    // Stratum-1 GNSS server maintains master time: do not overwrite via BLE
    ESP_LOGW(TAG, "BLE client attempted write to CTS current time (ignored)");
    return 0;
}

static int set_local_time_info(struct ble_svc_cts_local_time_info info) {
    (void)info;
    return 0;
}

static void ble_cts_on_sync(void) {
    int rc = ble_hs_id_infer_auto(0, &s_own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error determining BLE address type (rc=%d)", rc);
        return;
    }

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(s_own_addr_type, addr_val, NULL);
    if (rc == 0) {
        ESP_LOGI(TAG, "BLE device address: %02x:%02x:%02x:%02x:%02x:%02x",
                 addr_val[5], addr_val[4], addr_val[3],
                 addr_val[2], addr_val[1], addr_val[0]);
    }

    ble_cts_advertise();
}

static void ble_cts_on_reset(int reason) {
    ESP_LOGW(TAG, "NimBLE reset (reason=%d)", reason);
}

static void ble_cts_host_task(void *param) {
    (void)param;
    ESP_LOGI(TAG, "NimBLE host task started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

esp_err_t ble_cts_service_start(const char *device_name) {
    if (s_started) {
        return ESP_OK;
    }

    if (device_name && device_name[0] != '\0') {
        strncpy(s_device_name, device_name, sizeof(s_device_name) - 1);
        s_device_name[sizeof(s_device_name) - 1] = '\0';
    }

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NVS for BLE: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NimBLE port: %s", esp_err_to_name(ret));
        return ret;
    }

    // GAP & GATT Services
    ble_svc_gap_init();
    ble_svc_gatt_init();

    // Device Information Service
    ble_svc_dis_init();
    ble_svc_dis_manufacturer_name_set("Heltec Automation");
    ble_svc_dis_model_number_set("Wireless Tracker");
    ble_svc_dis_firmware_revision_set("1.0.0");

    // Current Time Service (CTS)
    struct ble_svc_cts_cfg cts_cfg = {
        .fetch_time_cb = fetch_current_time,
        .local_time_info_cb = fetch_local_time_info,
        .ref_time_info_cb = fetch_reference_time_info,
        .set_time_cb = set_current_time,
        .set_local_time_info_cb = set_local_time_info
    };
    ble_svc_cts_init(cts_cfg);

    // Battery Service (BAS) & Power Telemetry Characteristic
    const app_config_t *cfg = config_mgr_get();
    if (cfg && cfg->enable_ble_battery) {
        ble_svc_bas_init();
        ble_gatts_count_cfg(s_custom_pwr_svcs);
        int pwr_rc = ble_gatts_add_svcs(s_custom_pwr_svcs);
        if (pwr_rc != 0) {
            ESP_LOGW(TAG, "Failed to register power telemetry service: %d", pwr_rc);
        } else {
            ESP_LOGI(TAG, "BLE Battery Service (BAS 0x180F) & Power Telemetry (0xFFE0) enabled");
        }
    }

    int rc = ble_svc_gap_device_name_set(s_device_name);
    if (rc != 0) {
        ESP_LOGW(TAG, "Failed to set GAP device name (rc=%d)", rc);
    }

    ble_hs_cfg.sync_cb = ble_cts_on_sync;
    ble_hs_cfg.reset_cb = ble_cts_on_reset;

    nimble_port_freertos_init(ble_cts_host_task);
    s_started = true;
    ESP_LOGI(TAG, "BLE CTS service initialized");
    return ESP_OK;
}

void ble_cts_service_notify_time(void) {
    if (s_started && s_connected) {
        ble_svc_cts_time_updated();
    }
}

void ble_cts_service_update_power(uint8_t percentage, uint16_t voltage_mv, bool usb_connected) {
    if (!s_started) return;
    s_pwr_data.percentage = percentage;
    s_pwr_data.voltage_mv = voltage_mv;
    s_pwr_data.usb_connected = usb_connected ? 1 : 0;

    const app_config_t *cfg = config_mgr_get();
    if (cfg && cfg->enable_ble_battery) {
        ble_svc_bas_battery_level_set(percentage);
        if (s_connected && s_pwr_val_handle != 0) {
            ble_gatts_chr_updated(s_pwr_val_handle);
        }
    }
}

bool ble_cts_service_is_connected(void) {
    return s_connected;
}
