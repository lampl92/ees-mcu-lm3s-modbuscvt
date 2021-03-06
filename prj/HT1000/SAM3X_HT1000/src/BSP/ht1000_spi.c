/**
 * \file
 *
 * \brief Serial Peripheral Interface (SPI) example for SAM.
 *
 * Copyright (c) 2011-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage SPI Example
 *
 * \par Purpose
 *
 * This example uses Serial Peripheral Interface (SPI) of one EK board in
 * slave mode to communicate with another EK board's SPI in master mode.
 *
 * \par Requirements
 *
 * This package can be used with two SAM evaluation kits boards.
 * Please connect the SPI pins from one board to another.
 * \copydoc spi_example_pin_defs
 *
 * \par Descriptions
 *
 * This example shows control of the SPI, and how to configure and
 * transfer data with SPI. By default, example runs in SPI slave mode,
 * waiting SPI slave & UART inputs.
 *
 * The code can be roughly broken down as follows:
 * <ul>
 * <li> 't' will start SPI transfer test.
 * <ol>
 * <li>Configure SPI as master, and set up SPI clock.
 * <li>Send 4-byte CMD_TEST to indicate the start of test.
 * <li>Send several 64-byte blocks, and after transmitting the next block, the
 * content of the last block is returned and checked.
 * <li>Send CMD_STATUS command and wait for the status reports from slave.
 * <li>Send CMD_END command to indicate the end of test.
 * </ol>
 * <li>Setup SPI clock for master.
 * </ul>
 *
 * \par Usage
 *
 * -# Compile the application.
 * -# Connect the UART port of the evaluation board to the computer and open
 * it in a terminal.
 *    - Settings: 115200 bauds, 8 bits, 1 stop bit, no parity, no flow control.
 * -# Download the program into the evaluation board and run it. Please refer to
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6224.pdf">
 *    SAM-BA User Guide</a>, the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *    GNU-Based Software Development</a> application note or the
 *    <a href="ftp://ftp.iar.se/WWWfiles/arm/Guides/EWARM_UserGuide.ENU.pdf">
 *    IAR EWARM User Guide</a>, depending on the solutions that users choose.
 * -# Upon startup, the application will output the following line on the
 *    terminal:
 *    \code
 *     -- Spi Example  --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *    \endcode
 * -# The following traces detail operations on the SPI example, displaying
 *    success or error messages depending on the results of the commands.
 *
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"
//#include "conf_spi.h"
#include "ht1000_spi.h"

// From module: FreeRTOS mini Real-Time Kernel
#include <FreeRTOS.h>
#include "task.h"
#include "status_codes.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond


#define SPI_MOSI_H()				gpio_set_pin_high(SPI0_MOSI_GPIO)

#define SPI_MOSI_L()				gpio_set_pin_low(SPI0_MOSI_GPIO)

#define SPI_SCLK_H()				gpio_set_pin_high(SPI0_SPCK_GPIO)

#define SPI_SCLK_L()				gpio_set_pin_low(SPI0_SPCK_GPIO)

#define SPI_MISO_READ()				gpio_pin_is_high(SPI0_MISO_GPIO)

/* Chip select. */
#define SPI_CHIP_SEL 0

/* Clock polarity. */
#define SPI_CLK_POLARITY 0

/* Clock phase. */
#define SPI_CLK_PHASE 0

/* Delay before SPCK. */
#define SPI_DLYBS 0x40

/* Delay between consecutive transfers. */
#define SPI_DLYBCT 0x10

/* SPI slave states for this example. */
#define SLAVE_STATE_IDLE           0
#define SLAVE_STATE_TEST           1
#define SLAVE_STATE_DATA           2
#define SLAVE_STATE_STATUS_ENTRY   3
#define SLAVE_STATE_STATUS         4
#define SLAVE_STATE_END            5

/* SPI example commands for this example. */
/* slave test state, begin to return RC_RDY. */
#define CMD_TEST     0x10101010

/* Slave data state, begin to return last data block. */
#define CMD_DATA     0x29380000

/* Slave status state, begin to return RC_RDY + RC_STATUS. */
#define CMD_STATUS   0x68390384

/* Slave idle state, begin to return RC_SYN. */
#define CMD_END      0x68390484

