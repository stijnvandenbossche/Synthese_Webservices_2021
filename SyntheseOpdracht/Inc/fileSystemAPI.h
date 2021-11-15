/*!
 *  \file fileSystemAPI.h
 *	\details This file contains all the function prototypes from the filesystem API.
 *  \date 4 nov. 2021
 *  \author Tijn De Wever
 */
#ifndef FILESYSTEMAPI_H_
#define FILESYSTEMAPI_H_
#include "main.h"
#include "lwip/apps/fs.h"

/*!
 * \def MAX_PATH_LENGTH
 * MAX_PATH_LENGTH defines the maximum file path length. It's value is set equal to the max path length from makefsdata, which is 256.
 */
#define MAX_PATH_LENGTH 256
typedef enum {png, raw} imageExtension;
typedef enum {ext, no_ext} extensionState;

uint8_t getImageList(char* imageList[], imageExtension extType);
void* getRawImageData(char* imagePath, uint16_t pathLength);
uint8_t initFileSystemAPI(void);
uint8_t getImageAmount(void);
uint8_t getLargestNameLength(void);
void extractNameOutOfPath(char* pPath, uint16_t pathLength, char* pName, extensionState nameType);

#endif /* FILESYSTEMAPI_H_ */
