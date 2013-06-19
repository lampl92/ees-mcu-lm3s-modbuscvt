/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include "asf.h"
#include <stdlib.h>
#include <string.h>


// From module: FreeRTOS mini Real-Time Kernel
#include <FreeRTOS.h>
#include "task.h"
#include "status_codes.h"

#include "ethernet.h"
#include "netif/etharp.h"
#include <compiler.h>
#include "ht1000_spi.h"
#include "net_handle.h"
#include "spi_handle.h"

xTaskHandle vStartTaskHandler = (xTaskHandle)NULL;

extern void vApplicationMallocFailedHook( void );
extern void vApplicationIdleHook( void );
extern void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName );
extern void vApplicationTickHook( void );
extern void xPortSysTickHandler(void);

void task_start(void *pvParameters);
void task_led(void *pvParameters);

/**
 * \brief Handler for System Tick interrupt.
 */
void SysTick_Handler(void)
{
	xPortSysTickHandler();
}

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
//	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}

/**
 * \brief Configure the console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	
	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
#if defined(__GNUC__)
	setbuf(stdout, NULL);
#else
	/* Already the case in IAR's Normal DLIB default configuration: printf()
	 * emits one character at a time.
	 */
#endif
}

/*-----------------------------------------------------------*/
void task_led(void *pvParameters)
{
	
	//uint16_t crc;
	//(void) pvParameters;
	//
	//spi_t.buf[0] = 0x01;
	//spi_t.buf[1] = 0x06;
	//spi_t.buf[2] = 0x00;
	//spi_t.buf[3] = 0x01;
	//spi_t.buf[4] = 0x01;
	//spi_t.buf[5] = 0x00;
	//
	//crc = Crc16CheckSum(spi_t.buf, 6);
	//
	//spi_t.buf[6] = crc>>8;
	//spi_t.buf[7] = crc;
	//
	//spi_t.len    = 8;
	
	for (;;)
	{
		//spi_csn0_disable();
		//spi_master_transfer(spi_t.buf, spi_t.len);
		//vTaskDelay(20);												/* Wait 20 millisecond.*/
		//spi_master_transfer(spi_t.buf, spi_t.len);					/* Update to spi.buf[].*/
		//spi_csn0_enable();
		//
		gpio_toggle_pin(LED0_GPIO);
		vTaskDelay(1000);
	}
}
/*-----------------------------------------------------------*/

void task_start(void *pvParameters)
{
	(void) pvParameters;
	
	/* Start the LED flash tasks */
	xTaskCreate(task_led, (signed char*)"LED", TASK_LED_STACK_SIZE, NULL, 
				TASK_LED_PRIORITY, ( xTaskHandle * ) NULL);
	
	vStartEthernetTaskLauncher( TASK_START_ETH_PRIORITY );			/* Start the ethernet tasks. */
	
	vStartSpiTaskLauncher( TASK_SPI_HANDLE_PRIORITY );				/* Start the SPI app tasks. */
	
	for (;;)
	{
		vTaskSuspend(vStartTaskHandler);
	}
}
/*-----------------------------------------------------------*/

int main (void)
{
	/* Initialize the SAM system */
	sysclk_init();
	
	/* Initialize mcu's peripheral.*/
	board_init();
	
	/* Initialize the console uart */
	configure_console();
	
	/* Output demo information. */
	RS232printf("\n\r-- FreeRTOS Example --\n\r");
	
	/* Initialize the SPI0. */
	spi_set_clock_configuration(0);
	
	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping( 0 );
	
	/* Create freeRTOS START task.*/
	xTaskCreate(task_start, (signed char *)"START", TASK_START_STACKSIZE, NULL,
				TASK_START_PRIORITY, NULL);
	
	/* Start the scheduler. */
	vTaskStartScheduler();
	
	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}