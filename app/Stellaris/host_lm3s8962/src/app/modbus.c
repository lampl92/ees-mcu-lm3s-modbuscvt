// Include configuration before implementation
#define LIGHTMODBUS_IMPL
#define LIGHTMODBUS_FULL
#define LIGHTMODBUS_DEBUG
#include "includes.h"
#include <lightmodbus.h>

#define PRINTF(...) UARTprintf(__VA_ARGS__);

#ifndef MAX_RESPONSE
#define MAX_RESPONSE 64
#endif
/*
	A register callback that prints out everything what's happening
*/
void printBytes(const uint8_t *data, int length)
{
	for (int i = 0; i < length; i++)
		PRINTF("0x%02x ", data[i]);
}

/*
	Helper function for printing out ModbusErrorInfo
*/
void printErrorInfo(ModbusErrorInfo err)
{
	if (modbusIsOk(err)) 
	{
		PRINTF("OK");
	}
	else
	{
		PRINTF("%s: %s\r\n",modbusErrorSourceStr(modbusGetErrorSource(err)),modbusErrorStr(modbusGetErrorCode(err)));
	}
	return;
}

ModbusError registerCallback(
	const ModbusSlave *slave,
	const ModbusRegisterCallbackArgs *args,
	ModbusRegisterCallbackResult *result)
{
	// Use functions from debug utilities to nicely
	// convert enum values to strings
	PRINTF(
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
		// Pretend we allow all access
		// Tip: Change MODBUS_EXCEP_NONE to something else
		// 		and see what happens
		case MODBUS_REGQ_R_CHECK:
		case MODBUS_REGQ_W_CHECK:
			result->exceptionCode = MODBUS_EXCEP_NONE;
			break;

		// Return 7 when reading
		case MODBUS_REGQ_R:
			result->value = 7;
			break;
		
		// Ignore write requests
		case MODBUS_REGQ_W:
			break;
	}

	// Always return MODBUS_OK
	return MODBUS_OK;
}
ModbusError dataCallback(const ModbusMaster *master, const ModbusDataCallbackArgs *args)
{
	char typechar = '?';
	switch (args->type)
	{
		case MODBUS_HOLDING_REGISTER: typechar = 'R'; break;
		case MODBUS_INPUT_REGISTER: typechar = 'I'; break;
		case MODBUS_COIL: typechar = 'C'; break;
		case MODBUS_DISCRETE_INPUT: typechar = 'D'; break;
	}
	PRINTF(
		"F: %03d, T: %c, ID: %03d, VAL: 0x%04x (%d)\n",
		args->function,
		typechar,
		args->index,
		args->value,
		args->value);
	return MODBUS_OK;
}

ModbusError exceptionCallback(const ModbusMaster *master, uint8_t address, uint8_t function, ModbusExceptionCode code)
{
	PRINTF(
		"EXCEPTION SLAVE: %03d, F: %03d, CODE: %03d (%s)\n",
		address,
		function,
		(int) code,
		modbusExceptionCodeStr(code));
	return MODBUS_OK;
}


/*
	A custom allocator. Returns memory
	from a statically allocated array.
*/

ModbusError staticAllocator( ModbusBuffer *buffer, uint16_t size, void *context)
{
	static uint8_t response[MAX_RESPONSE];
	// Array for holding the response frame
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
}

/*
	Exception callback for printing out exceptions
*/
ModbusError slaveExceptionCallback(const ModbusSlave *slave, uint8_t function, ModbusExceptionCode code)
{
	PRINTF("Slave exception %s (function %d)\n", modbusExceptionCodeStr(code), function);
	
	// Always return MODBUS_OK
	return MODBUS_OK;
}


void modbus_task(void *arg)
{
	static uint8 tcp_query[256];
	LWIP_UNUSED_ARG(arg);
	//	int ret;
	struct sockaddr_in servaddr;
//	struct timeval tv;
	fd_set refset;
  fd_set rdset;
	int fdmax;
	int master_socket;
	int rc;
	int enable;
	int listenfd = 0;
	ModbusSlave slave;
	ModbusErrorInfo err;
	
	LWIP_UNUSED_ARG(arg);

	memset(&servaddr, 0, sizeof(servaddr));							/* set up address to connect to */
	servaddr.sin_len = sizeof(servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SOCK_HOSR_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);				/* Set local IP address.*/

	listenfd = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);						
	enable = 1;
	if (lwip_setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&enable, sizeof(enable)) == -1) {
			lwip_close(listenfd);
			UARTprintf("set sock opt failed\r\n");
			return;
	}									
	if(lwip_bind(listenfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0)
	{
		UARTprintf("bind failed \r\n");
		close(listenfd);
		return;
	}

	if(lwip_listen(listenfd, 3) < 0)
	{
		UARTprintf("listenfd failed \r\n");
		close(listenfd);
		return;
	}

	PRINTF("Accepting connections ...\r\n");
	/* Clear the reference set of socket */
	FD_ZERO(&refset);
	/* Add the server socket */
	FD_SET(listenfd, &refset);
 /* Keep track of the max file descriptor */
	fdmax = listenfd;
	
	//modbus init
	
	// Init slave
	err = modbusSlaveInit(
		&slave,
		registerCallback,
		slaveExceptionCallback,
		modbusDefaultAllocator,
		modbusSlaveDefaultFunctions,
		modbusSlaveDefaultFunctionCount);
	PRINTF("Master init: "); printErrorInfo(err); PRINTF("\n");
	ASSERT(modbusIsOk(err) && "modbusSlaveInit() failed!");
	
	for(;;)
	{			
				FD_ZERO(&rdset);
				rdset = refset;
        if (select(fdmax+1, &rdset, NULL, NULL, NULL) < 0) 
				{
            PRINTF("Server select() failure.\r\n");
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
                PRINTF("modbus wating for new connection...\r\n");
                newfd = lwip_accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1) {
                    PRINTF("Server accept() error");
                } else {
                    FD_SET(newfd, &refset);

                    if (newfd > fdmax) {
                        /* Keep track of the maximum */
                        fdmax = newfd;
                    }
                    UARTprintf("New connection from %s:%d on socket %d\n",
                           inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                }
						} else
						{
								memset(tcp_query, 0, 256);
								rc = lwip_recv(master_socket, tcp_query, 256, 0);
								if	(rc > 0) 
								{
										UARTprintf("recv from sock %d: %s\r\n",master_socket, tcp_query);
										lwip_send(master_socket, tcp_query, rc, 0);
								}else
								{
										UARTprintf("Connection closed on socket %d\n", master_socket);
										close(master_socket);

                    /* Remove from reference set */
                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax) {
                        fdmax--;
                    }
								}
							
						}
				}
	}
}