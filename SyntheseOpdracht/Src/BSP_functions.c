/*
 * BSP_functions.c
 *
 *  Created on: 8 nov. 2021
 *      Author: Jonas Aertgeerts
 */
#include "BSP_functions.h"


/*
 *  declaration of used variables
 */

// error message that will be displayed when there is something wrong with the text
static char errorMessage[TEXT_BUFFER_LENGTH] = "something went wrong while printing the string (see Serial terminal for more info)";


/*
 * func	: LCD Initialization for normal operation
 * para	: no parameters
 * ret	: no return value
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
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_SetBackColor(LCD_COLOR_TRANSPARENT);
	  // select proper font
	  BSP_LCD_SetFont(&Font24);
}

/*
 * func	: prints text to the LCD
 * para	: textArray[TEXT_BUFFER_LENGTH] array containing the string that has to be printed
 * para	: len the amount of characters that has to be printed
 * ret	: 1 if all went well, 0 if something went wrong
 */
int textToLCD(char textArray[TEXT_BUFFER_LENGTH], int len)
{
	// make sure there is a '\0' at the end
	textArray[len] = '\0';
	// check if there are any weird charakters in the string
	for(int i = 0 ; i < len; i++)
	{
		if(textArray[i] < 0x20)
		{
			textToLCD(errorMessage, strlen(errorMessage));
			printf("the string that was going to be displayed contains weird characters\r\n");
			return 0;
		}
	}
	clearLCD();
	//make sure we are on the foreground layer
	BSP_LCD_SelectLayer( 1 );
	// set variables to correct starting value
	char BufString[CHARS_ON_LINE+1];
	// y position of text
	uint16_t LineCnt = 0;
	// itterators to loop in text arrays
	uint16_t Count = 0;
	uint16_t CountTotal = 0;
	// counter to check if there is a word longer than the line
	uint16_t CountPrevious = CountTotal;
	// stay in the loop as long as the '\0' is not found
	while( textArray[CountTotal] != '\0')
	{
		// save chars in the temporary buffer
		BufString[ Count ] = textArray[CountTotal];
		// only add one char a time some the while above can detect the '\0'
		Count ++;
		CountTotal ++;
		// if 25 chars were found -> print them
		if( Count == CHARS_ON_LINE )
		{
			// going back untill we find a space
			while( BufString[ ( Count - 1 ) ] != ' ' )
			{
				//printf("%s\r\n",BufString);
				// fill the buffer with '\0' and reduce where we were reading in the string
				BufString[ ( Count - 1 ) ] = '\0';
				Count --;
				CountTotal --;

			}
			// check if there is a word that is longer than the line (the word can not fit on one line)
			if(CountPrevious == CountTotal)
			{
				textToLCD(errorMessage, strlen(errorMessage));
				printf("the string that was going to be displayed contains a word that is longer then the line\r\n");

				return 0;
			}
			CountPrevious = CountTotal;
			// also fill up the space char with a '\0'
			BufString[ ( ( uint8_t ) strlen( BufString ) - 1 ) ] = '\0';
			// print on the lcd
			BSP_LCD_DisplayStringAt( 0, LineCnt, ( uint8_t * ) BufString, CENTER_MODE );
			// go down one line
			LineCnt += 24;
			// reset char counter
			Count = 0;
		}
	}
	// print the last chars
	if( textArray[CountTotal] == '\0')
	{
		// the bufstring is still filled with data from above
		// just add '\0' to the end
		BufString[ Count ] = '\0';
		//afdrukken
		BSP_LCD_DisplayStringAt( 0, LineCnt, ( uint8_t * ) BufString, CENTER_MODE );

	}
	return 1;


}

/*
 * func	: prints picture to the LCD
 * para	: picture pointer to the picture that has to be printed
 * ret	: no return value
 */
void pictureToLCD(uint16_t* picture)
{
	// make sure we draw on the right layer
	BSP_LCD_SelectLayer( 0 );
	// drawpicture based on given pointer
	WDA_LCD_DrawBitmap(picture, ( ( LCD_WIDTH - PICTURE_X_PIXEL ) / 2 ) , ( LCD_HEIGHT - PICTURE_Y_PIXEL ), PICTURE_X_PIXEL, PICTURE_Y_PIXEL, LTDC_PIXEL_FORMAT_ARGB1555);
}
/*
 * func	: clears previous text of the LCD
 * para	: no parameters
 * ret	: no return value
 */
void clearLCD()
{
	// clear layer 1 so all previous text is gone
	BSP_LCD_SelectLayer( 1 );
	// switch to transparent to make overwrite text with 'invisible' plane
	BSP_LCD_SetTextColor( LCD_COLOR_TRANSPARENT );
	// fille full screen with plane
	BSP_LCD_FillRect( 0, 0 , LCD_WIDTH, LCD_HEIGHT );
	// switch back to black so next text is properly displayed
	BSP_LCD_SetTextColor( LCD_COLOR_BLACK );
}
