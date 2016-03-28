#include "stdafx.h"

char* expand_string(char *source_string, char* target_string, int malloc_string) {
	int s_str_len = strlen(source_string);
	int t_str_len = strlen(target_string);
	char* new_string = (char *)malloc(sizeof(char) * (s_str_len + t_str_len + 4));
	char* freePt = "";
	strcpy(new_string, "");
	strcat(new_string, source_string);
	strcat(new_string, target_string);
	if (malloc_string) {
		free(source_string);
	}
	return new_string;
}

//void expand_string2(char source_string[], char target_string[]) {
//	int s_str_len = strlen(source_string);
//	int t_str_len = strlen(target_string);
//	int pos = 0;
//	char* new_string;
//	new_string = (char *)malloc(sizeof(char) * (s_str_len + t_str_len));
//	*new_string = 0;
//	char* freePt = "";
//	for (int i = 0; i < strlen(source_string); i++) {
//		new_string[pos++] = source_string[i];
//	}
//	for (int i = 0; i < strlen(target_string); i++) {
//		new_string[pos++] = target_string[i];
//	}
//	new_string[pos] = '\0';
//	free(new_string);
//	new_string = NULL;
//	source_string = (char *)malloc(sizeof(char) * (s_str_len + t_str_len));
//	strcpy(source_string, new_string);
//	strcpy(new_string, "");
//}