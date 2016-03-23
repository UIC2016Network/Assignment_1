#include "stdafx.h"
int response_200(SOCKET client, char* file_path) {
	FILE* response_file = fopen(file_path, "r");
	char* response = "";
	char content[100] = {'\0'};
	int ret;

	if (response_file == NULL) {
		puts("NULL \n");
		/*这部分留着写404部分*/
		return -1;
	}
	response = expand_string(response, "HTTP/1.1 200 OK \r\nContent-Type:text/html\r\nContent-Length: 200\r\n\r\n");
	while (fgets(content, 100, response_file) != NULL) {
		response = expand_string(response, content);
	}
	response = expand_string(response, "\0");
	puts(response);
	printf("strlen(response) = %d", strlen(response));
	ret = send(client, response, strlen(response), 0);
	if (ret == SOCKET_ERROR) {
		fprintf(stderr, "send() is failed with %d\n", WSAGetLastError());
		fclose(response_file);
		return -1;
	}
	return 1;
}