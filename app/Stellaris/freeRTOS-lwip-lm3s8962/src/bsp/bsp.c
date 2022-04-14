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
unsigned long clk;
void Timer3_Init(void);
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
	
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
		//SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_INT);
	  clk = SysCtlClockGet();
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
#if 1
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
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
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	
    //
    // Set GPIO A0 and A1 as UART.
    //
    //GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	
    //
    // Initialize the UART as a console for text I/O.
    //
    //UARTStdioInit(1);
#endif
		// cfg gpio input
		
		// RESET SW
  	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_3);

		//cfg gpio output
		
		// LED SYS RUNNING
		GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);
		GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
				
		// OUTPUT MODBUS DIRECTION
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0);		
		
		// OUTPUT MODBUS LED
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
		GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
		
		// OUTPUT LED 1
		//GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
		
		// OUTPUT LED 0
		//GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
		
		//UARTprintf("\r\n\r\nBSP initialise\r\n");
		
		user_epprom_init();
		
		// init watchdog
		SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
		WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet() * WATCHDOG_INTERVAL);
    WatchdogResetEnable(WATCHDOG0_BASE);
		WatchdogEnable(WATCHDOG0_BASE);
		LED_SYS_ON;
		MODBUS_DIR_RECV;
		LED_MODBUS_OFF;
}