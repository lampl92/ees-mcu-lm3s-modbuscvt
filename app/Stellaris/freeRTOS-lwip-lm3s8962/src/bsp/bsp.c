/* ------------------------------------------------------------------------------------------------------
 *											Local Includes
 * ------------------------------------------------------------------------------------------------------
 */
#include "includes.h"

/* ------------------------------------------------------------------------------------------------------
 *									BSP_Init()
 *
 * Description : MCU sysctl init function.
 *
 * Argument(s) : none.
 *
 */
void BSP_Init(void)
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
	
#if 1
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
#else
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	
    //
    // Set GPIO A0 and A1 as UART.
    //
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	
    //
    // Initialize the UART as a console for text I/O.
    //
    UARTStdioInit(1);
#endif
		UARTprintf("BSP initialise\n");
		
		user_epprom_init();
		
		// init watchdog
		SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
		WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet() * WATCHDOG_INTERVAL);
    WatchdogResetEnable(WATCHDOG0_BASE);
		WatchdogEnable(WATCHDOG0_BASE);
}

