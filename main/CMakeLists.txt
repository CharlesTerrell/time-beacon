idf_component_register(
    SRCS "main.c" "nmea_parser.c" "gnss_driver.c" "time_sync.c" "gnss_task.c" "wifi_manager.c" "ntp_server.c" "display.c" "config_mgr.c" "usb_msc.c" "power_mgr.c" "ble_cts_service.c" "sx1262.c" "lora_time_beacons.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_gpio esp_driver_uart esp_driver_spi esp_timer esp_wifi lwip nvs_flash esp_event esp_partition wear_levelling fatfs esp_tinyusb esp_adc esp_driver_ledc esp_driver_usb_serial_jtag bt mbedtls
)
