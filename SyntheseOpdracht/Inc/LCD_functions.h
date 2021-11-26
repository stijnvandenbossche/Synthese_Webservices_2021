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

// define lcd dimensions
#define LCD_WIDTH 480
#define LCD_HEIGHT 272

// hard coded image sizes
#define PICTURE_X_PIXEL_MAX 240
#define PICTURE_Y_PIXEL_MAX 272

// define the length of the buffer of the string thats going to be displayed
#define TEXT_BUFFER_LENGTH 300

// test the struct functionallity
struct imageMeta{
    char* data;
    uint16_t width;
    uint16_t height;
};

/* LCD Initialization for normal operation */
void initLCD(void);
/* prints text to the LCD */
int textToLCD(char *textArray, int len, uint32_t color);
/* prints picture to the LCD */
void pictureToLCD(struct imageMeta picture);


/* reads status of onboard blue button */
uint8_t readButton(void);

/* sets the time for the timer interrupt routine */
void setTimer_ms(uint16_t time_ms);
/* timer interrupt callback */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
/* start timer to receive timer interrupts */
void startTimer(void);
/* stop timer to receive no more timer interrupts */
void stopTimer(void);

#endif /* BSP_FUNCTIONS_H_ */
