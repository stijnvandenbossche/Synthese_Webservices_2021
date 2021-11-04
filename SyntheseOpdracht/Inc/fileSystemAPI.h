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

uint8_t getImageList(char imageList[][100]);
char* getImageData(char* imagePath);
#endif /* FILESYSTEMAPI_H_ */
