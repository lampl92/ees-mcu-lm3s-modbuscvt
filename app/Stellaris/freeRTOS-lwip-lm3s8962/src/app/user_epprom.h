#ifndef _USER_EPPROM_H_
#define _USER_EPPROM_H_

typedef struct
{
	unsigned char ucSequenceNum;
	//
	// The CRC of the parameter block.  When in RAM, this value is not used.
	// When in flash, this value is used to validate the contents of the
	// parameter block (to avoid using a partially written parameter block).
	//
	unsigned char ucCRC;
	//
	// The version of this parameter block.  This can be used to distinguish
	// saved parameters that correspond to an old version of the parameter
	// block.
	//
	unsigned char ucVersion;
	//
	// The current device number for the module.
	//
	unsigned char ucDeviceNumber;
	
	unsigned long baudrate;
	unsigned long databits;
	unsigned long parity;
	unsigned long stopbits;
	unsigned long ipaddr;
	unsigned long gateway;
	unsigned long netmask;
	unsigned long lifetime;
	unsigned char mac[6];
	uint8_t reserved[22];
}user_data_t;

void user_epprom_init(void);
void user_epprom_test(void);
int get_user_data(user_data_t * user_data);
int set_user_data(user_data_t * user_data);
#endif
