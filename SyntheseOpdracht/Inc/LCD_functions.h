/*!
 *	\BSP_functions.h
 *
 *  \Created on: 8 nov. 2021
 *  \Author: Jonas Aertgeerts
 */
#ifndef BSP_FUNCTIONS_H_
#define BSP_FUNCTIONS_H_
#include <string.h>
//added "${workspace_loc:/${ProjName}/Drivers/BSP/inc}" to project properties
#include "stm32746g_discovery_lcd.h"

// define lcd dimensions
#define LCD_WIDTH 480
#define LCD_HEIGHT 272

// hard coded image sizes
#define PICTURE_X_PIXEL 100
#define PICTURE_Y_PIXEL 100

// define the length of the buffer of the string thats going to be displayed
#define TEXT_BUFFER_LENGTH 300
// define the maximum amount of characters on one line on the LCD
#define CHARS_ON_LINE 25

/* LCD Initialization for normal operation */
void initLCD(void);
/* prints text to the LCD */
int textToLCD(char *textArray, int len);
/* prints picture to the LCD */
void pictureToLCD(void* picture);
/* clears previous text of the LCD */
void clearText(void);
void clearPicture(void);

#endif /* BSP_FUNCTIONS_H_ */
