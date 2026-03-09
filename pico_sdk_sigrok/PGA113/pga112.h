#ifndef PGA112_H
#define PGA112_H

#include <stdint.h>
#include <stddef.h>
#include "../SW_SPI/sw_spi.h"
#include "pga11x_spi.h"

// PGA112 Programmable Gain Amplifier Control
// TI PGA112 - Serial input controlled, 8 binary gain settings with channel selection

// Use definitions from pga11x_spi.h
typedef pga11x_gain_t pga112_gain_t;
typedef pga11x_ch_t pga112_ch_t;

// PGA112 device instance
typedef struct {
    sw_spi_t *spi;              // Pointer to SW_SPI instance
    uint8_t cs_index;           // Chip select index (0-2)
    pga112_gain_t current_gain; // Current gain setting
    pga112_ch_t current_channel; // Current channel setting
} pga112_t;

/**
 * Initialize PGA112 instance
 * @param pga Pointer to pga112_t structure
 * @param spi Pointer to initialized sw_spi_t instance
 * @param cs_index Chip select index to use (0-2)
 * @return 0 on success, -1 on invalid parameters
 */
int8_t pga112_init(pga112_t *pga, sw_spi_t *spi, uint8_t cs_index);

/**
 * Set the gain and channel of the amplifier
 * @param pga Pointer to pga112_t structure
 * @param gain Gain setting (PGA11X_GAIN_BINARY_*)
 * @param channel Channel setting (PGA11X_CH_VCAL_CH0 or PGA11X_CH_1)
 * @return 0 on success, -1 on error
 */
int8_t pga112_set_gain_and_channel(pga112_t *pga, pga112_gain_t gain, pga112_ch_t channel);

/**
 * Get the current gain setting
 * @param pga Pointer to pga112_t structure
 * @return Current gain setting
 */
pga112_gain_t pga112_get_gain(pga112_t *pga);

/**
 * Get the current channel setting
 * @param pga Pointer to pga112_t structure
 * @return Current channel setting
 */
pga112_ch_t pga112_get_channel(pga112_t *pga);

/**
 * Get the numeric gain value
 * @param gain Gain setting
 * @return Numeric gain value (1, 2, 4, 8, 16, 32, 64, or 128)
 */
uint16_t pga112_gain_to_value(pga112_gain_t gain);

/**
 * Set gain and channel from numeric value and channel
 * @param pga Pointer to pga112_t structure
 * @param gain_value Numeric gain (1, 2, 4, 8, 16, 32, 64, or 128)
 * @param channel Channel setting
 * @return 0 on success, -1 on invalid gain value
 */
int8_t pga112_set_gain_value_and_channel(pga112_t *pga, uint16_t gain_value, pga112_ch_t channel);

/**
 * Get gain value in dB
 * @param gain Gain setting
 * @return Gain in dB (approximately)
 */
float pga112_gain_to_db(pga112_gain_t gain);

/**
 * Enable the amplifier (set default gain and channel)
 * @param pga Pointer to pga112_t structure
 * @param initial_gain Initial gain setting
 * @param initial_channel Initial channel setting
 * @return 0 on success, -1 on error
 */
int8_t pga112_enable(pga112_t *pga, pga112_gain_t initial_gain, pga112_ch_t initial_channel);


int8_t pga112_shutdown(pga112_t *pga);

/**
 * Print current gain and channel setting status
 * @param pga Pointer to pga112_t structure
 */
void pga112_print_status(pga112_t *pga);

#endif // PGA112_H