/* General return value. */
#define RC_SYN       0x55AA55AA

/* Ready status. */
#define RC_RDY       0x12345678

/* Slave data mask. */
#define CMD_DATA_MSK 0xFFFF0000

/* Slave data block mask. */
#define DATA_BLOCK_MSK 0x0000FFFF

/* Number of commands logged in status. */
#define NB_STATUS_CMD   20

/* Number of SPI clock configurations. */
#define NUM_SPCK_CONFIGURATIONS 4

/* SPI Communicate buffer size. */
#define COMM_BUFFER_SIZE   64

/* UART baudrate. */
#define UART_BAUDRATE      115200

/* Data block number. */
#define MAX_DATA_BLOCK_NUMBER  4

/* Max retry times. */
#define MAX_RETRY    4

/* Status block. */
struct status_block_t {
	/** Number of data blocks. */
	uint32_t ul_total_block_number;
	/** Number of SPI commands (including data blocks). */
	uint32_t ul_total_command_number;
	/** Command list. */
	uint32_t ul_cmd_list[NB_STATUS_CMD];
};

/* SPI clock setting (Hz). */
static uint32_t gs_ul_spi_clock = 10000;

/* Current SPI return code. */
static uint32_t gs_ul_spi_cmd = RC_SYN;

/* Current SPI state. */
static uint32_t gs_ul_spi_state = 0;

/* 64 bytes data buffer for SPI transfer and receive. */
static uint8_t gs_uc_spi_buffer[COMM_BUFFER_SIZE];

/* Pointer to transfer buffer. */
static uint8_t *gs_puc_transfer_buffer;

/* Transfer buffer index. */
static uint32_t gs_ul_transfer_index;

/* Transfer buffer length. */
static uint32_t gs_ul_transfer_length;

/* SPI Status. */
static struct status_block_t gs_spi_status;

static uint32_t gs_ul_test_block_number;

/* SPI clock configuration. */
static const uint32_t gs_ul_clock_configurations[] =
		{ 100000, 1000000, 2000000, 5000000 };


/**
 * \brief Set SPI slave transfer.
 *
 * \param p_buf Pointer to buffer to transfer.
 * \param size Size of the buffer.
 */
static void spi_slave_transfer(void *p_buf, uint32_t size)
{
	gs_puc_transfer_buffer = p_buf;
	gs_ul_transfer_length = size;
	gs_ul_transfer_index = 0;
	spi_write(SPI_SLAVE_BASE, gs_puc_transfer_buffer[gs_ul_transfer_index], 0,
			0);
}

/**
 * \brief  SPI command block process.
 */
static void spi_slave_command_process(void)
{
	if (gs_ul_spi_cmd == CMD_END) {
		gs_ul_spi_state = SLAVE_STATE_IDLE;
		gs_spi_status.ul_total_block_number = 0;
		gs_spi_status.ul_total_command_number = 0;
	} else {
		switch (gs_ul_spi_state) {
		case SLAVE_STATE_IDLE:
			/* Only CMD_TEST accepted. */
			if (gs_ul_spi_cmd == CMD_TEST) {
				gs_ul_spi_state = SLAVE_STATE_TEST;
			}
			break;

		case SLAVE_STATE_TEST:
			/* Only CMD_DATA accepted. */
			if ((gs_ul_spi_cmd & CMD_DATA_MSK) == CMD_DATA) {
				gs_ul_spi_state = SLAVE_STATE_DATA;
			}
			gs_ul_test_block_number = gs_ul_spi_cmd & DATA_BLOCK_MSK;
			break;

		case SLAVE_STATE_DATA:
			gs_spi_status.ul_total_block_number++;

			if (gs_spi_status.ul_total_block_number == 
					gs_ul_test_block_number) {
				gs_ul_spi_state = SLAVE_STATE_STATUS_ENTRY;
			}
			break;

		case SLAVE_STATE_STATUS_ENTRY:
			gs_ul_spi_state = SLAVE_STATE_STATUS;
			break;

		case SLAVE_STATE_END:
			break;
		}
	}
}

/**
 * \brief  Start waiting new command.
 */
