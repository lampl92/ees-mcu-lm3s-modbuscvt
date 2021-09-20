/*
*********************************************************************************************************
*
*                                          lm3s8963-lwIP/Main
*                                           INTRODUCTION DEMO
*
*
* Filename      : main.c
* Version       : V0.01
* Programmer(s) : MC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/
#include "includes.h"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

/*
*********************************************************************************************************
*                                         LOCAL FUNCIONS
*********************************************************************************************************
*/
void BSP_Init(void);
static void prvStartTask(void *pvParameters);


/********************************************************************************************************
*                                             main()
*
* Description : main function.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*/
int main(void)
{
	BSP_Init();
	
	xTaskCreate( prvStartTask, ( signed char * ) "prvStartTask",
				configMINIMAL_STACK_SIZE, NULL, TASK_START_TASK_PRIORITY, NULL );

	/* Start the tasks running. */
	vTaskStartScheduler();
	
	return (0);
}

/********************************************************************************************************
*                                             prvStartTask()
*
* Description : main function.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*/
static void prvStartTask(void *pvParameters)
{
	(void) pvParameters;
	
	NetServerInit();
	
	for (;;)
	{
		vTaskSuspend( NULL );			//挂起这个任务
	}
}

/********************************************************************************************************
*                                        vApplicationStackOverflowHook()
*
* Description : freeRTOS hook function.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : 堆栈检查钩子函数.
*/
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	( void ) pxTask;
	( void ) pcTaskName;

    
    UARTprintf("Stack Overflow : ");
    UARTprintf((const char*)pcTaskName);
    
	for( ;; );
}

void deviceDebugHander() {
	while(1) {
	}
}
