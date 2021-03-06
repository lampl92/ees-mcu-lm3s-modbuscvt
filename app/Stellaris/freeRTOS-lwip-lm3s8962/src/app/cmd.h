#ifndef _CMD_H_
#define _CMD_H_
#define INPUT_BUFER_SIZE 128

#define CMD_UART_PORT				UART0_BASE
#define CMD_UART_PORT_INT	  INT_UART0
void cmd_init(void);
void do_reboot(void);
int validate_ip(char *ip);
void set_default_config(void);
#endif
