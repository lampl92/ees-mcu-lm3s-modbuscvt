#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_
#define TASK_ETH_INT_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define TASK_START_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define TASK_LWIP_TCPIP_PRIORITY	( tskIDLE_PRIORITY + 4 )
//#define TASK_ETH_INT_PRIORITY			( tskIDLE_PRIORITY + 3 )
#define TASK_NET_SERVER_PRIORITY	( tskIDLE_PRIORITY + 5 )
#define TASK_LWIP_MODBUS_PRIORITY ( tskIDLE_PRIORITY + 6 )

#define TASK_ETH_INT_STACK_SIZE					512
#define TASK_NET_SERVER_STACK_SIZE			512
#define TASK_LWIP_TCPIP_STACKSIZE				1024
#define TASK_LWIP_MODBUS_STACKSIZE			2048

#define malloc pvPortMalloc
#define free vPortFree

#endif
