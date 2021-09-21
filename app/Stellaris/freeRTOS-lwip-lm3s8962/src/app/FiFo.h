
#ifndef _FIFOQUEUE_
#define _FIFOQUEUE_

/***************************** Include Files *********************************/
#include <stdint.h>

/************************** Constant Definitions *****************************/
#define SIZE_FIFO_RECV 512
/**************************** Type Definitions *******************************/


typedef struct _SFIFO {
	uint8_t 			arrBuff[SIZE_FIFO_RECV];
	uint16_t 		u16MaxSize;
	uint16_t		u16Size;
	uint16_t		u16Head;
	uint16_t		u16Tail;
	int			bEnProtect;
	uint16_t		u16ProtectPtr;
} SFIFO;


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

void FIFO_Create(SFIFO *pFF);
void FIFO_Destroy(SFIFO *pFF);
void FIFO_Reset(SFIFO *pFF);
uint8_t FIFO_Pop(SFIFO *pFF, uint8_t *b);
uint8_t FIFO_Push(SFIFO *pFF, uint8_t b);
uint16_t FIFO_Recv(SFIFO *pFF, uint8_t *pData, uint16_t len);
uint8_t FIFO_ProbeFirst(SFIFO *FF, uint8_t *b);
uint8_t FIFO_ProbeLast(SFIFO *FF, uint8_t *b);
uint8_t FIFO_Contain(SFIFO *pFF, uint8_t b);
void FIFO_EnableProtect(SFIFO *pFF);
void FIFO_DisableProtect(SFIFO *pFF);
void FIFO_RewindHead(SFIFO *pFF);
uint8_t FIFO_IsEnablePush(SFIFO *pFF);
uint8_t FIFO_IsEmpty(SFIFO *pFF);
uint8_t FIFO_IsFull(SFIFO *pFF);
uint16_t FIFO_GetCount(SFIFO *pFF);
/************************** Variable Definitions *****************************/

/*****************************************************************************/



#endif
