#pragma once

#include <driver/gpio.h>

/**
 * Heltec Wireless Tracker v1.1 Hardware Pin Definitions
 * Reference: Heltec Wireless Tracker Rev 1.1 Datasheet (Table 2-2-1 & 2-2-2)
 */

/* Power Control */
#define PIN_VEXT_CTRL       GPIO_NUM_3   /* Active HIGH: powers built-in TFT and GNSS */

/* UC6580 GNSS Module */
#define PIN_GNSS_RX         GPIO_NUM_33  /* ESP32 RX <- UC6580 TX */
#define PIN_GNSS_TX         GPIO_NUM_34  /* ESP32 TX -> UC6580 RX */
#define PIN_GNSS_RST        GPIO_NUM_35  /* GNSS Reset */
#define PIN_GNSS_PPS        GPIO_NUM_36  /* Note: Not routed on v1.1 PCB */
#define GNSS_UART_NUM       UART_NUM_1
#define GNSS_DEFAULT_BAUD   115200

/* Status LED & User Controls */
#define PIN_STATUS_LED      GPIO_NUM_18  /* Status LED (Active HIGH) */
#define PIN_USER_BTN        GPIO_NUM_0   /* User Button PRG (Active LOW, pull-up) */

/* ST7735 0.96" SPI TFT Display (80x160) */
#define PIN_TFT_SDIN        GPIO_NUM_42  /* SPI MOSI */
#define PIN_TFT_SCLK        GPIO_NUM_41  /* SPI CLK */
#define PIN_TFT_DC          GPIO_NUM_40  /* Data / Command (RS) */
#define PIN_TFT_RST         GPIO_NUM_39  /* Display Reset (RES) */
#define PIN_TFT_CS          GPIO_NUM_38  /* Chip Select */
#define PIN_TFT_BL          GPIO_NUM_21  /* Backlight control (Active HIGH) */

/* Battery Voltage ADC */
#define PIN_VBAT_ADC        GPIO_NUM_1   /* VBAT = Vbat_Read * 4.9 */
#define PIN_VBAT_CTRL       GPIO_NUM_2   /* ADC Ctrl */
