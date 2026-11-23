#pragma once

#include <driver/gpio.h>

#define BOARD_HARDWARE_NAME     "Heltec Wireless Tracker v1.1"
#define BOARD_HARDWARE_REV      1
#define BOARD_HAS_GNSS_PPS      1  /* Active GNSS 1PPS on GPIO 36 */
#define BOARD_HAS_TFT_ST7735    1
#define BOARD_HAS_OLED_SSD1306  0
#define BOARD_HAS_FEM_PA        0

/* Power Control */
#define PIN_VEXT_CTRL           GPIO_NUM_3   /* Active HIGH: powers built-in TFT and GNSS */

/* UC6580 GNSS Module */
#define PIN_GNSS_RX             GPIO_NUM_33  /* ESP32 RX <- UC6580 TX */
#define PIN_GNSS_TX             GPIO_NUM_34  /* ESP32 TX -> UC6580 RX */
#define PIN_GNSS_RST            GPIO_NUM_35  /* GNSS Reset */
#define PIN_GNSS_PPS            GPIO_NUM_36  /* GNSS 1PPS */
#define GNSS_UART_NUM           UART_NUM_1
#define GNSS_DEFAULT_BAUD       115200

/* Status LED & User Controls */
#define PIN_STATUS_LED          GPIO_NUM_18  /* Status LED (Active HIGH) */
#define PIN_USER_BTN            GPIO_NUM_0   /* User Button PRG (Active LOW, pull-up) */

/* ST7735 0.96" SPI TFT Display (80x160) */
#define PIN_TFT_SDIN            GPIO_NUM_42  /* SPI MOSI */
#define PIN_TFT_SCLK            GPIO_NUM_41  /* SPI CLK */
#define PIN_TFT_DC              GPIO_NUM_40  /* Data / Command (RS) */
#define PIN_TFT_RST             GPIO_NUM_39  /* Display Reset (RES) */
#define PIN_TFT_CS              GPIO_NUM_38  /* Chip Select */
#define PIN_TFT_BL              GPIO_NUM_21  /* Backlight control (Active HIGH) */

/* Battery Voltage ADC */
#define PIN_VBAT_ADC            GPIO_NUM_1   /* VBAT = Vbat_Read * 4.9 */
#define PIN_VBAT_CTRL           GPIO_NUM_2   /* ADC Ctrl */
#define VBAT_DIVIDER_RATIO      4.9f

/* Semtech SX1262 LoRa Transceiver */
#define PIN_LORA_SCK            GPIO_NUM_9   /* SPI SCK */
#define PIN_LORA_MOSI           GPIO_NUM_10  /* SPI MOSI */
#define PIN_LORA_MISO           GPIO_NUM_11  /* SPI MISO */
#define PIN_LORA_NSS            GPIO_NUM_8   /* SPI NSS / Chip Select */
#define PIN_LORA_RST            GPIO_NUM_12  /* SX1262 Reset (Active LOW) */
#define PIN_LORA_BUSY           GPIO_NUM_13  /* SX1262 Busy (Active HIGH) */
#define PIN_LORA_DIO1           GPIO_NUM_14  /* SX1262 DIO1 Interrupt */
