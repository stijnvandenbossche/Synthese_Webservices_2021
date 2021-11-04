/*
 * fileSystemAPI.c
 *
 *  Created on: 4 nov. 2021
 *      Author: tijnd
 */
#include "fileSystemAPI.h"

static uint8_t validateImage(char* imagePath);
static uint8_t getPathLengthNoExt(char* pstr);
static uint8_t imageAmount = 0;
static uint8_t largestNameLength = 0;

/*
 * -Function: This function intializes the file system API. It has to be called before any API functions are used.
 *
 * -Parameters:
 *  void
 *
 * -Returns:
 *  void
 */
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

/*
 * -Function: This function returns the amount of valid images that are present in the file system.
 *
 * -Parameters:
 *  void
 *
 * -Returns:
 *  amount of valid images that are present in the file system.
 */
uint8_t getImageAmount(void)
{
	return imageAmount;
}

/*
 * -Function: This function returns the length of the largest filename from the file system.
 *
 * -Parameters:
 *  void
 *
 * -Returns:
 *  length of the largest filename from the file system.
 */
uint8_t getLargestNameLength(void)
{
	return largestNameLength;
}

/*
 * -Function: This function extracts the name of a file (with or without the extension) from the specified path.
 * 			  E.g: /Folder/file.png -> function returns file.png or file
 *
 * -Parameters:
 *  pPath -> a pointer to the path of the file whose name has to be extracted.
 *  pName -> a pointer where the extracted name will be stored.
 *  nameType -> specifies if the extension has to be removed from the name or not
 *
 * -Returns:
 *  void
 */
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

/*
 * -Function: This function generates a list with the file paths from all the valid images that are found in the file system.
 *
 * -Parameters:
 *  imageList -> a pointer where the list will be stored (array of strings).
 *  imageExtension -> specifies the desired file type (png or raw).
 *
 * -Returns:
 *  The amount of images present in the generated list.
 *  This will be 0 if there was an error or no valid images were found.
 *
 *  -Note: the column size of imageList has to be MAX_PATH_LENGTH.
 */
uint8_t getImageList(char imageList[][MAX_PATH_LENGTH], imageExtension extType)
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

/*
 * -Function: This function calculates a pointer to the start of the raw data of the specified image.
 *
 * -Parameters:
 *  imagePath -> specifies from which image data has to be retrieved.
 *
 * -Returns:
 *  NULL when there has been an error.
 *  Otherwise, it will return a pointer that points to the start of the raw data of the specified image.
 *
 *  -Note: regardless of the extension (or no extension) of imagePath, the returned pointer will point to the start of the data from the .raw file.
 */
char* getRawImageData(char* imagePath)
{
	char* dataPointer = NULL;
	if(validateImage(imagePath) != 0x00)
	{
		uint8_t pathLength = getPathLengthNoExt(imagePath);
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

/*
 * -Function: This function checks if there is both a .png and .raw file of the specified file present in the file system.
 *
 * -Parameters:
 *  imagePath -> specifies the image that has to be validated. This can be the .raw or .png file or the file path without any extensions.
 *
 * -Returns:
 *  0x01 when both the .png and .raw files are found.
 *  0x00 when both the .png and .raw files aren't found.
 *
 *  -Note: both the .raw and .png file NEED to be located in the same folder.
 */
static uint8_t validateImage(char* imagePath)
{
	uint8_t pathLength = getPathLengthNoExt(imagePath);
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

/*
 * -Function: This function generates the length of the file path minus the extensions.
 * 			  E.g: /Folder/file.png -> function returns length of /Folder/file
 *
 * -Parameters:
 *  pPath -> a pointer to the path whose length - extension has to be calculated.
 *
 * -Returns:
 *  The length of the path - extension
 *
 *  -Note: the returned length does not include the \0 (if present)
 *   Example: /img/file -> 9
 */
static uint8_t getPathLengthNoExt(char* pPath)
{
	uint8_t pathLength;
	char* pToDot;
	if((pToDot = strchr(pPath, '.')) != NULL)
	{
		pathLength = pToDot - pPath;
	}
	else
	{
		pathLength = strlen(pPath);
	}
	return pathLength;
}
