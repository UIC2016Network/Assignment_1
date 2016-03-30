#include "stdafx.h"
int response_200(SOCKET client, char* file_path) {
	FILE* response_file = fopen(file_path, "rb+");
	int size = 0;
	int malloced = 0;
	char* response = "";
	char header[100] = { '\0' };
	char content[100] = { '\0' };
    char* content_string = "";
	char content_type[50];

    //char* test_string = "<html><body><h1>hi</h1></body></html>\0";
	int ret;
	if (response_file == NULL) {
		puts("NULL \n");
        response_404(client);
		return -1;
	}
	else {
		fseek(response_file, 0, SEEK_END);
		size = ftell(response_file);
		fseek(response_file, 0, 0);
		printf("size: %d\n", size);
	}
	while (fgets(content, 100, response_file) != NULL) {
		if (!malloced) {
			content_string = expand_string(content_string, content, 0);
			malloced = 1;
		}
		else {
			content_string = expand_string(content_string, content, 1);
		}
        
	}
	fclose(response_file);
    //content_string = expand_string(content_string, "\0");
	strcpy(content_type, return_content_type(file_path));
    sprintf(header, "HTTP/1.1 200 OK \r\nContent-Type:%s\r\nContent-length: %d\r\n\r\n", content_type, size);
	response = expand_string(response, header, 0);
    response = expand_string(response, content_string, 1);
	ret = send(client, response, strlen(response), 0);
	free(response);
	if (strcmp(content_string, "") != 0) {
		free(content_string);
	}
	response = NULL;
	content_string = NULL;
	if (ret == SOCKET_ERROR) {
		fprintf(stderr, "send() is failed with %d\n", WSAGetLastError());
		return -1;
	}
	return 1;
}