/*!
 * \file TCP_functions.c
 *
 *  \remarkCreated on: 18 Nov 2021
 *  \author Stijn Vdb
 */

#include <TCP_functions.h>
#ifndef BSP_FUNCTIONS_H_
#include <LCD_functions.h>
#endif

char welcome_message_tcp[MAX_LENGTH_WELCOME_MESSAGE]="Welcome to the image picker program for our group project.\r\nSend 'l' to list all possible images.\r\nThen send a number to display the corresponding image.\r\nOr send 't' to display text.";

int init_TCP(void){
	lwip_init();
	struct tcp_pcb* pcb = tcp_new();
	err_t error = tcp_bind(pcb, IP_ADDR_ANY, TCP_PORT);
	if(error==ERR_USE){
		//failed to bind port
		return 1;
	}

	struct tcp_pcb* connection = tcp_listen_with_backlog(pcb, AMOUNT_CONNECTIONS);
	tcp_accept(connection, handle_incoming_connection);

}

void check_TCP_timeouts(void){
	sys_check_timeouts();
}


err_t handle_incoming_connection(void* arg, struct tcp_pcb *tpcb, err_t err){
	//send welcome message
	tcp_write(tpcb,welcome_message_tcp,MAX_LENGTH_WELCOME_MESSAGE, 0);
	printf("Welcome\r\n");
	tcp_output(tpcb);
	tcp_sent(tpcb, succesful_send);
	tcp_recv(tpcb, handle_incoming_message);
	printf("callback functions done\r\n");
	return ERR_OK;
}

err_t succesful_send(void *arg, struct tcp_pcb *tpcb, u16_t len){
	//succesfully sent data, nothing needs to be done at the moment
	printf("sent\r\n");
	return ERR_OK;
}

err_t handle_incoming_message(void *arg, struct tcp_pcb *tpcb,struct pbuf *pbuf, err_t err){
	//write data to pbuf and depending from what is received to different action
	printf("incoming message\r\n");
	char data[200];
	if(pbuf!=NULL){
		/*if(pbuf->tot_len>pbuf->len){
			//message longer than expected, abort
			return ERR_OK;
		}else{*/
			data[0] = ((char*)(pbuf->payload))[0];
			data[pbuf->len]='\0';
			if(handle_command(data)==1){
				//unknown command -> give error
			}
			tcp_recved(tpcb,pbuf->len);
			pbuf_free(pbuf);
			//}
	}
	else{
		//pbuf empty -> means connection was closed, to do: close connection
	}
	return ERR_OK;
}

int handle_command(char* command){
	printf("Handle command: %c\r\n",command[0]);
	int image_number;
	if(strlen(command)==1 || strlen(command)==2 || 1){
		if(command[0]=='l' ||command[0] =='L'){
			char text_l[30] = "Received command 'l'";
			textToLCD(text_l,strlen(text_l),LCD_COLOR_BLUE);

			//list of all images -> use command to display this
			return 0;
		}else if(command[0]=='t' || command[0] == 'T'){
			char text_t[30] = "Received command 't'";
			textToLCD(text_t,strlen(text_t),LCD_COLOR_RED);
			//display text to screen -> use command for this
			return 0;
		}else if(isdigit(command[0])){
			//image_number = atoi(command);
			textToLCD(command,1,LCD_COLOR_GREEN);
			//display image with number image_number -> use command for this
			return 0;
		}else{
			return 1; //unknown command
		}
	}else{
		return 1; //unknown command
	}
}

