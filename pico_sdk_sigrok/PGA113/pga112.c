#include "pga112.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

int8_t pga112_init(pga112_t *pga, sw_spi_t *spi, uint8_t cs_index) {
    if (!pga || !spi || cs_index >= spi->cs_count) {
        return -1;
    }
    
    pga->spi = spi;
    pga->cs_index = cs_index;
    pga->current_gain = PGA11X_GAIN_BINARY_1; // Default to gain of 1
    pga->current_channel = PGA11X_CH_VCAL_CH0; // Default to CH0 (Vcal)
    
    return 0;
}

// Set gain and channel
int8_t pga112_set_gain_and_channel(pga112_t *pga, pga112_gain_t gain, pga112_ch_t channel) {
    if (!pga || !pga->spi) {
        return -1;
    }
    
    // Set the active CS for this device
    sw_spi_set_active_cs(pga->spi, pga->cs_index);
    
    // Build the 16-bit SPI command word
    uint16_t command = PGA11X_WRITE_WORD(gain, channel);
    
    // Send MSB first
    uint8_t data[2];
    data[0] = (command >> 8) & 0xFF; // MSB
    data[1] = command & 0xFF;         // LSB
    
    // Write the 16-bit word via SPI
    sw_spi_write(pga->spi, data, 2);
    
    // Update the cached values
    pga->current_gain = gain;
    pga->current_channel = channel;
    
    return 0;
}

pga112_gain_t pga112_get_gain(pga112_t *pga) {
    if (!pga) {
        return PGA11X_GAIN_BINARY_1;
    }
    return pga->current_gain;
}

pga112_ch_t pga112_get_channel(pga112_t *pga) {
    if (!pga) {
        return PGA11X_CH_VCAL_CH0;
    }
    return pga->current_channel;
}

uint16_t pga112_gain_to_value(pga112_gain_t gain) {
    // Map gain setting (0-7) to binary gain values
    // Gains are: 1, 2, 4, 8, 16, 32, 64, 128
    // gain = 2^(gain_setting)
    return (uint16_t)(1 << (gain & 0x07));
}

int8_t pga112_set_gain_value_and_channel(pga112_t *pga, uint16_t gain_value, pga112_ch_t channel) {
    pga112_gain_t gain_setting;
    
    // Convert numeric gain to gain setting
    switch (gain_value) {
        case 1:
            gain_setting = PGA11X_GAIN_BINARY_1;
            break;
        case 2:
            gain_setting = PGA11X_GAIN_BINARY_2;
            break;
        case 4:
            gain_setting = PGA11X_GAIN_BINARY_4;
            break;
        case 8:
            gain_setting = PGA11X_GAIN_BINARY_8;
            break;
        case 16:
            gain_setting = PGA11X_GAIN_BINARY_16;
            break;
        case 32:
            gain_setting = PGA11X_GAIN_BINARY_32;
            break;
        case 64:
            gain_setting = PGA11X_GAIN_BINARY_64;
            break;
        case 128:
            gain_setting = PGA11X_GAIN_BINARY_128;
            break;
        default:
            return -1; // Invalid gain value
    }
    
    return pga112_set_gain_and_channel(pga, gain_setting, channel);
}

float pga112_gain_to_db(pga112_gain_t gain) {
    // Convert linear gain to dB: dB = 20 * log10(gain)
    uint16_t gain_value = pga112_gain_to_value(gain);
    return 20.0f * log10f((float)gain_value);
}

int8_t pga112_enable(pga112_t *pga, pga112_gain_t initial_gain, pga112_ch_t initial_channel) {
    if (!pga) {
        return -1;
    }
    
    // Set the initial gain and channel
    return pga112_set_gain_and_channel(pga, initial_gain, initial_channel);
}

int8_t pga112_shutdown(pga112_t *pga) {
    if (!pga) {
        return -1;
    }
    
    // MAp command to 2 bytes for spi write
    uint8_t data[2];
    data[0] = (PGA11X_CMD_SDN_EN >> 8) & 0xFF; // MSB
    data[1] = PGA11X_CMD_SDN_EN & 0xFF;         // LSB
    
    // Write the 16-bit shutdown command via SPI
    sw_spi_write(pga->spi, data, 2);
   
    return 0;
}


void pga112_print_status(pga112_t *pga) {
    if (!pga) {
        printf("PGA112: Invalid device pointer\n");
        return;
    }
    
    uint16_t gain_value = pga112_gain_to_value(pga->current_gain);
    float gain_db = pga112_gain_to_db(pga->current_gain);
    
    const char *channel_str = (pga->current_channel == PGA11X_CH_VCAL_CH0) ? "CH0 (Vcal)" : "CH1 (Input)";
    
    printf("PGA112 Status (CS Index %u):\n", pga->cs_index);
    printf("  Gain Setting: %u\n", pga->current_gain);
    printf("  Gain Value: %u\n", gain_value);
    printf("  Gain (dB): %.2f dB\n", gain_db);
    printf("  Channel: %s\n", channel_str);
}
