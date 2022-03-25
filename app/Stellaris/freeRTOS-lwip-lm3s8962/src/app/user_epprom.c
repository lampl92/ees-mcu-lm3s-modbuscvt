#include <includes.h>
#include <utils/crc.h>
#include "lwip/inet.h"
#define USER_DATA_OFFSET 0
#define SLAVE_ADDR 0x50
#define OFFSET 		 0x0
#define PAGE_SIZE 	8
uint32_t ui32Index;

#define FLASH_PB_END            0x40000

//*****************************************************************************
//
// The address of the first block of flash to be used for storing parameters.
//
//*****************************************************************************
#define FLASH_PB_START          (FLASH_PB_END - 0x800)

//*****************************************************************************
//
// The size of the parameter block to save.  This must be a power of 2, and
// should be large enough to contain the tParameters structure.
//
//*****************************************************************************
#define FLASH_PB_SIZE           64
void user_epprom_init(void)
{
	UARTprintf("sizeof user_data: %d \r\n", sizeof(user_data_t));
	FlashPBInit(FLASH_PB_START, FLASH_PB_END, FLASH_PB_SIZE);
	
	return;
}

int get_user_data(user_data_t * user_data)
{
	unsigned char *pucBuffer = NULL;
	
	pucBuffer = FlashPBGet();
	
	if(pucBuffer == NULL)
	{
		UARTprintf("restore default config user data \r\n");
		memset(user_data,0xff, sizeof(user_data_t));
		user_data->ucSequenceNum = 0;
		user_data->ucCRC = 0;
		user_data->ucVersion = 1;
		user_data->ucDeviceNumber = 1;
		inet_aton("192.168.1.10", &user_data->ipaddr);
		inet_aton("192.168.1.1", &user_data->gateway);
		inet_aton("255.255.255.0", &user_data->netmask);
		user_data->lifetime = 0;
		user_data->baudrate = 9600;
		user_data->databits = UART_CONFIG_WLEN_8;
		user_data->parity = UART_CONFIG_PAR_NONE;
		user_data->stopbits = UART_CONFIG_STOP_ONE;
		user_data->mac[0] = 0x00;
		user_data->mac[1] = 0x14;
		user_data->mac[2] = 0x97;
		user_data->mac[3] = 0x0f;
		user_data->mac[4] = 0x1d;
		user_data->mac[5] = 0xe3;
		set_user_data(user_data);
	}
	else
	{
//		if(user_data->crc != Crc16(0, (const unsigned char *)user_data, sizeof(user_data_t) - 2)) 
//		{
//			UARTprintf("restore default config user data \r\n");
//			memset(user_data,0xff, sizeof(user_data_t));
//			user_data->ucSequenceNum = 0;
//			user_data->ucCRC = 0;
//			user_data->ucVersion = 1;
//			user_data->ucDeviceNumber = 1;
//			inet_aton("192.168.1.10", &user_data->ipaddr);
//			inet_aton("192.168.1.1", &user_data->gateway);
//			inet_aton("255.255.255.0", &user_data->netmask);
//			user_data->lifetime = 0;
//			user_data->baudrate = 9600;
//			user_data->databits = UART_CONFIG_WLEN_8;
//			user_data->parity = UART_CONFIG_PAR_NONE;
//			user_data->stopbits = UART_CONFIG_STOP_ONE;
//			user_data->mac[0] = 0x00;
//			user_data->mac[1] = 0x14;
//			user_data->mac[2] = 0x97;
//			user_data->mac[3] = 0x0f;
//			user_data->mac[4] = 0x1d;
//			user_data->mac[5] = 0xe3;
//			set_user_data(user_data);
//		}
		memcpy(user_data, pucBuffer, sizeof(user_data_t));
	}
	return 0;
}
 
void set_default_config(void)
{
	user_data_t user_data;
	memset(&user_data,0xff, sizeof(user_data_t));
	UARTprintf("restore default config user data \r\n");
	inet_aton("192.168.1.10", &user_data.ipaddr);
	inet_aton("192.168.1.1", &user_data.gateway);
	inet_aton("255.255.255.0", &user_data.netmask);
	user_data.lifetime = 0;
	user_data.baudrate = 9600;
	user_data.databits = UART_CONFIG_WLEN_8;
	user_data.parity = UART_CONFIG_PAR_NONE;
	user_data.stopbits = UART_CONFIG_STOP_ONE;
	user_data.mac[0] = 0x00;
	user_data.mac[1] = 0x14;
	user_data.mac[2] = 0x97;
	user_data.mac[3] = 0x0f;
	user_data.mac[4] = 0x1d;
	user_data.mac[5] = 0xe3;
	user_data.ucSequenceNum = 0;
	user_data.ucCRC = 0;
	user_data.ucVersion = 1;
	user_data.ucDeviceNumber = 1;
	set_user_data(&user_data);
}

int set_user_data(user_data_t * user_data)
{
	if (user_data == NULL)
		return -1;
	
	//user_data->crc = Crc16(0, (const unsigned char *)user_data, sizeof(user_data_t) - 2);
	FlashPBSave((unsigned char *)user_data);
	
	return 0;
}

