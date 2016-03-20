// Server.cpp : create a console application, and include the sources in the project
//
// 1. open the *.c in the Visual C++, then "rebuild all".
// 2. click "yes" to create a project workspace.
// 3. You need to -add the library 'ws2_32.lib' to your project 
//    (Project -> Properties -> Linker -> Input -> Additional Dependencies) 
// 4. recompile the source.


#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define DEFAULT_PORT	5019

char flush[100];


struct dummyValue {
	SOCKET msg_sock;
	struct sockaddr_in client_addr;
	char token[10];
};

struct _infor {
	char name[20];
	char phone[15];
	char fax[15];
	char note[100];
};

typedef struct _infor Infor;

void accept_conn(void *dummy);

int main(int argc, char **argv) {
	int i = 0;
	int msg_len;
	int addr_len;
	char token[10];
	struct sockaddr_in local, client_addr;
	SOCKET sock, msg_sock;
	WSADATA wsaData;

	printf("Socket Server V4.0\n");

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//closesocket(msg_sock); function terminates use of the Windows Sockets DLL. 
		closesocket(msg_sock);
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

	for (int i = 1; i <= 100; i++){
		printf("%d ", i);
		addr_len = sizeof(client_addr);
		msg_sock = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
		msg_len = recv(msg_sock, token, sizeof(token), 0);
		struct dummyValue masterSock;
		masterSock.msg_sock = msg_sock;
		masterSock.client_addr = client_addr;
		strcpy(masterSock.token,token);
		printf("accepted connection from %s, port %d\n",
			inet_ntoa(client_addr.sin_addr),
			htons(client_addr.sin_port));
		if (msg_sock == INVALID_SOCKET) {
			fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
			closesocket(msg_sock);
			return -1;
		}
		_beginthread(accept_conn, 0, (void*)&masterSock);

	}
	_endthread();
	closesocket(msg_sock);
	WSACleanup();
		
}


void accept_conn(void* dummy) {
	struct dummyValue* masterSock = (struct dummyValue*) dummy;

	SOCKET* msg_sock = masterSock->msg_sock;
	struct sockaddr_in client_addr = masterSock->client_addr;
	int msg_len;
	int addr_len;
	struct sockaddr_in local;
}


