#ifndef _USER_EPPROM_H_
#define _USER_EPPROM_H_

typedef struct
{
	unsigned long ipaddr;
	unsigned long gateway;
	unsigned long netmask;
	unsigned short lifetime;
	unsigned short crc;
}user_data_t;

void user_epprom_init(void);
void user_epprom_test(void);
int get_user_data(user_data_t * user_data);
int set_user_data(user_data_t * user_data);
#endif