static void spi_slave_new_command(void)
{
	switch (gs_ul_spi_state) {
	case SLAVE_STATE_IDLE:
	case SLAVE_STATE_END:
		gs_ul_spi_cmd = RC_SYN;
		spi_slave_transfer(&gs_ul_spi_cmd, sizeof(gs_ul_spi_cmd));
		break;

	case SLAVE_STATE_TEST:
		gs_ul_spi_cmd = RC_RDY;
		spi_slave_transfer(&gs_ul_spi_cmd, sizeof(gs_ul_spi_cmd));
		break;

	case SLAVE_STATE_DATA:
		if (gs_spi_status.ul_total_block_number < gs_ul_test_block_number) {
			spi_slave_transfer(gs_uc_spi_buffer, COMM_BUFFER_SIZE);
		}
		break;

	case SLAVE_STATE_STATUS_ENTRY:
		gs_ul_spi_cmd = RC_RDY;
		spi_slave_transfer(&gs_ul_spi_cmd, sizeof(gs_ul_spi_cmd));
		gs_ul_spi_state = SLAVE_STATE_STATUS;
		break;

	case SLAVE_STATE_STATUS:
		gs_ul_spi_cmd = RC_SYN;
		spi_slave_transfer(&gs_spi_status, sizeof(struct status_block_t));
		gs_ul_spi_state = SLAVE_STATE_END;
		break;
	}
}

/**
 * \brief Interrupt handler for the SPI slave.
 */
void SPI_Handler(void)
{
	uint32_t new_cmd = 0;
	static uint16_t data;
	uint8_t uc_pcs;

	if (spi_read_status(SPI_SLAVE_BASE) & SPI_SR_RDRF) {
		spi_read(SPI_SLAVE_BASE, &data, &uc_pcs);
		gs_puc_transfer_buffer[gs_ul_transfer_index] = data;
		gs_ul_transfer_index++;
		gs_ul_transfer_length--;
		if (gs_ul_transfer_length) {
			spi_write(SPI_SLAVE_BASE,
					gs_puc_transfer_buffer[gs_ul_transfer_index], 0, 0);
		}

		if (!gs_ul_transfer_length) {
			spi_slave_command_process();
			new_cmd = 1;
		}

		if (new_cmd) {
			if (gs_ul_spi_cmd != CMD_END) {
				gs_spi_status.ul_cmd_list[gs_spi_status.ul_total_command_number]
						= gs_ul_spi_cmd;
				gs_spi_status.ul_total_command_number++;
			}
			spi_slave_new_command();
		}
	}
}

/**
 * \brief Initialize SPI as slave.
 */
void spi_slave_initialize(void)
{
	uint32_t i;

	/* Reset status */
	gs_spi_status.ul_total_block_number = 0;
	gs_spi_status.ul_total_command_number = 0;
	for (i = 0; i < NB_STATUS_CMD; i++) {
		gs_spi_status.ul_cmd_list[i] = 0;
	}
	gs_ul_spi_state = SLAVE_STATE_IDLE;
	gs_ul_spi_cmd = RC_SYN;

	puts("-I- Initialize SPI as slave \r");
	/* Configure an SPI peripheral. */
	spi_enable_clock(SPI_SLAVE_BASE);
	spi_disable(SPI_SLAVE_BASE);
	spi_reset(SPI_SLAVE_BASE);
	spi_set_slave_mode(SPI_SLAVE_BASE);
	spi_disable_mode_fault_detect(SPI_SLAVE_BASE);
	spi_set_peripheral_chip_select_value(SPI_SLAVE_BASE, SPI_CHIP_SEL);
	spi_set_clock_polarity(SPI_SLAVE_BASE, SPI_CHIP_SEL, SPI_CLK_POLARITY);
	spi_set_clock_phase(SPI_SLAVE_BASE, SPI_CHIP_SEL, SPI_CLK_PHASE);
	spi_set_bits_per_transfer(SPI_SLAVE_BASE, SPI_CHIP_SEL, SPI_CSR_BITS_8_BIT);
	spi_enable_interrupt(SPI_SLAVE_BASE, SPI_IER_RDRF);
	spi_enable(SPI_SLAVE_BASE);

	/* Start waiting command. */
	spi_slave_transfer(&gs_ul_spi_cmd, sizeof(gs_ul_spi_cmd));
}

/**
 * \brief Initialize SPI as master.
 */
