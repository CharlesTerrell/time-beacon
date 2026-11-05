#include "gnss_driver.h"
#include "board_pins.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "gnss_driver";

esp_err_t gnss_driver_init(void) {
    // Configure PIN_VEXT_CTRL as GPIO output, no pull-up, no pull-down
    gpio_reset_pin(PIN_VEXT_CTRL);
    gpio_set_direction(PIN_VEXT_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(PIN_VEXT_CTRL, GPIO_PULLUP_DISABLE);
    gpio_set_pull_mode(PIN_VEXT_CTRL, GPIO_PULLDOWN_DISABLE);

    // Set gpio_set_level(PIN_VEXT_CTRL, 1) to power on the GNSS module
    gpio_set_level(PIN_VEXT_CTRL, 1);

    // Call vTaskDelay(pdMS_TO_TICKS(100)) to allow power stabilization
    vTaskDelay(pdMS_TO_TICKS(100));

    // Configure uart_config_t
    uart_config_t uart_config = {
        .baud_rate = GNSS_DEFAULT_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Call uart_param_config(GNSS_UART_NUM, &uart_config). Check error
    esp_err_t err = uart_param_config(GNSS_UART_NUM, &uart_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure UART parameters: %s", esp_err_to_name(err));
        return err;
    }

    // Call uart_set_pin(GNSS_UART_NUM, PIN_GNSS_TX, PIN_GNSS_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE). Check error
    err = uart_set_pin(GNSS_UART_NUM, PIN_GNSS_TX, PIN_GNSS_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set UART pins: %s", esp_err_to_name(err));
        return err;
    }

    // Call uart_driver_install(GNSS_UART_NUM, 1024, 0, 0, NULL, 0). Check error
    err = uart_driver_install(GNSS_UART_NUM, 1024, 0, 0, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install UART driver: %s", esp_err_to_name(err));
        return err;
    }

    // Log info ESP_LOGI(TAG, "GNSS hardware initialized on UART%d", GNSS_UART_NUM)
    ESP_LOGI(TAG, "GNSS hardware initialized on UART%d", GNSS_UART_NUM);

    // Return ESP_OK
    return ESP_OK;
}

int gnss_driver_read(uint8_t *buf, size_t max_len, uint32_t timeout_ms) {
    // Return -1 if buf == NULL || max_len == 0
    if (buf == NULL || max_len == 0) {
        return -1;
    }

    // Return uart_read_bytes(GNSS_UART_NUM, buf, max_len, pdMS_TO_TICKS(timeout_ms))
    return uart_read_bytes(GNSS_UART_NUM, buf, max_len, pdMS_TO_TICKS(timeout_ms));
}

esp_err_t gnss_driver_send_cmd(const char *cmd) {
    // Return ESP_ERR_INVALID_ARG if cmd == NULL
    if (cmd == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Write bytes: int written = uart_write_bytes(GNSS_UART_NUM, cmd, strlen(cmd));
    int written = uart_write_bytes(GNSS_UART_NUM, cmd, strlen(cmd));

    // Return written >= 0 ? ESP_OK : ESP_FAIL
    return written >= 0 ? ESP_OK : ESP_FAIL;
}
