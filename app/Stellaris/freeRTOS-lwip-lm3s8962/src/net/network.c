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
#include "lwip/inet.h"
#include "mb_cvt.h"
#include "user_epprom.h"
#include "webserver.h"
#include "io.h"
#include "cmd.h"
/* ------------------------------------------------------------------------------------------------------
 *											Local Variable
 * ------------------------------------------------------------------------------------------------------
 */
#define COUNTER_LIFE_TIME_MIN		60 * 12
#define BUTTON_RESET_TIMEOUT		300 // 300 * 10ms = 3s
unsigned char MACAddress[] = My_Mac_ID;
unsigned char IPAddress[] = MY_IP_ID;
unsigned char NetMaskAddr[] = IP_MARK_ID;
unsigned char GwWayAddr[] = MY_GATEWAY_ID;

unsigned char g_bNetStatus;
uint32_t	btn_cnt = 0;
uint32_t	led_cnt = 0;
void stringtoip(unsigned long ipaddr, char *str)
{
    char pucBuf[16];
    unsigned char *pucTemp = (unsigned char *)&ipaddr;
		if(str == NULL)
			return;

    usprintf(pucBuf, "%d.%d.%d.%d", pucTemp[0], pucTemp[1], pucTemp[2], pucTemp[3]);
		
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
	user_data_t user_data;
	ip_addr_t ip_address, net_mask, gateway;
	
	get_user_data(&user_data);
	IP4_ADDR( &ip_address,(unsigned char)(user_data.ipaddr >> 24),
	(unsigned char)(user_data.ipaddr >> 16),
	(unsigned char)(user_data.ipaddr >> 8),
	(unsigned char)(user_data.ipaddr));
	IP4_ADDR( &net_mask,(unsigned char)(user_data.netmask >> 24),
	(unsigned char)(user_data.netmask >> 16),
	(unsigned char)(user_data.netmask >> 8),
	(unsigned char)(user_data.netmask));
	IP4_ADDR( &gateway,(unsigned char)(user_data.gateway >> 24),
	(unsigned char)(user_data.gateway >> 16),
	(unsigned char)(user_data.gateway >> 8),
	(unsigned char)(user_data.gateway));
	/*load local net parameter*/
//	lwIPLocalMACGet(MACAddress);

	/*use dhcp mode*/
	lwIPInit((const unsigned char *)user_data.mac, ip_address.addr, net_mask.addr, gateway.addr, IPADDR_USE_STATIC);
	//lwIPInit(MACAddress, 0, 0, 0, IPADDR_USE_DHCP);
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
	//user_data_t userdata;
	switch(g_bNetStatus)
	{
	case NETS_INIT:
		do
		{
			g_sClientIP.s_addr = lwIPLocalIPAddrGet();
			vTaskDelay(300);
		}while(0 == g_sClientIP.s_addr);
		
		//UARTprintf("DEVICE INFO:\r\n");
		memset(str, 0, 16);
		stringtoip(g_sClientIP.s_addr, str);
		//UARTprintf("IP: %s\r\n",str);
		
		g_sClientIP.s_addr = lwIPLocalNetMaskGet();
		memset(str, 0, 16);
		stringtoip(g_sClientIP.s_addr, str);
		//UARTprintf("NetMask: %s\r\n",str);
		
		g_sClientIP.s_addr = lwIPLocalGWAddrGet();
		memset(str, 0, 16);
		stringtoip(g_sClientIP.s_addr, str);
		//UARTprintf("GWAddr: %s\r\n",str);
		g_bNetStatus = NETS_LOCIP;
		//UARTprintf("Free memory: %d\r\n", xPortGetFreeHeapSize());
		
		modbus_init();
		webserver_init();
		break;

	case NETS_LOCIP:
		if(++led_cnt == 50) {
			led_cnt = 0;
			if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1))
				LED_SYS_OFF;
			else
				LED_SYS_ON;
		}
		if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3))
			btn_cnt = 0;
		else
			btn_cnt++;
		if (btn_cnt > BUTTON_RESET_TIMEOUT)
		{
			btn_cnt = 0;
			LED_SYS_ON;
			LED_MODBUS_ON;
			vTaskDelay(1000);
			do_reboot();
			/*
			get_user_data(&userdata);
			if(userdata.lifetime == 0xFFFFFFFF)
				userdata.lifetime = 0;
			userdata.lifetime++;
			set_user_data(&userdata);
			*/
			//UARTprintf("lifetime : %u\r\n", userdata.lifetime);
		}
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
		vTaskDelay(10);
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
	
	
	xTaskCreate(TcpClientTask, ( signed portCHAR * )"TcpClient", TASK_NET_SERVER_STACK_SIZE, 
				NULL, TASK_NET_SERVER_PRIORITY, &xHandle);	
}



