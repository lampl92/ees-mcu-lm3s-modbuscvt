#include <includes.h>
#include "FiFo.h"
#include "lwip/inet.h"
static SFIFO rxFIFO;

void UART0IntHandler(void)
{
    unsigned long ulStatus;
    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(CMD_UART_PORT, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(CMD_UART_PORT, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(CMD_UART_PORT))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
			FIFO_Push(&rxFIFO, UARTCharGetNonBlocking(CMD_UART_PORT));
    }
}
int ProcessHelp(int argc, char *argv[]);
int network_config(int argc, char *argv[]);
int system_reboot(int argc, char *argv[]);
int do_setmac(int argc, char *argv[]);
int do_memfree(int argc, char *argv[]);
//
// The table of commands supported by this application.
//
tCmdLineEntry g_sCmdTable[] =
{
	{ "ifconfig", network_config, "network_config" },
	{ "reboot", system_reboot, "system rboot" },
	{ "help", ProcessHelp, "Application help." },
	{ "setmac", do_setmac, "Set mac address." },
	{ "free", do_memfree, "Get memory free" },
	{NULL, NULL, NULL}
};
int do_memfree(int argc, char *argv[])
{
	UARTprintf("[%d]Free memory: %d \r\n", xTaskGetTickCount(), xPortGetFreeHeapSize());
}
int isValidMacAddress(const char* mac) {
    int i = 0;
    int s = 0;

    while (*mac) {
       if (isxdigit(*mac)) {
          i++;
       }
       else if (*mac == ':' || *mac == '-') {

          if (i == 0 || i / 2 - 1 != s)
            break;

          ++s;
       }
       else {
           s = -1;
       }


       ++mac;
    }

    return (i == 12 && (s == 5 || s == 0));
}
int do_setmac(int argc, char *argv[])
{
	user_data_t user_data;
	if(argc != 2)
	{
		UARTprintf("usage: setmac [macaddress] \r\n");
		return -1;
	} 
	else
	{
		if (isValidMacAddress(argv[1]) == pdFALSE) 
		{
			UARTprintf("MAC address is not valid \r\n");	
			return -1;
		}
		get_user_data(&user_data);
		sscanf(argv[1], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &user_data.mac[0], 
			&user_data.mac[1], 
			&user_data.mac[2], 
			&user_data.mac[3], 
			&user_data.mac[4], 
			&user_data.mac[5]);
		if(set_user_data(&user_data) == 0)
			UARTprintf("Success configuration MAC address\r\n");
		else
			UARTprintf("Failed Configuration MAC address\r\n");
	}
	return 0;
}
//
// Code for the "help" command.
//
int ProcessHelp(int argc, char *argv[])
{
	UARTprintf("\r\nHelp cmd\r\n");
	user_epprom_test();
	return 0;
}
int system_reboot(int argc, char *argv[])
{
	do_reboot();
	return 0;
}

