#include <includes.h>
#include "lwip/inet.h"
unsigned char str[256];
void reboot_task(void *pArgs);
void reboot_start(int second);
static int SSIHandler(int iIndex, char *pcInsert, int iInsertLen);
void reboot_start(int second);
static const char *g_pcConfigSSITags[] =
{
    "baudtxt",        // SSI_INDEX_BAURATE
		"dbit",       
		"parity",       
		"sbits",
		"ipaddr",
		"gtway",
		"nmask",
};

#define SSI_INDEX_BAURATE  0
#define SSI_INDEX_DBIT		 1
#define SSI_INDEX_PARITY   2
#define SSI_INDEX_SBITS	   3
#define SSI_INDEX_IPADDR	 4
#define SSI_INDEX_GATEWAY  5
#define SSI_INDEX_NETMASK  6

#define NUM_CONFIG_SSI_TAGS     (sizeof(g_pcConfigSSITags) / sizeof (char *))

static char *LedCGIhandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const tCGI g_psConfigCGIURIs[] =
{
    { "/leds.cgi", LedCGIhandler },      // CGI_INDEX_CONTROL
};

#define NUM_CONFIG_CGI_URIS  (sizeof(g_psConfigCGIURIs) / sizeof(tCGI))

#define PARAM_ERROR_RESPONSE    "/404.htm"
#define DEFAULT_CGI_RESPONSE    "/index.shtml"
#define SUCCESS_RESPONSE        "/success.htm"
#define PARAM_FORMAT_RESPONSE    "/format_error.htm"
static char *LedCGIhandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{

		tBoolean bParamError = pdFALSE;
		long baudrate, databits, parity, stopbits;
		long lvalue;
		user_data_t userdata;
    get_user_data(&userdata);									
		
		baudrate = GetCGIParam("Baudrate", pcParam, pcValue, iNumParams, &bParamError);
		switch(baudrate)
		{
			case 115200:
			case 38400:
			case 9600:
				userdata.baudrate = baudrate;
			break;
			
			default:
				userdata.baudrate = 115200;
			break;
		}
		
		databits = GetCGIParam("Databits", pcParam, pcValue, iNumParams, &bParamError);
		switch(databits)
		{
			case 5:
				userdata.databits = UART_CONFIG_WLEN_5;
				break;
			
			case 6:
				userdata.databits = UART_CONFIG_WLEN_6;
				break;
			
			case 7:
				userdata.databits = UART_CONFIG_WLEN_7;
				break;
			
			case 8:
			default:
				userdata.databits = UART_CONFIG_WLEN_8;
			break;
		}
		
		parity = GetCGIParam("Parity", pcParam, pcValue, iNumParams, &bParamError);
		switch(parity)
		{
			case 1:
				userdata.parity = UART_CONFIG_PAR_ODD;
				break;
			
			case 2:
				userdata.parity = UART_CONFIG_PAR_EVEN;
				break;
			
			case 0:
			default:
				userdata.parity = UART_CONFIG_PAR_NONE;
				break;
		}
		
		stopbits = GetCGIParam("Stopbits", pcParam, pcValue, iNumParams, &bParamError);
		switch(stopbits)
		{
			case 2:
				userdata.stopbits = UART_CONFIG_STOP_TWO;
				break;
			
			case 1:
			default:
				userdata.stopbits = UART_CONFIG_STOP_ONE;
				break;
		}
		UARTprintf("UART config %d %d %d %d\r\n",baudrate, databits,parity,stopbits);
	 
		lvalue = FindCGIParameter("ipaddress", pcParam, iNumParams);
		UARTprintf("ipaddr: %s\r\n", pcValue[lvalue]);
		if(inet_aton(pcValue[lvalue], &userdata.ipaddr) == 0 || validate_ip(pcValue[lvalue]) == 0)
		{
			UARTprintf("IP convert failed \r\n");
			bParamError = pdTRUE;
			goto exit;
		}

		lvalue = FindCGIParameter("gateway", pcParam, iNumParams);
		UARTprintf("gateway: %s\r\n", pcValue[lvalue]);
		if(inet_aton(pcValue[lvalue], &userdata.gateway) == 0|| validate_ip(pcValue[lvalue]) == 0)
		{
			UARTprintf("Gateway convert failed \r\n");
			bParamError = pdTRUE;
			goto exit;
		}

		lvalue = FindCGIParameter("netmask", pcParam, iNumParams);
		UARTprintf("netmask: %s\r\n", pcValue[lvalue]);
		if(inet_aton(pcValue[lvalue], &userdata.netmask) == 0|| validate_ip(pcValue[lvalue]) == 0)
		{
			UARTprintf("Netmask convert failed \r\n");
			bParamError = pdTRUE;
			goto exit;
		}

    //
    // Send back the default response page.
    //
exit:
		if(bParamError == pdTRUE)
			return (PARAM_FORMAT_RESPONSE);
		else {
			set_user_data(&userdata);
			reboot_start(3);
			return(SUCCESS_RESPONSE);
		}
}

