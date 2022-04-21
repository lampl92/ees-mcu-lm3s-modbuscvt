/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                              (c) Copyright 2009; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           MASTER INCLUDES
*
*                             LUMINARY MICRO LM3S8962 on the EK-LM3S8962
*
* Filename      : includes.h
* Version       : V1.00
* Programmer(s) : BAN
*********************************************************************************************************
*/
#ifndef  INCLUDES_PRESENT
#define  INCLUDES_PRESENT

/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/
#include  <stdio.h>
#include  <stdlib.h>
#include  <stdint.h>
#include  <string.h>
#include  <stdarg.h>
#include  <math.h>
/*
*********************************************************************************************************
*                                              LIBRARIES
*********************************************************************************************************
*/
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_timer.h"
#include "inc/hw_sysctl.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/i2c.h"
#include "driverlib/watchdog.h"
/*
*********************************************************************************************************
*                                              freeRTOS
*********************************************************************************************************
*/
#include <FreeRTOSConfig.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


/*
*********************************************************************************************************
*                                             NET/lwIP
*********************************************************************************************************
*/
#include "network.h"


/*
*********************************************************************************************************
*                                               UTILS
*********************************************************************************************************
*/
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"


/*
*********************************************************************************************************
*                                              APP / BSP
*********************************************************************************************************
*/
#include "app_cfg.h"

#include "bsp.h"
#include "mb_cvt.h"
#include "cmd.h"
#include "utils/cmdline.h"
#include "utils/flash_pb.h"
#include "user_epprom.h"
#include "webserver.h"
#include "cgifuncs.h"
#include "httpd.h"
#include "io.h"
#include "comm.h"
#endif
