#include "stdafx.h"
int response_200(SOCKET client, char* file_path) {
	//FILE* response_file = fopen(file_path, "r");
	char* response = "";
	char content[100] = {'\0'};
    char* content_string = "";
    char* test_string = "<html><body><h1>hi</h1></body></html>\0";
	int ret;

	/*if (response_file == NULL) {
		puts("NULL \n");
        response_404(client);
		return -1;
	}*/
	response = expand_string(response, "HTTP/1.1 200 OK \r\nContent-Type:text/html\r\n\r\n");
	/*while (fgets(content, 100, response_file) != NULL) {
        content_string = expand_string(content_string, content);
	}*/
    content_string = expand_string(content_string, "\0");
    sprintf(response, "HTTP/1.1 200 OK \r\nContent-Type:text/html\r\nContent-length: %d\r\n\r\n", strlen(test_string));
    puts(response);
    response = expand_string(response, test_string);
	//puts(response);
	printf("strlen(response) = %d\n", strlen(response));
	ret = send(client, response, strlen(response), 0);
	if (ret == SOCKET_ERROR) {
		fprintf(stderr, "send() is failed with %d\n", WSAGetLastError());
		//fclose(response_file);
		return -1;
	}
	return 1;
}