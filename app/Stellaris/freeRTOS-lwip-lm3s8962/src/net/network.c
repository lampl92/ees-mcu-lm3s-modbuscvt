/*
*********************************************************************************************************
* Filename      : network.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/
#include <stdint.h>
#include <string.h>
#include "network.h"
#include "app_cfg.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"
#include "ping.h"
#include "utils/uartstdio.h"
#include "inc/hw_ints.h"
#include "inc/hw_ethernet.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/ethernet.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "modbus.h"
#include "mb_cvt.h"
/* ------------------------------------------------------------------------------------------------------
 *											Local Variable
 * ------------------------------------------------------------------------------------------------------
 */
//OS_STK  Task_Eth_Stk[TASK_NET_CLIENT_STACK_SIZE];

unsigned char MACAddress[] = My_Mac_ID;
unsigned char IPAddress[] = MY_IP_ID;
unsigned char NetMaskAddr[] = IP_MARK_ID;
unsigned char GwWayAddr[] = MY_GATEWAY_ID;

unsigned char g_bNetStatus;

void stringtoip(unsigned long ipaddr, char *str)
{
    char pucBuf[16];
    unsigned char *pucTemp = (unsigned char *)&ipaddr;
		if(str == NULL)
			return;
    //
    // Convert the IP Address into a string.
    //
    usprintf(pucBuf, "%d.%d.%d.%d", pucTemp[0], pucTemp[1], pucTemp[2],
             pucTemp[3]);
		memcpy(str, pucBuf, 16);
		return;
}
/* ------------------------------------------------------------------------------------------------------
 *									lwIP_init()
 *
 * Description : main function.
 *
 * Argument(s) : none.
 *
 */
err_t lwIP_init(void)
{
	err_t err = ERR_OK;
//	struct ip_addr stIpAddr, stNetMsk, stGatWay;

	/*load local net parameter*/
//	lwIPLocalMACGet(MACAddress);
	
	/*use dhcp mode*/
	lwIPInit(MACAddress, 0, 0, 0, IPADDR_USE_DHCP);

	g_bNetStatus = NETS_INIT;
	return err;
}

/* ------------------------------------------------------------------------------------------------------
 *									TcpClientTask()
 *
 * Description : main function.
 *
 * Argument(s) : none.
 *
 */
static void TcpClientMainProc(void)
{
	struct in_addr  g_sClientIP;
	char str[16];
	switch(g_bNetStatus)
	{
	case NETS_INIT:
		do
		{
			g_sClientIP.s_addr = lwIPLocalIPAddrGet();
			UARTprintf(".");
			vTaskDelay(300);
		}while(0 == g_sClientIP.s_addr);//获取DHCP分配的IP地址
		
		UARTprintf("Get IP completed: \r\n");
		memset(str, 0, 16);
		stringtoip(g_sClientIP.s_addr, str);
		UARTprintf("IP: %s \r\n",str);
		
		g_sClientIP.s_addr = lwIPLocalNetMaskGet();
		memset(str, 0, 16);
		stringtoip(g_sClientIP.s_addr, str);
		UARTprintf("NetMask: %s \r\n",str);
		
		g_sClientIP.s_addr = lwIPLocalGWAddrGet();
		memset(str, 0, 16);
		stringtoip(g_sClientIP.s_addr, str);
		UARTprintf("GWAddr: %s \r\n",str);
		g_bNetStatus = NETS_LOCIP;
		UARTprintf("free mem:%d \r\n", xPortGetFreeHeapSize());
		
		modbus_init();
		break;

	case NETS_LOCIP:
//		TcpGetLocalIp();
		break;

	case NETS_SRVIP:
//		TcpGetServerIp();
		break;

	case NETS_LOGIN:
//		TcpClientRelogin();
		break;

	case NETS_NORMAL:
//		TcpNormalProc();
		break;

	default:
		break;
	}
}

/* ------------------------------------------------------------------------------------------------------
 *									TcpClientTask()
 *
 * Description : main function.
 *
 * Argument(s) : none.
 *
 */
static void TcpClientTask(void *pArg)
{
	for(;;)
	{
        TcpClientMainProc();
		vTaskDelay(2);
	}
}

/* ------------------------------------------------------------------------------------------------------
 *									NetServerInit()
 *
 * Description : main function.
 *
 * Argument(s) : none.
 *
 */
void NetServerInit(void)
{
	xTaskHandle xHandle;
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

	lwIP_init();
	
//	ping_init();
	
	/*创建TCP/IP应用任务
	OSTaskCreate(TcpClientTask,
				 (void *)0,
				 &Task_Eth_Stk[TASK_NET_CLIENT_STACK_SIZE-1],
				 TASK_NET_CLIENT_PRIORITY);*/
	
	xTaskCreate(TcpClientTask, ( signed portCHAR * )"TcpClient", TASK_NET_SERVER_STACK_SIZE, 
				NULL, TASK_NET_SERVER_PRIORITY, &xHandle);	
	
/*	sys_thread_new("TcpClt", TcpClt, NULL, 
					TASK_NET_SERVER_STACK_SIZE, 
					TASK_NET_SERVER_PRIORITY);*/
}



