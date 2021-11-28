/*!
 *  \file fileSystemAPI.c
 *	\details This file contains all the function implementations from the filesystem API. Static functions are used in the API, but are not accessible for the user (and not needed).
 *  \details A file path can exist of the following parts: /Folder/Folder/.../name__arg__arg.ext The arguments are only required with .raw files (width and height).
 *  \remark Every file path length variable, parameter and return value is of type uint16_t. This is currently not really needed but it is used so that the max path length can be easily enlarged in the future.
 *  \date 4 nov. 2021
 *  \author Tijn De Wever
 */
#include "fileSystemAPI.h"
static uint8_t validateImage(char* imagePath, uint16_t pathLength);
static void convExtToLowerCase(char* pOrgPath, uint16_t orgPathLength, char* pModPath, uint16_t modPathSize);
static void insertImagePath(char* imageList[], uint8_t imageAmount, char* newImage, sortType sortState);
static uint16_t getPathLength(char* pPath, uint16_t pathLength, pathStopType stopMode);
static uint8_t extractArgsOutOfPath(char* pPath, uint16_t pathLength);
static uint8_t imageAmount = 0;
static uint8_t largestNameLength = 0;
extern const struct fsdata_file* const pFirstFile;


/*!
 *  \brief This function initializes the file system API. It has to be called before any API functions are used.
 *
 *  \param void
 *
 *  \retval 1 when the function has succeeded.
 *  \retval 0 when the function has failed. (The path of a file is longer than MAX_PATH_LENGTH)
 */
uint8_t initFileSystemAPI(void)
{
	char* pLastSlash;
	char pathBuffer[MAX_PATH_LENGTH];
	uint8_t returnVal = 1;

	for(struct fsdata_file* f = (struct fsdata_file*)pFirstFile; f != NULL; f = (struct fsdata_file*)f->next)
	{
		// Check if the length of the full file path is smaller than MAX_PATH_LENGTH. When >= -> ERROR.
		returnVal = (strlen((const char*)f->name) + 1 >= MAX_PATH_LENGTH)? 0 : returnVal;
		// This if statement is used to check if the file f is a valid image.
		if(validateImage((char*)f->name, strlen((const char*)f->name)) != 0x00)
		{
			// The last '/' is searched in order to get the name + extension out of the path.
			pLastSlash = strrchr((const char*)f->name, '/');
			// When no '/' has been found -> pLastSlash = start address of the file path.
			pLastSlash = (pLastSlash == NULL)? (char*)f->name : pLastSlash + 1;

			// The length of the name is compared with largestNameLength so that largestNameLength can be updated when necessary.
			largestNameLength = ((strlen(pLastSlash) + 1 ) > largestNameLength)? (strlen(pLastSlash) + 1) : largestNameLength;

			// convExtToLowerCase is called to convert the extension of the file f to lowercase. This way it doesn't matter whether the extension is e.g a .RAW or .raw .
			convExtToLowerCase((char*)f->name, strlen((const char*)f->name), pathBuffer, sizeof(pathBuffer));
			// This if statement is used to check whether the file f is an image or not.
			if(strstr(pathBuffer, ".png") != NULL)
			{
				imageAmount++;
			}
		}
	}
	return returnVal;
}


/*!
 *  \brief This function returns the amount of valid images that are present in the file system.
 *
 *  \param void
 *
 *  \return Amount of valid images that are present in the file system.
 */
uint8_t getImageAmount(void)
{
	return imageAmount;
}


/*!
 *  \brief This function returns the length of the largest filename from the file system.
 *
 *  \param void
 *
 *  \return Length of the largest filename from the file system.
 */
uint8_t getLargestNameLength(void)
{
	return largestNameLength;
}


/*!
 *  \brief This function extracts the name of a file (with or without the extension) from the specified path.
 *  	   E.g: /Folder/file.png -> function returns file.png or file
 *
 *  \param pPath -> a pointer to the file path whose filename has to be extracted.
 *  \param pathLength -> the length of the path. (Length of the string without \0)
 *  \param pName -> a pointer to the location where the extracted name will be stored.
 *  \param nameState -> specifies if the extension has to be removed from the name or not
 *	\param caseState -> specifies the desired character case (upper, lower, initial)
 *
 *  \return void
 *
 *  \warning The size of the name array which is referenced by pName HAS to be largestNameLength (which can be acquired through getLargestNameLength()).
 *  \remark Arguments of .raw files are omitted and thus not extracted.
 */
