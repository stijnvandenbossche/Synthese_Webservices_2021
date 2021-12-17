/*!
 *  \file TCP_functions.h
 *  \details Contains function protoypes for TCP functionality
 *  \remarkCreated on: 18 Nov 2021
 *  \author Stijn Vdb
 */

#ifndef INC_TCP_FUNCTIONS_H_
#define INC_TCP_FUNCTIONS_H_

#ifndef _STRING_H_
#include <string.h>
#endif

#ifndef __mx_lwip_H
#include <lwip.h>
#endif

#ifndef LWIP_HDR_TCP_H
#include <tcp.h>
#endif

#include <LCD_functions.h>

/*!
 *  \def TCP_PORT
 *  TCP_PORT sets up default port used for TCP connections to 64000
 */
#define TCP_PORT 64000
/*!
 *  \def AMOUNT_CONNECTIONS
 *  AMOUNT_CONNECTIONS sets how many connections can be open at the same time, for our use case 1 is plenty, this is just added to improve scalabilty if wanted/needed
 */
#define AMOUNT_CONNECTIONS 1
/*!
 *  \def MAX_LENGTH_WELCOME_MESSAGE
 *  MAX_LENGTH_WELCOME_MESSAGE sets the maximum length of the welcome message, to make it easier to initialize the string
 */
#define MAX_LENGTH_WELCOME_MESSAGE 500


int init_TCP(void);
err_t handle_incoming_connection(void* , struct tcp_pcb *, err_t);
err_t handle_incoming_message(void *, struct tcp_pcb *,struct pbuf *, err_t);
err_t succesful_send(void*, struct tcp_pcb *, u16_t );
int handle_command(char*,int,struct tcp_pcb *);

#endif /* INC_TCP_FUNCTIONS_H_ */
