#include <includes.h>
#include <utils/crc.h>
#include "lwip/inet.h"
#define USER_DATA_OFFSET 0
#define SLAVE_ADDR 0x50
#define OFFSET 		 0x0
#define PAGE_SIZE 	8
uint32_t ui32Index;

void user_epprom_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	
  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_OD_WPD);
	
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
	
	I2CMasterInitExpClk(I2C0_MASTER_BASE, SysCtlClockGet(), true);

	return;
}

void EepromWrite(uint8_t Slave_Address,uint16_t Address, uint8_t *pui32DataTx, uint8_t Size)
{
		while (I2CMasterBusy(I2C0_MASTER_BASE));

    I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, Slave_Address , false);

		I2CMasterDataPut(I2C0_MASTER_BASE,(uint8_t)Address);

    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    while (I2CMasterBusy(I2C0_MASTER_BASE));

    for (ui32Index = 0; ui32Index < Size - 1; ui32Index++)
    {

       // UARTprintf("  Sending: '%x'  . . . \n", pui32DataTx[ui32Index]);

        I2CMasterDataPut(I2C0_MASTER_BASE, pui32DataTx[ui32Index]);

        I2CMasterControl(I2C0_MASTER_BASE,I2C_MASTER_CMD_BURST_SEND_CONT);

        while (I2CMasterBusy(I2C0_MASTER_BASE));
    }
		
		I2CMasterDataPut(I2C0_MASTER_BASE, pui32DataTx[ui32Index]);

		I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
		
		while (I2CMasterBusy(I2C0_MASTER_BASE));
		vTaskDelay(500);
}

void Ext_EepromRandomRead(uint32_t SlaveAddress,uint16_t Address , uint8_t *DataReceived, uint16_t NumOfBytes)
{
        uint16_t Index;
				
				while (I2CMasterBusy(I2C0_MASTER_BASE));
	
        I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SlaveAddress, false);

				I2CMasterDataPut(I2C0_MASTER_BASE,(uint8_t)(Address));

        I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        while (I2CMasterBusy(I2C0_MASTER_BASE));

        I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SlaveAddress, true);

        I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

        for (Index = 0; Index < NumOfBytes ; Index++)
        {
					while (I2CMasterBusy(I2C0_MASTER_BASE));					

					DataReceived[Index] = I2CMasterDataGet(I2C0_MASTER_BASE);

					I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        }
				
        I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
				
				while (I2CMasterBusy(I2C0_MASTER_BASE));
}

void Eeprom_Ext_Write(uint16_t address,uint8_t * data, uint16_t length)
{
	int page_count = 0;
	int lastpage_cnt = 0;
	int i;
	page_count = length / PAGE_SIZE;
	lastpage_cnt = length % PAGE_SIZE;
	if(lastpage_cnt > 0)
		page_count++;
	else
		lastpage_cnt = 8;
	
	for(i = 0; i < page_count; i++)
	{
		if(i < page_count - 1)
			EepromWrite(SLAVE_ADDR,address + (i * PAGE_SIZE),data + (i * PAGE_SIZE),8);
		else 
			EepromWrite(SLAVE_ADDR,address + (i * PAGE_SIZE),data + (i * PAGE_SIZE),lastpage_cnt);
	}
	
}
void Eeprom_Ext_Read(uint16_t address,uint8_t * data, uint16_t length)
{
	int page_count = 0;
	int lastpage_cnt = 0;
	int i;
	page_count = length / PAGE_SIZE;
	lastpage_cnt = length % PAGE_SIZE;
	if(lastpage_cnt > 0)
		page_count++;
	else
		lastpage_cnt = 8;
	
	for(i = 0; i < page_count; i++)
	{
		if(i < page_count - 1)
			Ext_EepromRandomRead(SLAVE_ADDR,address + (i * PAGE_SIZE),data + (i * PAGE_SIZE),8);
		else 
			Ext_EepromRandomRead(SLAVE_ADDR,address + (i * PAGE_SIZE),data + (i * PAGE_SIZE),lastpage_cnt);
	}
}

int get_user_data(user_data_t * user_data)
{
	if (user_data == NULL)
		return -1;
	
	Eeprom_Ext_Read(USER_DATA_OFFSET, (uint8_t *)user_data, sizeof(user_data_t));
	
	if(user_data->crc != Crc16(0, (const unsigned char *)user_data, sizeof(user_data_t) - 2))
	{
		memset(user_data,0xff, sizeof(user_data_t));
		UARTprintf(" crc failed, restore default config user data \r\n");
		inet_aton("192.168.1.10", &user_data->ipaddr);
		inet_aton("192.168.1.1", &user_data->gateway);
		inet_aton("255.255.255.0", &user_data->netmask);
		user_data->lifetime = 0;
		user_data->baudrate = 115200;
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
	set_user_data(&user_data);
}
int set_user_data(user_data_t * user_data)
{
	if (user_data == NULL)
		return -1;
	
	user_data->crc = Crc16(0, (const unsigned char *)user_data, sizeof(user_data_t) - 2);
	
	Eeprom_Ext_Write(USER_DATA_OFFSET, (uint8_t *)user_data, sizeof(user_data_t));
	
	return 0;
}
void user_epprom_test(void)
{
	//uint8_t buff[64];
	//int i = 0;
	//int index = 0;
  //UARTprintf("EPPROM TEST INIT \r\n");
	//user_epprom_init();
#if 0
	UARTprintf("EPPROM READ\r\n");

	Eeprom_Ext_Read(OFFSET, buff, 64);
	for (index = 0; index < 64; index++)
	{
		UARTprintf("%d ", buff[index]);
		if(index % 8 == 7)
			UARTprintf("\r\n");
	}

	
	UARTprintf("EPPROM WRITE\r\n");
	for (i = 0; i < 64; i ++)
	{
		buff[i] = i;
	}
	Eeprom_Ext_Write(OFFSET, buff, 64);

	
	UARTprintf("EPPROM READ\r\n");
	Eeprom_Ext_Read(OFFSET, buff, 64);
	for (index = 0; index < 64; index++)
	{
		UARTprintf("%d ", buff[index]);
		if(index % 8 == 7)
			UARTprintf("\r\n");
	}
#endif
	UARTprintf("sizeof user_data_t : %d \r\n", sizeof(user_data_t));
	return;
}