void extractNameOutOfPath(char* pPath, uint16_t pathLength, char* pName, extensionType nameState, caseType caseState)
{
	// This function searches for the first '/' and last '.' character. With these results can the name be cut out of the path.

	// memrchr and memchr are used because it is uncertain that the path string contains a \0.
	char* pLastSlash = (char*)memrchr(pPath, '/', pathLength);
	char* endOfName = pPath + getPathLength(pPath, pathLength, stop_at_any);
	char* pDot = (char*)memchr(pPath, '.', pathLength);
	uint8_t charCnt = 0;

	// If there is no '/' found -> pLastSlash = start address of the path string.
	pLastSlash = (pLastSlash == NULL)? pPath : pLastSlash + 1;

	// If there is no '.' found -> pDot = start address of the path string + pathLength as offset.
	pDot = (pDot == NULL)? pPath + pathLength : pDot;
	memset(pName, '\0', largestNameLength);
	strncpy(pName, pLastSlash, endOfName - pLastSlash);
	if(nameState == ext)
	{
		strcat(pName, pDot);
	}
	while(*(pName + charCnt) != '\0' && caseState != initial)
	{
		if(caseState == lower && *(pName + charCnt) >= 'A' && *(pName + charCnt) <= 'Z')
		{
			*(pName + charCnt) += 0x20;
		}
		else if(caseState == upper && *(pName + charCnt) >= 'a' && *(pName + charCnt) <= 'z')
		{
			*(pName + charCnt) -= 0x20;
		}
		charCnt++;
	}
}


/*!
 *  \brief This function generates a list of char pointers to the file paths from all the valid images that are found in the file system.
 *
 *  \param imageList -> an array where the list will be stored (array of char pointers. Each char pointer points to the start of a file path string).
 *  \param imageExtension -> specifies the desired file type (png or raw).
 *	\param sortState -> specifies the desired output order (a_z, z_a, no_sort)
 *
 *  \return The amount of images present in the generated list.
 *  \return This amount will be 0 if an error has occurred or no valid images were found.
 *
 *  \warning The array size HAS to be equal to imageAmount (which can be acquired through getImageAmount).
 */
uint8_t getImageList(char* imageList[], imageExtension extType, sortType sortState)
{
	uint8_t imageCnt = 0;
	char pathBuffer[MAX_PATH_LENGTH];

	for(struct fsdata_file* f = (struct fsdata_file*)pFirstFile; f != NULL; f = (struct fsdata_file*)f->next)
	{
		// validateImage is called to check if the file f is valid.
		if(validateImage((char*)f->name, strlen((const char*)f->name)) != 0x00)
		{
			// convExtToLowerCase is called to convert the extension of the file f to lowercase. This way it doesn't matter whether the extension is e.g a .RAW or .raw .
			convExtToLowerCase((char*)f->name, strlen((const char*)f->name), pathBuffer, sizeof(pathBuffer));
			if((extType == png && strstr(pathBuffer, ".png") != NULL) || (extType == raw && strstr(pathBuffer, ".raw") != NULL))
			{
				insertImagePath(imageList, imageCnt, (char*)f->name, sortState);
				imageCnt++;
			}
		}
	}
	return imageCnt;
}


/*!
 *  \brief This function inserts a new image path in imageList at the correct location (place), which can be specified by sortState.
 *
 *  \param imageList -> a pointer to the array that contains the sorted images and were the new image has to be inserted in
 *  \param imagesInList -> specifies the amount of sorted images present in imageList
 *	\param newImage -> a pointer to the path of the image that has to be inserted in imageList
 *	\param sortState ->specifies the desired sort order (a_z, z_a, no_sort)
 *
 *  \return void
 */
static void insertImagePath(char* imageList[], uint8_t imagesInList, char* newImagePath, sortType sortState)
{
	uint8_t index;
	uint8_t sortedFlag = 0;
	char nameBuf[largestNameLength];
	char newNameBuf[largestNameLength];
	int8_t compareRes;

	extractNameOutOfPath(newImagePath, strlen(newImagePath), newNameBuf, ext, lower);
	// This if statement is used to check if the list has to be sorted + check whether the list is empty and thus cannot be sorted.
	if(imagesInList == 0 || (sortState != a_z && sortState != z_a))
	{
		*(imageList + imagesInList) = newImagePath;
	}
	else
	{
		index = 0;
		while(index < imagesInList && sortedFlag == 0)
		{
			extractNameOutOfPath(*(imageList + index), strlen(*(imageList + index)), nameBuf, ext, lower);
			compareRes = strcmp(newNameBuf, nameBuf);

			if((sortState == a_z && compareRes <= 0) || (sortState == z_a && compareRes >= 0))
			{
				// This for loop is used to shift every image path from [index] -> [end of the list] one index to the right.
				for(uint8_t copyIndex = imagesInList; copyIndex > index; copyIndex--)
				{
					*(imageList + copyIndex) = *(imageList + copyIndex - 1);
				}
				*(imageList + index) = newImagePath;
				sortedFlag = 1;
			}
			// This if statement is used to check if the end of the list - 1 has been reached. If so -> insert new image path at the back.
			else if(((sortState == a_z && compareRes > 0) || (sortState == z_a && compareRes < 0)) && index == imagesInList - 1)
			{
				*(imageList + imagesInList) = newImagePath;
				sortedFlag = 1;
			}
			index++;
		}
	}

}


