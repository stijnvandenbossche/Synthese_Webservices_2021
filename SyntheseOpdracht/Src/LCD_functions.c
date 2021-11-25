/*!
 *	\file BSP_functions.c
 *
 *  \remarkCreated on: 8 nov. 2021
 *  \author: Jonas Aertgeerts
 */
#include <LCD_functions.h>


/*
 *  declaration of used variables
 */

// error message that will be displayed on the LCD when there is something wrong with the text
static char errorMessage[TEXT_BUFFER_LENGTH] = "something went wrong while printing the string (see Serial terminal for more info)";

// save time for timer interrupt
uint16_t timerTime_ms;

extern TIM_HandleTypeDef htim2;

/*!
 * \brief LCD Initialization for normal operation.
 *
 * \param
 *  void
 *
 * \retval
 *  void
 *
 * \remark
 *  please make sure there are no words longer then 25 characters in the string.
 */
void initLCD(void)
{
	  BSP_LCD_Init();
	  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS );
	  BSP_LCD_LayerDefaultInit(0, (LCD_FB_START_ADDRESS+(LCD_WIDTH*LCD_HEIGHT*4 )));

	  BSP_LCD_DisplayOn();

	  // background layer is now white
	  // pictures will be displayed on this layer
	  BSP_LCD_SelectLayer(0);
	  BSP_LCD_Clear(LCD_COLOR_WHITE);


	  // text will be displayed on foreground layer
	  BSP_LCD_SelectLayer( 1 );
	  // layer is made transparent so background is visible
	  BSP_LCD_Clear(LCD_COLOR_TRANSPARENT);

	  // set text and text background color
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	  // select proper font
	  BSP_LCD_SetFont(&Font24);
}

/*!
 * \brief prints text to the LCD.
 *
 * \param
 *  textArray -> array containing the string that has to be printed.
 *  len -> the amount of characters that has to be printed
 *  color -> color the text will be printed in
 *
 * \retval
 *  len when the function has succeeded.
 *  0 when the function has failed.
 *
 * \note please don't use black text color because the text background is black
 * \note red text color can be used for errors
 */
int textToLCD(char textArray[TEXT_BUFFER_LENGTH], int len, uint32_t color)
{
	// check if length is valid
	if(len > TEXT_BUFFER_LENGTH)
	{
		textToLCD(errorMessage, strlen(errorMessage),LCD_COLOR_RED);
		printf("the string that was going to be displayed is to long in total\r\n");
		return 0;
	}
	// make sure there is a '\0' at the end
	textArray[len] = '\0';
	// check if there are any weird charakters in the string
	for(int i = 0 ; i < len; i++)
	{
		if(textArray[i] < ' ')
		{
			textToLCD(errorMessage, strlen(errorMessage),LCD_COLOR_RED);
			printf("the string that was going to be displayed contains weird characters\r\n");
			return 0;
		}
	}
	clearText();
	BSP_LCD_SetTextColor(color);
	// make sure we are on the foreground layer
	// small string to save the line that is going to be printed.
	// one char longer than max length for '\0'
	char BufString[CHARS_ON_LINE+1];
	// y position of text
	uint16_t LineCnt = 0;
	// itterators to loop in text arrays
	uint16_t Count = 0;
	uint16_t CountTotal = 0;
	// var to register if a ' ' was found in the buffer string
	uint8_t Spacefound = 0;
	// stay in the loop as long as the '\0' is not found
	while( textArray[CountTotal] != '\0')
	{
		// save chars in the temporary buffer
		BufString[ Count ] = textArray[CountTotal];
		if( BufString[ Count ] == ' ')
		{
			Spacefound = 1;
		}
		// only add one char a time some the while above can detect the '\0'
		Count ++;
		CountTotal ++;
		// if 25 chars were found -> print them
		if( Count == CHARS_ON_LINE )
		{
			// if there was a space in the line -> go back until we find it
			if(Spacefound == 1)
			{
				// going back untill we find a space
				while( BufString[ ( Count - 1 ) ] != ' ')
				{
					// fill the buffer with '\0' and reduce where we were reading in the string
					BufString[ ( Count - 1 ) ] = '\0';
					Count --;
					CountTotal --;
				}
			}
			// put a - at the end of the long word to continue on the next line if there was no space in the line
			if(Spacefound == 0)
			{
				BufString[ ( Count+1 ) ] = '\0';
				BufString[ ( Count - 1 ) ] = '-';
				CountTotal -= 1;
			}
			// make sure the last char is '\0'
			BufString[ Count ] = '\0';
			// if the last char would be a ' ', it does not have to be printed
			if(BufString[ Count -1 ] == ' ')
			{
				BufString[ Count -1 ] = '\0';
			}
			// print on the lcd
			BSP_LCD_DisplayStringAt( 0, LineCnt, ( uint8_t * ) BufString, CENTER_MODE );
			// go down one line
			LineCnt += 24;
			// reset char counter
			Count = 0;
			// reset to indicate no space was yet found
			Spacefound = 0;
		}
	}
	// print the last chars
	if( textArray[CountTotal] == '\0')
	{
		// the bufstring is still filled with data from above
		// just add '\0' to the end
		BufString[ Count ] = '\0';
		// print the last section
		BSP_LCD_DisplayStringAt( 0, LineCnt, ( uint8_t * ) BufString, CENTER_MODE );

	}
	return len;


}

