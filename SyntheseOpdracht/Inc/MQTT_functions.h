#ifndef MQTT_functions
#define MQTT_functions
#include <string.h>
#include "LCD_functions.h"
#include "fileSystemAPI.h"
#include "lwip.h"
#include "mqtt.h"

void mqtt_connection_cb(mqtt_client_t *, void *, mqtt_connection_status_t);
void mqtt_sub_request_cb(void *, err_t);
void mqtt_incoming_publish_cb(void *, const char *topic, u32_t);
void mqtt_incoming_data_cb(void *, const u8_t *, u16_t, u8_t);
void example_publish(mqtt_client_t*, void *);
void mqtt_pub_request_cb(void *, err_t);
void example_do_connect(mqtt_client_t*);

#endif
