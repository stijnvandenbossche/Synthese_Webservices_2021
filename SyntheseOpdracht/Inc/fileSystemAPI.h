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

typedef enum {png, raw} imageExtension;
typedef enum {ext, no_ext} extensionState;
uint8_t getImageList(char imageList[][100], imageExtension extType);
char* getRawImageData(char* imagePath);
void initFileSystemAPI(void);
uint8_t getImageAmount(void);
uint8_t getLargestNameLength(void);
void extractNameOutOfPath(char* pPath, char* pName, extensionState nameType);
#endif /* FILESYSTEMAPI_H_ */
