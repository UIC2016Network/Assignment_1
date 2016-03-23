// Server.cpp : create a console application, and include the sources in the project
//
// 1. open the *.c in the Visual C++, then "rebuild all".
// 2. click "yes" to create a project workspace.
// 3. You need to -add the library 'ws2_32.lib' to your project 
//    (Project -> Properties -> Linker -> Input -> Additional Dependencies) 
// 4. recompile the source.

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#define DEFAULT_PORT	5019



char* expand_string(char* source_string, char* target_string);


void accept_conn(void *dummy) {
	SOCKET client = (SOCKET)dummy; // initialize a socket
	int ret;  // record the error code
	char buffer[500] = { '\0' };
	char* response = "";
	char content[100];
	FILE* web = fopen("index.html", "r");
	if (web == NULL) {
		puts("NULL \n");
	}
	response = expand_string(response, "HTTP/1.1 200 OK \r\nContent-Type:text/html\r\nContent-Length: 200\r\n\r\n");
	while (1) {
		ret = recv(client, buffer, sizeof(buffer), 0);  // get enquiry keyword
		while (fgets(content, 100, web) != NULL) {
			response = expand_string(response, content);
		}
		response = expand_string(response, "\0");
		puts(response);
		send(client, response, sizeof(response), 0);
	}
	fclose(web);
}

char* expand_string(char *source_string, char* target_string) {
	int s_str_len = strlen(source_string);
	int t_str_len = strlen(target_string);
	char* new_string = (char *)malloc(sizeof(char) * (s_str_len + t_str_len));
	strcpy(new_string, "");
	strcat(new_string, source_string);
	strcat(new_string, target_string);
	return new_string;

}

int main(int argc, char **argv) {
	char szBuff[100];
	int msg_len;
	int addr_len;
	struct sockaddr_in local, client_addr;

	SOCKET sock, msg_sock;
	WSADATA wsaData;

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}
	// Fill in the address structure
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(DEFAULT_PORT);

	sock = socket(AF_INET, SOCK_STREAM, 0);	//TCp socket


	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	if (bind(sock, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR) {
		fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//waiting for the connections
	if (listen(sock, 5) == SOCKET_ERROR) {
		fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}


	printf("Waiting for the connections ........\n");

	// At this point start multithread coding

	while (1) {
		addr_len = sizeof(client_addr);
		msg_sock = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
		if (msg_sock == INVALID_SOCKET) {
			fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
			break;
		}

		printf("accepted connection from %s, port %d\n",
			inet_ntoa(client_addr.sin_addr),
			htons(client_addr.sin_port));

		_beginthread(accept_conn, 0, (void*)msg_sock); // pass param to the thread function
	}
	closesocket(msg_sock);
	WSACleanup();
}

