/*!
 * \file TCP_functions.c
 * \details Contains all functions necessary for correct TCP functionality. The only function users need and should use, is init_TCP; this sets up all callback functions, and starts listening on port 64000 by default (set up in TCP_functions.h)
 *
 *  \remarkCreated on: 18 Nov 2021
 *  \author Stijn Vdb
 */

#include <TCP_functions.h>
#ifndef BSP_FUNCTIONS_H_
#include <LCD_functions.h>
#endif
#include <string.h>
#ifndef FILESYSTEMAPI_H_
#include <fileSystemAPI.h>
#endif

char welcome_message_tcp[MAX_LENGTH_WELCOME_MESSAGE]="Welcome to the image picker program for our group project.\r\nSend 'l' to list all possible images.\r\nThen send a number to display the corresponding image.\r\nOr send 't' followed by a space, then your text to display that text.\r\n";
char** image_list;
//char* image_list[getImageAmount()];


/*!
 * \brief This function initializes TCP functionality & listens at port 64000 by default. Has to be called to correctly handle TCP commands
 *
 * \param void
 *
 * \retval 1 when the function has failed to bind to the port 64000
 * \retval 0 when the function succeeded.
 *
 */
int init_TCP(void){
	int returnvalue = 0;
	lwip_init();
	struct tcp_pcb* pcb = tcp_new();
	err_t error = tcp_bind(pcb, IP_ADDR_ANY, TCP_PORT);
	if(error==ERR_USE){
		//failed to bind port
		returnvalue= 1;
	}

	struct tcp_pcb* connection = tcp_listen_with_backlog(pcb, AMOUNT_CONNECTIONS);
	tcp_accept(connection, handle_incoming_connection);

	return returnvalue;
}

/*!
 * \brief this function is the callback function that is called when there is an incoming connection on the port the TCP server is listening on. It sends a welcome message with some basic instructions as to what commands are supported and how to use them, as well as setting up callback functions on what to do after a successful send (of a message), and what to do with an incoming message.
 *
 * \param arg -> any extra arguments to identify the connection, not used in our use case
 * \param tpcb -> the tcp_pcb (tcp protocol block) which is created when accepting the new connection, and which is used to send and receive data on
 * \param err -> error message
 *
 * \return returns the error code
 */
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

/*!
 * \brief callback function that is called after a successful send, prints a message over serial port for debugging.
 *
 * \param arg -> any extra arguments to identify the connection, not used in our use case
 * \param tpcb -> the tcp_pcb (tcp protocol block) on which data is sent.
 * \param len -> length of sent data
 *
 * \return returns error code
 */
err_t succesful_send(void *arg, struct tcp_pcb *tpcb, u16_t len){
	//succesfully sent data, nothing needs to be done at the moment
	printf("sent\r\n");
	return ERR_OK;
}

/*!
 *
 * \brief Callback function which is called when receiving a message. This reads the pbuf and stores it in a string, so the pbuf can be freed as quick as possible. Afterwards, the data is processes with the function handle_command
 *
 * \param arg -> any extra arguments to identify the connection, not used in our use case
 * \param tcp_pcb -> tcp_pcb (tcp protocol block) on which a message is received.
 * \param pbuf -> a structure on which the incoming message is stored, and which needs to be read out. It is a linked list with multiple payloads if the message is too long.
 * \param err -> error code
 *
 * \return returns error code.
 */

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
			if(handle_command(data,pbuf_len,tpcb)==1){
				//unknown command -> give error
			}
			tcp_recved(tpcb,pbuf->len);
			pbuf_free(pbuf);
		//}
	}
	else{
		for(int i=0;i<getImageAmount();i++){
			free(image_list[i]);
		}
		free(image_list);
		tcp_close(tpcb);

		//pbuf empty -> means connection was closed, to do: close connection
	}
	return ERR_OK;
}

/*!
 *
 * \brief Handles the actions which need to be performed depending of the incoming message via tcp. Currently implemented are following commands: 'l', 't', and any number of max 2 digits
 *
 * \param command -> the message received over tcp, of which the contents are checked
 * \param command_length -> the length of the message, used because it command is not null-byte terminated per se
 * \param tpcb -> the tcp pcb (tcp protocol block) over which the message has been received, and which is used to send data back to for some commands.
 *
 * \return returns the error code
 *
 * \retval 0 if everything went fine
 * \retval 1 if something went wrong
 */

int handle_command(char* command,int command_length,struct tcp_pcb *tpcb){
	printf("Handle command: %c\r\n",command[0]);
	int image_number;
	char image_number_string[5];
	int longest_name = getLargestNameLength();
	//variables are static, needs to persist between different commands, to remember the list given by 'l' command, to be able to choose an image to display by the number command
	int amount_images=getImageAmount();
	//static char* image_list[getImageAmount()];

	/* Checking if it's the first creation of the image list. If it does exist, resize the existing array with realloc
	 * 	(extra measurements in case the image list changes in size in runtime, probably not necessary but it is safer
	 *
	 * 	If it's the first time, create it with malloc
	 */

	if(image_list==NULL){
		image_list = (char**)malloc(amount_images*sizeof(char*));
		for(int i=0;i<amount_images;i++){
			image_list[i]=(char*)malloc(longest_name*sizeof(char));
		}
	}
	/*}else{
		image_list = (char**)realloc(image_list,getImageAmount() * longest_name);
	}*/


	int err_code = 0;
	int i;

	if(command[0]=='l' ||command[0] =='L'){

		char imagelisttext[1000];

		char temp_text[longest_name+10];
		int tot_len=0;

		//list of all images
		char image_name[longest_name];
		int amount_images = getImageList(image_list,raw,a_z);
		for(i=0; i< amount_images; i++){
			extractNameOutOfPath(image_list[i],longest_name,image_name,no_ext,lower);
			snprintf(temp_text,100,"#%d: %s\r\n",i,image_name);
			strncpy(&imagelisttext[i*(longest_name+10)],temp_text,longest_name+10);
			tot_len+=longest_name+10;
		}
		tcp_write(tpcb,imagelisttext,tot_len,0);
		imagelisttext[(i+1)*(longest_name+10)]='\0';
		printf("%s\r\n\r\n",imagelisttext);
		//textToLCD("list ding",10,LCD_COLOR_BLUE);
		tcp_output(tpcb);


	}else if(command[0]=='t' || command[0] == 'T'){
		char text_t[TEXT_BUFFER_LENGTH];
		strncpy(text_t,&command[2],command_length);
		textToLCD(text_t,command_length-2,LCD_COLOR_RED);
	}else if(isdigit(command[0])){
		strncpy(image_number_string,command,2);	//Support up to two-digit numbers, should be plenty. If necessary, can be easily expanded by changing the '2'
		image_number = atoi(image_number_string);

		printf("image #%d\r\n",image_number);

		if(image_number < amount_images){
			pictureToLCD(getRawImageData((image_list[image_number]),longest_name));
		}else{
			//no image with that number exists
			printf("No image with that number exists\r\n");
			char errortext1[40]="No image with that number exists\r\n";
			tcp_write(tpcb,errortext1,strlen(errortext1),0);
			tcp_output(tpcb);
		}

	}else{
		err_code=1; //unknown command
		printf("Unknown command\r\n");
			/*char errortext2[20]="Unknown command\r\n";
			tcp_write(tpcb,errortext2,strlen(errortext2),0);
			tcp_output(tpcb);*/
	}
	return err_code;
}
