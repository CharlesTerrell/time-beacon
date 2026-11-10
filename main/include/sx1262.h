#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bandwidth constants for SX1262 */
#define SX1262_BW_125_KHZ       0x04
#define SX1262_BW_250_KHZ       0x05
#define SX1262_BW_500_KHZ       0x06

/* Coding rate constants for SX1262 */
#define SX1262_CR_4_5           0x01
#define SX1262_CR_4_6           0x02
#define SX1262_CR_4_7           0x03
#define SX1262_CR_4_8           0x04

/* Well-known sync words */
#define SX1262_SYNC_MESHTASTIC  0x24B4
#define SX1262_SYNC_PUBLIC      0x1424

/**
 * Initialize SX1262 hardware SPI, reset transceiver,
 * configure DIO3 TCXO (1.8V), DIO2 RF switch, and DC-DC regulator.
 */
esp_err_t sx1262_init(void);

/**
 * Put SX1262 into Standby mode.
 */
esp_err_t sx1262_set_standby(void);

/**
 * Configure carrier frequency in Hz (e.g. 906875000 for US915 Ch 0).
 */
esp_err_t sx1262_set_frequency(uint32_t freq_hz);

/**
 * Configure LoRa modulation parameters.
 * @param sf Spreading factor (7 to 12)
 * @param bw Bandwidth code (SX1262_BW_125_KHZ, SX1262_BW_250_KHZ, etc.)
 * @param cr Coding rate code (SX1262_CR_4_5, etc.)
 * @param ldro Low data rate optimize enabled
 */
esp_err_t sx1262_set_lora_params(uint8_t sf, uint8_t bw, uint8_t cr, bool ldro);

/**
 * Set LoRa sync word.
 */
esp_err_t sx1262_set_sync_word(uint16_t sync_word);

/**
 * Set transmit power in dBm (up to +22 dBm).
 */
esp_err_t sx1262_set_tx_power(int8_t power_dbm);

/**
 * Transmit a payload synchronously. Blocks until packet TX completes or times out.
 */
esp_err_t sx1262_transmit(const uint8_t *payload, uint8_t len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
