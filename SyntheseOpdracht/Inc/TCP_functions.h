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

//As default port for tcp connections, we will us port 64000
#define TCP_PORT 64000
//limit amount of connections to 1, as we only need this one connection
#define AMOUNT_CONNECTIONS 1
#define MAX_LENGHTH_WELCOME_MESSAGE 200

char welcome_message[MAX_LENGHTH_WELCOME_MESSAGE]="Welcome to the image picker program for our group project.\r\nSend 'l' to list all possible images.\r\nThen send a number to display the corresponding image.\r\nOr send 't' to display text."

int init_TCP(void);
void check_TCP_timeouts(void);
err_t handle_incoming_connection(void* , struct tcp_pcb *, err_t);
err_t succesful_send(void*, struct tcp_pcb *, u16_t );
int handle_command(char*);

#endif /* INC_TCP_FUNCTIONS_H_ */
