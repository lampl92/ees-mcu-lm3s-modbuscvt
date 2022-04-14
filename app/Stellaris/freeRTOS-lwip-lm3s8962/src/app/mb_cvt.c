#include <includes.h>
#include <signal.h>
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "FiFo.h"
#include <errno.h>
#define LIGHTMODBUS_FULL
#define LIGHTMODBUS_DEBUG
#define LIGHTMODBUS_IMPL
#include <lightmodbus.h>

const uint16_t UT_REGISTERS_NB_SPECIAL = 0x2;

const uint16_t UT_REGISTERS_ADDRESS_SLEEP_500_MS = 0x172;

const uint16_t UT_REGISTERS_ADDRESS_SPECIAL = 0x170;

const uint16_t UT_REGISTERS_ADDRESS_INVALID_TID_OR_SLAVE = 0x171;

const uint16_t UT_REGISTERS_ADDRESS_BYTE_SLEEP_5_MS = 0x173;

//#define CVT_DEBUG

#define MODBUS_TCP_MAX_ADU_LENGTH  260

#define MODBUS_RTU_MAX_ADU_LENGTH  256

#define MODBUS_TCP_SLAVE_POSITION 6

#define MODBUS_TCP_HEADER_LENGHT 6

#define MODBUS_RTU_CRC_LENGHT 2

#define TCP_CLIENT_SOCKET_TIMEOUT              (5000)

#define MAX_RESPONSE MODBUS_RTU_MAX_ADU_LENGTH

#define MAX_CLIENT_SOCKET						4

#define WATCHDOG_FEED_TIMEOUT				5000 // milisecond

#define RTU_MAX_TIMEOUT							50


static void lwModbusTask(void *pArg);

static SFIFO rxFifo;

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

void UARTIntHandler(void)
{
    unsigned long ulStatus;
    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART1_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART1_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART1_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
				FIFO_Push(&rxFifo, UARTCharGetNonBlocking(UART1_BASE));
    }
}

static void rtu_init(void)
{
	user_data_t userdata;
  get_user_data(&userdata);
	//UARTprintf("[===RTU INIT====] \r\nBAURATE: %d\r\n", userdata.baudrate);
	switch(userdata.databits)
	{
		case UART_CONFIG_WLEN_5:
			//UARTprintf("DATABITS: 5\r\n");
			break;
		case UART_CONFIG_WLEN_6:
			//UARTprintf("DATABITS: 6\r\n");
			break;
		case UART_CONFIG_WLEN_7:
			//UARTprintf("DATABITS: 7\r\n");
			break;
		case UART_CONFIG_WLEN_8:
			//UARTprintf("DATABITS: 8\r\n");
			break;
		default:
			//UARTprintf("DATABITS: unknow\r\n");
			break;
	}
	
	switch(userdata.parity)
	{
		case UART_CONFIG_PAR_NONE:
			//UARTprintf("PARITY: NONE\r\n");
			break;
		case UART_CONFIG_PAR_ODD:
			//UARTprintf("PARITY: ODD\r\n");
			break;
		case UART_CONFIG_PAR_EVEN:
			//UARTprintf("PARITY: EVEN\r\n");
			break;
		default:
			//UARTprintf("PARITY: UNKNOW\r\n");
			break;
	}
	
	switch(userdata.stopbits)
	{
		case UART_CONFIG_STOP_ONE:
			//UARTprintf("STOPBITS: 1\r\n");
			break;
		case UART_CONFIG_STOP_TWO:
			//UARTprintf("STOPBITS: 2\r\n");
			break;
		default:
			//UARTprintf("STOPBITS: UNKNOW\r\n");
			break;
	}
	//UARTprintf("[===============]\r\n");
	FIFO_Create(&rxFifo);
	//
	// Enable the peripherals used by this example.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	//
	// Configure the UART for 115,200, 8-N-1 operation.
	//
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), userdata.baudrate,
											(userdata.databits | userdata.stopbits |
											 userdata.parity));
	//
	// Set GPIO A0 and A1 as UART.
	//
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	
	//
	// Init UART Fifo
	//
	UARTFIFOEnable(UART1_BASE);
	
	//
	// Enable the UART interrupt.
	//
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

