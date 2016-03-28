#include "stdafx.h"
char* return_content_type(char* filename) {
	char type[10];
	char tmpType[10];
	char return_string[15];
	char type_position = 0;
	int tmp_position = 0;
	int file_position = strlen(filename) - 1;
	for (; file_position >= 0;) {
		if (filename[file_position] == '.') {
			break;
		}
		tmpType[tmp_position++] = filename[file_position--];
	}
	for (tmp_position--; tmp_position >= 0;) {
		type[type_position++] = tmpType[tmp_position--];
	}
	type[type_position] = '\0';
	puts(type);
	if (strcmp(type, "html") == 0 || strcmp(type, "css") == 0) {
		strcpy(return_string, "text/");
		puts(return_string);
		strcat(return_string, type);
		puts(return_string);
		return return_string;
	}
	else if (strcmp(type, "js") == 0) {
		return "application/x-javascript";
	}
	else if (strcmp(type, "png") == 0 || strcmp(type, "gif") == 0) {
		strcpy(return_string, "image/");
		strcat(return_string, type);
		return return_string;
	}
	else if (strcmp(type, "ico") == 0) {
		return "image/x-icon";
	}
	else if (strcmp(type, "jpg") == 0) {
		return "image/jpeg";
	}
	else {
		return "text/html";
	}
}