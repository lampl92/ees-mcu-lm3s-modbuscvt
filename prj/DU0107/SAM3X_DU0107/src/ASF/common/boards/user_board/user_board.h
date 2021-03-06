/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

/*----------------------------------------------------------------------------*/
/**
 *  \page sam3u_ek_opfreq SAM3U-EK - Operating frequencies
 *  This page lists several definition related to the board operating frequency
 *
 *  \section Definitions
 *  - \ref BOARD_FREQ_*
 *  - \ref BOARD_MCK
 */

/** Board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL		(32768U)
#define BOARD_FREQ_SLCK_BYPASS		(32768U)
#define BOARD_FREQ_MAINCK_XTAL		(12000000U)
#define BOARD_FREQ_MAINCK_BYPASS	(12000000U)

/** Master clock frequency */
#define BOARD_MCK					CHIP_FREQ_CPU_MAX

/** board main clock xtal statup time */
#define BOARD_OSC_STARTUP_US   15625


/* ------------------------------------------------------------------------ */
/* LED0 LED1                                                                */
/* ------------------------------------------------------------------------ */
//! LED #0 pin definition (BLUE).
#define LED_0_NAME    "green LED D2"
#define LED0_GPIO     (PIO_PA2_IDX)
#define LED0_FLAGS    (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define LED0_ACTIVE_LEVEL 0
//! LED #1 pin definition (BLUE).
#define LED_1_NAME    "green LED D1"
#define LED1_GPIO     (PIO_PA3_IDX)
#define LED1_FLAGS    (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define LED1_ACTIVE_LEVEL 0


/* ------------------------------------------------------------------------ */
/* Reset Key                                                                */
/* ------------------------------------------------------------------------ */
#define RESTKEY_GPIO     (PIO_PA20_IDX)

/* ------------------------------------------------------------------------ */
/* UART                                                                     */
/* ------------------------------------------------------------------------ */
//! UART pins (UTXD0 and URXD0) definitions, PA8,9.
#define PINS_UART		(PIO_PA8A_URXD | PIO_PA9A_UTXD)
#define PINS_UART_FLAGS	(PIO_PERIPH_A | PIO_DEFAULT)

#define PINS_UART_MASK	(PIO_PA8A_URXD | PIO_PA9A_UTXD)
#define PINS_UART_PIO	PIOA
#define PINS_UART_ID	ID_PIOA
#define PINS_UART_TYPE	PIO_PERIPH_A
#define PINS_UART_ATTR	PIO_DEFAULT

/* ------------------------------------------------------------------------ */
/* UART                                                                     */
/* ------------------------------------------------------------------------ */
//! UART pins (UTXD0 and URXD0) definitions, PA8,9.
#define SMS_RESET_GPIO	(PIO_PA19_IDX)
#define SMS_RESET_FLAGS	(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)

#define SMS_CMD_GPIO	(PIO_PA16_IDX)
#define SMS_CMD_FLAGS	(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)

/**
 * \file
 * USART0
 * - \ref PIN_USART0_RXD
 * - \ref PIN_USART0_TXD
 * - \ref PIN_USART0_CTS
 * - \ref PIN_USART0_RTS
 * - \ref PIN_USART0_SCK
 *
 * - \ref PIN_USART0_EN
 */

/* ------------------------------------------------------------------------ */
/* USART0                                                                   */
/* ------------------------------------------------------------------------ */
/*! USART0 pin RX */
#define PIN_USART0_RXD\
	{PIO_PA10A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_RXD_IDX        (PIO_PA10_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/*! USART0 pin TX */
#define PIN_USART0_TXD\
	{PIO_PA11A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_TXD_IDX        (PIO_PA11_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)

/* ------------------------------------------------------------------------ */
/* USART1                                                                   */
/* ------------------------------------------------------------------------ */
/*! USART1 pin RX */
#define PIN_USART1_RXD\
	{PIO_PA12A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RXD_IDX        (PIO_PA12_IDX)
#define PIN_USART1_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
/*! USART1 pin TX */
#define PIN_USART1_TXD\
	{PIO_PA13A_TXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_TXD_IDX        (PIO_PA13_IDX)
#define PIN_USART1_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)


/**
 * \file
 * EMAC
 *
 * - BOARD_EMAC_PHY_ADDR: Phy MAC address
 * - BOARD_EMAC_MODE_RMII: Enable RMII connection with the PHY
 */
#define PIN_EEMAC_EREFCK      PIO_PB0_IDX
#define PIN_EMAC_ETXEN        PIO_PB1_IDX
#define PIN_EMAC_ETX0         PIO_PB2_IDX
#define PIN_EMAC_ETX1         PIO_PB3_IDX
#define PIN_EMAC_ECRSDV       PIO_PB4_IDX
#define PIN_EMAC_ERX0         PIO_PB5_IDX
#define PIN_EMAC_ERX1         PIO_PB6_IDX
#define PIN_EMAC_ERXER        PIO_PB7_IDX
#define PIN_EMAC_EMDC         PIO_PB8_IDX
#define PIN_EMAC_EMDIO        PIO_PB9_IDX
#define PIN_EMAC_FLAGS        PIO_PERIPH_A | PIO_DEFAULT

/* ------------------------------------------------------------------------ */
/* SPI                                                                      */
/* ------------------------------------------------------------------------ */
/*! SPI0 MISO pin definition. */
#define SPI0_MISO_GPIO        (PIO_PA25_IDX)
#define SPI0_MISO_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI0_MISO_PORT		  (IOPORT_PIOA)
#define SPI0_MISO_MASK		  (PIO_PA25)

/*! SPI0 MOSI pin definition. */
#define SPI0_MOSI_GPIO        (PIO_PA26_IDX)
#define SPI0_MOSI_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)

/*! SPI0 SPCK pin definition. */
#define SPI0_SPCK_GPIO        (PIO_PA27_IDX)
#define SPI0_SPCK_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)

/*! SPI0 chip select 0 pin definition. (Only one configuration is possible) */
#define SPI0_NPCS0_GPIO            (PIO_PA28_IDX)
#define SPI0_NPCS0_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)
/*! SPI0 chip select 1 pin definition. (multiple configurations are possible) */
#define SPI0_NPCS1_PA29_GPIO       (PIO_PA29_IDX)
#define SPI0_NPCS1_PA29_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI0_NPCS1_PB20_GPIO       (PIO_PB20_IDX)
#define SPI0_NPCS1_PB20_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)
/*! SPI0 chip select 2 pin definition. (multiple configurations are possible) */
#define SPI0_NPCS2_PA30_GPIO       (PIO_PA30_IDX)
#define SPI0_NPCS2_PA30_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI0_NPCS2_PB21_GPIO       (PIO_PB21_IDX)
#define SPI0_NPCS2_PB21_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)
/*! SPI0 chip select 3 pin definition. (multiple configurations are possible) */
#define SPI0_NPCS3_PA31_GPIO       (PIO_PA31_IDX)
#define SPI0_NPCS3_PA31_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI0_NPCS3_PB23_GPIO       (PIO_PB23_IDX)
#define SPI0_NPCS3_PB23_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)

/** CC1101 GPIO pin definition. */
#define CC1101_GPIO0_PIO          PIOA
#define CC1101_INT_ID           ID_PIOA
#define CC1101_GPIO0			(1 << 23)
#define CC1101_INT_ATTR         (PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE)
#define CC1101_GPIO0_GPIO		(PIO_PA23_IDX)
#define CC1101_GPIO0_FLAGS      (PIO_INPUT | PIO_PULLUP)

#endif // USER_BOARD_H