static int SSIHandler(int iIndex, char *pcInsert, int iInsertLen)
{
		user_data_t userdata;
    get_user_data(&userdata);
		switch(iIndex)
		{
			case SSI_INDEX_BAURATE:
				usnprintf(pcInsert, iInsertLen, BAUDRATE_STRING, userdata.baudrate == 115200 ? "selected":"",
																												userdata.baudrate == 38400 ? "selected":"",
																											 userdata.baudrate == 9600 ? "selected":"");
				break;
			
			case SSI_INDEX_DBIT:
				snprintf(pcInsert, iInsertLen, DATABITS_STRING, userdata.databits == UART_CONFIG_WLEN_5 ? "selected":"",
																												userdata.databits == UART_CONFIG_WLEN_6 ? "selected":"",
																												userdata.databits == UART_CONFIG_WLEN_7 ? "selected":"",
																											  userdata.databits == UART_CONFIG_WLEN_8 ? "selected":"");
				break;
			
			case SSI_INDEX_PARITY:
				snprintf(pcInsert, iInsertLen, PARITY_STRING, userdata.parity == UART_CONFIG_PAR_NONE ? "selected":"",
																											userdata.parity == UART_CONFIG_PAR_ODD ? "selected":"",
																											userdata.parity == UART_CONFIG_PAR_EVEN ? "selected":"");
				break;
			
			case SSI_INDEX_SBITS:
				snprintf(pcInsert, iInsertLen, STOPBIT_STRING, userdata.stopbits == UART_CONFIG_STOP_ONE ? "selected":"",
																											userdata.stopbits == UART_CONFIG_STOP_TWO ? "selected":"");
				break;
			case SSI_INDEX_IPADDR:
				snprintf(pcInsert, iInsertLen, IPADDR_STRING,inet_ntoa(userdata.ipaddr));
				break;
			case SSI_INDEX_GATEWAY:
				snprintf(pcInsert, iInsertLen, GATEWAY_STRING,inet_ntoa(userdata.gateway));
				break;
			case SSI_INDEX_NETMASK:
				snprintf(pcInsert, iInsertLen, NETMASK_STRING,inet_ntoa(userdata.netmask));
				break;
			default:
				usnprintf(pcInsert, iInsertLen, "??");
				break;
		}
		return(strlen(pcInsert));
}
void webserver_init(void)
{
	httpd_init();

	http_set_ssi_handler(SSIHandler, g_pcConfigSSITags, NUM_CONFIG_SSI_TAGS);
	
	http_set_cgi_handlers(g_psConfigCGIURIs, NUM_CONFIG_CGI_URIS);

	return;
}

int timeout;
void reboot_start(int second)
{	
	timeout = second;
	xTaskCreate(reboot_task, ( signed portCHAR * )"reboot_task", TASK_REBOOT_STACKSIZE, 
				(void*)timeout, TASK_REBOOT_PRIORITY, NULL);	
}

void reboot_task(void *pArgs)
{
	int i = 0;
	int timeout;
	timeout = (int)pArgs;
	for(i = timeout; i > 0; i--)
	{
		vTaskDelay(1000);
		UARTprintf("System reboot in %d second(s)..\r\n", i);
	}
	do_reboot();
	for(;;)
	{
	}
}
