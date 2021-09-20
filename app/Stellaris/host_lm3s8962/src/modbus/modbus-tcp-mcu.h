/*
 * modbus-tcp-mcu.h
 *
 *  Created on: Aug 27, 2021
 *      Author: PC
 */

#ifndef SOURCES_MODBUS_MODBUS_TCP_MCU_H_
#define SOURCES_MODBUS_MODBUS_TCP_MCU_H_

#include "modbus.h"

#define MODBUS_TCP_DEFAULT_PORT   502
#define MODBUS_TCP_SLAVE         0xFF

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * TCP MODBUS ADU = 253 bytes + MBAP (7 bytes) = 260 bytes
 */
#define MODBUS_TCP_MAX_ADU_LENGTH  260	// bytes


MODBUS_API modbus_t* modbus_new_tcp(const char *ip_address, int port);
MODBUS_API int modbus_tcp_listen(modbus_t *ctx, int nb_connection);
MODBUS_API int modbus_tcp_accept(modbus_t *ctx, int *s);

MODBUS_API modbus_t* modbus_new_tcp_pi(const char *node, const char *service);
MODBUS_API int modbus_tcp_pi_listen(modbus_t *ctx, int nb_connection);
MODBUS_API int modbus_tcp_pi_accept(modbus_t *ctx, int *s);

#endif /* SOURCES_MODBUS_MODBUS_TCP_MCU_H_ */