static void rtu_flush(void)
{
	FIFO_Reset(&rxFifo);
	return;
}
int rtu_recv(uint8_t *rsp, int rsp_length,uint16_t timeout)
{
	int rc;
	uint16_t counter = 0;
	
	while(counter <  timeout)
	{
		//if(isRecving == true) 
			//counter = 0;
		if(FIFO_GetCount(&rxFifo) > 0)
			break;
		counter += 10;
		vTaskDelay(10);
	}
	vTaskDelay(50);
	rc = FIFO_Recv(&rxFifo, rsp, rsp_length);
	return rc;
}
void rtu_send(const uint8_t *req, int req_length)
{
	int len = req_length;
	/*
	*TODO: add RTU send togge
	*/
  LED_MODBUS_ON;
	MODBUS_DIR_SEND;
	vTaskDelay(5);
	while(len--)
	{
			//
			// Write the next character to the UART.
			//
			UARTCharPut(UART1_BASE, *req++);
	}
	vTaskDelay(20);
	MODBUS_DIR_RECV;
	LED_MODBUS_OFF;
	return;
}
ModbusError registerCallback(
	const ModbusSlave *slave,
	const ModbusRegisterCallbackArgs *args,
	ModbusRegisterCallbackResult *result)
{
#if MODBUS_PROTOCOL_DEBUG
	UARTprintf(
		"Register query:\n"
		"\tquery: %s\n"
		"\t type: %s\n"
		"\t   id: %d\n"
		"\tvalue: %d\n"
		"\t  fun: %d\n",
		modbusRegisterQueryStr(args->query),
		modbusDataTypeStr(args->type),
		args->index,
		args->value,
		args->function
	);

	switch (args->query)
	{
		// Pretend to allow all access
		case MODBUS_REGQ_R_CHECK:
		case MODBUS_REGQ_W_CHECK:
			result->exceptionCode = MODBUS_EXCEP_NONE;
			break;

		// Return 7 when reading
		case MODBUS_REGQ_R:
			result->value = 7;
			break;
		
		default: break;
	}
#endif
	return MODBUS_OK;
}

ModbusError exceptionCallback(const ModbusSlave *slave,  uint8_t function, ModbusExceptionCode code)
{
#ifdef MODBUS_PROTOCOL_DEBUG
	UARTprintf("Slave exception %s (function %d)\n", modbusExceptionCodeStr(code), function);
#endif
	return MODBUS_OK;
}

void printErrorInfo(ModbusErrorInfo err)
{
	if (modbusIsOk(err))
		UARTprintf("OK\r\n");
	else
		UARTprintf("%s: %s\r\n",
			modbusErrorSourceStr(modbusGetErrorSource(err)),
			modbusErrorStr(modbusGetErrorCode(err)));
}

void printResponse(const ModbusSlave *slave)
{
	//for (int i = 0; i < modbusSlaveGetResponseLength(slave); i++)
		//UARTprintf("0x%02x ", modbusSlaveGetResponse(slave)[i]);
}

static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (uint16_t)(crc_hi << 8 | crc_lo);
}

int rtu_send_msg_pre(uint8_t *req, int req_length)
{
    uint16_t crc = crc16(req, (uint16_t)req_length);
    req[req_length++] = crc >> 8;
    req[req_length++] = crc & 0x00FF;

    return req_length;
}

