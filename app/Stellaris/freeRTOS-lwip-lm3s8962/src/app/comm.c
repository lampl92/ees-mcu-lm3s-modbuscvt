#include "includes.h"
#include "comm.h"
#include "lwpkt.h"
#include "lwrb.h"
#include "lwip/inet.h"

#define MAX_PKT_LEN 20
#define RINGBUF_LEN 32
static lwpkt_t pkt;
static lwrb_t pkt_rx_rb, pkt_tx_rb;
static uint8_t pkt_rx_rb_data[RINGBUF_LEN];
static uint8_t pkt_tx_rb_data[RINGBUF_LEN];
static uint8_t tx_data[MAX_PKT_LEN];
static void my_lwpkt_evt_fn(lwpkt_t* pkt, lwpkt_evt_type_t type);
static void comm_parser(lwpkt_t* pkt);

tBoolean ClientHold;

tBoolean getClientState(void) {
	return ClientHold;
}
void UART0IntHandler(void)
{
    unsigned long ulStatus;
		uint8_t usb_data;
    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(CMD_UART_PORT, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(CMD_UART_PORT, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(COMM_UART_PORT))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
				usb_data = UARTCharGetNonBlocking(COMM_UART_PORT);
				lwrb_write(&pkt_rx_rb, &usb_data, 1);
		}
}

void comm_task(void *pArgs) {
	uint32_t comm_cnt = 0;
	for(;;) {
		vTaskDelay(2);
		comm_cnt += 2;
		lwpkt_process(&pkt, comm_cnt);
	}
}

void comm_init(void) {
	xTaskHandle xHandle;
	lwrb_init(&pkt_rx_rb, pkt_rx_rb_data, RINGBUF_LEN);
	lwrb_init(&pkt_tx_rb, pkt_tx_rb_data, RINGBUF_LEN);
	lwpkt_init(&pkt, &pkt_tx_rb, &pkt_rx_rb);
	/* Set callback function */
	lwpkt_set_evt_fn(&pkt, my_lwpkt_evt_fn);
	
	//
	// Enable the UART interrupt.
	//
	IntEnable(CMD_UART_PORT_INT);
	UARTIntEnable(CMD_UART_PORT, UART_INT_RX | UART_INT_RT);
	ClientHold = false;
	xTaskCreate(comm_task, ( signed portCHAR * )"comm_task", 512, NULL, TASK_USER_CMD_PRIORITY, &xHandle);	
}

