/**
 * @file pga11x_spi.h
 * @brief SPI register definitions for TI PGA112/PGA113/PGA116/PGA117
 *        Zerø-Drift Programmable Gain Amplifier with MUX
 *
 * Datasheet: SBOS424C – March 2008 – Revised November 2015
 *
 * SPI interface: Mode 0,0 or Mode 1,1 | Max clock: 10 MHz (9.09 MHz daisy-chain)
 * Word length: 16 bits, MSB first. Data latched on rising edge of /CS.
 * A transaction is only executed when an even multiple of 16 clocks occurred
 * between /CS falling and /CS rising.
 */

#ifndef PGA11X_SPI_H
#define PGA11X_SPI_H

#include <stdint.h>

/* =========================================================================
 * SPI TIMING PARAMETERS (nanoseconds, unless noted)
 * ========================================================================= */

#define PGA11X_SCLK_FREQ_MAX_HZ         10000000U   /**< Max SCLK frequency (standalone) */
#define PGA11X_T_CSH_MIN_NS                   40U   /**< /CS high time minimum */
#define PGA11X_T_CSSC_MIN_NS                  10U   /**< /CS fall to first SCLK edge */
#define PGA11X_T_SCCS_MIN_NS                  10U   /**< SCLK last edge to /CS rise */
#define PGA11X_T_SU_MIN_NS                    10U   /**< DIN setup time */
#define PGA11X_T_HD_MIN_NS                    10U   /**< DIN hold time */
#define PGA11X_T_SOZ_MAX_NS                   20U   /**< /CS rise to DOUT Hi-Z */

/* =========================================================================
 * THREE-WIRE SPI COMMAND WORDS (PGA112 / PGA113)
 * Table 3, Datasheet p. 29
 *
 * 16-bit words: D[15:0]
 * ========================================================================= */

/** READ command – triggers a read transaction (two 16-clock phases required).
 *  First phase: send READ command.
 *  Second phase: send NOP; DOUT returns the current register contents. */
#define PGA11X_CMD_READ         0x6A00U   /* 0110 1010 0000 0000 */

/** WRITE command – bits [7:0] carry G[3:0] and CH[3:0], see masks below. */
#define PGA11X_CMD_WRITE_BASE   0x2A00U   /* 0010 1010 GGGG CCCC */

/** NOP WRITE – no operation, register unchanged. */
#define PGA11X_CMD_NOP          0x0000U   /* 0000 0000 0000 0000 */

/** SDN_DIS – exit software shutdown, restore last valid configuration. */
#define PGA11X_CMD_SDN_DIS      0xE100U   /* 1110 0001 0000 0000 */

/** SDN_EN – enter software shutdown (IQ ≤ 4 µA typ). */
#define PGA11X_CMD_SDN_EN       0xE1F1U   /* 1110 0001 1111 0001 */



/* =========================================================================
 * BIT-FIELD MASKS AND SHIFTS (common to both command sets)
 * ========================================================================= */

/** Gain field: D[7:4] */
#define PGA11X_GAIN_MASK        0x00F0U
#define PGA11X_GAIN_SHIFT       4U

/** Channel field: D[3:0] */
#define PGA11X_CH_MASK          0x000FU
#define PGA11X_CH_SHIFT         0U

/* =========================================================================
 * GAIN SELECTION BITS  G[3:0]
 * Table 5, Datasheet p. 29
 * ========================================================================= */

typedef enum {
    PGA11X_GAIN_BINARY_1   = 0x0U,  /**< Binary gain ×1   / Scope gain ×1   */
    PGA11X_GAIN_BINARY_2   = 0x1U,  /**< Binary gain ×2   / Scope gain ×2   */
    PGA11X_GAIN_BINARY_4   = 0x2U,  /**< Binary gain ×4   / Scope gain ×5   */
    PGA11X_GAIN_BINARY_8   = 0x3U,  /**< Binary gain ×8   / Scope gain ×10  */
    PGA11X_GAIN_BINARY_16  = 0x4U,  /**< Binary gain ×16  / Scope gain ×20  */
    PGA11X_GAIN_BINARY_32  = 0x5U,  /**< Binary gain ×32  / Scope gain ×50  */
    PGA11X_GAIN_BINARY_64  = 0x6U,  /**< Binary gain ×64  / Scope gain ×100 */
    PGA11X_GAIN_BINARY_128 = 0x7U,  /**< Binary gain ×128 / Scope gain ×200 */
} pga11x_gain_t;

/* Aliases for scope-gain devices (PGA113, PGA117) */
#define PGA11X_GAIN_SCOPE_1    PGA11X_GAIN_BINARY_1
#define PGA11X_GAIN_SCOPE_2    PGA11X_GAIN_BINARY_2
#define PGA11X_GAIN_SCOPE_5    PGA11X_GAIN_BINARY_4
#define PGA11X_GAIN_SCOPE_10   PGA11X_GAIN_BINARY_8
#define PGA11X_GAIN_SCOPE_20   PGA11X_GAIN_BINARY_16
#define PGA11X_GAIN_SCOPE_50   PGA11X_GAIN_BINARY_32
#define PGA11X_GAIN_SCOPE_100  PGA11X_GAIN_BINARY_64
#define PGA11X_GAIN_SCOPE_200  PGA11X_GAIN_BINARY_128

