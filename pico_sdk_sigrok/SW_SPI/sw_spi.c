#include "sw_spi.h"
#include "pico/stdlib.h"

static inline void delay_us(uint32_t us) {
    if (us) busy_wait_us_32(us);
}

static inline void cs_deactivate_all(sw_spi_t *spi) {
    for (uint8_t i = 0; i < spi->cs_count; ++i) {
        gpio_put(spi->cs_pins[i], 1);
    }
}

static inline void cs_activate_index(sw_spi_t *spi, int8_t idx) {
    if (idx < 0 || idx >= spi->cs_count) return;
    gpio_put(spi->cs_pins[idx], 0);
}

void sw_spi_init(sw_spi_t *spi, const uint32_t *cs_pins, uint8_t cs_count, uint32_t clk_pin, uint32_t dio_pin, uint32_t delay_us) {
    if (cs_count == 0) cs_count = 1;
    if (cs_count > 3) cs_count = 3;
    spi->cs_count = cs_count;
    spi->active_cs = -1;
    for (uint8_t i = 0; i < spi->cs_count; ++i) {
        spi->cs_pins[i] = cs_pins[i];
    }
    spi->clk_pin = clk_pin;
    spi->dio_pin = dio_pin;
    spi->delay_us = delay_us;

    // init CS pins and set idle (high)
    for (uint8_t i = 0; i < spi->cs_count; ++i) {
        gpio_init(spi->cs_pins[i]);
        gpio_disable_pulls(spi->cs_pins[i]);
        gpio_set_dir(spi->cs_pins[i], GPIO_OUT);
        gpio_put(spi->cs_pins[i], 1);
    }

    gpio_init(spi->clk_pin);
    gpio_disable_pulls(spi->clk_pin);
    gpio_set_dir(spi->clk_pin, GPIO_OUT);
    gpio_put(spi->clk_pin, 0); // idle low

    gpio_init(spi->dio_pin);
    gpio_disable_pulls(spi->dio_pin);
    // default to output so the line can be driven on first write
    gpio_set_dir(spi->dio_pin, GPIO_OUT);
    gpio_put(spi->dio_pin, 0);
}

void sw_spi_set_active_cs(sw_spi_t *spi, int8_t index) {
    if (index < 0 || index >= spi->cs_count) {
        spi->active_cs = -1;
        return;
    }
    spi->active_cs = index;
}

void sw_spi_setDelay(sw_spi_t *spi, uint32_t delay_us){
    spi->delay_us = delay_us;
}

void sw_spi_getDelay(sw_spi_t *spi, uint32_t *delay_us){
    *delay_us = spi->delay_us;
}


void sw_spi_deactivate_all_cs(sw_spi_t *spi) {
    cs_deactivate_all(spi);
    spi->active_cs = -1;
}

static inline int8_t active_idx_or_default(sw_spi_t *spi) {
    if (spi->active_cs >= 0 && spi->active_cs < spi->cs_count) return spi->active_cs;
    return 0;
}

void sw_spi_write(sw_spi_t *spi, const uint8_t *data, size_t len) {
    // Ensure DIO is output
    gpio_set_dir(spi->dio_pin, GPIO_OUT);
    gpio_put(spi->clk_pin, 0);

    int8_t idx = active_idx_or_default(spi);
    cs_activate_index(spi, idx);

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = data[i];
        for (int bit = 7; bit >= 0; --bit) {
            int v = (b >> bit) & 1;
            gpio_put(spi->dio_pin, v);
            delay_us(spi->delay_us);
            gpio_put(spi->clk_pin, 1);
            delay_us(spi->delay_us);
            gpio_put(spi->clk_pin, 0);
            // small settle
            delay_us(1);
        }
    }

    cs_deactivate_all(spi);
}

void sw_spi_write32(sw_spi_t *spi, uint32_t data) {
    uint8_t b[4];
    // send MSB first
    b[0] = (data >> 24) & 0xFF;
    b[1] = (data >> 16) & 0xFF;
    b[2] = (data >> 8) & 0xFF;
    b[3] = (data) & 0xFF;
    sw_spi_write(spi, b, 4);
}

void sw_spi_write_then_read(sw_spi_t *spi, const uint8_t *cmd, size_t cmd_len, uint32_t pause_us, uint8_t *out, size_t out_len) {
    // 1) Write command while CS low
    gpio_set_dir(spi->dio_pin, GPIO_OUT);
    gpio_put(spi->clk_pin, 0);
    int8_t idx = active_idx_or_default(spi);
    cs_activate_index(spi, idx);
    for (size_t i = 0; i < cmd_len; ++i) {
        uint8_t b = cmd[i];
        for (int bit = 7; bit >= 0; --bit) {
            int v = (b >> bit) & 1;
            gpio_put(spi->dio_pin, v);
            delay_us(spi->delay_us);
            gpio_put(spi->clk_pin, 1);
            delay_us(spi->delay_us);
            gpio_put(spi->clk_pin, 0);
            delay_us(1);
        }
    }
    cs_deactivate_all(spi);

    // 2) Pause with CS idle and switch DIO to input
    gpio_set_dir(spi->dio_pin, GPIO_IN);
    if (pause_us) delay_us(pause_us);

    // 3) Read phase: CS active again, DIO is input
    cs_activate_index(spi, idx);
    gpio_put(spi->clk_pin, 0);

    for (size_t i = 0; i < out_len; ++i) {
        uint8_t b = 0;
        for (int bit = 7; bit >= 0; --bit) {
            // generate clock: low -> high, read on high
            delay_us(spi->delay_us);
            gpio_put(spi->clk_pin, 1);
            delay_us(spi->delay_us);
            int v = gpio_get(spi->dio_pin) ? 1 : 0;
            b |= (v << bit);
            gpio_put(spi->clk_pin, 0);
            delay_us(1);
        }
        out[i] = b;
    }

    cs_deactivate_all(spi);
    // return DIO to output (safe default)
    gpio_set_dir(spi->dio_pin, GPIO_OUT);
    gpio_put(spi->dio_pin, 0);
}
