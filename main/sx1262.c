#include "sx1262.h"
#include "board_pins.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "sx1262";
static spi_device_handle_t s_spi = NULL;

static esp_err_t sx1262_wait_busy(uint32_t timeout_ms) {
    int64_t start = esp_timer_get_time();
    int64_t limit = (int64_t)timeout_ms * 1000LL;
    while (gpio_get_level(PIN_LORA_BUSY) == 1) {
        if ((esp_timer_get_time() - start) > limit) {
            ESP_LOGW(TAG, "Wait busy timed out (%lu ms)", (unsigned long)timeout_ms);
            return ESP_ERR_TIMEOUT;
        }
        esp_rom_delay_us(10);
    }
    return ESP_OK;
}

static esp_err_t sx1262_write_cmd(uint8_t opcode, const uint8_t *data, size_t len) {
    sx1262_wait_busy(100);
    gpio_set_level(PIN_LORA_NSS, 0);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &opcode;
    esp_err_t err = spi_device_polling_transmit(s_spi, &t);
    if (err == ESP_OK && data && len > 0) {
        memset(&t, 0, sizeof(t));
        t.length = len * 8;
        t.tx_buffer = data;
        err = spi_device_polling_transmit(s_spi, &t);
    }
    gpio_set_level(PIN_LORA_NSS, 1);
    sx1262_wait_busy(100);
    return err;
}

static esp_err_t sx1262_write_register(uint16_t reg, const uint8_t *data, size_t len) {
    sx1262_wait_busy(100);
    gpio_set_level(PIN_LORA_NSS, 0);

    uint8_t hdr[3] = { 0x0D, (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF) };
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 24;
    t.tx_buffer = hdr;
    esp_err_t err = spi_device_polling_transmit(s_spi, &t);
    if (err == ESP_OK && data && len > 0) {
        memset(&t, 0, sizeof(t));
        t.length = len * 8;
        t.tx_buffer = data;
        err = spi_device_polling_transmit(s_spi, &t);
    }
    gpio_set_level(PIN_LORA_NSS, 1);
    sx1262_wait_busy(100);
    return err;
}

esp_err_t sx1262_init(void) {
    // Configure GPIOs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_LORA_NSS) | (1ULL << PIN_LORA_RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(PIN_LORA_NSS, 1);
    gpio_set_level(PIN_LORA_RST, 1);

    io_conf.pin_bit_mask = (1ULL << PIN_LORA_BUSY) | (1ULL << PIN_LORA_DIO1);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

#if defined(CONFIG_BOARD_HELTEC_V2)
    // Configure Front-End Module (FEM) PA controls on v2
    io_conf.pin_bit_mask = (1ULL << PIN_PA_CSD) | (1ULL << PIN_PA_CPS);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(PIN_PA_CSD, 0);
    gpio_set_level(PIN_PA_CPS, 0);
#endif

    // Initialize SPI3 bus
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_LORA_MISO,
        .mosi_io_num = PIN_LORA_MOSI,
        .sclk_io_num = PIN_LORA_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 256,
    };
    esp_err_t err = spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI3 bus: %s", esp_err_to_name(err));
        return err;
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 8000000, // 8 MHz
        .mode = 0,                 // CPOL=0, CPHA=0
        .spics_io_num = -1,        // Manually managed NSS
        .queue_size = 1,
    };
    err = spi_bus_add_device(SPI3_HOST, &devcfg, &s_spi);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(err));
        return err;
    }

    // Hardware Reset
    gpio_set_level(PIN_LORA_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(15));
    gpio_set_level(PIN_LORA_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(25));
    sx1262_wait_busy(100);

    // Set Standby (STDBY_RC = 0x00)
    uint8_t stdby_mode = 0x00;
    sx1262_write_cmd(0x80, &stdby_mode, 1);

    // Set Regulator Mode (DC-DC = 0x01)
    uint8_t reg_mode = 0x01;
    sx1262_write_cmd(0x96, &reg_mode, 1);

    // Set DIO3 as TCXO Ctrl (1.8V = 0x02, delay = 320 -> 5ms)
    uint8_t tcxo_cfg[4] = { 0x02, 0x00, 0x01, 0x40 };
    sx1262_write_cmd(0x97, tcxo_cfg, 4);

    // Set DIO2 as RF Switch Control (0x01 = enable)
    uint8_t rf_sw = 0x01;
    sx1262_write_cmd(0x9D, &rf_sw, 1);

    // Set Packet Type (LoRa = 0x01)
    uint8_t pkt_type = 0x01;
    sx1262_write_cmd(0x8A, &pkt_type, 1);

    // Set Default PA Config (+22 dBm capable)
    uint8_t pa_cfg[4] = { 0x04, 0x07, 0x00, 0x01 };
    sx1262_write_cmd(0x95, pa_cfg, 4);

    // Set Overcurrent Protection (OCP: 140 mA = 0x38 in reg 0x08E7)
    uint8_t ocp = 0x38;
    sx1262_write_register(0x08E7, &ocp, 1);

    ESP_LOGI(TAG, "SX1262 transceiver initialized successfully");
    return ESP_OK;
}

