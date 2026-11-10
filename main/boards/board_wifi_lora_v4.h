#pragma once

#include <driver/gpio.h>

#define BOARD_HARDWARE_NAME     "Heltec WiFi LoRa 32 V4"
#define BOARD_HARDWARE_REV      4
#define BOARD_HAS_GNSS_PPS      1  /* On SH1.25-8P GNSS connector */
#define BOARD_HAS_TFT_ST7735    0
#define BOARD_HAS_OLED_SSD1306  1  /* 0.96" 128x64 OLED (I2C) */
#define BOARD_HAS_FEM_PA        1  /* High-power model has FEM PA */

/* Front-End Module (FEM) / PA Controls */
#define PIN_PA_CSD              GPIO_NUM_7   /* V_FEM_Control */

/* Power Control */
#define PIN_VEXT_CTRL           GPIO_NUM_36  /* Active HIGH: powers external peripherals */

/* GNSS Interface (SH1.25-8Pin Connector) */
#define PIN_GNSS_RX             GPIO_NUM_39  /* ESP32 RX <- GNSS TX */
#define PIN_GNSS_TX             GPIO_NUM_38  /* ESP32 TX -> GNSS RX */
#define PIN_GNSS_RST            GPIO_NUM_42  /* GNSS Reset */
#define PIN_GNSS_PPS            GPIO_NUM_41  /* GNSS 1PPS */
#define PIN_GNSS_WAKE           GPIO_NUM_40  /* GNSS Wakeup */
#define GNSS_UART_NUM           UART_NUM_1
#define GNSS_DEFAULT_BAUD       115200

/* Status LED & User Controls */
#define PIN_STATUS_LED          GPIO_NUM_35  /* Status LED (Active HIGH) */
#define PIN_USER_BTN            GPIO_NUM_0   /* User Button PRG / BOOT (Active LOW, pull-up) */

/* OLED 0.96" I2C Display (128x64) */
#define PIN_OLED_SDA            GPIO_NUM_17  /* I2C SDA */
#define PIN_OLED_SCL            GPIO_NUM_18  /* I2C SCL */
#define PIN_OLED_RST            GPIO_NUM_21  /* OLED Reset */

/* Battery Voltage ADC */
#define PIN_VBAT_ADC            GPIO_NUM_1   /* ADC1_CH0 */
#define PIN_VBAT_CTRL           GPIO_NUM_37  /* ADC Ctrl (Active HIGH) */
#define VBAT_DIVIDER_RATIO      4.9f         /* (100k + 390k) / 100k = 4.9 */

/* Semtech SX1262 LoRa Transceiver */
#define PIN_LORA_SCK            GPIO_NUM_9   /* SPI SCK */
#define PIN_LORA_MOSI           GPIO_NUM_10  /* SPI MOSI */
#define PIN_LORA_MISO           GPIO_NUM_11  /* SPI MISO */
#define PIN_LORA_NSS            GPIO_NUM_8   /* SPI NSS / Chip Select */
#define PIN_LORA_RST            GPIO_NUM_12  /* SX1262 Reset (Active LOW) */
#define PIN_LORA_BUSY           GPIO_NUM_13  /* SX1262 Busy (Active HIGH) */
#define PIN_LORA_DIO1           GPIO_NUM_14  /* SX1262 DIO1 Interrupt */
