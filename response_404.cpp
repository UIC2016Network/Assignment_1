#include "stdafx.h"

int response_404(SOCKET client) {
    char* response = "";
    int ret;
    char* content = "<html><body><h1>Error 404!</h1><p>Object Not Found!</p></body></html>";
	response = expand_string(response, "HTTP/1.1 404 Not Found\r\n\r\n", 0);
    response = expand_string(response, content, 1);
    puts("404 Not found");
    printf("strlen(response) = %d\n", strlen(response));
    ret = send(client, response, strlen(response), 0);
	free(response);
	response = NULL;
    if (ret == SOCKET_ERROR) {
        fprintf(stderr, "send() is failed with %d\n", WSAGetLastError());
        return -1;
    }
    return 1;
}