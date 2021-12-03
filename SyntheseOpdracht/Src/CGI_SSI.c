#include "CGI_SSI.h"


int fs_open_custom(struct fs_file *file, const char *name) {

	return 0;
}


void fs_close_custom(struct fs_file *file){

	return 0;
}


extern void httpd_cgi_handler(struct fs_file *file, const char* uri, int iNumParams,
                              char **pcParam, char **pcValue){
	for(int i = 0; i < iNumParams; i++){
		if(strcmp(pcParam[i], "msg") == 0){
			textToLCD(pcValue[i], strlen(pcValue[i]));
		}
		if(strcmp(pcParam[i], "photo") == 0){
			textToLCD(pcValue[i], strlen(pcValue[i]));
			//show photo on lcd screen
		}
	}
}


u16_t mySsiHandler(const char* ssi_tag_name, char *pcInsert, int iInsertLen){



	if(strcmp(ssi_tag_name, "photo") == 0){
		//vraag lijst op funtie
		char photolist[9][256];
		strcpy(photolist[0], "images/alien.png");
		strcpy(photolist[1], "images/chicken.png");
		strcpy(photolist[2], "images/derp.png");
		strcpy(photolist[3], "images/lion.png");
		strcpy(photolist[4], "images/poop.png");
		strcpy(photolist[5], "images/skull.png");
		strcpy(photolist[6], "images/tl.png");
		strcpy(photolist[7], "images/vogel.png");
		strcpy(photolist[8], "images/rick.png");

		//end lijst op funtie
		char teststr[2560];

		sprintf(pcInsert, "<h2 class = 'count'>%d Photo's were detected.</h2>", (sizeof photolist / sizeof photolist[0]));

		strcat(pcInsert, "<form><p class = 'photo'>");
		int leng = (sizeof photolist / sizeof photolist[0]);
		for(int i = 0; i < leng; i++){

			sprintf(teststr, "<img src = '%s' alt = 'photo %d' onclick =\"sendphoto(\'%s\')\">",photolist[i], i, photolist[i]);
			strcat(pcInsert, teststr);
		}
		strcat(pcInsert, "</form><p>");
	}

	return strlen(pcInsert);
}
