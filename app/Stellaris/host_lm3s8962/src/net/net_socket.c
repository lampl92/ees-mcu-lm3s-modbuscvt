/* ------------------------------------------------------------------------------------------------------
 * File: net_socket.h
 * Data: 2012/11/4
 * Author: Stephen
 * Ver: V0.1.2a
 * -------------------------------------------------------------------------------------------------------
 */
#include <includes.h>


/* ------------------------------------------------------------------------------------------------------
 *											Local Define
 * ------------------------------------------------------------------------------------------------------
 */
#define RS232printf(...)			UARTprintf(__VA_ARGS__)
#define perror(x)

/* ------------------------------------------------------------------------------------------------------
 *											Local Define
 * ------------------------------------------------------------------------------------------------------
 */

void utilReverseBuf(uint8* pBuf, uint8 length)
{
   uint8 temp;
   uint8* pBufLast = (pBuf + length - 1);

   while(pBufLast > pBuf){
     temp = *pBuf;
     *pBuf++ = *pBufLast;
     *pBufLast-- = temp;	
  }
}

/* ------------------------------------------------------------------------------------------------------
 *									   sockex_nonblocking_connect()
 *
 * Description : Handing socket receive data.
 *
 * Argument(s) : none.
 *
 */
uint8 tcp_query[256];
void sockex_testrecv(void *arg)
{
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
	
	LWIP_UNUSED_ARG(arg);

	memset(&servaddr, 0, sizeof(servaddr));							/* set up address to connect to */
	servaddr.sin_len = sizeof(servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SOCK_HOSR_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);				/* Set local IP address.*/

	listenfd = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	UARTprintf("is that you \r\n");							
//	enable = 1;
//	if (lwip_setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
//                   (char *)&enable, sizeof(enable)) == -1) {
//			lwip_close(listenfd);
//			UARTprintf("set sock opt failed\r\n");
//			return;
//	}									
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

	RS232printf("Accepting connections ...\r\n");
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
        if (select(fdmax+1, &rdset, NULL, NULL, NULL) < 0) 
				{
            perror("Server select() failure.");
            RS232printf("Server select() failure.\r\n");
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
                RS232printf("modbus wating for new connection...\r\n");
                newfd = lwip_accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1) {
                    perror("Server accept() error");
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

/* ------------------------------------------------------------------------------------------------------
 *									      sockex_selects()
 *
 * Description : socket selects test.
 *
 * Argument(s) : none.
 *
 */
void sockex_selects(void *arg)
{
	int conn_amount;				/* current connection amount.*/
	int sock_fd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	int yes;
	INT8U buf[BUF_SIZE];
	int ret;
	int i;
	int fd_A[BACKLOG];				/* Accepted connection fd.*/
	
	fd_set fdsr;													/* Create file descriptor.*/
	int maxsock;
	struct timeval tv;
	
	conn_amount = 0;
	LWIP_UNUSED_ARG(arg);
	
	sock_fd = lwip_socket(AF_INET, SOCK_STREAM, 0);
	
	yes = 1;
	ret = lwip_setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(ret == -1)
	{
		return;
	}
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_len = sizeof(server_addr);
	server_addr.sin_port = PP_HTONS(SOCK_HOSR_PORT);
	server_addr.sin_addr.s_addr = lwIPLocalIPAddrGet();				/* IP_ADDR_ANY is '0.0.0.0'.*/
	
	lwip_bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	lwip_listen(sock_fd, BACKLOG + 1);								/* MAX TCP client is BACKLOG.*/
	
	sin_size = sizeof(client_addr);
	maxsock = sock_fd;
	
	UARTprintf("sockex_selects running \r\n");
	
	while(1)
	{
		FD_ZERO(&fdsr);												/* Initialize file descriptor set.*/
		FD_SET(sock_fd, &fdsr);
		UARTprintf("connection amount :%d \r\n", conn_amount);
		tv.tv_sec = 10;												/* Timeout setting.*/
		tv.tv_usec = 0;
		
		for (i = 0; i < BACKLOG; i++)								/* Add active connection to fd set.*/
		{								
			if (fd_A[i] != 0) {
				FD_SET(fd_A[i], &fdsr);
			}
		}
		
		ret = lwip_select(maxsock + 1, &fdsr, NULL, NULL, NULL);
		if(ret < 0)
		{
			break;
		}
		else if(ret == 0)
		{
			continue;
		}
		
		for (i = 0; i < conn_amount; i++)							/* Check every fd in the set.*/
		{
			if (FD_ISSET(fd_A[i], &fdsr))
			{
				int opt = 100;										/* set recv timeout (100 ms) */
				lwip_setsockopt(fd_A[i], SOL_SOCKET, SO_RCVTIMEO, &opt, sizeof(int));

				ret = lwip_recv(fd_A[i], buf, 50, 0);
				if (ret < 0)
				{
					UARTprintf("close socket : %d \r\n", fd_A[i]);
					lwip_close(fd_A[i]);
 					FD_CLR(fd_A[i], &fdsr);
 					fd_A[i] = 0;
					conn_amount--;
				}
				else        										/* receive data.*/
				{
					UARTprintf("recv from sock %d: %s\r\n",fd_A[i], buf);
										lwip_send(fd_A[i], buf, ret, 0);
				}
			}
		}

		if(FD_ISSET(sock_fd, &fdsr))								/* Check whether a new connection comes.*/
		{
			new_fd = lwip_accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
			if(new_fd <= 0)
			{
				UARTprintf("fail to accecpt new connection \r\n");
				continue;
			}
//			lwip_send(new_fd, "con", 4, 0);
			if(conn_amount < BACKLOG)								/* Add to fd queue.*/
			{
				UARTprintf("add new socket : %d \r\n", new_fd);
				fd_A[conn_amount++] = new_fd;
				
				if(new_fd > maxsock)
						maxsock = new_fd;
			}
			else
			{
//				conn_amount = 0;
				UARTprintf("delete socket at sock: %d \r\n", fd_A[conn_amount-1]);
				lwip_close(fd_A[conn_amount-1]);
				fd_A[conn_amount-1] = new_fd;
				if(new_fd > maxsock)
						maxsock = new_fd;
					//lwip_send(new_fd, "bye", 4, 0);
				//lwip_close(new_fd);									/* Close larger than 5 socket.*/
			}
		}
		
// 		for (i = 0; i < BACKLOG; i++)								/* Close other connections.*/
// 		{
// 			if (fd_A[i] != 0) {
// 				lwip_close(fd_A[i]);
// 			}
// 		}
	}
}
/* ------------------------------------------------------------------------------------------------------
 *									      socket_examples_init()
 *
 * Description : socket initialisation function.
 *
 * Argument(s) : none.
 *
 */
void TaskSocket_Create(void)
{
		//sys_thread_new("sockex_testrecv", sockex_testrecv, NULL, 128, 3);
		//sys_thread_new("sockex_selects", sockex_selects, NULL, 256, 2);
		sys_thread_new("modbus_task", modbus_task, NULL, 1024, 2);
}

