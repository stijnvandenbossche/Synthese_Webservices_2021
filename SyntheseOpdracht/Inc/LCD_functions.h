/*!
 *	\file BSP_functions.h
 *
 *  \Created 8 nov. 2021
 *  \Author Jonas Aertgeerts
 */
#ifndef BSP_FUNCTIONS_H_
#define BSP_FUNCTIONS_H_
#include <string.h>
#include <stdio.h>
//added "${workspace_loc:/${ProjName}/Drivers/BSP/inc}" to project properties
#include "stm32746g_discovery_lcd.h"

#include "fileSystemAPI.h"

// define lcd dimensions
#define LCD_WIDTH 480
#define LCD_HEIGHT 272

// define the length of the buffer of the string thats going to be displayed
#define TEXT_BUFFER_LENGTH 300

/* LCD Initialization for normal operation */
void initLCD(void);
/* prints text to the LCD */
int textToLCD(char *textArray, int len, uint32_t color);
/* prints picture to the LCD */
uint8_t pictureToLCD(struct imageMetaData picture);

/* reads status of onboard blue button */
uint8_t readButton(void);

/* timer interrupt callback */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);


#endif /* BSP_FUNCTIONS_H_ */
