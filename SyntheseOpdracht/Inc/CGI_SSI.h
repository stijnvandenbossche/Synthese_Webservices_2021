#ifndef CGI_SSI_H_
#define CGI_SSI_H_

#include <LCD_functions.h>
#include "httpd.h"
#include "lwip/init.h"

u16_t mySsiHandler(const char*, char *, int);
#endif /* CGI_SSI_H_ */