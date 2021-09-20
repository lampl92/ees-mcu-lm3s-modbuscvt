#ifndef __NET_SOCKET_H__
#define __NET_SOCKET_H__



/* ------------------------------------------------------------------------------------------------------
 *											Global Define
 * ------------------------------------------------------------------------------------------------------
 */
#ifndef SOCK_TARGET_HOST
#define SOCK_TARGET_HOST  "192.168.2.99"
#endif

#ifndef SOCK_TARGET_PORT
#define SOCK_TARGET_PORT  	8091
#define SOCK_HOSR_PORT		8090
#endif

#define BACKLOG				3
#define BUF_SIZE			50

#define SOCKET_NEW			1
#define SOCKET_CON			2
#define SOCKET_REC			3
#define SOCKET_CLS			4
#define SOCKET_CHECK		5
#define SOCKET_IDIE			6


/* ------------------------------------------------------------------------------------------------------
 *											Global Variable
 * ------------------------------------------------------------------------------------------------------
 */
extern int fd_A[BACKLOG];				/* Accepted connection fd.*/
extern int conn_amount;					/* current connection amount.*/


/* ------------------------------------------------------------------------------------------------------
 *											Global Function
 * ------------------------------------------------------------------------------------------------------
 */
void TaskSocket_Create(void);


#endif /* __SOCKET_EXAMPLES_H__ */
