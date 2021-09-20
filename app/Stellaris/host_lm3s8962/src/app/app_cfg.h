/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                       APPLICATION CONFIGURATION
*
*                                          Atmel AT91SAM3U4
*                                                on the
*                                 Atmel AT91SAM3U-EK Development Board.
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE
#define  APP_CFG_MODULE


/*
*********************************************************************************************************
*                                       ADDITIONAL uC/MODULE ENABLES
*
* Note(s)  (1) The following #define's enables or disables the uC/modules

*              (a) APP_CFG_PROBE_OS_PLUGIN_EN   Enables the uC/OS-II plugin in uC/Probe
*
*              (b) APP_CFG_PROBE_COM_EN         Enables communication in uC/Probe 
*                                               Look at the probe_com_cfg.h file to see all the available 
*                                               comunication methods.
* 
*              (c) APP_CFG_PROBE_DEMO_INTRO_EN  Enables The introductory demo for uC/Probe
*********************************************************************************************************
*/


#define  APP_CFG_PROBE_COM_EN                    DEF_ENABLED     /* DEF_ENABLED = Present, DEF_DISABLED = Not Present    */
#define  APP_CFG_PROBE_OS_PLUGIN_EN              DEF_ENABLED
#define  APP_CFG_PROBE_DEMO_INTRO_EN             DEF_ENABLED


/*
*********************************************************************************************************
*                                            TASKS PRIORITIES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_PRIO                           	2
#define  APP_CFG_TASK_CC2520_PRIO							3
#define  APP_CFG_TASK_LCD_PRIO								4

/*lwIP TCP/IP stack task.*/
#define  TASK_LWIP_TASK_ISR_PRIO                            14
#define  TASK_NET_CLIENT_PRIORITY							11


#define  LWIP_TASK_START_PRIO                             	5
#define  LWIP_TASK_END_PRIO                               	8

/*uC/OS port task.*/
#define  OS_PROBE_TASK_PRIO              	(OS_LOWEST_PRIO - 3)
#define  OS_TASK_TMR_PRIO                	(OS_LOWEST_PRIO - 2)



/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                            Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  TASK_START_STK_SIZE                     	 50
#define  TASK_CC2520_STK_SIZE                       512
#define  TASK_NET_CLIENT_STACK_SIZE					512
#define  TASK_LCD_STK_SIZE							128
#define  TASK_LWIP_STK_SIZE							512
/*
*********************************************************************************************************
*                               uC/Probe plug-in for uC/OS-II CONFIGURATION
*********************************************************************************************************
*/

#define  OS_PROBE_TASK                                     1    /* Task will be created for uC/Probe OS Plug-In         */
#define  OS_PROBE_TMR_32_BITS                              1    /* uC/Probe OS Plug-In timer is a 32-bit timer          */
#define  OS_PROBE_HOOKS_EN                                 1    /* Hooks to update OS_TCB profiling members will be ... */
                                                                /* ... included                                         */


/*
*********************************************************************************************************
*                                      uC/OS-II DCC CONFIGURATION
*********************************************************************************************************
*/

#define  OS_CPU_ARM_DCC_EN                                 1


/*
*********************************************************************************************************
*                                             uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN             DEF_ENABLED
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN              DEF_ENABLED
#define  LIB_MEM_CFG_ALLOC_EN                    DEF_DISABLED


/*
*********************************************************************************************************
*                                        BSP CONFIGURATION
*
* Note(s) : (1) BSP_CFG_SER_COMM_SEL defines the UART to be used to output the system state:
*
*                   BSP_SER_COMM_UART_01           UART #1  is selected to output the system state
*
*                   BSP_SER_COMM_UART_DBG          UART DBG is selected to output the system state
*            
*********************************************************************************************************
*/

#define  BSP_CFG_SER_COMM_SEL                    BSP_SER_COMM_UART_DBG


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                  0
#define  TRACE_LEVEL_INFO                                 1
#define  TRACE_LEVEL_DBG                                  2

#define  APP_CFG_TRACE_LEVEL                       TRACE_LEVEL_INFO
#define  APP_CFG_TRACE                             BSP_SerPrintf           

#define  APP_TRACE_INFO(x)                        ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                         ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                     MODULE CONFIGURATION
*********************************************************************************************************
*/
//#define LCD
//#define CC2520



#endif