void do_reboot(void)
{
	IntMasterDisable();

	HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) & ~(SYSCTL_RCC_USESYSDIV)) |
	SYSCTL_RCC_BYPASS);
	HWREG(SYSCTL_RCC2) |= SYSCTL_RCC2_BYPASS2;

	HWREG(SYSCTL_RCC) = 0x078e3ad0 | (HWREG(SYSCTL_RCC) & SYSCTL_RCC_MOSCDIS);
	HWREG(SYSCTL_RCC2) = 0x07806810;
	HWREG(SYSCTL_RCC) = 0x078e3ad1;

	HWREG(SYSCTL_DSLPCLKCFG) = 0x07800000;

	HWREG(SYSCTL_RCGC0) = 0x00000040;
	HWREG(SYSCTL_RCGC1) = 0;
	HWREG(SYSCTL_RCGC2) = 0;
	HWREG(SYSCTL_SCGC0) = 0x00000040;
	HWREG(SYSCTL_SCGC1) = 0;
	HWREG(SYSCTL_SCGC2) = 0;
	HWREG(SYSCTL_DCGC0) = 0x00000040;
	HWREG(SYSCTL_DCGC1) = 0;
	HWREG(SYSCTL_DCGC2) = 0;

	HWREG(SYSCTL_PBORCTL) = 0;
	HWREG(SYSCTL_IMC) = 0;
	HWREG(SYSCTL_GPIOHBCTL) = 0;
	HWREG(SYSCTL_MOSCCTL) = 0;
	HWREG(SYSCTL_PIOSCCAL) = 0;
	HWREG(SYSCTL_I2SMCLKCFG) = 0;

	HWREG(SYSCTL_SRCR0) = 0xffffffff;
	HWREG(SYSCTL_SRCR1) = 0xffffffff;
	HWREG(SYSCTL_SRCR2) = 0xffffffff;
	HWREG(SYSCTL_SRCR0) = 0;
	HWREG(SYSCTL_SRCR1) = 0;
	HWREG(SYSCTL_SRCR2) = 0;

	HWREG(SYSCTL_MISC) = 0xffffffff;

	SysCtlReset();
	return;
}
int network_config(int argc, char *argv[])
{
	user_data_t user_data;
	if(argc == 1) 
	{
		get_user_data(&user_data);
		UARTprintf("IPddr: %s \r\n",inet_ntoa(user_data.ipaddr));
		UARTprintf("Gateway: %s \r\n",inet_ntoa(user_data.gateway));
		UARTprintf("Netmask: %s \r\n",inet_ntoa(user_data.netmask));
		UARTprintf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x \r\n",user_data.mac[0],
			user_data.mac[1],
			user_data.mac[2],
			user_data.mac[3],
			user_data.mac[4],
			user_data.mac[5]);

		UARTprintf("Lifetime: %d \r\n",user_data.lifetime);
	}else if(argc == 4)
	{
		if(inet_aton(argv[1], &user_data.ipaddr) == 0)
		{
			UARTprintf("IP convert failed \r\n");
			return -1;
		}
		if(inet_aton(argv[2], &user_data.gateway) == 0)
		{
			UARTprintf("Gateway convert failed \r\n");
			return -1;
		}
		if(inet_aton(argv[3], &user_data.netmask) == 0)
		{
			UARTprintf("Netmask convert failed \r\n");
			return -1;
		}
		if(set_user_data(&user_data) == 0)
			UARTprintf("Success configuration netconfig\r\n");
		else
			UARTprintf("Failed Configuration netconfig\r\n");
	} 
	else 
	{
		UARTprintf("usesage: ifconfig [IPaddress] [Gateway] [Netmask] \r\n");
	}
	
	return 0;
}

static void cmd_task(void *pArgs)
{
	char rxbuf[INPUT_BUFER_SIZE];
	uint16_t rxIndex = 0;
	uint8_t Char;

	for(;;)
	{
		if(FIFO_GetCount(&rxFIFO) > 0)
		{
			FIFO_Recv(&rxFIFO, &Char, 1);			
			if (rxIndex < INPUT_BUFER_SIZE)
			{
				if(Char  == '\n' || Char  == '\r')
				{
proc:
					if(rxIndex > 0)
						rxbuf[rxIndex] = '\0';
					UARTprintf("\r\n");
					CmdLineProcess(rxbuf);
					UARTprintf("\r\n>");
					rxIndex = 0;
				}
				else if (Char == 0x08)
				{
					if(rxIndex>0) 
					{
						rxbuf[rxIndex--]='\0';
						UARTCharPut(CMD_UART_PORT, Char);						
						UARTCharPut(CMD_UART_PORT, ' ');		
						UARTCharPut(CMD_UART_PORT, Char);						
					}
				}
				else
				{
					rxbuf[rxIndex++] = Char;
					UARTCharPut(CMD_UART_PORT, Char);
				}
			}
			else
			{
				goto proc;
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
}

void cmd_init(void)
{
	xTaskHandle xHandle;
	
	// Init FIFO uart
	FIFO_Create(&rxFIFO);
	FIFO_Reset(&rxFIFO);
	
	//
	// Init UART Fifo
	//
	UARTFIFOEnable(CMD_UART_PORT);
	
	//
	// Enable the UART interrupt.
	//
	IntEnable(CMD_UART_PORT_INT);
	UARTIntEnable(CMD_UART_PORT, UART_INT_RX | UART_INT_RT);
	
	xTaskCreate(cmd_task, ( signed portCHAR * )"cmd_task", TASK_USER_CMD_STACKSIZE, 
			NULL, TASK_USER_CMD_PRIORITY, &xHandle);	
}