/* =========================================================================
 * MUX CHANNEL SELECTION BITS  CH[3:0]
 * Table 6, Datasheet p. 30
 * ========================================================================= */

typedef enum {
    PGA11X_CH_VCAL_CH0 = 0x0U,  /**< CH0 / VCAL input (both device families)    */
    PGA11X_CH_1        = 0x1U,  /**< CH1  (both device families)                 */
    PGA11X_CH_CAL1     = 0xCU,  /**< Internal CAL1 → GND                         */
    PGA11X_CH_CAL2     = 0xDU,  /**< Internal CAL2 → 0.9 × VCAL                  */
    PGA11X_CH_CAL3     = 0xEU,  /**< Internal CAL3 → 0.1 × VCAL                  */
    PGA11X_CH_CAL4     = 0xFU,  /**< Internal CAL4 → VREF                        */
} pga11x_ch_t;

/* =========================================================================
 * REGISTER STRUCTURE
 * Represents the 8-bit payload carried in D[7:0] of every WRITE command.
 * ========================================================================= */

/** Decoded view of the gain/channel register (D[7:0]). */
typedef struct {
    pga11x_gain_t gain;     /**< G[3:0] – gain selection                   */
    pga11x_ch_t   channel;  /**< CH[3:0] – MUX channel selection            */
} pga11x_reg_t;

/* =========================================================================
 * HELPER MACROS – assemble 16-bit SPI words
 * ========================================================================= */

/**
 * @brief Build a three-wire WRITE word for PGA112/PGA113.
 * @param gain    pga11x_gain_t value
 * @param channel pga11x_ch_t value
 */
#define PGA11X_WRITE_WORD(gain, channel) \
    ((uint16_t)(PGA11X_CMD_WRITE_BASE                        \
        | (((uint16_t)(gain)    << PGA11X_GAIN_SHIFT) & PGA11X_GAIN_MASK) \
        | (((uint16_t)(channel) << PGA11X_CH_SHIFT)   & PGA11X_CH_MASK)))


/**
 * @brief Extract gain bits from a raw 16-bit DOUT word (read-back).
 * @param raw  16-bit word received from DOUT
 */
#define PGA11X_PARSE_GAIN(raw)    ((pga11x_gain_t)(((raw) & PGA11X_GAIN_MASK) >> PGA11X_GAIN_SHIFT))

/**
 * @brief Extract channel bits from a raw 16-bit DOUT word (read-back).
 * @param raw  16-bit word received from DOUT
 */
#define PGA11X_PARSE_CH(raw)      ((pga11x_ch_t)(((raw) & PGA11X_CH_MASK) >> PGA11X_CH_SHIFT))

/* =========================================================================
 * POWER-ON RESET STATE
 * After POR the internal register is all-zero: Gain = 1, Channel = VCAL/CH0
 * ========================================================================= */

#define PGA11X_POR_GAIN     PGA11X_GAIN_BINARY_1
#define PGA11X_POR_CHANNEL  PGA11X_CH_VCAL_CH0

/* =========================================================================
 * USAGE EXAMPLES (informational, compiled away)
 * =========================================================================
 *
 *  --- Write gain ×8, channel 1 (PGA112/PGA113, three-wire) ---
 *
 *      uint16_t tx = PGA11X_WRITE_WORD(PGA11X_GAIN_BINARY_8, PGA11X_CH_1);
 *      spi_transfer_16(tx);   // assert /CS, send tx[15:0], deassert /CS
 *
 *  --- Write scope gain ×50, channel CAL2 (PGA113) ---
 *
 *      uint16_t tx = PGA11X_WRITE_WORD(PGA11X_GAIN_SCOPE_50, PGA11X_CH_CAL2);
 *      spi_transfer_16(tx);
 *
 *  --- Read back the register (PGA112/PGA113) ---
 *
 *      spi_transfer_16(PGA11X_CMD_READ);   // phase 1
 *      uint16_t rx = spi_transfer_16(PGA11X_CMD_NOP);  // phase 2, read DOUT
 *      pga11x_gain_t g  = PGA11X_PARSE_GAIN(rx);
 *      pga11x_ch_t   ch = PGA11X_PARSE_CH(rx);
 *
 *  --- Software shutdown / wake-up ---
 *
 *      spi_transfer_16(PGA11X_CMD_SDN_EN);   // enter shutdown
 *      spi_transfer_16(PGA11X_CMD_SDN_DIS);  // exit shutdown
 *
 *  --- Daisy-chain write to two PGA116 devices (U1 first, U2 last) ---
 *
 *      uint16_t tx_u1 = PGA11X_DC_WRITE_WORD(PGA11X_GAIN_BINARY_16, PGA11X_CH_3);
 *      uint16_t tx_u2 = PGA11X_DC_WRITE_WORD(PGA11X_GAIN_BINARY_4,  PGA11X_CH_1);
 *      cs_low();
 *      spi_send_16(tx_u2);  // U2 is physically last → shifted in first
 *      spi_send_16(tx_u1);  // U1 is physically first
 *      cs_high();           // data latched on rising edge
 *
 * ========================================================================= */

#endif /* PGA11X_SPI_H */
