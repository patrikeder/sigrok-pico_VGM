# PGA112 Programmable Gain Amplifier Control

Control library for the TI PGA112 programmable gain amplifier using the SW_SPI software SPI interface.

## Features

- **8 Binary Gain Settings**: 1, 2, 4, 8, 16, 32, 64, 128
- **Channel Selection**: CH0 (Vcal for calibration) and CH1 (actual input)
- **Multiple Device Support**: Control up to 3 PGA112 devices via different CS lines
- **Numeric Gain Control**: Set gains by numeric value or enumeration
- **dB Conversion**: Get gain values in decibels
- **Status Reporting**: Easy monitoring of current settings

## Hardware Configuration

The PGA112 uses a 16-bit serial interface:
- **CS (Chip Select)**: Digital output to select the device
- **SCL (Serial Clock)**: Digital output from SPI
- **SDI (Serial Data Input)**: Digital data line from SPI

Connected to your RP2040 via SW_SPI with:
- CLK_PIN = 16
- DIO_PIN = 22
- CS_PINS = [21, 19, 20] (up to 3 devices)

## SPI Protocol

The PGA112 uses 16-bit SPI commands with MSB first:
- Command: 0x2A00 (WRITE) + gain[7:4] + channel[3:0]
- Gain: 0-7 (binary gains 1, 2, 4, 8, 16, 32, 64, 128)
- Channel: 0x0 (CH0/Vcal), 0x1 (CH1/Input)

## API Reference

### Initialization

```c
int8_t pga112_init(pga112_t *pga, sw_spi_t *spi, uint8_t cs_index);
```
Initialize a PGA112 instance with a specific chip select index.
- **Returns**: 0 on success, -1 on error

### Gain and Channel Control

```c
int8_t pga112_set_gain_and_channel(pga112_t *pga, pga112_gain_t gain, pga112_ch_t channel);
int8_t pga112_set_gain_value_and_channel(pga112_t *pga, uint16_t gain_value, pga112_ch_t channel);
int8_t pga112_enable(pga112_t *pga, pga112_gain_t initial_gain, pga112_ch_t initial_channel);
```
Set the amplifier gain and channel. Use either:
- `pga112_set_gain_and_channel()` with enumerated values
- `pga112_set_gain_value_and_channel()` with numeric gain values

**Gain Values**: PGA11X_GAIN_BINARY_1 through PGA11X_GAIN_BINARY_128
**Channel Values**: PGA11X_CH_VCAL_CH0 (calibration), PGA11X_CH_1 (input)

### Status and Conversion

```c
pga112_gain_t pga112_get_gain(pga112_t *pga);
pga112_ch_t pga112_get_channel(pga112_t *pga);
uint16_t pga112_gain_to_value(pga112_gain_t gain);
float pga112_gain_to_db(pga112_gain_t gain);
void pga112_print_status(pga112_t *pga);
```

## Usage Examples

### Single Device
```c
sw_spi_t spi;
sw_spi_init(&spi, cs_pins, 3, CLK_PIN, DIO_PIN, DELAY_US);

pga112_t pga;
pga112_init(&pga, &spi, 0);               // Use CS index 0
pga112_set_gain_and_channel(&pga, PGA11X_GAIN_BINARY_16, PGA11X_CH_1);    // Set gain to 16 on CH1
pga112_print_status(&pga);
```

### Multiple Devices
```c
pga112_t pga0, pga1, pga2;
pga112_init(&pga0, &spi, 0);
pga112_init(&pga1, &spi, 1);
pga112_init(&pga2, &spi, 2);

pga112_set_gain_and_channel(&pga0, PGA11X_GAIN_BINARY_1, PGA11X_CH_1);    // Gain = 1
pga112_set_gain_and_channel(&pga1, PGA11X_GAIN_BINARY_16, PGA11X_CH_1);   // Gain = 16
pga112_set_gain_and_channel(&pga2, PGA11X_GAIN_BINARY_128, PGA11X_CH_1);  // Gain = 128
```

### Channel Selection
```c
// Switch to calibration channel
pga112_set_gain_and_channel(&pga, PGA11X_GAIN_BINARY_1, PGA11X_CH_VCAL_CH0);

// Switch to input channel
pga112_set_gain_and_channel(&pga, PGA11X_GAIN_BINARY_16, PGA11X_CH_1);
```

### Numeric Gain
```c
pga112_set_gain_value_and_channel(&pga, 16, PGA11X_CH_1);   // Set gain = 16 on CH1
pga112_set_gain_value_and_channel(&pga, 64, PGA11X_CH_1);   // Set gain = 64 on CH1
```

## Gain Reference Table

| Setting | Gain | dB (approx) |
|---------|------|----------|
| 0       | 1    | 0.00    |
| 1       | 2    | 6.02    |
| 2       | 4    | 12.04   |
| 3       | 8    | 18.06   |
| 4       | 16   | 24.08   |
| 5       | 32   | 30.10   |
| 6       | 64   | 36.12   |
| 7       | 128  | 42.14   |

## Applications

The PGA112 is ideal for:
- **General purpose amplification** - 8 binary gain levels for flexible signal boost
- **Multi-level signal conditioning** - Programmable gain for different input ranges
- **Sensor signal amplification** - Boost weak signals to ADC input range
- **Data acquisition systems** - Programmable per-channel amplification
- **Instrumentation** - Binary gain steps for precise signal scaling

## Integration with CMakeLists.txt

Add the following to your CMakeLists.txt:

```cmake
add_executable(SPI_PGA112
    SPI_PGA112.c
    SW_SPI/sw_spi.c
    PGA113/pga112.c
)

target_include_directories(SPI_PGA112 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/PGA113
    ${CMAKE_CURRENT_SOURCE_DIR}/SW_SPI
)
```

## Files

- `pga112.h` - Header file with API definitions
- `pga112.c` - Implementation of PGA112 control functions
- `pga112_examples.h` - Usage examples and demonstrations
- `README.md` - This documentation

## Protocol Details

The PGA112 serial interface operates as follows:
1. Pull CS low to select the device
2. Send 8-bit control byte via SPI (MSB first)
   - Bits [2:0] = Gain setting (0-7)
   - Bits [7:3] = Reserved (set to 0)
3. Pull CS high to latch the gain setting

The SW_SPI library handles all timing and bit-banging automatically.

## Notes

- Gain changes take effect immediately upon CS rising edge
- Multiple devices can share common CLK and DIO connections
- The control byte is automatically formatted by the library
- Current gain setting is cached in the `pga112_t` structure for fast access
- Typical settling time: < 1 µs
- Gain accuracy: ±1% typical

## Differences from PGA113

The PGA112 uses the same binary gain structure as PGA113:

| Feature | Both Devices |
|---------|---|
| Gain Steps | Binary (1, 2, 4, 8, 16, 32, 64, 128) |
| Control Interface | SPI |
| Number of Gains | 8 selectable levels |
| Settling Time | < 1 µs |
| Gain Accuracy | ±1% typical |
