/*
*********************************************************************************************************
*
*                                             hal_cc2530/Main
*                                            INTRODUCTION DEMO
*
*
* Filename      : Host.c
* Version       : V1.01a
* Programmer(s) : MC
*********************************************************************************************************
*/
/* ------------------------------------------------------------------------------------------------------
 *                                            INCLUDE FILES
 * -------------------------------------------------------------------------------------------------------
 */
#include "includes.h"


/* ------------------------------------------------------------------------------------------------------
 *                                            Local Variable
 * -------------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------------
 *                                            Local Function
 * -------------------------------------------------------------------------------------------------------
 */
extern void EnterSleepModeDisableInterruptsOnWakeup(void);

/* ------------------------------------------------------------------------------------------------------
 *												main()
 *
 * Description : main function.
 *
 */
int main(void)
{
	/* Initialise clock, and put on INT.*/
	halBoardInit();
	
	timer1_init();			// Initialise timer 1(16bit).
	
	mac_init();				// Initialise radio.
	
	SLEEPCMD = (SLEEPCMD & ~0x02) | 0x02;
	EnterSleepModeDisableInterruptsOnWakeup();
	
	for(;;)
	{
		if(pconflag == 1)
		{
			pconflag = 0;
			halLedClear(1);
			EnterSleepModeDisableInterruptsOnWakeup();
//			PCON = 0x01;
			asm("NOP");
			asm("NOP");
			asm("NOP");
//			EA = 1;
		}
		mac_event_handle();
	}
}

