/**
 * \file
 *
 * \brief Ethernet management for the FreeRTOS with lwIP example.
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <string.h>

/* Have to include gpio.h before FreeRTOS.h as long as FreeRTO redefines
 * the inline keyword to empty.
 */
#include "gpio.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
//#include "partest.h"
#include "conf_lwip_threads.h"

/* ethernet includes */
#include "ethernet.h"
#include "netif/ethernetif.h"
#include "conf_eth.h"

#include "emac.h"
#include "ethernet_phy.h"

#include "BasicWEB.h"
#include "net_handle.h"

#include "ethernet.h"
/* lwIP includes */
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include <lwip/dhcp.h>
#include <lwip/dns.h>
#include <lwip/netif.h>
#include "lwip/stats.h"
#include "lwip/init.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#else
#include "lwip/inet.h"
#endif

#include "net_config.h"

/* Global variable containing MAC Config (hw addr, IP, GW, ...) */
struct netif gs_net_if;

/* Timer for calling lwIP tmr functions without system */
typedef struct timers_info {
	uint32_t timer;
	uint32_t timer_interval;
	void (*timer_func)(void);
} timers_info_t;

void prvlwIPInit( void );

/**
 *  \brief Set ethernet config.
 */
static void ethernet_configure_interface(void)
{
	struct ip_addr x_ip_addr, x_net_mask, x_gateway;
	extern err_t ethernetif_init(struct netif *netif);

	if(f_ip_config.mode == IP_CONFIG_MODE_DHCP)
	{
		x_ip_addr.addr = 0;
		x_net_mask.addr = 0;
	}
	else
	{
		/* Default ip addr */
		IP4_ADDR(&x_ip_addr, f_ip_config.ip[0], f_ip_config.ip[1],
				f_ip_config.ip[2], f_ip_config.ip[3]);

		/* Default subnet mask */
		IP4_ADDR(&x_net_mask, f_ip_config.mask[0], f_ip_config.mask[1],
				f_ip_config.mask[2], f_ip_config.mask[3]);
		
		/* Default gateway addr */
		IP4_ADDR(&x_gateway, ETHERNET_CONF_GATEWAY_ADDR0,
				ETHERNET_CONF_GATEWAY_ADDR1,
				ETHERNET_CONF_GATEWAY_ADDR2,
				ETHERNET_CONF_GATEWAY_ADDR3);
	}
	
	/* Add data to netif */
	netif_add(&gs_net_if, &x_ip_addr, &x_net_mask, &x_gateway, NULL,
			ethernetif_init, ethernet_input);

	/* Make it the default interface */
	netif_set_default(&gs_net_if);

	/* Setup callback function for netif status change */
	netif_set_status_callback(&gs_net_if, status_callback);

	/* Bring it up */
	if(f_ip_config.mode == IP_CONFIG_MODE_DHCP)
	{
		dhcp_start(&gs_net_if);
	}
	else
	{
		netif_set_up(&gs_net_if);
	}
}

/**
 *  \brief Callback executed when the TCP/IP init is done.
 */
static void tcpip_init_done(void *arg)
{
	sys_sem_t *sem;
	sem = (sys_sem_t *)arg;

	/* Set hw and IP parameters, initialize MAC too */
	ethernet_configure_interface();

#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
  sys_sem_signal (*sem);    /* Signal the waiting thread that the TCP/IP init is done. */
#else
  sys_sem_signal (sem);    /* Signal the waiting thread that the TCP/IP init is done. */
#endif
}

/**
 *  \brief start lwIP layer.
 */
void prvlwIPInit( void )
{
	sys_sem_t sem;

#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
	sem = sys_sem_new(0);   /* Create a new semaphore. */
	tcpip_init(tcpip_init_done, &sem);
	sys_sem_wait(sem);      /* Block until the lwIP stack is initialized. */
	sys_sem_free(sem);      /* Free the semaphore. */
#else
	err_t err_sem;
	err_sem = sys_sem_new(&sem, 0); /* Create a new semaphore. */
	tcpip_init(tcpip_init_done, &sem);
	sys_sem_wait(&sem);     /* Block until the lwIP stack is initialized. */
	sys_sem_free(&sem);     /* Free the semaphore. */
#endif
}

void vStartEthernetTaskLauncher( unsigned portBASE_TYPE uxPriority )
{
	/* Spawn the Sentinel task. */
	xTaskCreate( vStartEthernetTask, (const signed portCHAR *)"ETHLAUNCH",
				 configMINIMAL_STACK_SIZE, NULL, uxPriority,
				 (xTaskHandle *)NULL );
}

/*! \brief create ethernet task, for ethernet management.
 *
 *  \param uxPriority   Input. priority for the task, it should be low
 *
 */
portTASK_FUNCTION(vStartEthernetTask, pvParameters)
{
	/* Setup lwIP. */
	prvlwIPInit();

	/* Create the WEB server task.  This uses the lwIP RTOS abstraction layer. */
	sys_thread_new("WEB", vBasicWEBServer, (void *)NULL,
					lwipBASIC_WEB_SERVER_STACK_SIZE,
					lwipBASIC_WEB_SERVER_PRIORITY);

	/* Create the Socket Server task.  This uses the lwIP RTOS abstraction layer. */
	sys_thread_new("NETS", vNetHandle, (void *)NULL,
					TASK_TCP_SERVER_STACK_SIZE,
					TASK_TCP_SERVER_PRIORITY);

	/* Kill this task. */
	vTaskDelete(NULL);
}

/**
 *  \brief Status callback used to print address given by DHCP.
 *
 * \param netif Instance to network interface.
 */
void status_callback(struct netif *netif)
{
	int8_t c_mess[25];
	if (netif_is_up(netif)) {
		RS232printf("Network up");
		strcpy((char *)c_mess, "IP=");
		strcat((char *)c_mess,
				inet_ntoa(*(struct in_addr *)&(netif->ip_addr)));
		RS232printf((char const *)c_mess);
	} else {
		RS232printf("Network down");
	}
}


unsigned long lwIPLocalIPAddrGet(void)
{
	return((unsigned long)gs_net_if.ip_addr.addr);
}