/*!
 *  \brief This function calculates a pointer to the start of the raw data of the specified image.
 *
 *  \param imagePath -> specifies from which image the data has to be retrieved.
 *  \param pathLength -> the length of the image path. (Length of the string without \0)
 *
 *  \return A void pointer that points to the start of the raw data from the specified image. Depending on the image format this pointer will need to be casted. (E.g. ARGB1555 -> cast to (uint16_t*))
 *	\return NULL when an error has occurred or the image hasn't been found.
 *
 *  \remark Regardless of the extension type (or no extension) of imagePath, the returned pointer will point to the start of the data from the .raw file.
 */
void* getRawImageData(char* imagePath, uint16_t pathLength)
{
	// In this function the specified image path without extension or arguments is compared to the names of the files in the file system.
	// When both match and the file from the file system is a raw image, the correct raw file is found and a pointer to the image data is returned.

	char* dataPointer = NULL;
	uint16_t lengthUntilArgsOrExt;
	char pathBuffer[MAX_PATH_LENGTH];
	// validateImage is called to check if the specified image is valid.
	if(validateImage(imagePath, pathLength) != 0x00)
	{
		// getPathLength is called to get the length of the path without arguments or the extension.
		lengthUntilArgsOrExt = getPathLength(imagePath, pathLength, stop_at_any);

		// This loop is used to check every file in the fs.
		for(struct fsdata_file* f = (struct fsdata_file*)pFirstFile; f != NULL; f = (struct fsdata_file*)f->next)
		{
			// convExtToLowerCase is called to convert the extension of the file f to lowercase. This way it doesn't matter whether the extension is e.g a .RAW or .raw .
			convExtToLowerCase((char*)f->name, strlen((const char*)f->name), pathBuffer, sizeof(pathBuffer));
			if(strncmp(pathBuffer, imagePath, lengthUntilArgsOrExt) == 0 && strstr(pathBuffer, ".raw") != NULL)
			{
				dataPointer = (char*)f->data;
			}
		}
	}
	return dataPointer;
}


/*!
 *  \brief This function checks if there is both a .png and .raw file of the specified file present in the file system.
 *
 *  \param imagePath -> specifies the image that has to be validated. This can be the .raw or .png file or the file path without any extensions.
 *  \param pathLength -> the length of the image path. (Length of the string without \0)
 *
 *  \retval 0x01 when both the .png and .raw files are found and the arguments of the .raw file are valid.
 *  \retval 0x00 if the .png or .raw file isn't found or when the arguments of the .raw file are not valid.
 *
 *  \warning Both the .raw and .png file NEED to be located in the same folder.
 */
static uint8_t validateImage(char* imagePath, uint16_t pathLength)
{
	uint16_t lengthUntilArgsOrExt = getPathLength(imagePath, pathLength, stop_at_any);
	char pathBuffer[MAX_PATH_LENGTH];
	uint8_t pngFound = 0;
	uint8_t rawFound = 0;
	uint8_t argsValid = 0;

	for(struct fsdata_file* f = (struct fsdata_file*)pFirstFile; f != NULL; f = (struct fsdata_file*)f->next)
	{
		// convExtToLowerCase is called to convert the extension of the file f to lowercase. This way it doesn't matter whether the extension is e.g a .PNG or .png .
		convExtToLowerCase((char*)f->name, strlen((const char*)f->name), pathBuffer, sizeof(pathBuffer));
		if(getPathLength((char*)f->name, strlen((const char*)f->name), stop_at_any) == lengthUntilArgsOrExt && strncmp(imagePath, (const char*)f->name, lengthUntilArgsOrExt) == 0)
		{
			pngFound = (strstr(pathBuffer, ".png") != NULL)? 1 : pngFound;
			rawFound = (strstr(pathBuffer, ".raw") != NULL)? 1 : rawFound;
			argsValid = (rawFound == 1 && extractArgsOutOfPath((char*)f->name, strlen((const char*)f->name)) == 1)? 1 : argsValid;
		}
	}
	return (argsValid == 1 && pngFound == 1 && rawFound == 1)? 1 : 0;
}


/*!
 *  \brief This function extracts the arguments out of the given path.
 *
 *  \param pPath -> a pointer to the file path whose arguments have to be extracted.
 *  \param pathLength -> the length of the file path. (Length of the string without \0)
 *
 *  \retval 0x01 when arguments where found which were all valid.
 *  \retval 0x00 when invalid or no arguments were found or if an error has occurred.
 *
 */