/*!
 * \brief prints picture to the LCD.
 *
 * \param
 *  picture -> pointer to the picture that has to be printed
 *
 * \retval
 *  void
 *
 */
void pictureToLCD(void* picture)
{
	//remove previous picture
	clearPicture();
	// drawpicture based on given pointer
	WDA_LCD_DrawBitmap((uint16_t*)picture, ( ( LCD_WIDTH - PICTURE_X_PIXEL ) / 2 ) , ( LCD_HEIGHT - PICTURE_Y_PIXEL ), PICTURE_X_PIXEL, PICTURE_Y_PIXEL, LTDC_PIXEL_FORMAT_ARGB1555);
}

/*!
 * \brief clears previous text of the LCD.
 *
 * \param
 *  void
 *
 * \retval
 *  void
 *
 */
void clearText(void)
{
	// switch to transparent to make overwrite text with 'invisible' plane
	BSP_LCD_SetTextColor( LCD_COLOR_TRANSPARENT );
	// fill upper screen with plane
	BSP_LCD_FillRect( 0, 0 , LCD_WIDTH, LCD_HEIGHT-PICTURE_Y_PIXEL );
}

/*!
 * \brief clears previous picture of the LCD.
 *
 * \param
 *  void
 *
 * \retval
 *  void
 *
 */
void clearPicture(void)
{
	// switch to transparent to make overwrite text with 'invisible' plane
	BSP_LCD_SetTextColor( LCD_COLOR_TRANSPARENT );
	// fill lower screen with plane
	BSP_LCD_FillRect( 0, LCD_HEIGHT-PICTURE_Y_PIXEL , LCD_WIDTH, PICTURE_Y_PIXEL );
}

/*!
 * \brief reads status of onboard blue button
 *
 * \param
 *  void
 *
 * \retval
 *  1 when blue button is pushed
 *  0 when blue button is released
 *
 */
uint8_t readButton(void)
{
	if(GPIOI->IDR & GPIO_PIN_11)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*!
 * \brief sets the time for the timer interrupt routine
 *
 * \param
 *  time_ms time in ms for the interrupt routine
 *
 * \retval
 *  void
 *
 */
void setTimer_ms(uint16_t time_ms)
{
	timerTime_ms = time_ms;
	htim2.Instance->ARR = (timerTime_ms * 2) - 1;
}

/*!
 * \brief timer interrupt callback
 *
 * \param
 *  htim timer that generated the interrupt
 *
 * \retval
 *  void
 *
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim == &htim2)
	{
		static uint8_t ledstate = 0;
		if(ledstate == 0)
		{
			ledstate =1;
			GPIOI->BSRR = GPIO_PIN_1;
		}
		else
		{
			ledstate = 0;
			GPIOI->BSRR = (uint32_t)GPIO_PIN_1 << 16;
		}
	}
}

/*!
 * \brief start timer to receive timer interrupts
 *
 * \param
 *  void
 *
 * \retval
 *  void
 *
 */
void startTimer(void)
{
	setTimer_ms(timerTime_ms);
	HAL_TIM_Base_Start_IT(&htim2);
}

/*!
 * \brief stop timer to receive no more timer interrupts
 *
 * \param
 *  void
 *
 * \retval
 *  void
 *
 */
void stopTimer(void)
{
	HAL_TIM_Base_Stop_IT(&htim2);
}
