/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:              lwiports.h
** Last modified Date:     2008.8.20
** Last Version:            v1.0
** Description:            Configuration file for lwIP
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              ?????
** Created date:            2008.8.20
** Version:                 v1.0
** Descriptions:            The original version ??'??
**
*********************************************************************************************************/

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/*********************************************************************************************************
**-----------------------------------------------------------
*********************************************************************************************************/
#define   SYS_LIGHTWEIGHT_PROT          (NO_SYS==0)     
                                               
                                               
#define   NO_SYS                        0      
                                               
#define LWIP_PROVIDE_ERRNO     			1
//#define LWIP_DEBUG
#define LWIP_SO_RCVTIMEO 						1
#define SO_REUSE										1
#define LWIP_IGMP                   0
#define LWIP_ICMP                   1
#define LWIP_SNMP                   1

/*********************************************************************************************************
**-------------------------------lwip 2.1.2 LAMPL PORTING----------------------------
*********************************************************************************************************/

#define LWIP_NO_STDINT_H								1
/*********************************************************************************************************
**-------------------------------Memory options----------------------------
*********************************************************************************************************/
#define MEM_LIBC_MALLOC                 0       
                                                
                                                

#define MEM_ALIGNMENT                   4       
                                                

#define MEM_SIZE                        (4*1024)
                                                

#define MEMP_OVERFLOW_CHECK             0       
                                                
                                                
                                                

#define MEMP_SANITY_CHECK               0       
                                                

/*********************************************************************************************************
**-------------------------------Internal Memory Pool Sizes---------------------------
*********************************************************************************************************/

#define MEMP_NUM_PBUF                   16    
                                              

#define MEMP_NUM_RAW_PCB                4     

#define MEMP_NUM_UDP_PCB                4     

#define MEMP_NUM_TCP_PCB                5     

#define MEMP_NUM_TCP_PCB_LISTEN         8     

#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN + 2 // LAMPL_SET , default 8

#define MEMP_NUM_REASSDATA              5   //LAMPL_SET  .default comment

#define MEMP_NUM_ARP_QUEUE              30  

//#define MEMP_NUM_IGMP_GROUP             8   

#define MEMP_NUM_SYS_TIMEOUT            8   

#define MEMP_NUM_NETBUF                 2   

#define MEMP_NUM_NETCONN                 4 //LAMPL_SET , default 10  


//#define MEMP_NUM_API_MSG                8 
                                            

//#define MEMP_NUM_TCPIP_MSG              8 
                                            
#define MEMP_NUM_TCPIP_MSG_API      			4
#define MEMP_NUM_TCPIP_MSG_INPKT    			4

/*********************************************************************************************************
**-------------------------------ARP options----------------------------
*********************************************************************************************************/
#define ARP_TABLE_SIZE                  10  

#define ARP_QUEUEING                    1 

//*****************************************************************************
//
// ---------- Pbuf options ----------
//
//*****************************************************************************
#define PBUF_POOL_SIZE                  32      // default is 16
#define PBUF_POOL_BUFSIZE               256     // default is 128
#define PBUF_LINK_HLEN                  16      // default is 14
#define ETH_PAD_SIZE                    2       // default is 0



/*********************************************************************************************************
**-------------------------------IP options----------------------------
*********************************************************************************************************/
//#define IP_FORWARD                      0
//#define IP_OPTIONS                      1
#define IP_REASSEMBLY                   0       // default is 1
#define IP_FRAG                         0       // default is 1


/*********************************************************************************************************
**-------------------------------ICMP options----------------------------
*********************************************************************************************************/
#define ICMP_TTL                        64


/*********************************************************************************************************
**-------------------------------RAW options----------------------------
*********************************************************************************************************/
//#define LWIP_RAW                        1
//#define RAW_TTL                        255

/*********************************************************************************************************
**-------------------------------DHCP options----------------------------
*********************************************************************************************************/
#define LWIP_DHCP                         0      // default is 0
//#define DHCP_DOES_ARP_CHECK             1
//#define DHCP_HOST_NAME                  "fury-dev"
#define LWIP_AUTOIP 											0
/*********************************************************************************************************
**-------------------------------UDP options----------------------------
*********************************************************************************************************/
#define LWIP_UDP                    			0
#define UDP_TTL                     			255
#define DEFAULT_UDP_RECVMBOX_SIZE   			8
/*********************************************************************************************************
**-------------------------------TCP options----------------------------
*********************************************************************************************************/
#define LWIP_TCP                        	1
#define TCP_TTL                        		255
#define TCP_WND                         	4096    // default is 2048
#define TCP_MAXRTX                      	12
#define TCP_SYNMAXRTX                   	6
#define TCP_QUEUE_OOSEQ                 	1
#define TCP_MSS                         	1500    // default is 128
#define TCP_SND_BUF                     	(6*TCP_MSS) // default is 256
//#define TCP_SND_QUEUELEN                (4 * TCP_SND_BUF/TCP_MSS)
//#define TCP_SNDLOWAT                    (TCP_SND_BUF/2)
//#define LWIP_HAVE_LOOPIF                0


