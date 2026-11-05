idf_component_register(
    SRCS "main.c" "nmea_parser.c" "gnss_driver.c" "time_sync.c" "gnss_task.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_gpio esp_driver_uart esp_timer esp_wifi lwip
)