ModbusError myAllocator(ModbusBuffer *buffer, uint16_t size, void *context)
{
	// Make sure to handle the case when *ptr = NULL and size = 0
	// We don't want to allocate any memory then, but realloc(NULL, 0) would
	// result in malloc(0)
#ifdef DYNAMIC_MALLOC
	if (!size)
	{
		vPortFree(buffer->data);
		buffer->data = NULL;
	}
	else
	{
		uint8_t *old_ptr = buffer->data;
		buffer->data = (uint8_t*)pvPortMalloc(size);
		
		if (!buffer->data)
		{
			vPortFree(old_ptr);
			return MODBUS_ERROR_ALLOC;
		}
	}

	return MODBUS_OK;
#else
	// Array for holding the response frame
	static uint8_t response[MAX_RESPONSE];

	if (size != 0) // Allocation reqest
	{
		if (size <= MAX_RESPONSE) // Allocation request is within bounds
		{
			buffer->data = response;
			return MODBUS_OK;
		}
		else // Allocation error
		{
			buffer->data = NULL;
			return MODBUS_ERROR_ALLOC;
		}
	}
	else // Free request
	{
		buffer->data = NULL;
		return MODBUS_OK;
	}
#endif
}

/*
	Data callback for printing all incoming data
*/
ModbusError dataCallback(const ModbusMaster *master, const ModbusDataCallbackArgs *args)
{
#ifdef MODBUS_PROTOCOL_DEBUG
	UARTprintf(
		"Received data:\n"
		"\t from: %d\n"
		"\t  fun: %d\n"
		"\t type: %s\n"
		"\t   id: %d\n"
		"\tvalue: %d\n",
		args->address,
		args->function,
		modbusDataTypeStr(args->type),
		args->index,
		args->value
	);
#endif
	// Always return MODBUS_OK
	return MODBUS_OK;
}
ModbusError masterExceptionCallback(const ModbusMaster *master, uint8_t address, uint8_t function, ModbusExceptionCode code)
{
#ifdef MODBUS_PROTOCOL_DEBUG
	UARTprintf("Received slave %d exception %s (function %d)\n", address, modbusExceptionCodeStr(code), function);
#endif
	// Always return MODBUS_OK
	return MODBUS_OK;
}
void modbus_init(void)
{
		xTaskHandle xHandle;
		xTaskCreate(lwModbusTask, ( signed portCHAR * )"ModbusTask", TASK_LWIP_MODBUS_STACKSIZE, 
				NULL, TASK_LWIP_MODBUS_PRIORITY, &xHandle);	
}

