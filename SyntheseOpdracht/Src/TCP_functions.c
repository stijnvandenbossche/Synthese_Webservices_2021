/*!
 * \file TCP_functions.c
 *
 *  \remarkCreated on: 18 Nov 2021
 *  \author Stijn Vdb
 */

#include <TCP_functions.h>


int init_TCP(void){
	lwip_init();
	tcp_pcb* pcb = tcp_new();
	err_t error = tcp_bind(pcb, IP_ADDRESS, TCP_PORT);
	if(error==ERR_USE){
		//failed to bind port
		return 1;
	}

	tcp_pcb* connection = tcp_listen_with_backlog(pcb, AMOUNT_CONNECTIONS);
	tcp_accept(connection, handle_incoming_connection);
	tcp_sent(pcb, succesful_send);
	tcp_recv(pcb, handle_incoming_message);
}

void check_TCP_timeouts(void){
	sys_check_timeouts();
}


err_t handle_incoming_connection(void* arg, struct tcp_pcb *tpcb, err_t err){
	//send welcome message
	tcp_write(tpcb,welcome_message,MAX_LENGTH_WELCOME_MESSAGE, 0);
	tcp_output(tpcb);
}

err_t succesful_send(void *arg, struct tcp_pcb *tpcb, u16_t len){
	//succesfully sent data, nothing needs to be done at the moment
	return ERR_OK;
}

err_t handle_incoming_message(void *arg, struct tcp_pcb *tpcb,struct pbuf *pbuf, err_t err){
	//write data to pbuf and depending from what is received to different action
	char* data;
	if(pbuf!=NULL){
		if(pbuf->tot_len>pbuf->len){
			//message longer than expected, abort
			return ERR_BUF;
		}else{
			*data = (*char)(pbuf->payload);
			tcp_recved(tpcb,pbuf->tot_len);
			pbuf_free(pbuf);
			if(handle_command(data)==1){
				//unknown command -> give error
			}
		}
	}
	else{
		//pbuf empty -> means connection was closed, to do: close connection
	}
}

int handle_command(char* command){
	int image_number;
	if(strlen(command)==1 || strlen(command)==2){
		if(command=='l' ||command =='L'){
			//list of all images -> use command to display this
			return 0;
		}else if(command=='t' || command == 'T'){
			//display text to screen -> use command for this
			return 0;
		}else if(isnumber(command)){
			image_number = atoi(command);
			//display image with number image_number -> use command for this
			return 0;
		}else{
			return 1; //unknown command
		}
	}else{
		return 1; //unknown command
	}
}

