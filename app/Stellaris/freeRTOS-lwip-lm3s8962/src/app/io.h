#ifndef __IO_H__
#define __IO_H__

#define LED_SYS_ON    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 2)
#define LED_SYS_OFF		GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0)

#define MODBUS_DIR_RECV   GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1)
#define MODBUS_DIR_SEND   GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0)

#define LED_MODBUS_ON   GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2)
#define LED_MODBUS_OFF  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0)

#endif
