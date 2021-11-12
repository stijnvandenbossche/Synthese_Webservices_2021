/*
 * fileSystemAPI.h
 *
 *  Created on: 4 nov. 2021
 *      Author: tijnd
 */

#ifndef FILESYSTEMAPI_H_
#define FILESYSTEMAPI_H_
#include "main.h"
#include "lwip/apps/fs.h"
#include "fsdata_custom.h"

#define MAX_PATH_LENGTH 255
typedef enum {png, raw} imageExtension;
typedef enum {ext, no_ext} extensionState;

uint8_t getImageList(char* imageList[], imageExtension extType);
void* getRawImageData(char* imagePath, uint16_t pathLength);
uint8_t initFileSystemAPI(void);
uint8_t getImageAmount(void);
uint8_t getLargestNameLength(void);
void extractNameOutOfPath(char* pPath, uint16_t pathLength, char* pName, extensionState nameType);

#endif /* FILESYSTEMAPI_H_ */
