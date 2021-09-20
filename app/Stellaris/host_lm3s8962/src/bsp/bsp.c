/* ------------------------------------------------------------------------------------------------------
 *											Local Includes
 * ------------------------------------------------------------------------------------------------------
 */
#include "bsp.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"


/* ------------------------------------------------------------------------------------------------------
 *											Local Functions
 * ------------------------------------------------------------------------------------------------------
 */
void BSP_SysclkInit(void);
void BSP_UartInit(void);
void BSP_EthernetInit(void);


/* ------------------------------------------------------------------------------------------------------
 *									BSP_Init()
 *
 * Description : bsp init function.
 *
 * Argument(s) : none.
 *
 */
void BSP_Init(void)
{
	//
    // Enable the Sys clock.
    //
	BSP_SysclkInit();
	

	//
    // Enable the Uart.
    //
	BSP_UartInit();
	
	
	//
    // Enable the Ethernet.
    //
	BSP_EthernetInit();
}

/* ------------------------------------------------------------------------------------------------------
 *									BSP_SysclkInit()
 *
 * Description : Sys clock init function.
 *
 * Argument(s) : none.
 *
 */
void BSP_SysclkInit(void)
{
	/* If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
    a workaround to allow the PLL to operate reliably. */
    if( DEVICE_IS_REVA2 )
    {
		SysCtlLDOSet( SYSCTL_LDO_2_75V );
    }
	
	//
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
}

/* ------------------------------------------------------------------------------------------------------
 *									BSP_UartInit()
 *
 * Description : Uart init function.
 *
 * Argument(s) : none.
 *
 */
void BSP_UartInit(void)
{
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
    //
    // Set GPIO A0 and A1 as UART.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
    //
    // Initialize the UART as a console for text I/O.
    //
    UARTStdioInit(0);
	
	// test code.
	UARTprintf("BSP initialise\n");
}

/* ------------------------------------------------------------------------------------------------------
 *									BSP_UartInit()
 *
 * Description : Ethernet init function.
 *
 * Argument(s) : none.
 *
 */
void BSP_EthernetInit(void)
{
	//
	// Enable and Reset the Ethernet Controller.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
	SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

	//
	// Enable Port F for Ethernet LEDs.
	//  LED0        Bit 3   Output
	//  LED1        Bit 2   Output
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
}

