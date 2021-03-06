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
__xdata volatile unsigned long st_offset = 0;


/* ------------------------------------------------------------------------------------------------------
 *											Local Functions
 * ------------------------------------------------------------------------------------------------------
 */
void util_Timer1CallBack ( uint8 timerId, uint8 channel, uint8 channelMode );
void util_Timer3CallBack ( uint8 timerId, uint8 channel, uint8 channelMode );

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
	
	HalTimerStart(HAL_TIMER_1, 2500);								/* 6.4s of Timer_1 ISR on channel 0.*/
#if (0)
	HalTimerConfig(HAL_TIMER_1,
				   HAL_TIMER_MODE_CTC,
				   HAL_TIMER_CHANNEL_1, 
				   HAL_TIMER_CH_MODE_OUTPUT_COMPARE,
				   TRUE,
				   util_Timer1CallBack);
	HalTimerStart(HAL_TIMER_1, 3906*3);		// 1s run timer1 ISR on channel 1.
	
	HalTimerConfig(HAL_TIMER_3,
				   HAL_TIMER_MODE_CTC,
				   HAL_TIMER_CHANNEL_0, 
				   HAL_TIMER_CH_MODE_OUTPUT_COMPARE,
				   TRUE,
				   util_Timer3CallBack);
	HalTimerStart(HAL_TIMER_3, 250);		// 1s run timer1 ISR on channel 0.
#endif
}

/* ------------------------------------------------------------------------------------------------------
 *									util_Timer1CallBack()
 *
 * Description : timer1 initlisation.
 *
 * Argument(s) : none.
 *
 */
void util_Timer1CallBack ( uint8 timerId, uint8 channel, uint8 channelMode )
{
	(void)timerId;
	(void)channelMode;
	
	switch(channel)
	{
	case HAL_TIMER_CHANNEL_SINGLE:
		break;
	case HAL_TIMER_CHANNEL_0:
		sysflag = BROADCASR_FLAG_SYN;
		break;
	case HAL_TIMER_CHANNEL_1:
		break;
	case HAL_TIMER_CHANNEL_2:
		break;
	case HAL_TIMER_CHANNEL_3:
		break;
	case HAL_TIMER_CHANNEL_4:
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
void util_Timer3CallBack ( uint8 timerId, uint8 channel, uint8 channelMode )
{
	(void)timerId;
	(void)channelMode;
	
	switch(channel)
	{
	case HAL_TIMER_CHANNEL_SINGLE:
		break;
	case HAL_TIMER_CHANNEL_0:
		mac_host_bcn(st_offset);
		st_offset += 3277;
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


