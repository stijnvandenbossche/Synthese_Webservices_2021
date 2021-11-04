/*
 * fileSystemAPI.c
 *
 *  Created on: 4 nov. 2021
 *      Author: tijnd
 */
#include "fileSystemAPI.h"

static uint8_t validateImage(char* imagePath);
static uint8_t getStrLengthNoExt(char* pstr);
static uint8_t imageAmount = 0;
static uint8_t largestNameLength = 0;

void initFileSystemAPI(void)
{
	uint8_t nameLength = 0;
	char* pLastSlash;
	for(struct fsdata_file* f = FS_FIRST_FILE; f != NULL; f = f->next)
	{
		if(validateImage((char*)f->name) != 0x00)
		{
			pLastSlash = strrchr((const char*)f->name, '/');
			pLastSlash = (pLastSlash == NULL)? f->name : pLastSlash + 1;
			largestNameLength = ((strlen(pLastSlash) + 1 ) > largestNameLength)? (strlen(pLastSlash) + 1) : largestNameLength;
			if(strstr((const char*)f->name, ".png") != NULL)
			{
				imageAmount++;
			}
		}
	}
}
uint8_t getImageAmount(void)
{
	return imageAmount;
}
uint8_t getLargestNameLength(void)
{
	return largestNameLength;
}
void extractNameOutOfPath(char* pPath, char* pName, extensionState nameType)
{
	char* pLastSlash = strrchr(pPath, '/');
	pLastSlash = (pLastSlash == NULL)? pPath : pLastSlash + 1;
	char* pDot = strchr(pPath, '.');
	pDot = (pDot == NULL)? pPath + strlen(pPath) : pDot;
	memset(pName, '\0', largestNameLength);
	if(nameType == ext)
	{
		strcpy(pName, pLastSlash);
	}
	else if(nameType == no_ext)
	{
		strncpy(pName, pLastSlash, pDot - pLastSlash);
	}
}
uint8_t getImageList(char imageList[][100], imageExtension extType)
{
	uint8_t imageCnt = 0;
	for(struct fsdata_file* f = FS_FIRST_FILE; f != NULL; f = f->next)
	{
		if(validateImage((char*)f->name) != 0x00)
		{
			if((extType == png && strstr((const char*)f->name, ".png") != NULL) || (extType == raw && strstr((const char*)f->name, ".raw") != NULL))
			{
				strcpy(*(imageList + imageCnt), (const char*)f->name);
				imageCnt++;
			}
		}
	}
	return imageCnt;
}

char* getRawImageData(char* imagePath)
{
	char* dataPointer = NULL;
	if(validateImage(imagePath) != 0x00)
	{
		uint8_t pathLength = getStrLengthNoExt(imagePath);
		char rawName[pathLength + 5];
		memset(rawName, '\0', pathLength + 5);
		strncpy(rawName, imagePath, pathLength);
		strcat(rawName, ".raw");

		for(struct fsdata_file* f = FS_FIRST_FILE; f != NULL; f = f->next)
		{
			if(strcmp((const char*)f->name, rawName) == 0)
			{
				dataPointer = strstr((const char*)f->data, "\r\n\r\n") + 4;
			}
		}
	}
	return dataPointer;
}

static uint8_t validateImage(char* imagePath)
{
	uint8_t pathLength = getStrLengthNoExt(imagePath);
	uint8_t pngFound = 0;
	uint8_t rawFound = 0;
	char rawName[pathLength + 5];
	char pngName[pathLength + 5];
	memset(rawName, '\0', pathLength + 5);
	strncpy(rawName, imagePath, pathLength);
	strcat(rawName, ".raw");
	memset(pngName, '\0', pathLength + 5);
	strncpy(pngName, imagePath, pathLength);
	strcat(pngName, ".png");

	for(struct fsdata_file* f = FS_FIRST_FILE; f != NULL; f = f->next)
	{
		// TODO: check for capital letters in ext.
		pngFound = (strcmp((const char*)f->name, pngName) == 0)? 1 : pngFound;
		rawFound = (strcmp((const char*)f->name, rawName) == 0)? 1 : rawFound;
	}
	return (pngFound == 1 && rawFound == 1)? 1 : 0;
}

static uint8_t getStrLengthNoExt(char* pstr)
{
	uint8_t strLength;
	char* pToDot;
	if((pToDot = strchr(pstr, '.')) != NULL)
	{
		strLength = pToDot - pstr;
	}
	else
	{
		strLength = strlen(pstr);
	}
	return strLength;
}
