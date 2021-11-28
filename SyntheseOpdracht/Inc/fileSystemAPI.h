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
#define MIN_IMAGE_WIDTH 1
#define MAX_IMAGE_WIDTH 240
#define MIN_IMAGE_HEIGHT 1
#define MAX_IMAGE_HEIGHT 272
struct imageMetaData{
	char* name;
    void* data;
    uint16_t width;
    uint16_t height;
    uint16_t num;
    uint16_t frameTime;
};

typedef enum {png, raw} imageExtension;
typedef enum {ext, no_ext} extensionType;
typedef enum {lower, upper, initial} caseType;
typedef enum {a_z, z_a, no_sort} sortType;
typedef enum {stop_at_ext, stop_at_args, stop_at_any} pathStopType;

uint8_t getImageList(char* imageList[], imageExtension extType, sortType sortState);
uint8_t getRawImageMetaData(char* imagePath, uint16_t pathLength, struct imageMetaData* pMetaData);
uint8_t initFileSystemAPI(void);
uint8_t getImageAmount(void);
uint8_t getLargestNameLength(void);
void extractNameOutOfPath(char* pPath, uint16_t pathLength, char* pName, extensionType nameState, caseType caseState);

#endif /* FILESYSTEMAPI_H_ */
