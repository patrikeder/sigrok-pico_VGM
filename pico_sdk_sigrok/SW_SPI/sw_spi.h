#ifndef SW_SPI_H
#define SW_SPI_H

#include <stdint.h>
#include <stddef.h>

// Support up to 3 CS lines and dynamic selection
typedef struct {
    uint32_t cs_pins[3];
    uint8_t cs_count;     // number of CS pins provided (1..3)
    int8_t active_cs;     // index of selected CS (-1 = none)
    uint32_t clk_pin;
    uint32_t dio_pin;
    uint32_t delay_us; // delay before/after clock edge to control speed
} sw_spi_t;

// Initialize with an array of CS pins (length cs_count, max 3), plus CLK and DIO pins and delay.
void sw_spi_init(sw_spi_t *spi, const uint32_t *cs_pins, uint8_t cs_count, uint32_t clk_pin, uint32_t dio_pin, uint32_t delay_us);

void sw_spi_setDelay(sw_spi_t *spi, uint32_t delay_us);
void sw_spi_getDelay(sw_spi_t *spi, uint32_t *delay_us);

void sw_spi_set_active_cs(sw_spi_t *spi, int8_t index);
void sw_spi_deactivate_all_cs(sw_spi_t *spi);

void sw_spi_write32(sw_spi_t *spi, uint32_t data);
void sw_spi_write(sw_spi_t *spi, const uint8_t *data, size_t len);

// Perform a write (command) then pause (CS idle) then a read. pause_us is the idle time in microseconds.
void sw_spi_write_then_read(sw_spi_t *spi, const uint8_t *cmd, size_t cmd_len, uint32_t pause_us, uint8_t *out, size_t out_len);

#endif // SW_SPI_H
