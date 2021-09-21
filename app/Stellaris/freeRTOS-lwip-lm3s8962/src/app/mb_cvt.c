#include <includes.h>
#include <signal.h>
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "modbus.h"
#include <errno.h>

const uint16_t UT_REGISTERS_NB_SPECIAL = 0x2;
const uint16_t UT_REGISTERS_ADDRESS_SLEEP_500_MS = 0x172;
const uint16_t UT_REGISTERS_ADDRESS_SPECIAL = 0x170;
const uint16_t UT_REGISTERS_ADDRESS_INVALID_TID_OR_SLAVE = 0x171;
const uint16_t UT_REGISTERS_ADDRESS_BYTE_SLEEP_5_MS = 0x173;

#define MODBUS_TCP_SLAVE_POSITION 6
#define MODBUS_TCP_HEADER_LENGHT 6
#define MODBUS_RTU_CRC_LENGHT 2

static void ModbusTask(void *pArg);
void modbus_init(void)
{
		xTaskHandle xHandle;
		xTaskCreate(ModbusTask, ( signed portCHAR * )"ModbusTask", TASK_LWIP_MODBUS_STACKSIZE, 
				NULL, TASK_LWIP_MODBUS_PRIORITY, &xHandle);	
}
//uint8_t rtu_query[256];
static void ModbusTask(void *pArg)
{
		UARTprintf("ModbusTask is running \r\n");
		int s = -1;
		int master_socket = -1;
    char ip[]="                ";
    modbus_t *ctx_tcp = NULL, *ctx_rtu = NULL;
    uint8_t *query_tcp, *query_rtu;
		modbus_mapping_t *mb_mapping_tcp;
    int rc;
    int rc_tcp;
    int i;
		fd_set refset;
		fd_set rdset;
    int fdmax;
		int header_length;
	
    ctx_tcp = modbus_new_tcp("0.0.0.0", 502);
		modbus_set_debug(ctx_tcp, TRUE);
		if(ctx_tcp == NULL) 
		{
			UARTprintf("fail to create tcp port \r\n");
			while(1){};
		}
#if 1
		mb_mapping_tcp = modbus_mapping_new(
		MODBUS_MAX_READ_BITS,
		MODBUS_MAX_READ_REGISTERS,
		MODBUS_MAX_READ_REGISTERS,
			MODBUS_MAX_READ_REGISTERS);
    if (mb_mapping_tcp == NULL) {
        UARTprintf("Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        goto exit;
    }
#endif
    query_tcp = malloc(MODBUS_TCP_MAX_ADU_LENGTH);
		query_rtu = malloc(MODBUS_RTU_MAX_ADU_LENGTH);
		
    s = modbus_tcp_listen(ctx_tcp, 1);
		if (s == -1) {
        UARTprintf("Unable to listen TCP connection\n");
        goto exit;
    } else {
    	UARTprintf("TCP listen on socket %d\r\n", s);
    }
		modbus_tcp_accept(ctx_tcp, &s);
		ctx_rtu = modbus_new_rtu("COM1", 115200, 'N', 8, 1);
    if(ctx_rtu != NULL) {
		UARTprintf("created modbus rtu\r\n");
		}
		modbus_set_debug(ctx_rtu, TRUE);
		if (modbus_connect(ctx_rtu) == -1) 
    {
        UARTprintf("Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx_rtu);
        return;
    }else
		{
			UARTprintf("RTU listenning\r\n");
		}
     /* Clear the reference set of socket */
    //FD_ZERO(&refset);
    /* Add the server socket */
    //FD_SET(s, &refset);
		//fdmax = s;
		UARTprintf("ModbusTask is listening \r\n");
#if 1
		for(;;)
		{
			do 
			{
					rc_tcp = modbus_receive(ctx_tcp, query_tcp);
			} 
			while (rc_tcp == 0);
			UARTprintf("free mem:%d \r\n", xPortGetFreeHeapSize());
			if (rc_tcp == -1 && errno != EMBBADCRC) 
			{
					break;
			}
			modbus_set_slave(ctx_rtu, query_tcp[MODBUS_TCP_SLAVE_POSITION]);
			modbus_send_raw_request(ctx_rtu, query_tcp + MODBUS_TCP_HEADER_LENGHT , (rc_tcp - MODBUS_TCP_HEADER_LENGHT) * sizeof(uint8_t));
			rc = modbus_receive_confirmation(ctx_rtu, query_rtu);  
			if (rc > 0) 
			{
				UARTprintf("RTU %d:", rc);
				for(i = 0;i <  rc - MODBUS_RTU_CRC_LENGHT ; i++) 
				{
						query_tcp[i + MODBUS_TCP_HEADER_LENGHT] = query_rtu[i];
						UARTprintf("[%02X]", query_rtu[i]);
				}  
				UARTprintf("\r\n");
			}else
			{
				UARTprintf("RTU error\r\n");
				continue;
			}					
			rc_tcp= rc + MODBUS_TCP_HEADER_LENGHT - MODBUS_RTU_CRC_LENGHT;

			int w_s = modbus_get_socket(ctx_tcp);
			int sent;	
			//vTaskDelay(5);
			sent = lwip_send(w_s, query_tcp, rc_tcp, 0);
			if (sent == -1) 
			{
					UARTprintf("tcp send back failed\r\n");
					continue;
			}           			
			//									rc = modbus_reply(ctx_tcp, query_tcp, rc, mb_mapping_tcp);
			//									if (rc == -1) 
			//									{
			//										 UARTprintf("TCP send back failed\r\n");
			//											continue;
			//									} 		
			UARTprintf("TCP send back completed\r\n");		
		}
#else
		for (;;) {
				FD_ZERO(&rdset);
        rdset = refset;
				UARTprintf("wait for new event\r\n");
        if (lwip_select(fdmax+1, &rdset, NULL, NULL, NULL) < 0) 
				{
            UARTprintf("Server select() failure.\r\n");
        }

        /* Run through the existing connections looking for data to be
         * read */
        for (master_socket = 0; master_socket <= fdmax; master_socket++) {

            if (!FD_ISSET(master_socket, &rdset)) {
                continue;
            }

            if (master_socket == s) {
                /* A client is asking a new connection */
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;

                /* Handle new connections */
                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
                UARTprintf("modbus wating for new connection...\r\n");
                newfd = lwip_accept(s, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1) {
                    UARTprintf("Server accept() error");
                } else {
                    FD_SET(newfd, &refset);

                    if (newfd > fdmax) {
                        /* Keep track of the maximum */
                        fdmax = newfd;
                    }
                    UARTprintf("New connection from %s:%d on socket %d\n",
                           inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                }
            } else {
                modbus_set_socket(ctx_tcp, master_socket);
                rc_tcp = modbus_receive(ctx_tcp, query_tcp);
                if (rc_tcp > 0) {
                	UARTprintf("TCP %d:", rc_tcp);
                	for(int i = 0; i < rc_tcp; i++) {
                		UARTprintf("<%02X>", query_tcp[i]);
                	}
                	UARTprintf("\r\n");

									modbus_set_slave(ctx_rtu, query_tcp[MODBUS_TCP_SLAVE_POSITION]);
									modbus_send_raw_request(ctx_rtu, query_tcp + MODBUS_TCP_HEADER_LENGHT , (rc_tcp - MODBUS_TCP_HEADER_LENGHT) * sizeof(uint8_t));
									rc = modbus_receive_confirmation(ctx_rtu, query_rtu);  
									if (rc > 0) 
									{
										UARTprintf("RTU %d:", rc);
										for(i = 0;i <  rc - MODBUS_RTU_CRC_LENGHT ; i++) 
										{
												query_tcp[i + MODBUS_TCP_HEADER_LENGHT] = query_rtu[i];
												UARTprintf("[%02X]", query_rtu[i]);
										}  
										UARTprintf("\r\n");
									}else
									{
										UARTprintf("RTU error\r\n");
										continue;
									}					
									rc_tcp= rc + MODBUS_TCP_HEADER_LENGHT - MODBUS_RTU_CRC_LENGHT;

									//int w_s = modbus_get_socket(ctx_tcp);
									int sent;	
									//vTaskDelay(5);
									sent = lwip_send(master_socket, query_tcp, rc_tcp, 0);
									if (sent == -1) 
									{
											UARTprintf("tcp send back failed\r\n");
											continue;
									}           			
//									rc = modbus_reply(ctx_tcp, query_tcp, rc, mb_mapping_tcp);
//									if (rc == -1) 
//									{
//										 UARTprintf("TCP send back failed\r\n");
//											continue;
//									} 		
									UARTprintf("TCP send back completed\r\n");										
                } else
								{
                    /* This example server in ended on connection closing or
                     * any errors. */
                    UARTprintf("Connection closed on socket %d\n", master_socket);
                    lwip_close(master_socket);

                    /* Remove from reference set */
                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax) {
                        fdmax--;
                    }
                }
            }
        }
    }
#endif
exit:
		if(ctx_rtu != NULL)
			modbus_free(ctx_rtu);
		if(ctx_tcp != NULL)
			modbus_free(ctx_tcp);
		return;
}