/*********************************************************************************************************
**-------------------------------Task options----------------------------
*********************************************************************************************************/
#define TCPIP_THREAD_PRIO                TASK_LWIP_TCPIP_PRIORITY
//#define SLIPIF_THREAD_PRIO              1
//#define PPP_THREAD_PRIO                 1
//#define DEFAULT_THREAD_PRIO             1
#define TCPIP_MBOX_SIZE             			10
#define TCPIP_THREAD_STACKSIZE      			TASK_LWIP_TCPIP_STACKSIZE
#define TCPIP_THREAD_NAME           			"tcpip"
#define DEFAULT_TCP_RECVMBOX_SIZE   			10				// miss this will make create socket fail
/*********************************************************************************************************
**-------------------------------Sequential layer options----------------------------
*********************************************************************************************************/
#define LWIP_NETCONN                   		1


/*********************************************************************************************************
**-------------------------------Socket options----------------------------
*********************************************************************************************************/
#define LWIP_SOCKET                     	1
#define LWIP_COMPAT_SOCKETS             	0

/*********************************************************************************************************
**-------------------------------Statistics options----------------------------
*********************************************************************************************************/
//#define LWIP_STATS                      1
//#define LWIP_STATS_DISPLAY              0
//#define LINK_STATS                      1
//#define IP_STATS                        1
//#define IPFRAG_STATS                    1
//#define ICMP_STATS                      1
//#define UDP_STATS                       1
//#define TCP_STATS                       1
//#define MEM_STATS                       1
//#define MEMP_STATS                      1
//#define PBUF_STATS                      1
//#define SYS_STATS                       1
//#define RAW_STATS                       0


/*********************************************************************************************************
**-------------------------------PPP options----------------------------
*********************************************************************************************************/
//#define PPP_SUPPORT                     0
//#define PAP_SUPPORT                     0
//#define CHAP_SUPPORT                    0
//#define VJ_SUPPORT                      0
//#define MD5_SUPPORT                     0

/*********************************************************************************************************
**-------------------------------checksum options----------------------------
*********************************************************************************************************/
//#define CHECKSUM_GEN_IP                 1
//#define CHECKSUM_GEN_UDP                1
//#define CHECKSUM_GEN_TCP                1
//#define CHECKSUM_CHECK_IP               1
//#define CHECKSUM_CHECK_UDP              1
//#define CHECKSUM_CHECK_TCP              1
/*********************************************************************************************************
---------- RAW options ---------- IF MISS THIS, TCP LISTEN FAIL
*********************************************************************************************************/
#define DEFAULT_RAW_RECVMBOX_SIZE   			8
#define DEFAULT_ACCEPTMBOX_SIZE     			8
/*********************************************************************************************************
**-------------------------------Debugging options----------------------------
*********************************************************************************************************/
#ifdef LWIP_DEBUG
#define U8_F "c"
#define S8_F "c"
#define X8_F "02x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
extern void UARTprintf(const char *pcString, ...);
#define LWIP_PLATFORM_DIAG(msg) UARTprintf msg
#define LWIP_PLATFORM_ASSERT(msg) \
    UARTprintf("ASSERT_FAIL at line %d of %s: %s\n", __LINE__, __FILE__, msg)

#define DBG_TYPES_ON                    	LWIP_DBG_ON
//#define ETHARP_DEBUG                    LWIP_DBG_ON
//#define NETIF_DEBUG                     LWIP_DBG_ON
//#define PBUF_DEBUG                      LWIP_DBG_ON
//#define API_LIB_DEBUG                   LWIP_DBG_ON
//#define API_MSG_DEBUG                   LWIP_DBG_ON
#define SOCKETS_DEBUG                  		LWIP_DBG_ON
//#define ICMP_DEBUG                      DBG_OFF
//#define INET_DEBUG                      DBG_OFF
//#define IP_DEBUG                        DBG_OFF
//#define IP_REASS_DEBUG                  DBG_OFF
//#define RAW_DEBUG                       DBG_OFF
//#define MEM_DEBUG                       DBG_OFF
//#define MEMP_DEBUG                      DBG_OFF
//#define SYS_DEBUG                       DBG_OFF
//#define TCP_DEBUG                       LWIP_DBG_ON
//#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
//#define TCP_FR_DEBUG                    DBG_OFF
//#define TCP_RTO_DEBUG                   DBG_OFF
//#define TCP_REXMIT_DEBUG                DBG_OFF
//#define TCP_CWND_DEBUG                  DBG_OFF
//#define TCP_WND_DEBUG                   DBG_OFF
//#define TCP_OUTPUT_DEBUG                DBG_OFF
//#define TCP_RST_DEBUG                   DBG_OFF
//#define TCP_QLEN_DEBUG                  DBG_OFF
//#define UDP_DEBUG                       DBG_OFF
//#define TCPIP_DEBUG                     LWIP_DBG_ON
//#define PPP_DEBUG                       DBG_OFF
//#define SLIP_DEBUG                      DBG_OFF
//#define DHCP_DEBUG                      DBG_OFF
#define DBG_MIN_LEVEL                   	LWIP_DBG_LEVEL_ALL
#endif

#endif /* __LWIPOPTS_H__ */
