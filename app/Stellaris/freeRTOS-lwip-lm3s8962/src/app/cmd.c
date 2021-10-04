#include <includes.h>
#include "FiFo.h"
#include "lwip/inet.h"
static SFIFO rxFIFO;

int validate_number(char *str) {
   while (*str) {
      if(!isdigit(*str)){ //if the character is not a number, return false
         return 0;
      }
      str++; //point to next character
   }
   return 1;
}
int validate_ip(char *ip) { //check whether the IP is valid or not
   int i, num, dots = 0;
   char *ptr;
   if (ip == NULL)
      return 0;
      ptr = strtok(ip, "."); //cut the string using dor delimiter
      if (ptr == NULL)
         return 0;
   while (ptr) {
      if (!validate_number(ptr)) //check whether the sub string is holding only number or not
         return 0;
         num = atoi(ptr); //convert substring to number
         if (num >= 0 && num <= 255) {
            ptr = strtok(NULL, "."); //cut the next part of the string
            if (ptr != NULL)
               dots++; //increase the dot count
         } else
            return 0;
    }
    if (dots != 3) //if the number of dots are not 3, return false
       return 0;
      return 1;
}

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
int do_rtuconfig(int argc, char *argv[]);
//
// The table of commands supported by this application.
//
tCmdLineEntry g_sCmdTable[] =
{
	{ "ifconfig", network_config, "network_config" },
	{ "setmac", do_setmac, "Set mac address." },
	{ "rtuconfig", do_rtuconfig, "rtu config" },
	{ "reboot", system_reboot, "system rboot" },
	{ "free", do_memfree, "Get memory free" },
	{ "?", ProcessHelp, "Application help." },
	{NULL, NULL, NULL}
};
int ProcessHelp(int argc, char *argv[])
{
	UARTprintf("\r\nHelp cmd:\r\n");
	UARTprintf("ifconfig - config network ip\r\n");
	UARTprintf("setmac - set mac address\r\n");
	UARTprintf("rtuconfig - config rtu\r\n");
	UARTprintf("reboot - system reboot\r\n");
	UARTprintf("free - show free memory\r\n");	
	//user_epprom_test();
	return 0;
}

int do_memfree(int argc, char *argv[])
{
	UARTprintf("[%d]Free memory: %d \r\n", xTaskGetTickCount(), xPortGetFreeHeapSize());
	return 0;
}

int isValidMacAddress(const char* mac) {
    int i = 0;
    int s = 0;

	while (*mac) 
	{
	 if (isxdigit(*mac)) 
	 {
			i++;
	 }
	 else if (*mac == ':' || *mac == '-') 
	 {
		if (i == 0 || i / 2 - 1 != s)
			break;
		++s;
	 }
	 else 
	 {
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
	}
	else if(argc == 4)
	{
		if((inet_aton(argv[1], &user_data.ipaddr) == 0) || validate_ip(argv[1]) == 0)
		{
			UARTprintf("IP convert failed \r\n");
			return -1;
		}
		if((inet_aton(argv[2], &user_data.gateway) == 0) || validate_ip(argv[2]) == 0)
		{
			UARTprintf("Gateway convert failed \r\n");
			return -1;
		}
		if((inet_aton(argv[3], &user_data.netmask) == 0)|| validate_ip(argv[3]) == 0)
		{
			UARTprintf("Netmask convert failed \r\n");
			return -1;
		}
		if(set_user_data(&user_data) == 0)
			UARTprintf("Success configuration netconfig\r\n");
		else
			UARTprintf("Failed Configuration netconfig\r\n");
	}
	else if(argc == 2)
	{
		if(!strcmp("default", argv[1]))
		{
			set_default_config();
		}
	}
	else 
	{
		UARTprintf("usesage: ifconfig [IPaddress] [Gateway] [Netmask] \r\n");
	}
	
	return 0;
}

int do_rtuconfig(int argc, char *argv[])
{
	user_data_t user_data;
	get_user_data(&user_data);

	if(argc ==1)
	{
		UARTprintf("UART config: %d %d %s %d \r\n", user_data.baudrate, 
		user_data.databits == UART_CONFIG_WLEN_5 ? 5: user_data.databits == UART_CONFIG_WLEN_6 ? 6 : user_data.databits == UART_CONFIG_WLEN_7 ? 7 : user_data.databits == UART_CONFIG_WLEN_8? 8 : 0,
		user_data.parity == UART_CONFIG_PAR_NONE ? "N" : user_data.parity == UART_CONFIG_PAR_ODD ? "O": UART_CONFIG_PAR_EVEN ? "E": "unknown",
		user_data.stopbits == UART_CONFIG_STOP_ONE ? 1 : user_data.stopbits == UART_CONFIG_STOP_TWO ? 2 : 0);
	}
	else if( argc == 5)
	{
		if (strcmp(argv[1], "115200") == 0) 
		{
			user_data.baudrate = 115200;
		}
		else if (strcmp(argv[1], "38400") == 0) 
		{
			user_data.baudrate = 38400;
		}
		else if (strcmp(argv[1], "19200") == 0) 
		{
			user_data.baudrate = 19200;
		}
		else if (strcmp(argv[1], "9600") == 0) 
		{
			user_data.baudrate = 9600;
		}
		else
		{
			goto usage;
		}
		
		if (strcmp(argv[2], "5") == 0) 
		{
			user_data.databits = UART_CONFIG_WLEN_5;
		}
		else if (strcmp(argv[2], "6") == 0) 
		{
			user_data.databits = UART_CONFIG_WLEN_6;
		}
		else if (strcmp(argv[2], "7") == 0) 
		{
			user_data.databits = UART_CONFIG_WLEN_7;
		}
		else if (strcmp(argv[2], "8") == 0) 
		{
			user_data.databits = UART_CONFIG_WLEN_8;
		}
		else
		{
			goto usage;
		}
		
		if (strcmp(argv[3], "N") == 0 || strcmp(argv[3], "n") == 0) 
		{
			user_data.parity = UART_CONFIG_PAR_NONE;
		}
		else if (strcmp(argv[3], "O") == 0 || strcmp(argv[3], "o") == 0 ) 
		{
			user_data.parity = UART_CONFIG_PAR_ODD;
		}
		else if (strcmp(argv[3], "E") == 0 || strcmp(argv[3], "e") == 0) 
		{
			user_data.parity = UART_CONFIG_PAR_EVEN;
		}
		else
		{
			goto usage;
		}
		
		if (strcmp(argv[4], "1") == 0) 
		{
			user_data.stopbits = UART_CONFIG_STOP_ONE;
		}
		else if (strcmp(argv[4], "2") == 0) 
		{
			user_data.stopbits = UART_CONFIG_STOP_TWO;
		}
		else
		{
			goto usage;
		}
		
		if(set_user_data(&user_data) == 0)
			UARTprintf("Success configuration netconfig\r\n");
		else
			UARTprintf("Failed Configuration netconfig\r\n");
	}
	else
	{
		goto usage;
	}
	return 0;
	
usage:
	UARTprintf("rtuconfig [baudrate] [bitlen] [parity] [stopbit]\r\n");
	UARTprintf("- Baurate: 115200, 38400, 19200, 9600 \r\n");
	UARTprintf("- Bitlen:  5, 6, 7, 8 \r\n");
	UARTprintf("- Sarity:  [N]one, [O]dd, [E]ven \r\n");
	UARTprintf("- Stopbit: 1, 2\r\n");	
	return -1;
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

