/* ------------------------------------------------------------------------------------------------------
* File: util_timer.c
*
* -------------------------------------------------------------------------------------------------------
*/
#include "includes.h"


/* ------------------------------------------------------------------------------------------------------
 *											Local Variable
 * ------------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------------
 *											Local Functions
 * ------------------------------------------------------------------------------------------------------
 */
static void util_Timer1CallBack ( uint8 timerId, uint8 channel, uint8 channelMode );
static void util_Timer3CallBack ( uint8 timerId, uint8 channel, uint8 channelMode );


/* ------------------------------------------------------------------------------------------------------
 *									timer1_init()
 *
 * Description : timer1 initlisation.
 *
 * Argument(s) : none.
 *
 */
void timer_init(void)
{
	/* Initalise Timer1(16bit),Timer3(8bit),Timer4(8bit).*/
	HalTimerInit();
	

	HalTimerConfig(HAL_TIMER_1,
				   HAL_TIMER_MODE_CTC,
				   HAL_TIMER_CHANNEL_0, 
				   HAL_TIMER_CH_MODE_OUTPUT_COMPARE,
				   TRUE,
				   util_Timer1CallBack);
	HalTimerStart(HAL_TIMER_1, 3906*3);		// 3s run timer1 ISR on channel 0.
	
	/* Set timer1 channel 1.*/
	/*
	HalTimerConfig(HAL_TIMER_1,
				   HAL_TIMER_MODE_CTC,
				   HAL_TIMER_CHANNEL_1, 
				   HAL_TIMER_CH_MODE_OUTPUT_COMPARE,
				   TRUE,
				   util_Timer1CallBack);
	*/
//	HalTimerStart(HAL_TIMER_1, 3906*3);		// 1s run timer1 ISR on channel 1.
	

	/* Initialise timer 3, not using ISR.*/
	HalTimerConfig(HAL_TIMER_3,
				   HAL_TIMER_MODE_NORMAL,
				   HAL_TIMER_CHANNEL_SINGLE, 
				   HAL_TIMER_CH_MODE_OVERFLOW,
				   true,
				   util_Timer3CallBack);
//	HalTimerStart(HAL_TIMER_3, 250);		// 1s run timer3 ISR on channel 1.
	
}

/* ------------------------------------------------------------------------------------------------------
 *									util_Timer1CallBack()
 *
 * Description : timer1 initlisation.
 *
 * Argument(s) : none.
 *
 */
static void util_Timer1CallBack ( uint8 timerId, uint8 channel, uint8 channelMode )
{
	(void)timerId;
	(void)channelMode;
	
	switch(channel)
	{
	case HAL_TIMER_CHANNEL_SINGLE:
		
		break;
	case HAL_TIMER_CHANNEL_0:										/* Every 3s HOST send beacon frame.*/
//		iCnt_last = (iCnt/3)*3.57;
//		iCnt = 0;
		break;
	case HAL_TIMER_CHANNEL_1:
		break;
	case HAL_TIMER_CHANNEL_2:
		break;
	case HAL_TIMER_CHANNEL_3:
		break;
	case HAL_TIMER_CHANNEL_4:
		halRfReceiveOff();											/* Turn off FSM modul.*/
		HalTimerStop(HAL_TIMER_1);
		HalTimerStop(HAL_TIMER_3);
		halLedClear(2);
		if (!(sysflag & SYS_FLAG_SLEEP_SET))
		{
			halRfReceiveOn();
			break;
		}
		sysflag |= SYS_FLAG_SLEEP_START;
		break;
	default:
		break;
	}
}

/* ------------------------------------------------------------------------------------------------------
 *									util_Timer3CallBack()
 *
 * Description : timer3 initlisation.
 *
 * Argument(s) : none.
 *
 */
static void util_Timer3CallBack ( uint8 timerId, uint8 channel, uint8 channelMode )
{
	(void)timerId;
	(void)channelMode;
	
	switch(channel)
	{
	/* Delay 50ms for waiting RX data, than pown down in mode 2.*/
	case HAL_TIMER_CHANNEL_SINGLE:
		set_sleeptimer(TICK_VAL);									/* Reset sleep timer.*/
		sysflag = SYS_FLAG_SLEEP_START;
		break;
		
	case HAL_TIMER_CHANNEL_0:
		halLedToggle(2);
		break;
		
	case HAL_TIMER_CHANNEL_1:
		break;
		
	default:
		break;
	}
}

/* ------------------------------------------------------------------------------------------------------
 *									set_timer1isr()
 *
 * Description : timer1 initlisation.
 *
 * Argument(s) : none.
 *
 */
void set_timer1isr(U16 sec)
{
	HalTimerConfig(HAL_TIMER_1,
			   HAL_TIMER_MODE_CTC,
			   HAL_TIMER_CHANNEL_0, 
			   HAL_TIMER_CH_MODE_OUTPUT_COMPARE,
			   TRUE,
			   util_Timer1CallBack);
	HalTimerStart(HAL_TIMER_1, 3906*sec);		// 3s run timer1 ISR on channel 0.
}