static void lwModbusTask(void *pArg)
{
	struct sockaddr_in servaddr;
	fd_set refset;
  fd_set rdset;
	fd_set fderror;
	int fdmax;
	int master_socket;
	int rc;
	int rc_len = 0;
	int listenfd = 0;
	int i = 0;
	int client_sockets[MAX_CLIENT_SOCKET];
	int enable = 0;
  tBoolean enableWatchdog = pdFALSE;
	uint8_t *tcp_query = NULL;
	uint8_t *rtu_buffer = NULL;
	LWIP_UNUSED_ARG(pArg);
	struct timeval tv; //used for timeout interval
	ModbusErrorInfo err;
	ModbusSlave slave;
	ModbusMaster master;
	uint32_t feeddog_timeout = 0;
	uint32_t listen_count = 0;
	int rtu_timeout_count = 0;
	tcp_query = pvPortMalloc(MODBUS_TCP_MAX_ADU_LENGTH);
	if(tcp_query == NULL)
	{
		UARTprintf("fail alloc tcp_query memory \r\n");
		goto exit;
	}
	
	rtu_buffer = pvPortMalloc(MODBUS_RTU_MAX_ADU_LENGTH);
	if (rtu_buffer == NULL)
	{
		UARTprintf("fail alloc rtu_buffer memory \r\n");
		goto exit;
	}
	
	memset(&servaddr, 0, sizeof(servaddr));							/* set up address to connect to */
	servaddr.sin_len = sizeof(servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(502);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);				/* Set local IP address.*/

	listenfd = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);			
	if( listenfd < 0)
	{
		UARTprintf("create socket failed %d\r\n",listenfd);
		return;
	}

	enable = 1;
	if (lwip_setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&enable, sizeof(enable)) == -1) {
			lwip_close(listenfd);
			UARTprintf("set sock opt failed\r\n");
			return;
	}	
	
	rc = lwip_bind(listenfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	if( rc < 0)
	{
		UARTprintf("bind failed %d\r\n",rc);
		lwip_close(listenfd);
		return;
	}

	if(lwip_listen(listenfd, MAX_CLIENT_SOCKET) < 0)
	{
		UARTprintf("listenfd failed \r\n");
		lwip_close(listenfd);
		return;
	}
	err = modbusSlaveInit(
		&slave,
		registerCallback,
		exceptionCallback,
		myAllocator,
		modbusSlaveDefaultFunctions,
		modbusSlaveDefaultFunctionCount);
	//UARTprintf("Slave init: "); printErrorInfo(err); 
	if (!modbusIsOk(err))
	{
		goto exit;
	}
	
	//UARTprintf("RTU port init\r\n");
	rtu_init();
	MODBUS_DIR_RECV;
	rtu_flush();
		err = modbusMasterInit(
		&master,
		dataCallback,
		masterExceptionCallback,
		myAllocator,
		modbusMasterDefaultFunctions,
		modbusMasterDefaultFunctionCount);
	//UARTprintf("Master init: "); printErrorInfo(err);
	if (!modbusIsOk(err))
	{
		goto exit;
	}
	UARTprintf("Converter running");
	
	for(i = 0; i < MAX_CLIENT_SOCKET; i ++)
	{
		client_sockets[i] = 0;
	}
	/* Clear the reference set of socket */
	FD_ZERO(&refset);
	/* Add the server socket */
	FD_SET(listenfd, &refset);
 /* Keep track of the max file descriptor */
	fdmax = listenfd;
	for(;;)
	{			
				FD_ZERO(&rdset);
				rdset = refset;
				FD_ZERO(&fderror);
				FD_SET(fdmax, &fderror);
				tv.tv_sec = 5;
				tv.tv_usec = 0;
				if((xTaskGetTickCount() - feeddog_timeout) > WATCHDOG_FEED_TIMEOUT)
				{				
					WatchdogReloadSet(WATCHDOG0_BASE, WatchdogReloadGet(WATCHDOG0_BASE));
					feeddog_timeout = xTaskGetTickCount();					
					//UARTprintf("reload watchog at %d \r\n",feeddog_timeout);
				}
				rc = lwip_select(fdmax+1, &rdset, NULL, &fderror, &tv);
				
        if (rc < 0)
				{
          UARTprintf("Server select() failure \r\n");
					continue;
        }
				else if(rc == 0)
				{
						//UARTprintf("%u Free memory:%d \r\n", xTaskGetTickCount(), xPortGetFreeHeapSize()); // detect memleak
					if(fdmax >0)
					{
						for(i = 0; i < MAX_CLIENT_SOCKET; i++)
						{							
							if(client_sockets[i])
							{
								UARTprintf("Close socket #%d = %d \r\n",i, client_sockets[i]);
								lwip_close(client_sockets[i]);
								FD_CLR(client_sockets[i], &refset);
								client_sockets[i] = 0;
							}
						}
						fdmax = 0;
					}
					if( ++listen_count >= 2 && enableWatchdog == pdTRUE)
					{
						//UARTprintf("listen timeout, reset \r\n");
						vTaskDelay(500);
						do_reboot();
					}
				}
				else
				{
					listen_count = 0;
				}
				
				for (master_socket = 0; master_socket <= fdmax; master_socket++) 
				{
						if (!FD_ISSET(master_socket, &rdset)) 
						{
                continue;
            }
						if (master_socket == listenfd) 
						{
                /* A client is asking a new connection */
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;

								/* Handle new connections */
								addrlen = sizeof(clientaddr);
								memset(&clientaddr, 0, sizeof(clientaddr));
								UARTprintf("modbus wating for new connection...\r\n");
								newfd = lwip_accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);
								if (newfd == -1) 
								{
										UARTprintf("Server accept error\r\n");
								} 
								else 
								{																			
										FD_SET(newfd, &refset);
										if (newfd > fdmax) {
												/* Keep track of the maximum */
												fdmax = newfd;
										}
										for(i = 0; i < MAX_CLIENT_SOCKET; i++)
										{
											if(client_sockets[i] == 0)
											{
												client_sockets[i] = newfd;
												break;
											}
										}	
										if(enableWatchdog == pdFALSE)
											enableWatchdog = pdTRUE;
										UARTprintf("Save connection from %s:%d on socket %d at #%d\n",										
													 inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd, i);																
								}
						} 
						else
						{
								memset(tcp_query, 0, MODBUS_TCP_MAX_ADU_LENGTH);
								rc = lwip_recv(master_socket, tcp_query, MODBUS_TCP_MAX_ADU_LENGTH, 0);
								if	(rc > 0) 
								{
#ifdef CVT_DEBUG									
									UARTprintf("TCP REQ: [",rc);
									for(i = 0; i < rc; i++) 
									{
										UARTprintf(" %02x",tcp_query[i]);
									}
									UARTprintf(" ]\r\n");
#endif
									err = modbusParseRequestTCP(&slave, tcp_query, rc);
#ifdef MODBUS_PROTOCOL_DEBUG
									printErrorInfo(err);		
#endif									
									if (modbusIsOk(err)) //frame ok, send to RTU
									{
										rtu_flush();
										rc = rtu_send_msg_pre(&tcp_query[MODBUS_TCP_HEADER_LENGHT], rc - MODBUS_TCP_HEADER_LENGHT);
										rtu_send(tcp_query + MODBUS_TCP_HEADER_LENGHT, rc);
										memset(rtu_buffer, 0, MODBUS_RTU_MAX_ADU_LENGTH);
										rc_len = rtu_recv(rtu_buffer, MODBUS_RTU_MAX_ADU_LENGTH, 700); // TODO: need to change to dynamic timeout
#ifdef CVT_DEBUG	
										UARTprintf("RTU RESP:<", rc_len);	
										for(i = 0; i < rc_len; i++) 
										{
											UARTprintf(" %02x", rtu_buffer[i]);
										}
										UARTprintf(" >\r\n");
#endif
										// Let the master parse the response
										// Parse response
										err = modbusParseResponseRTU(
											&master,
											tcp_query + MODBUS_TCP_HEADER_LENGHT,
											rc,
											rtu_buffer,
											rc_len);
										//UARTprintf("Parse response: "); printErrorInfo(err);
										//if (modbusIsOk(err))
										//if (err.error != MODBUS_ERROR_LENGTH && err.error != MODBUS_ERROR_CRC)
										if(rc_len > 0)
										{						
											rtu_timeout_count = 0; // reset rtu timeout
											for(i = 0;i <  rc_len - MODBUS_RTU_CRC_LENGHT ; i++) 
											{
												tcp_query[i + MODBUS_TCP_HEADER_LENGHT] = rtu_buffer[i];
											}  									
										}
										else
										{
											if(rtu_timeout_count++ > RTU_MAX_TIMEOUT)
											{
												//UARTprintf("RTU reach max timeout, reboot\r\n");
												vTaskDelay(500);
												do_reboot();
											}
											//UARTprintf("RTU timeout %d\r\n", rtu_timeout_count);
											continue;
										}					
										
										rc_len= rc_len + MODBUS_TCP_HEADER_LENGHT - MODBUS_RTU_CRC_LENGHT;
										rc = lwip_send(master_socket, tcp_query, rc_len, MSG_NOSIGNAL);
										if (rc < 0)
										{
											//UARTprintf("send failed %d\r\n",rc);
										}
									}						
								}
								else
								{										
										lwip_close(master_socket);
										for( i = 0; i < MAX_CLIENT_SOCKET; i++)
										{
											if(client_sockets[i] == master_socket)
											{
												client_sockets[i] = 0;
												break;
											}
										}
										//UARTprintf("Connection closed on socket %d at #%d\n", master_socket, i);
                    /* Remove from reference set */
                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax) {
                        fdmax--;
                    }
								}
						}
				}
	}
	exit:
		return;
}


