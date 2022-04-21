#ifndef _COMM_H_
#define _COMM_H_
#include "includes.h"
void comm_init(void);
tBoolean getClientState(void);
#define COMM_UART_PORT				UART0_BASE
#define COMM_UART_PORT_INT	  INT_UART0
#define OpcodeSetMac    	0x01
#define OpcodeGetMac    	0x02
#define OpcodeSetNet    	0x03
#define OpcodeGetNet    	0x04
#define OpcodeSetModbus 	0x05
#define OpcodeGetModbus 	0x06
#define OpcodeSave      	0x07
#define OpcodeReqPort     0x08
#define OpCodeReleasePort 0x09

#define RetOK								0x00
#define RetFail							0x01
#define RetLength				    0x02

#endif
