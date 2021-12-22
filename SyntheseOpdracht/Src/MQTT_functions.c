#include "lwip.h"
#include "mqtt.h"
#include "MQTT_functions.h"
#include "LCD_functions.h"
#include "fileSystemAPI.h"

static int inpub_id;
static enum data_types {Text, Img, Gif};

void mqtt_do_connect(mqtt_client_t *client)
{
  struct mqtt_connect_client_info_t ci;
  err_t err;

  ip4_addr_t ip_addr;
  IP_ADDR4(&ip_addr, 192, 168, 69, 11);

  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));

  /* Minimal amount of information required is client identifier, so set it here */
  ci.client_id ="connection";

  /* Initiate client and connect to server, if this fails immediately an error code is returned
     otherwise mqtt_connection_cb will be called with connection result after attempting
     to establish a connection with the server.
     For now MQTT version 3.1.1 is always used */

  err = mqtt_client_connect(client, &ip_addr, MQTT_PORT, mqtt_connection_cb, 0, &ci);

  printf("mqtt_connect err_1 return %d\n\r", err);

}

void mqtt_sub_request_cb(void *arg, err_t result)
{
  /* Just print the result code here for simplicity,
     normal behaviour would be to take some action if subscribe fails like
     notifying user, retry subscribe or disconnect from server */
}

void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
  printf("Incoming publish at topic %s with total length %u\n\r", topic, (unsigned int)tot_len);

  //check if topic = 1 of the 3 send topics
  if(strncmp(topic, "sendText", strlen("sendText")) == 0)
  {
	  inpub_id = Text;
  }
  else if(strncmp(topic, "sendImage", strlen("sendImage")) == 0)
  {
	  inpub_id = Img;
  }
  else if(strncmp(topic, "sendGif", strlen("sendGif")) == 0)
  {
	  inpub_id = Gif;
  }


}

void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  printf("Incoming publish payload with length %d, flags %u\n\r", len, (unsigned int)flags);

  char*	imageList[getImageAmount()];
  char 	name[getLargestNameLength()];
  getImageList(imageList, png, a_z);
  char* gifList[getGifAmount()];
  getImageList(gifList, gif, a_z);
  struct imageMetaData buf = {.data = NULL, .name = NULL, .num = 0, .frameTime = 0, .height = 0, .width = 0};


  if(flags & MQTT_DATA_FLAG_LAST)
  {
	  if(inpub_id == Text)
	  {
		  //If inpub_id = text then send text to lcd
		  char textTosSendToLCD[len+1];
		  for(int i = 0; i< len ; i++)
		  {
			  textTosSendToLCD[i] = data[i];
		  }
		  textTosSendToLCD[len+1] = '\0';
		  textToLCD(textTosSendToLCD, strlen(textTosSendToLCD),LCD_COLOR_WHITE);
	  }

	  else if(inpub_id == Img)
	  {
		  //If inpub_id = img then start sending the requested image to lcd
		  uint8_t i = 0;
		  while(i < getImageAmount())
		  {
			  extractNameOutOfPath(imageList[i], strlen(imageList[i]), name[i], ext, lower);
			  if(strncmp(data, name[i], strlen(name[i])) == 0)
			  {
				  getRawImageMetaData(imageList[i], strlen(imageList[i]), &buf);
				  pictureToLCD(buf);
				  break;
			  }
			  else
			  {
				  i++;
			  }
		  }
	  }

	  else if(inpub_id == Gif)
	  {
		 //If inpub_id = Gif then start sending the requested gif to lcd
		 uint8_t j = 0;
		 while(j < getGifAmount())
		 {
			  extractNameOutOfPath(gifList[j], strlen(gifList[j]), name[j], ext, lower);
			  if(strncmp(data, name[j], strlen(name[j])) == 0)
			  {
				  getRawImageMetaData(gifList[j], strlen(gifList[j]), &buf);
				  pictureToLCD(buf);
				  break;
			  }
			  else
			  {
				  j++;
			  }
		 }
	  }
  }
}


void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  err_t err;
  if(status == MQTT_CONNECT_ACCEPTED) {
    printf("mqtt_connection_cb: Successfully connected\n\r");

    /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

    //subscribe to every topic at this level because i already look for the specific ones in mqtt_incoming_data_cb
    err = mqtt_subscribe(client, "+", 0, mqtt_sub_request_cb, arg);

    if(err != ERR_OK) {
      printf("mqtt_subscribe return: %d\n\r", err);
    }
  } else {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n\r", status);

    /* Its more nice to be connected, so try to reconnect */
    mqtt_do_connect(client);
  }
}

/* Called when publish is complete either with sucess or failure */
//returns 0 when succes
void mqtt_pub_request_cb(void *arg, err_t result)
{
  if(result != ERR_OK) {
    printf("Publish result: %d\n\r", result);
  }
}

void mqtt_do_publish(mqtt_client_t *client, void *arg)
{
  err_t err;
  u8_t qos = 0; /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such crappy payload... */
  char* imageList[getImageAmount()];
  char* gifList[getImageAmount()];
  char name[getLargestNameLength()];
  getImageList(imageList, png, a_z);
  getImageList(gifList, gif, a_z);
  char fullImageList[250] = " ";
  char fullGifList[250] = " ";

  for(uint8_t i = 0; i < getImageAmount(); i++)
  	{
	  //extracting the names and putting them in 1 list so i can sent them with 1 publish
	  extractNameOutOfPath(imageList[i], strlen(imageList[i]), name, ext, lower);
	  strncat(fullImageList, name, strlen(*imageList));
	  strncat(fullImageList, "\n\r", 5);
  	}

  err = mqtt_publish(client, "showImageList", fullImageList, strlen(fullImageList), qos, retain, mqtt_pub_request_cb, arg);

  for(uint8_t j = 0; j < getGifAmount(); j++)
  	{
	  //extracting the names and putting them in 1 list so i can sent them with 1 publish
	  extractNameOutOfPath(gifList[j], strlen(gifList[j]), name, ext, lower);
	  strncat(fullGifList, name, strlen(*gifList));
	  strncat(fullGifList, "\n\r", 5);
  	}

  err = mqtt_publish(client, "showGifList", fullGifList, strlen(fullGifList), qos, retain, mqtt_pub_request_cb, arg);

  if(err != ERR_OK) {
    printf("Publish err: %d\n\r", err);
  }
}

