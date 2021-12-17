#include "CGI_SSI.h"
struct imageMetaData buf = {.data = NULL, .name = NULL, .num = 0, .frameTime = 0, .height = 0, .width = 0};

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
			textToLCD(pcValue[i], strlen(pcValue[i]), LCD_COLOR_WHITE);
		}
		if(strcmp(pcParam[i], "photo") == 0){

			char buffer[255];
			buffer[0] = '\0';
			strcat(buffer, "/");
			strcat(buffer, pcValue[i]);
			getRawImageMetaData(buffer, strlen(buffer), &buf);
			pictureToLCD(buf);

			//show photo on lcd screen
		}
	}
}


u16_t mySsiHandler(const char* ssi_tag_name, char *pcInsert, int iInsertLen){



	if(strcmp(ssi_tag_name, "photo") == 0){
		//vraag lijst op funtie

		char* photolist[getImageAmount()];
		getImageList(photolist, png, a_z);


		//end lijst op funtie
		char teststr[2560];

		sprintf(pcInsert, "<h2 class = 'count'>%d Photo's were detected.</h2>", getImageAmount());

		strcat(pcInsert, "<div><p style = 'text-align: center;'>");
		for(int i = 0; i < getImageAmount(); i++){

			sprintf(teststr, "<img src = '%s' alt = 'photo %d' onclick =\"sendphoto(\'%s\')\" class = 'photo'>",photolist[i]+1, i, photolist[i]+1);
			strcat(pcInsert, teststr);
		}
		strcat(pcInsert, "</div></p>");

		//gifs
		char* giflist[getGifAmount()];
		getImageList(giflist, gif, a_z);


		//end lijst op funtie

		sprintf(teststr, "</br></br><h2 class = 'count'>%d Gifs were detected.</h2>", getGifAmount());
		strcat(pcInsert, teststr);

		strcat(pcInsert, "<div><p style = 'text-align: center;'>");
		for(int i = 0; i < getGifAmount(); i++){

			sprintf(teststr, "<img src = '%s' alt = 'photo %d' onclick =\"sendphoto(\'%s\')\" class = 'gif'>",giflist[i]+1, i, giflist[i]+1);
			strcat(pcInsert, teststr);
		}
		strcat(pcInsert, "</div></p>");


	}

	return strlen(pcInsert);
}
