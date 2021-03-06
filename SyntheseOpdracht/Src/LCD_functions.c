/*!
 *	\file BSP_functions.c
 *
 *  \date 8 nov. 2021
 *  \author Jonas Aertgeerts
 */
#include "LCD_functions.h"
// include picture that will be shown when something is wrong with the images
#include "errorPicture.h"

//declaration of used variables

// error message that will be displayed on the LCD when there is something wrong with the text
static char errorMessageText[TEXT_BUFFER_LENGTH] = "something went wrong while printing the string (see Serial terminal for more info)";

// timerhandler for hardware timer
extern TIM_HandleTypeDef htim2;

// lcd handler
extern LTDC_HandleTypeDef hltdc;

// to light up screen when new image is displayed
extern uint32_t ScreensaverStart;

// var to store how many chars fit on one line
uint8_t charsOnLine;

// struct to save picture data that is currently displayed
struct imageMetaData currentPicture;

// list of all frames of one gif
char* frameList[MAX_GIF_FRAMES];

// store the amount of frames of the gif
uint8_t frameAmount;
// to itterate over all gif frames
uint8_t frameCounter;


/* print one frame/picture to the LCD */
static void frameToLCD(void* data, uint16_t width, uint16_t height);
/* sets the time for the timer interrupt routine */
static void setTimer_ms(uint16_t time_ms);
/* start timer to receive timer interrupts */
static void startTimer(void);
/* stop timer to receive no more timer interrupts */
static void stopTimer(void);

/*!
 * \brief LCD Initialization for normal operation.
 *
 * \param void
 *
 * \retval void
 *
 *  \remark please make sure there are no words longer then 25 characters in the string.
 */
void initLCD(void)
{
	  BSP_LCD_Init();
	  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS );
	  BSP_LCD_LayerDefaultInit(0, (LCD_FB_START_ADDRESS+(LCD_WIDTH*LCD_HEIGHT*4 )));

	  BSP_LCD_DisplayOn();

	  // background layer is now white
	  BSP_LCD_SelectLayer(0);
	  BSP_LCD_Clear(LCD_COLOR_WHITE);


	  // text and pictures will be displayed on foreground layer
	  BSP_LCD_SelectLayer( 1 );
	  // layer is made transparent so background is visible
	  BSP_LCD_Clear(LCD_COLOR_TRANSPARENT);


	  // select proper font
	  BSP_LCD_SetFont(&Font12);
	  // calculate the amount of chars that fit on one line
	  charsOnLine = ((LCD_WIDTH-11)/2)/Font12.Width;

	  // draw line in the middle of the screen to devide screen in two parts
	  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	  BSP_LCD_DrawLine(LCD_WIDTH/2-1, 0, LCD_WIDTH/2-1, LCD_HEIGHT);

	  // set text and text background color
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
}

/*!
 * \brief prints text to the LCD.
 *
 * \param textArray -> array containing the string that has to be printed.
 * \param len -> the amount of characters that has to be printed
 * \param color -> color the text will be printed in
 *
 *
 * \retval len when the function has succeeded.
 * \retval 0 when the function has failed.
 *
 * \note please don't use black text color because the text background is black
 * \note red text color can be used for errors
 */
int textToLCD(char textArray[TEXT_BUFFER_LENGTH], int len, uint32_t color)
{
	// check if length is valid
	if(len > TEXT_BUFFER_LENGTH)
	{
		textToLCD(errorMessageText, strlen(errorMessageText),LCD_COLOR_RED);
		printf("the string that was going to be displayed is to long in total\r\n");
		return 0;
	}
	// make sure there is a '\0' at the end
	textArray[len] = '\0';
	// check if there are any weird characters in the string
	for(int i = 0 ; i < len; i++)
	{
		if(textArray[i] < ' ')
		{
			textToLCD(errorMessageText, strlen(errorMessageText),LCD_COLOR_RED);
			printf("the string that was going to be displayed contains weird characters\r\n");
			return 0;
		}
	}
	//light up screen
	ScreensaverStart = HAL_GetTick() + SCREENSAVER_DELAY;
	HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);
	//clear text
	clearText();
	BSP_LCD_SetTextColor(color);
	// small string to save the line that is going to be printed.
	// one char longer than max length for '\0'
	char BufString[charsOnLine+1];
	// y position of text
	uint16_t LineCnt = 10;
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
		if( Count == charsOnLine )
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
			BSP_LCD_DisplayStringAt( 5, LineCnt, ( uint8_t * ) BufString, LEFT_MODE );
			// go down one line
			LineCnt += 12;
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
		BSP_LCD_DisplayStringAt( 5, LineCnt, ( uint8_t * ) BufString, LEFT_MODE );

	}
	// return len to indicate all went well
	return len;
}

