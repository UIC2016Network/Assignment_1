#include "stdafx.h"
char* split_path(char* header) {
	char return_path[200];
	int pos_path = 0;
	int pos_header = 0;
	int is_path = 0;
	while (header[pos_header] != '\0' || header[pos_header] != NULL){
		if (header[pos_header] == ' ') {
			if (!is_path) {
				is_path = 1;
				pos_header+=2;
			}
			else if (is_path) {
				return_path[pos_path] = '\0';
				break;
			}
			continue;

		}
		if (is_path) {
			return_path[pos_path] = header[pos_header];
			pos_header++;
			pos_path++;
		}
		else {
			pos_header++;
		}
	}
	return return_path;
}