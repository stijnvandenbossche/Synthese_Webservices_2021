/*!
 * \file TCP_functions.h
 *
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

//As default port for tcp connections, we will us port 64000
#define TCP_PORT 64000
//limit amount of connections to 1, as we only need this one connection
#define AMOUNT_CONNECTIONS 1
#define MAX_LENGTH_WELCOME_MESSAGE 300


int init_TCP(void);
void check_TCP_timeouts(void);
err_t handle_incoming_connection(void* , struct tcp_pcb *, err_t);
err_t handle_incoming_message(void *, struct tcp_pcb *,struct pbuf *, err_t);
err_t succesful_send(void*, struct tcp_pcb *, u16_t );
int handle_command(char*,int,struct tcp_pcb *);

#endif /* INC_TCP_FUNCTIONS_H_ */
