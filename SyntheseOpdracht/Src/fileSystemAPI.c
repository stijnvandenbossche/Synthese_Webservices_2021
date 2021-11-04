/*
 * fileSystemAPI.c
 *
 *  Created on: 4 nov. 2021
 *      Author: tijnd
 */
#include "fileSystemAPI.h"

uint8_t getImageList(char imageList[][100])
{
	uint8_t imageCnt = 0;
	for(struct fsdata_file* f = FS_FIRST_FILE; f != NULL; f = f->next)
	{
		if(strstr((const char*)f->name, ".png") != NULL)
		{
			strcpy(*(imageList + imageCnt), (const char*)f->name);
			imageCnt++;
		}
	}
	return imageCnt;
}

char* getImageData(char* imagePath)
{
	char* dataPointer = NULL;
	for(struct fsdata_file* f = FS_FIRST_FILE; f != NULL; f = f->next)
	{
		if(strcmp((const char*)f->name, imagePath) == 0)
		{
			dataPointer = strstr((const char*)f->data, "\r\n\r\n") + 4;// + 4);
		}
	}
	return dataPointer;
}