static void my_lwpkt_evt_fn(lwpkt_t* pkt, lwpkt_evt_type_t type) {
	    int len = 0, i = 0;
    switch (type) {
        case LWPKT_EVT_PKT: {
            comm_parser(pkt);
            break;
        }
        case LWPKT_EVT_WRITE: {
            len = lwrb_read(&pkt_tx_rb,tx_data, MAX_PKT_LEN);
            if(len > 0)
            {
                for(i = 0; i < len; i ++)
                {
                    //XMC_UART_CH_Transmit(UART_USB.channel, (const uint16_t)(0x00FF & tx_data[i]));
										UARTCharPut(CMD_UART_PORT, tx_data[i]);
                }
            }
            break;
        }
        case LWPKT_EVT_READ: {
            break;
        }
        case LWPKT_EVT_TIMEOUT: {
            break;
        }
    }
}
static void comm_parser(lwpkt_t* pkt) {
	uint8_t *rxData;
	//lwpkt_write(pkt, lwpkt_get_cmd(pkt), lwpkt_get_data(pkt), lwpkt_get_data_len(pkt));
	user_data_t user_data;
  get_user_data(&user_data);
	rxData = lwpkt_get_data(pkt);
	switch(lwpkt_get_cmd(pkt)) {
		case OpcodeSetMac: {
			if(lwpkt_get_data_len(pkt) != 6)
				tx_data[0] = RetLength;
			else {
				user_data.mac[0] = rxData[0];
				user_data.mac[1] = rxData[1];
				user_data.mac[2] = rxData[2];
				user_data.mac[3] = rxData[3];
				user_data.mac[4] = rxData[4];
				user_data.mac[5] = rxData[5];
			  if(set_user_data(&user_data) == 0)
					tx_data[0] = RetOK;
				else
					tx_data[0] = RetFail;
			}
			lwpkt_write(pkt, OpcodeSetMac, &tx_data, 1);
		}
			break;
			
		case OpcodeGetMac: {
			tx_data[0] = user_data.mac[0];
			tx_data[1] = user_data.mac[1];
			tx_data[2] = user_data.mac[2];
			tx_data[3] = user_data.mac[3];
			tx_data[4] = user_data.mac[4];
			tx_data[5] = user_data.mac[5];
			lwpkt_write(pkt, OpcodeGetMac, &tx_data, 6);
		}
			break;
		
		case OpcodeSetNet: {
			if(lwpkt_get_data_len(pkt) != 12)
				tx_data[0] = RetLength;
			else {
				user_data.ipaddr = (unsigned long)(rxData[3] << 24);
				user_data.ipaddr |= (unsigned long)(rxData[2] << 16);
				user_data.ipaddr |= (unsigned long)(rxData[1] << 8);
				user_data.ipaddr |= rxData[0];

				user_data.gateway = (unsigned long)(rxData[7] << 24);
				user_data.gateway |= (unsigned long)(rxData[6] << 16);
				user_data.gateway |= (unsigned long)(rxData[5] << 8);
				user_data.gateway |= rxData[4];
			
				user_data.netmask = (unsigned long)(rxData[11] << 24);
				user_data.netmask |= (unsigned long)(rxData[10] << 16);
				user_data.netmask |= (unsigned long)(rxData[9] << 8);
				user_data.netmask |= rxData[8];
				
			if(set_user_data(&user_data) == 0)
				tx_data[0] = RetOK;
			else
				tx_data[0] = RetFail;
			}
			lwpkt_write(pkt, OpcodeSetNet, &tx_data, 1);
		}
			break;
		
		case OpcodeGetNet: {
			tx_data[3] = (uint8_t)((user_data.ipaddr & 0xff000000) >> 24);
			tx_data[2] = (uint8_t)((user_data.ipaddr & 0x00ff0000) >> 16);
			tx_data[1] = (uint8_t)((user_data.ipaddr & 0x0000ff00) >> 8);
			tx_data[0] = (uint8_t)(user_data.ipaddr & 0x000000ff);
			
			tx_data[7] = (uint8_t)((user_data.gateway & 0xff000000) >> 24);
			tx_data[6] = (uint8_t)((user_data.gateway & 0x00ff0000) >> 16);
			tx_data[5] = (uint8_t)((user_data.gateway & 0x0000ff00) >> 8);
			tx_data[4] = (uint8_t)(user_data.gateway & 0x000000ff);
			
			tx_data[11] = (uint8_t)((user_data.netmask & 0xff000000) >> 24);
			tx_data[10] = (uint8_t)((user_data.netmask & 0x00ff0000) >> 16);
			tx_data[9] = (uint8_t)((user_data.netmask & 0x0000ff00) >> 8);
			tx_data[8] = (uint8_t)(user_data.netmask & 0x000000ff);
			
			lwpkt_write(pkt, OpcodeGetNet, &tx_data, 12);			
		}
			break;
		
		case OpcodeSetModbus: {
			if(lwpkt_get_data_len(pkt) != 9)
				tx_data[0] = RetLength;
			else {
				user_data.baudrate = (unsigned long)(rxData[0] << 24);
				user_data.baudrate |= (unsigned long)(rxData[1] << 16);
				user_data.baudrate |= (unsigned long)(rxData[2] << 8);
				user_data.baudrate |= rxData[3];
					
				user_data.databits = (uint32_t)(rxData[4] & 0x000000ff);
					
				user_data.parity = (uint32_t)(rxData[5] & 0x000000ff);
					
				user_data.stopbits = (uint32_t)(rxData[6] & 0x000000ff);
				
				user_data.port = (unsigned long)(rxData[7] << 8);
				user_data.port |= rxData[8];
				if(set_user_data(&user_data) == 0)
				tx_data[0] = RetOK;
			else
				tx_data[0] = RetFail;
			}
			lwpkt_write(pkt, OpcodeSetModbus, &tx_data, 1);
		}
			break;
		
		case OpcodeGetModbus: {
			tx_data[0] = (uint8_t)((user_data.baudrate & 0xff000000) >> 24);
			tx_data[1] = (uint8_t)((user_data.baudrate & 0x00ff0000) >> 16);
			tx_data[2] = (uint8_t)((user_data.baudrate & 0x0000ff00) >> 8);
			tx_data[3] = (uint8_t)(user_data.baudrate & 0x000000ff);
			tx_data[4] = (uint8_t)(user_data.databits & 0x000000ff);
			tx_data[5] = (uint8_t)(user_data.parity & 0x000000ff);
			tx_data[6] = (uint8_t)(user_data.stopbits & 0x000000ff);
			tx_data[7] = (uint8_t)((user_data.port & 0x0000ff00) >> 8);
			tx_data[8] = (uint8_t)((user_data.port & 0x000000ff));
			lwpkt_write(pkt, OpcodeGetModbus, &tx_data, 9);	
		}			
			break;
		
		case OpcodeSave: {
			tx_data[0] = RetOK;
			lwpkt_write(pkt, OpcodeSave, &tx_data, 1);			
			vTaskDelay(100);
			do_reboot();
		}
			break;
		
		case OpcodeReqPort: {
			if (getMasterState()) {
				tx_data[0] = RetFail;
			} else {
				ClientHold = true;
				tx_data[0] = RetOK;
			}
			lwpkt_write(pkt, OpcodeReqPort, &tx_data, 1);	
		}
		break;		
		
		case OpCodeReleasePort: {
			tx_data[0] = RetOK;
			ClientHold = false;
			lwpkt_write(pkt, OpCodeReleasePort, &tx_data, 1);			
		}
		break;
		
		default:
			break;
	}
}
