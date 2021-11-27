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

char welcome_message_tcp[MAX_LENGTH_WELCOME_MESSAGE]="Welcome to the image picker program for our group project.\r\nSend 'l' to list all possible images.\r\nThen send a number to display the corresponding image.\r\nOr send 't' followed by a space, then your text to display that text.\r\n";

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
	char data[TEXT_BUFFER_LENGTH];
	int pbuf_len = pbuf->tot_len;
	if(pbuf!=NULL){
		/*if(pbuf->tot_len>pbuf->len){
			//message longer than expected, abort
			return ERR_OK;
		}else{*/
			for(int i=0;i<(pbuf->len);i++){
				data[i] = ((char*)(pbuf->payload))[i];
			}
			data[pbuf->len]='\0';
			if(handle_command(data,pbuf_len)==1){
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

int handle_command(char* command,int command_length){
	printf("Handle command: %c\r\n",command[0]);
	int image_number;
	int err_code = 0;
	if(command[0]=='l' ||command[0] =='L'){

		char text_l[30] = "Received command 'l'";
		textToLCD(text_l,strlen(text_l),LCD_COLOR_BLUE);
		//list of all images -> use command to display this
	}else if(command[0]=='t' || command[0] == 'T'){
		char text_t[TEXT_BUFFER_LENGTH];
		strncpy(text_t,&command[2],command_length);
		textToLCD(text_t,command_length-2,LCD_COLOR_RED);
	}else if(isdigit(command[0])){
		if(command_length>=2 && isdigit(command[1])){ //two-digit number
			image_number = atoi(command[0])*10 + atoi(command[1]);
		}else{
			//one - digit number
			image_number = atoi(command[0]);
		}
		char displaystring[2];
		snprintf(displaystring,2,"%d",image_number);
		textToLCD(displaystring,2,LCD_COLOR_GREEN);

		//display image with number image_number -> use command for this
	}else{
		err_code=1; //unknown command
	}
	return err_code;
}

