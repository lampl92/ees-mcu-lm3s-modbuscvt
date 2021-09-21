

/***************************** Include Files *********************************/
#include <FiFo.h>
#include "modbus.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/

/** @brief 
 *
 *  @param
 *  @return Void.
 *  @note
 */
void FIFO_Create(SFIFO *pFF) {
	pFF->u16Size = 0;
	pFF->u16Head = 0;
	pFF->u16Tail = 0;
	pFF->bEnProtect    = FALSE;
	pFF->u16ProtectPtr = 0;
	pFF->u16MaxSize    = SIZE_FIFO_RECV;
}

void FIFO_Destroy(SFIFO *pFF){
	//free(pFF->arrBuff);
}	
/** @brief 
 *
 *  @param
 *  @return Void.
 *  @note
 */
void FIFO_Reset(SFIFO *pFF) {
	pFF->u16Size 		= 0;
	pFF->u16Head 		= 0;
	pFF->u16Tail 		= 0;
	pFF->bEnProtect     = FALSE;
	pFF->u16ProtectPtr  = 0;
	return;
}

/** @brief
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t FIFO_Push(SFIFO *pFF, uint8_t b) {
	if(pFF->u16Size < pFF->u16MaxSize) {
		pFF->arrBuff[pFF->u16Tail] = b;
		pFF->u16Tail++;				
		if(pFF->u16Tail >= pFF->u16MaxSize) {
			pFF->u16Tail = 0;
		}
		pFF->u16Size++;

		return TRUE;
	}	
	else{
		return FALSE;
	}
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
uint8_t FIFO_Pop(SFIFO *pFF, uint8_t *b) {
	
	//UART_ENTER_CRITICAL();
	
	if(pFF->u16Size != 0) {
		*b = pFF->arrBuff[pFF->u16Head];
		pFF->u16Head++;
		if(pFF->u16Head >= pFF->u16MaxSize) {
			pFF->u16Head = 0;
		}
		pFF->u16Size--;

		//UART_EXIT_CRITICAL();
		
		return TRUE;
	}		

	*b = 0xFF;
	
	//UART_EXIT_CRITICAL();
	
	return FALSE;
}
/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
uint16_t FIFO_Recv(SFIFO *pFF, uint8_t *pData, uint16_t len) {

	uint16_t u16Size;
	uint16_t u16Idx;
	uint8_t u8Data;
	if(len > FIFO_GetCount(pFF)) {
		u16Size = FIFO_GetCount(pFF);
	} else {
		u16Size = len;
	}

	for(u16Idx = 0; u16Idx < u16Size; u16Idx++)  {
		FIFO_Pop(pFF, &u8Data);
		*pData++ = u8Data;
	}

	return u16Size;
}
/** @brief
 *
 *  @param
 *  @return
 *  @note
 */
void FIFO_EnableProtect(SFIFO *pFF) {
	//UART_ENTER_CRITICAL();
	pFF->bEnProtect  = TRUE;
	pFF->u16ProtectPtr = pFF->u16Head;
	//UART_EXIT_CRITICAL();
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
void FIFO_DisableProtect(SFIFO *pFF) {
	//UART_ENTER_CRITICAL();
	pFF->bEnProtect  = FALSE;
	//UART_EXIT_CRITICAL();
}
/** @brief FIFO_RewindHead
 *			Push back the FIFO some marked protected bytes.
 *			This function does not perform thread safe. User must take care of it.
 *  @param
 *  @return 
 *  @note
 */
 void FIFO_RewindHead(SFIFO *pFF) {
	//UART_ENTER_CRITICAL();
	if(pFF->bEnProtect) {
		while(pFF->u16Head != pFF->u16ProtectPtr) {
			if(pFF->u16Head == 0) {
				pFF->u16Head = pFF->u16MaxSize - 1;
			}
			else {
				pFF->u16Head--;
			}
			pFF->u16Size++;
			
			if(pFF->u16Head == pFF->u16Tail)
				break;
		}
	}	
	//UART_EXIT_CRITICAL();
}


/** @brief 	FIFO_IsEnablePush
 *			Check if possible to push more data into the FIFO
 *  @param
 *  @return 
 *  @note
 */
uint8_t FIFO_IsEnablePush(SFIFO *pFF) {
	if(  (pFF->u16Size == pFF->u16MaxSize) ||
	     (pFF->bEnProtect == TRUE && pFF->u16Size  != 0 && pFF->u16Tail  == pFF->u16ProtectPtr) 
	  ) {
		return FALSE;	    	
	}
	
	return TRUE;
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
uint8_t FIFO_IsEmpty(SFIFO *pFF) {
	return (pFF->u16Size == 0);
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
uint8_t FIFO_IsFull(SFIFO *pFF) {
	return (FIFO_IsEnablePush(pFF) ? FALSE : TRUE);
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
uint16_t FIFO_GetCount(SFIFO *pFF) {
	return (pFF->u16Size);
}

uint8_t FIFO_ProbeFirst(SFIFO *pFF, uint8_t *b) {
	if(pFF->u16Size > 0) {
		*b = pFF->arrBuff[pFF->u16Head];
		return TRUE;
	}
	return FALSE;
}

uint8_t FIFO_ProbeLast(SFIFO *pFF, uint8_t *b) {
	if(pFF->u16Size > 0) {
		*b = pFF->arrBuff[pFF->u16Tail];
		return TRUE;
	}
	return FALSE;
}


