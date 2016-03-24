#include "stdafx.h"

int response_404(SOCKET client) {
    char* response = "";
    int ret;
    char* content = "<html><body><h1>Error 404!</h1><p>Object Not Found!</p></body></html>";

    response = expand_string(response, "HTTP/1.1 404 Not Found\r\n\r\n");
    response = expand_string(response, content);
    puts(response);
    printf("strlen(response) = %d\n", strlen(response));
    ret = send(client, response, strlen(response), 0);
    if (ret == SOCKET_ERROR) {
        fprintf(stderr, "send() is failed with %d\n", WSAGetLastError());
        return -1;
    }
    return 1;
}