/*!
 * \brief prints picture to the LCD.
 *
 * \param picture -> struct with picture data
 *
 * \retval 1 when the function has succeeded.
 * \retval 0 when the picture is not displayed.
 *
 */
uint8_t pictureToLCD(struct imageMetaData picture)
{
	//light up screen
	ScreensaverStart = HAL_GetTick() + SCREENSAVER_DELAY;
	HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

	// save which picture is printed
	currentPicture = picture;
	// check image size
	if(currentPicture.width > MAX_IMAGE_WIDTH || currentPicture.height > MAX_IMAGE_HEIGHT)
	{
		stopTimer();
		//remove previous picture
		clearPicture();
		// print the error picture
		frameToLCD((void*)ERROR_PICTURE_DATA, ERROR_PICTURE_DATA_X_PIXEL, ERROR_PICTURE_DATA_Y_PIXEL);
		printf("something went wrong while printing the picture, it is to big\r\n");
		return 0;
	}
	else
	{
		// indicates it's a regular picture
		if(picture.frameTime == 0)
		{
			// no interrupts needed
			stopTimer();
			//remove previous picture
			clearPicture();
			// draw picture based on given pointer
			frameToLCD(currentPicture.data, currentPicture.width, currentPicture.height);
		}
		// it's a gif
		else
		{
			//remove previous picture
			clearPicture();
			// store the amount of frames
			frameAmount = getGifFrames(currentPicture.name, strlen(currentPicture.name), frameList);
			// start with first frame
			frameCounter = 0;
			// set interrupt interval
			setTimer_ms(picture.frameTime);
			// start the timer
			startTimer();
		}
		return 1;
	}
}

/*!
 * \brief print one frame/picture to the LCD.
 *
 * \param data -> pointer to data
 * \param width -> width of the picture
 * \param height -> height of the picture
 *
 * \retval void
 *
 */
static void frameToLCD(void* data, uint16_t width, uint16_t height)
{
	// wait for vsync
	while(!(hltdc.Instance->CDSR & 1<<2));
	// draw frame/image
	WDA_LCD_DrawBitmap((uint16_t*)data, (LCD_WIDTH/2) +  ( ( (LCD_WIDTH/2) - width ) / 2 ) , ( LCD_HEIGHT - height ) / 2, width, height, LTDC_PIXEL_FORMAT_ARGB1555);
}
/*!
 * \brief clears previous text of the LCD.
 *
 * \param void
 *
 * \retval void
 *
 */
void clearText(void)
{
	// switch to transparent to overwrite text with 'invisible' plane
	BSP_LCD_SetTextColor( LCD_COLOR_TRANSPARENT );
	// fill left screen half with plane
	BSP_LCD_FillRect( 0, 0 , LCD_WIDTH/2-1, LCD_HEIGHT );
}

/*!
 * \brief clears previous picture of the LCD.
 *
 * \param void
 *
 * \retval void
 *
 */
void clearPicture(void)
{
	// switch to transparent to overwrite previous picture with 'invisible' plane
	BSP_LCD_SetTextColor( LCD_COLOR_TRANSPARENT );
	// fill right screen half with plane
	BSP_LCD_FillRect( (LCD_WIDTH/2), 0 , LCD_WIDTH/2, LCD_HEIGHT );
}

/*!
 * \brief reads status of onboard blue button
 *
 * \param void
 *
 *  \retval 1 when blue button is pushed
 *  \retval 0 when blue button is released
 *
 */
uint8_t readButton(void)
{
	// read input register
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
 * \param time_ms time in ms for the interrupt routine
 *
 * \retval void
 *
 */
static void setTimer_ms(uint16_t time_ms)
{
	// set reload register
	htim2.Instance->ARR = (time_ms * 2) - 1;
}

/*!
 * \brief timer interrupt callback
 *
 * \param
 *  htim timer that generated the interrupt
 *
 * \retval void
 *
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	// check if interrupt came from right timer
	if(htim == &htim2)
	{
		// get image data and place it in local struct
		getRawImageMetaData(frameList[frameCounter], strlen(frameList[frameCounter]), &currentPicture);
		// print current frame
		frameToLCD(currentPicture.data, currentPicture.width, currentPicture.height);
		// increase framecounter
		frameCounter++;
		// resetcounter if it was last frame
		if(frameCounter == frameAmount)
		{
			frameCounter = 0;
		}
	}
}

/*!
 * \brief start timer to receive timer interrupts
 *
 * \param void
 *
 * \retval void
 *
 */
static void startTimer(void)
{
	// start timer
	HAL_TIM_Base_Start_IT(&htim2);
}

/*!
 * \brief stop timer to receive no more timer interrupts
 *
 * \param void
 *
 * \retval void
 *
 */
static void stopTimer(void)
{
	// stop timer
	HAL_TIM_Base_Stop_IT(&htim2);
}
