#include "usb_msc.h"
#include "config_mgr.h"
#include "board_pins.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "driver/gpio.h"
#include "wear_levelling.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tinyusb_msc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <stdio.h>

static const char *TAG = "usb_msc";

#define EPNUM_MSC       1
#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN)

enum {
    ITF_NUM_MSC = 0,
    ITF_NUM_TOTAL
};

enum {
    EDPT_CTRL_OUT = 0x00,
    EDPT_CTRL_IN  = 0x80,
    EDPT_MSC_OUT  = 0x01,
    EDPT_MSC_IN   = 0x81,
};

static tusb_desc_device_t descriptor_config = {
    .bLength = sizeof(descriptor_config),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x303A,
    .idProduct = 0x4002,
    .bcdDevice = 0x100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

static uint8_t const msc_fs_configuration_desc[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EDPT_MSC_OUT, EDPT_MSC_IN, 64),
};

static char const *string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 },  // English (0x0409)
    "Heltec",                       // Manufacturer
    "Tracker NTP Config Drive",     // Product
    "12345678",                     // Serial
    "NTP_CONFIG",                   // Volume label
};

static tinyusb_msc_storage_handle_t s_storage_hdl = NULL;
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

static void storage_mount_cb(tinyusb_msc_storage_handle_t handle, tinyusb_msc_event_t *event, void *arg) {
    (void)handle;
    (void)arg;
    if (event->id == TINYUSB_MSC_EVENT_MOUNT_COMPLETE) {
        ESP_LOGI(TAG, "Storage mount complete: %s",
                 event->mount_point == TINYUSB_MSC_STORAGE_MOUNT_USB ? "USB Host" : "App");
    }
}

esp_err_t usb_msc_start(void) {
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "storage");
    if (!part) {
        ESP_LOGE(TAG, "Storage partition not found");
        return ESP_ERR_NOT_FOUND;
    }

    esp_err_t err = wl_mount(part, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wl_mount failed: %s", esp_err_to_name(err));
        return err;
    }

    tinyusb_msc_storage_config_t storage_cfg = {
        .mount_point = TINYUSB_MSC_STORAGE_MOUNT_APP,
        .fat_fs = {
            .base_path = "/storage",
            .config.max_files = 4,
            .format_flags = 0,
        },
        .medium.wl_handle = s_wl_handle,
    };

    err = tinyusb_msc_new_storage_spiflash(&storage_cfg, &s_storage_hdl);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "tinyusb_msc_new_storage_spiflash failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_ERROR_CHECK(tinyusb_msc_set_storage_callback(storage_mount_cb, NULL));

    // Ensure default config.ini exists before exposing to USB host
    FILE *f = fopen("/storage/config.ini", "r");
    if (!f) {
        config_mgr_write_default_ini("/storage/config.ini");
    } else {
        fclose(f);
    }

    // Expose storage to USB host PC
    ESP_ERROR_CHECK(tinyusb_msc_set_storage_mount_point(s_storage_hdl, TINYUSB_MSC_STORAGE_MOUNT_USB));

    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    tusb_cfg.descriptor.device = &descriptor_config;
    tusb_cfg.descriptor.full_speed_config = msc_fs_configuration_desc;
    tusb_cfg.descriptor.string = string_desc_arr;
    tusb_cfg.descriptor.string_count = sizeof(string_desc_arr) / sizeof(string_desc_arr[0]);

    err = tinyusb_driver_install(&tusb_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "tinyusb_driver_install failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "USB MSC config drive mounted successfully");
    return ESP_OK;
}

void usb_msc_run_loop(void) {
    ESP_LOGI(TAG, "USB MSC active. Tap USER button when done to reboot.");
    vTaskDelay(pdMS_TO_TICKS(1500)); // Allow button debounce after boot

    while (1) {
        if (gpio_get_level(PIN_USER_BTN) == 0) {
            ESP_LOGI(TAG, "USER button tapped. Rebooting...");
            vTaskDelay(pdMS_TO_TICKS(500));
            esp_restart();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