esp_err_t sx1262_set_standby(void) {
    uint8_t mode = 0x00; // STDBY_RC
    return sx1262_write_cmd(0x80, &mode, 1);
}

esp_err_t sx1262_set_frequency(uint32_t freq_hz) {
    // freq_step = 32 MHz / 2^25
    uint32_t freq_reg = (uint32_t)(((uint64_t)freq_hz << 25) / 32000000ULL);
    uint8_t buf[4] = {
        (uint8_t)((freq_reg >> 24) & 0xFF),
        (uint8_t)((freq_reg >> 16) & 0xFF),
        (uint8_t)((freq_reg >> 8) & 0xFF),
        (uint8_t)(freq_reg & 0xFF)
    };
    return sx1262_write_cmd(0x86, buf, 4);
}

esp_err_t sx1262_set_lora_params(uint8_t sf, uint8_t bw, uint8_t cr, bool ldro) {
    uint8_t buf[4] = {
        sf,
        bw,
        cr,
        ldro ? 0x01 : 0x00
    };
    return sx1262_write_cmd(0x8B, buf, 4);
}

esp_err_t sx1262_set_sync_word(uint16_t sync_word) {
    uint8_t sw[2] = {
        (uint8_t)((sync_word >> 8) & 0xFF),
        (uint8_t)(sync_word & 0xFF)
    };
    return sx1262_write_register(0x0740, sw, 2);
}

esp_err_t sx1262_set_tx_power(int8_t power_dbm) {
    if (power_dbm > 22) power_dbm = 22;
    if (power_dbm < -9) power_dbm = -9;

    // SetTxParams: power, rampTime = 0x02 (40 us)
    uint8_t tx_params[2] = { (uint8_t)power_dbm, 0x02 };
    return sx1262_write_cmd(0x8E, tx_params, 2);
}

esp_err_t sx1262_transmit(const uint8_t *payload, uint8_t len, uint32_t timeout_ms) {
    if (!payload || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    // Set buffer base addresses: TX=0, RX=0
    uint8_t base_addr[2] = { 0x00, 0x00 };
    sx1262_write_cmd(0x8F, base_addr, 2);

    // Set Packet Params:
    // PreambleLength = 16 (0x0010), HeaderType = Explicit (0x00),
    // PayloadLength = len, CRCType = On (0x01), InvertIQ = Standard (0x00)
    uint8_t pkt_params[6] = { 0x00, 0x10, 0x00, len, 0x01, 0x00 };
    sx1262_write_cmd(0x8C, pkt_params, 6);

    // Write Payload to buffer at offset 0
    sx1262_wait_busy(100);
    gpio_set_level(PIN_LORA_NSS, 0);
    uint8_t write_hdr[2] = { 0x0E, 0x00 };
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = write_hdr;
    spi_device_polling_transmit(s_spi, &t);

    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = payload;
    spi_device_polling_transmit(s_spi, &t);
    gpio_set_level(PIN_LORA_NSS, 1);
    sx1262_wait_busy(100);

    // Clear IRQ status: 0x03FF
    uint8_t clr_irq[2] = { 0x03, 0xFF };
    sx1262_write_cmd(0x02, clr_irq, 2);

    // Set DIO1 to trigger on TxDone (0x0001)
    uint8_t dio1_mask[8] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 };
    sx1262_write_cmd(0x08, dio1_mask, 8);

#if defined(CONFIG_BOARD_HELTEC_V2)
    // Enable Front-End Module PA for TX
    gpio_set_level(PIN_PA_CSD, 1);
    gpio_set_level(PIN_PA_CPS, 1);
#endif

    // SetTx: timeout = 0 (single transmission)
    uint8_t tx_timeout[3] = { 0x00, 0x00, 0x00 };
    sx1262_write_cmd(0x83, tx_timeout, 3);

    // Wait for TxDone on DIO1 pin
    int64_t start = esp_timer_get_time();
    int64_t limit = (int64_t)timeout_ms * 1000LL;
    bool done = false;
    while ((esp_timer_get_time() - start) < limit) {
        if (gpio_get_level(PIN_LORA_DIO1) == 1) {
            done = true;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

#if defined(CONFIG_BOARD_HELTEC_V2)
    // Disable Front-End Module PA
    gpio_set_level(PIN_PA_CPS, 0);
    gpio_set_level(PIN_PA_CSD, 0);
#endif

    // Clear IRQ and put back in Standby
    sx1262_write_cmd(0x02, clr_irq, 2);
    sx1262_set_standby();

    if (!done) {
        ESP_LOGW(TAG, "Transmission timed out after %lu ms", (unsigned long)timeout_ms);
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}
