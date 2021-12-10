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

#define MAX_GIF_FRAMES 255

struct imageMetaData{
	char* name;
    void* data;
    uint16_t width;
    uint16_t height;
    uint8_t num;
    uint16_t frameTime;
};

typedef enum {png, gif} fileExtension;
typedef enum {ext, no_ext} extensionType;
typedef enum {lower, upper, initial} caseType;
typedef enum {a_z, z_a, no_sort} sortType;
typedef enum {stop_at_ext, stop_at_args, stop_at_any} pathStopType;

uint8_t getImageList(char* imageList[], fileExtension extType, sortType sortState);
uint8_t getRawImageMetaData(char* imagePath, uint16_t pathLength, struct imageMetaData* pMetaData);
uint8_t initFileSystemAPI(void);
uint8_t getImageAmount(void);
uint8_t getLargestNameLength(void);
void extractNameOutOfPath(char* pPath, uint16_t pathLength, char* pName, extensionType nameState, caseType caseState);
uint8_t getGifFrames(char* pGif, uint16_t pathLength, char* frameList[]);
uint8_t getGifAmount(void);

#endif /* FILESYSTEMAPI_H_ */