static uint8_t extractArgsOutOfPath(char* pPath, uint16_t pathLength)
{
	char pathBuffer[MAX_PATH_LENGTH];
	char* pStartArg;
	uint16_t argValBuf;
	uint16_t lengthBeforeArgs = getPathLength(pPath, pathLength, stop_at_args);
	uint16_t lengthBeforeExt = getPathLength(pPath, pathLength, stop_at_ext);
	uint8_t retVal = 0;
	convExtToLowerCase(pPath, pathLength, pathBuffer, sizeof(pathBuffer));
	if(strstr(pathBuffer, ".raw") != NULL && (lengthBeforeExt - lengthBeforeArgs) == strlen("__xxx__xxx"))
	{
		if((pStartArg = strstr(pathBuffer + lengthBeforeArgs, "__")) != NULL && strstr(pathBuffer + lengthBeforeArgs + 5, "__") != NULL)
		{
			retVal = 1;
			argValBuf = strtol(pStartArg + 2, NULL, 10);
			retVal = (argValBuf < MIN_IMAGE_WIDTH || argValBuf > MAX_IMAGE_WIDTH)? 0 : retVal;
			pStartArg += 5;
			argValBuf = strtol(pStartArg + 2, NULL, 10);
			retVal = (argValBuf < MIN_IMAGE_HEIGHT || argValBuf > MAX_IMAGE_HEIGHT)? 0 : retVal;
		}
	}
	return retVal;
}

/*!
 *  \brief This function generates the length of the file path minus the extensions or arguments + extension.
 *
 *  \param pPath -> a pointer to the path whose (length - extension) has to be calculated.
 *  \param pathLength -> the length of the path. (Length of the string without \0)
 *	\param pathStopType -> specifies the location where the function has to stop calculating the path length.
 *
 *  \return The calculated length or pathLength if the specified part (arguments/extension) is not found.
 *
 *  \remark The returned length does NOT include the \0 (if present)
 *  		Example: /img/file -> 9 characters
 */
static uint16_t getPathLength(char* pPath, uint16_t pathLength, pathStopType stopMode)
{
	uint16_t newLength = pathLength;
	char* pToSymbol;
	// This if statement is used to check if pPath ends with a \0.
	if(*(pPath + pathLength) == '\0')
	{
		if(stopMode == stop_at_args || stopMode == stop_at_any)
		{
			// This if statement is used to get a pointer to the first "__" string and to check if there are arguments present in the path (arguments have to start with __).
			if((pToSymbol = (char*)strstr(pPath, "__")) != NULL)
			{
				// The length of the path - start of arguments is calculated by subtracting the pToSymbol address with the start address of the path.
				newLength = pToSymbol - pPath;
			}
		}
		if(stopMode == stop_at_ext || (stopMode == stop_at_any && newLength == pathLength))
		{
			// This if statement is used to get a pointer to the first '.' character and to check if there is a '.' present in the path.
			if((pToSymbol = (char*)strchr(pPath, '.')) != NULL)
			{
				// The length of the path - extension is calculated by subtracting the pToSymbol address with the start address of the path.
				newLength = pToSymbol - pPath;
			}
		}
	}
	return newLength;
}

/*!
 *  \brief This function converts the extension of the filename to lowercase letters.
 * 			E.g: /Folder/file.PNG -> /Folder/file.png
 *
 *  \param pOrgPath -> a pointer to the original path.
 *  \param orgPathLength -> the length of the original path. (Length of the string without \0)
 *  \param pModPath -> a pointer to the array where the modified path will be stored.
 *  \param modPathSize -> the size of the array where pModPath points to.
 *
 *  \return void
 *
 *  \remark It is recommended that the size of the array referenced by pModPath (modPathSize) is equal to MAX_PATH_LENGTH. This guarantees that the modified path will always fit into the array.
 *  \remark The array referenced by pModPath will ALWAYS be cleared at the start of this function.
 */
static void convExtToLowerCase(char* pOrgPath, uint16_t orgPathLength, char* pModPath, uint16_t modPathSize)
{
	char* pDot;
	memset(pModPath, '\0', modPathSize);
	// This if statement is used to check if the original path can fit in the array referenced by pModPath.
	// A modified path will only be created when the statement is true.
	if(orgPathLength < modPathSize)
	{
		strncpy(pModPath, pOrgPath, orgPathLength);
		// This if statement is used to get a pointer to the first '.' character and to check if there is a '.' present in the path.
		if((pDot = strchr(pModPath, '.')) != NULL)
		{
			pDot++;
			// Change every uppercase letter to lowercase until the end of the path is reached.
			while(*pDot != '\0')
			{
				*pDot = (*pDot >= 'A' && *pDot <= 'Z')? *pDot + 0x20 : *pDot;
				pDot++;
			}
		}
	}

}