void spi_master_initialize(void)
{
	RS232printf("\n\r-I- Initialize SPI as master");

	/* Configure an SPI peripheral. */
	spi_enable_clock(SPI_MASTER_BASE);
	spi_disable(SPI_MASTER_BASE);
	spi_reset(SPI_MASTER_BASE);
	spi_set_lastxfer(SPI_MASTER_BASE);
	spi_set_master_mode(SPI_MASTER_BASE);
	spi_disable_mode_fault_detect(SPI_MASTER_BASE);
	spi_set_peripheral_chip_select_value(SPI_MASTER_BASE, SPI_CHIP_SEL);
	spi_set_clock_polarity(SPI_MASTER_BASE, SPI_CHIP_SEL, SPI_CLK_POLARITY);
	spi_set_clock_phase(SPI_MASTER_BASE, SPI_CHIP_SEL, SPI_CLK_PHASE);
	spi_set_bits_per_transfer(SPI_MASTER_BASE, SPI_CHIP_SEL,
			SPI_CSR_BITS_8_BIT);
	spi_set_baudrate_div(SPI_MASTER_BASE, SPI_CHIP_SEL,
			(sysclk_get_cpu_hz() / gs_ul_spi_clock));
	spi_set_transfer_delay(SPI_MASTER_BASE, SPI_CHIP_SEL, SPI_DLYBS,
			SPI_DLYBCT);
	spi_enable(SPI_MASTER_BASE);
	
	spi_csn0_disable();
}

/**
 * \brief Set the specified SPI clock configuration.
 *
 * \param configuration  Index of the configuration to set.
 */
void spi_set_clock_configuration(uint8_t configuration)
{
	gs_ul_spi_clock = gs_ul_clock_configurations[configuration];
	RS232printf("\n\rSetting SPI clock #%lu ... ", (unsigned long)gs_ul_spi_clock);
	spi_master_initialize();
}

/**
 * \brief Perform SPI master transfer.
 *
 * \param pbuf Pointer to buffer to transfer.
 * \param size Size of the buffer.
 */
void spi_master_transfer(void *p_buf, uint32_t size)
{
	uint32_t i;
	uint8_t uc_pcs;
	static uint16_t data;

	uint8_t *p_buffer;

	p_buffer = p_buf;

	for (i = 0; i < size; i++) {
		spi_write(SPI_MASTER_BASE, p_buffer[i], 0, 0);
		/* Wait transfer done. */
		while ((spi_read_status(SPI_MASTER_BASE) & SPI_SR_RDRF) == 0);
		spi_read(SPI_MASTER_BASE, &data, &uc_pcs);
		p_buffer[i] = data;
	}
}

/**
 * \brief 
 * 
 * \param p_buffer
 * 
 * \return void
 */
uint8_t spi_soft_write( uint8_t data ) 
{
	uint8_t i, temp = 0, b;
	
	for(i = 0;i < 8;i ++)
	{
		SPI_SCLK_H();
		
		if((data&0x80) == 0x80)
			SPI_MOSI_H();
		else
			SPI_MOSI_L();
		
		delay_us(500);
		
		data = data<<1;
		
		SPI_SCLK_L();
		delay_us(500);
		
		if(SPI_MISO_READ() == 1)
		{
			temp = temp | (1<<(7-i));
		}
		
		SPI_MOSI_L();
	}
	
	return temp;
}

/**
 * \brief Perform SPI soft transfer.
 * 
 * \param p_buf Pointer to buffer to transfer.
 * \param size Size of the buffer.
 * 
 * \return void
 */
void spi_soft_transfer(void *p_buf, uint32_t size)
{
	uint32_t i;
	uint8_t uc_pcs;
	static uint16_t data;

	uint8_t *p_buffer;

	p_buffer = p_buf;
	
	for (i = 0; i < size; i++) {
		p_buffer[i] = spi_soft_write( p_buffer[i] );
	}
}

/**
 * \brief Disable SPI active.
 */
void spi_csn0_disable(void)
{
	gpio_set_pin_high(SPI0_NPCS0_GPIO);
}

/**
 * \brief Enable SPI active.
 */
void spi_csn0_enable(void)
{
	gpio_set_pin_low(SPI0_NPCS0_GPIO);
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
