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
    modbus_t *ctx_tcp;
    uint8_t *query_tcp;
		modbus_mapping_t *mb_mapping;
    int rc;
    int rc_tcp;
    int i;
		fd_set refset;
		fd_set rdset;
    int lcd;
    char buffer[10];
    int fdmax;
		int header_length;
	
    ctx_tcp = modbus_new_tcp("0.0.0.0", 502);
		modbus_set_debug(ctx_tcp, TRUE);
		if(ctx_tcp == NULL) 
		{
			UARTprintf("fail to create tcp port \r\n");
			while(1){};
		}
		mb_mapping = modbus_mapping_new(
		MODBUS_MAX_READ_BITS,
		MODBUS_MAX_READ_REGISTERS,
		MODBUS_MAX_READ_REGISTERS,
			MODBUS_MAX_READ_REGISTERS);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx_tcp);
        return;
    }
    query_tcp = malloc(MODBUS_TCP_MAX_ADU_LENGTH);

    s = modbus_tcp_listen(ctx_tcp, 5);
		if (s == -1) {
        UARTprintf("Unable to listen TCP connection\n");
        modbus_free(ctx_tcp);
        return;
    } else {
    	UARTprintf("TCP listen on socket %d\r\n", s);
    }
     /* Clear the reference set of socket */
    FD_ZERO(&refset);
    /* Add the server socket */
    FD_SET(s, &refset);
		fdmax = s;
		UARTprintf("ModbusTask is listening \r\n");
		header_length = modbus_get_header_length(ctx_tcp);
		UARTprintf("hlen:%d \r\n", header_length);
		for (;;) {
        rdset = refset;
        if (lwip_select(fdmax+1, &rdset, NULL, NULL, NULL) == -1) {
            perror("Server select() failure.");
            printf("Server select() failure.\r\n");
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
                rc = modbus_receive(ctx_tcp, query_tcp);
                if (rc > 0) {
                	//UARTprintf("recv at socket %d:", rc);
                	//for(int i = 0; i < rc; i++) {
                	//	UARTprintf("<%02X>", query_tcp[i]);
                	//}
                	//UARTprintf("\r\n");
									   if (query_tcp[header_length] == 0x03) {
											/* Read holding registers */
					
											if (MODBUS_GET_INT16_FROM_INT8(query_tcp, header_length + 3)
													== UT_REGISTERS_NB_SPECIAL) {
													printf("Set an incorrect number of values\n");
													MODBUS_SET_INT16_TO_INT8(query_tcp, header_length + 3,
																										UT_REGISTERS_NB_SPECIAL - 1);
											} else if (MODBUS_GET_INT16_FROM_INT8(query_tcp, header_length + 1)
																	== UT_REGISTERS_ADDRESS_SPECIAL) {
													printf("Reply to this special register address by an exception\n");
													modbus_reply_exception(ctx_tcp, query_tcp,
																									MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY);
													continue;
											} else if (MODBUS_GET_INT16_FROM_INT8(query_tcp, header_length + 1)
																	== UT_REGISTERS_ADDRESS_INVALID_TID_OR_SLAVE) {
													const int RAW_REQ_LENGTH = 5;
													uint8_t raw_req[] = {
															0xFF,
															0x03,
															0x02, 0x00, 0x00
													};
					
													printf("Reply with an invalid TID or slave\n");
													modbus_send_raw_request(ctx_tcp, raw_req, RAW_REQ_LENGTH * sizeof(uint8_t));
													continue;
											} else if (MODBUS_GET_INT16_FROM_INT8(query_tcp, header_length + 1)
																	== UT_REGISTERS_ADDRESS_SLEEP_500_MS) {
													printf("Sleep 0.5 s before replying\n");
													vTaskDelay(500);
											} else if (MODBUS_GET_INT16_FROM_INT8(query_tcp, header_length + 1)
																	== UT_REGISTERS_ADDRESS_BYTE_SLEEP_5_MS) {
													/* Test low level only available in TCP mode */
													/* Catch the reply and send reply byte a byte */
													uint8_t req[] = "\x00\x1C\x00\x00\x00\x05\xFF\x03\x02\x00\x00";
													int req_length = 11;
													int w_s = modbus_get_socket(ctx_tcp);
													if (w_s == -1) {
															fprintf(stderr, "Unable to get a valid socket in special test\n");
															continue;
													}
					
													/* Copy TID */
													req[1] = query_tcp[1];
													for (i=0; i < req_length; i++) {
															printf("(%.2X)", req[i]);
															vTaskDelay(5);
															rc = lwip_send(w_s, (const char*)(req + i), 1, MSG_NOSIGNAL);
															if (rc == -1) {
																	break;
															}
													}
													continue;
											}
									}
										rc = modbus_reply(ctx_tcp, query_tcp, rc, mb_mapping);
										 if (rc == -1) {
												 break;
										 } 
                } else if (rc == -1) {
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
}
