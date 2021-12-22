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


//variables are static, needs to persist between different commands, to remember the list given by 'l' command, to be able to choose an image to display by the number command
char** image_list;

char welcome_message_tcp[MAX_LENGTH_WELCOME_MESSAGE]="Welcome to the image picker program for our group project.\r\n";
char welcome_message_tcp_commands[MAX_LENGTH_WELCOME_MESSAGE]="Send '\x1b[32;40ml\x1b[39;49m' to list all possible images.\r\nThen send a number to display the corresponding image.\r\nSend '\x1b[35;40mt\x1b[39;49m' followed by a space, then your text to display that text.\r\nSend '\x1b[33;40mc\x1b[39;49m' to clear the screen.\r\nSend '\x1b[36;40mh\x1b[39;49m' to display a list of commands.\r\n";

//Variables used for regex
char* regexImage ="^\\d+[,\\s]*$";
char* regexHelp="^[hH]\\s*$";
char* regexList="^[lL]\\s*$";
char* regexText="^[tT][,\\s+].*$";
char* regexClear="^[cC]\\s*$";

size_t maxMatches;

re_t regexCompiledImage;
re_t regexCompiledHelp;
re_t regexCompiledList;
re_t regexCompiledText;
re_t regexCompiledClear;

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


	//compiling regex -> by testing, this is broken
	regexCompiledHelp = re_compile(regexHelp);
	regexCompiledClear = re_compile(regexClear);
	regexCompiledList = re_compile(regexList);
	regexCompiledText = re_compile(regexText);
	regexCompiledImage = re_compile(regexImage);



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
	tcp_output(tpcb);
	tcp_write(tpcb,welcome_message_tcp_commands,MAX_LENGTH_WELCOME_MESSAGE, 0);
	tcp_output(tpcb);
	tcp_sent(tpcb, succesful_send);
	tcp_recv(tpcb, handle_incoming_message);
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
		free(image_list);
		image_list=NULL;
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
	int image_number;
	char image_number_string[5];
	int longest_name = getLargestNameLength();
	int amount_total=getImageAmount()+getGifAmount();
	struct imageMetaData buf = {.data = NULL, .name = NULL, .num = 0, .frameTime = 0, .height = 0, .width = 0};
	int match_length;

	//making sure command is a null-terminated string
	command[command_length]='\0';

	/* Checking if it's the first creation of the image list.
	 * If it doesn't exist yet, create it with malloc
	 *
	 */

	if(image_list==NULL){
		image_list = (char**)malloc(amount_total*sizeof(char*));
	}

	int err_code = 0;
	int i;

	if(re_match(regexList,command, &match_length) != -1){

		char imagelisttext[1000];

		char temp_text[longest_name+10];
		int tot_len=0;

		//list of all images
		char image_name[longest_name];
		int amount_images = getImageList(image_list,png,a_z);
		int amount_gifs = getImageList(image_list+amount_images,gif,a_z);
		for(i=0; i< amount_total; i++){
			extractNameOutOfPath(image_list[i],strlen(image_list[i]),image_name,ext,lower);
			snprintf(temp_text,strlen(image_list[i])+6,"#%d: %s\r\n",i,image_name);
			strncpy(&imagelisttext[tot_len],temp_text,strlen(image_list[i])+6);
			tot_len+=strlen(image_list[i])+6;
		}
		tcp_write(tpcb,imagelisttext,tot_len,0);
		imagelisttext[tot_len+2]='\0';
		tcp_output(tpcb);


	}else if(re_match(regexText,command, &match_length) != -1){
		char text_t[TEXT_BUFFER_LENGTH];
		int index_start = re_match(regexText,command, &match_length);
		textToLCD(command+2,strlen(command)-2,LCD_COLOR_RED);
	}else if(re_match(regexHelp,command, &match_length) != -1){
		tcp_write(tpcb,welcome_message_tcp_commands,MAX_LENGTH_WELCOME_MESSAGE, 0);
		tcp_output(tpcb);
	}else if(re_match(regexClear,command, &match_length) != -1){
		//clearPicture();
		//clearText();
		printf("Clear\r\n");
	}else if(re_match(regexImage,command, &match_length) != -1){
		if(image_list != NULL){
			image_number = atoi(command);
			printf("image #%d\r\n", image_number);

			if(image_number < amount_total){
				getRawImageMetaData((image_list[image_number]),strlen(image_list[image_number]),&buf);
				pictureToLCD(buf);
			}else{
				//no image with that number exists
				printf("No image with that number exists\r\n");
				char errortext1[40]="No image with that number exists\r\n";
				tcp_write(tpcb,errortext1,strlen(errortext1),0);
				tcp_output(tpcb);
			}
		}else{
			//list not populated yet
			char errortext2[85]= "The list of images isn't generated yet, enter 'l' to display the list first\r\n";
			tcp_write(tpcb,errortext2,strlen(errortext2),0);
			tcp_output(tpcb);
		}

	}else{
		if(command[0] != '\r'){
			err_code=1; //unknown command
			char errortext3[85]="\x1b[31;40mUnknown command, for a list of possible commands, type 'h'\x1b[39;49m\r\n";
			tcp_write(tpcb,errortext3,strlen(errortext3),0);
			tcp_output(tpcb);
			printf("Unknown command\r\n");
		}
	}
	return err_code;
}
