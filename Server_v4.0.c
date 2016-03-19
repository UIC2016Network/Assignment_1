// Server.cpp : create a console application, and include the sources in the project
//
// 1. open the *.c in the Visual C++, then "rebuild all".
// 2. click "yes" to create a project workspace.
// 3. You need to -add the library 'ws2_32.lib' to your project 
//    (Project -> Properties -> Linker -> Input -> Additional Dependencies) 
// 4. recompile the source.


#include <winsock2.h>
#include <mysql.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define DEFAULT_PORT	5019

char flush[100];
char const pwd[] = "uoeoi~o?AA:;>?G";
char truePwd[16];

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

void accept_conn();
void serverMode();
void clientMode();
int mysql_insert();
int mysql_modify();
char* mysql_select();
char* mysql_list();
int mysql_delete();
int server_mode_insert(SOCKET msg_sock, struct sockaddr_in client_addr);
int server_mode_select(SOCKET msg_sock, struct sockaddr_in client_addr);
int server_mode_modify(SOCKET msg_sock, struct sockaddr_in client_addr);
int server_mode_delete(SOCKET msg_sock, struct sockaddr_in client_addr);

int main(int argc, char **argv) {
	int i = 0;
	int msg_len;
	int addr_len;
	char token[10];
	struct sockaddr_in local, client_addr;
	SOCKET sock, msg_sock;
	WSADATA wsaData;

	printf("Socket Server V4.0\n");

	for (int i = 0; i < strlen(pwd); i++) {
		truePwd[i] = pwd[i] - i;
	}
	truePwd[15] = '\0';

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
	if (strcmp(masterSock->token, "Guest") == 0) {
		clientMode(msg_sock, client_addr);
	}
	else if (strcmp(masterSock->token, "Admin") == 0) {
		serverMode(msg_sock, client_addr);
	}
}

void serverMode(SOCKET msg_sock, struct sockaddr_in client_addr) {
	int msg_len;
	int addr_len;
	int mode;
	int status;
	char modeStr[10];
	char welcomeMsg[] = "Welcome administrator.\n";
	msg_len = send(msg_sock, welcomeMsg, sizeof(welcomeMsg), 0);
	if (msg_len == 0) {
		printf("%s closed connection\n", client_addr.sin_addr);
		closesocket(msg_sock);
		return -1;
	}

	while (1) {
		msg_len = recv(msg_sock, modeStr, sizeof(modeStr), 0);
		if (msg_len == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			closesocket(msg_sock);
			return -1;
		}
		if (strcat(modeStr, "exit") == 0) {
			printf("The client %s has exited.\n", inet_ntoa(client_addr.sin_addr));
			break;
		}

		mode = atoi(modeStr);

		switch (mode) {
		case 1:
			status = server_mode_insert(msg_sock, client_addr);
			break;
		case 2:
			status = server_mode_select(msg_sock, client_addr);
			break;
		case 3:
			status = server_mode_modify(msg_sock, client_addr);
			break;
		case 4:
			status = server_mode_delete(msg_sock, client_addr);
			break;
		default:
			break;
		}
		if (status == -1) {
			return;
		}
	}
}

void clientMode(SOCKET msg_sock, struct sockaddr_in client_addr) {
	int msg_len;
	int addr_len;
	char welcomeMsg[] = "Welcome to online phone book system.\nPlease input the name you want to search.\nInput 'exit' to exit the system.\n\0";
	msg_len = send(msg_sock, welcomeMsg, sizeof(welcomeMsg), 0);
	if (msg_len == 0) {
		printf("Client closed connection\n");
		closesocket(msg_sock);
		return -1;
	}
	while (1) {
		char szBuff[100] = "";
		char column[10] = "";
		char* resultString;

		msg_len = recv(msg_sock, column, sizeof(column), 0);
		if (msg_len == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			closesocket(msg_sock);
			return -1;
		}
		msg_len = recv(msg_sock, szBuff, sizeof(szBuff), 0);
		if (msg_len == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			closesocket(msg_sock);
			return -1;
		}
		recv(msg_sock, flush, sizeof(flush), 0);
		if (msg_len == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			closesocket(msg_sock);
			return -1;
		}

		if (szBuff[0] < 0) {
			continue;
		}
		if (strcmp(szBuff, "exit") == 0) {
			printf("The client %s has exited.\n", inet_ntoa(client_addr.sin_addr));
			break;
		}

		if (msg_len == 0) {
			printf("Client %s closed connection\n", inet_ntoa(client_addr.sin_addr));
			closesocket(msg_sock);
			return -1;
		}

		resultString = mysql_select(column, szBuff);
		msg_len = send(msg_sock, resultString, 500, 0);

		printf("Bytes Received: %d, message: %s from %s\n", msg_len, szBuff, inet_ntoa(client_addr.sin_addr));

		if (msg_len == 0) {
			printf("Client closed connection\n");
			closesocket(msg_sock);
			return -1;
		}
	}
}

int server_mode_insert(SOCKET msg_sock, struct sockaddr_in client_addr) {
	int msg_len;
	int result;
	char const FAILED[] = "Insert failed!\n";
	char const SUCCESS[] = "Insert successfully.\n";
	char name[20];
	char phone[15];
	char fax[15];
	char note[100];
	Infor person;

	msg_len = recv(msg_sock, person.name, sizeof(person.name), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}

	msg_len = recv(msg_sock, person.phone, sizeof(person.phone), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}

	msg_len = recv(msg_sock, person.fax, sizeof(person.fax), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}

	msg_len = recv(msg_sock, person.note, sizeof(person.note), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}
	result = mysql_insert(person);
	if (result) {
		msg_len = send(msg_sock, FAILED, sizeof(FAILED), 0);
	}
	else {
		msg_len = send(msg_sock, SUCCESS, sizeof(SUCCESS), 0);
	}
	return 0;
}

int server_mode_select(SOCKET msg_sock, struct sockaddr_in client_addr) {
	int msg_len;
	char* listString;
	char* resultString;
	char id[3];
	listString = mysql_list();
	strcat(listString, "Input the id to check detail:.\n");
	msg_len = send(msg_sock, listString, 600, 0);
	if (msg_len == 0) {
		printf("Client closed connection\n");
		closesocket(msg_sock);
		return -1;
	}
	msg_len = recv(msg_sock, id, 3, 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}
	resultString = mysql_select("id", id);
	msg_len = send(msg_sock, resultString, 500, 0);
	if (msg_len == 0) {
		printf("Client closed connection\n");
		closesocket(msg_sock);
		return -1;
	}
	return 0;
}

int server_mode_modify(SOCKET msg_sock, struct sockaddr_in client_addr) {
	char const FAILED[] = "Modify failed!\n";
	char const SUCCESS[] = "Modify successfully.\n";

	int msg_len;
	char* listString;
	char* resultString;
	char id[3];
	int result;
	Infor person;
	listString = mysql_list();
	strcat(listString, "Input the id to check detail:.\n");
	msg_len = send(msg_sock, listString, 600, 0);
	if (msg_len == 0) {
		printf("Client closed connection\n");
		closesocket(msg_sock);
		return -1;
	}
	msg_len = recv(msg_sock, id, 3, 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}
	resultString = mysql_select("id", id);
	msg_len = send(msg_sock, resultString, 500, 0);
	if (msg_len == 0) {
		printf("Client closed connection\n");
		closesocket(msg_sock);
		return -1;
	}
	msg_len = recv(msg_sock, person.name, sizeof(person.name), 0);
	//recv(msg_sock, flush, sizeof(flush), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}

	msg_len = recv(msg_sock, person.phone, sizeof(person.phone), 0);
	//recv(msg_sock, flush, sizeof(flush), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}

	msg_len = recv(msg_sock, person.fax, sizeof(person.fax), 0);
	//recv(msg_sock, flush, sizeof(flush), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}

	msg_len = recv(msg_sock, person.note, sizeof(person.note), 0);
	//recv(msg_sock, flush, sizeof(flush), 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}

	result = mysql_modify(id, person);
	if (result) {
		msg_len = send(msg_sock, FAILED, sizeof(FAILED), 0);
	}
	else {
		msg_len = send(msg_sock, SUCCESS, sizeof(SUCCESS), 0);
	}
	return 0;
}

int server_mode_delete(SOCKET msg_sock, struct sockaddr_in client_addr) {
	char const FAILED[] = "Delete failed!\n";
	char const SUCCESS[] = "Delete successfully.\n";

	int msg_len;
	char* listString;
	int result;
	char id[3];
	listString = mysql_list();
	strcat(listString, "Input the id to check detail:.\n");
	msg_len = send(msg_sock, listString, 600, 0);
	if (msg_len == 0) {
		printf("Client closed connection\n");
		closesocket(msg_sock);
		return -1;
	}
	msg_len = recv(msg_sock, id, 3, 0);
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		closesocket(msg_sock);
		return -1;
	}
	result = mysql_delete(id);
	if (result) {
		msg_len = send(msg_sock, FAILED, sizeof(FAILED), 0);
	}
	else {
		msg_len = send(msg_sock, SUCCESS, sizeof(SUCCESS), 0);
	}
	if (msg_len == 0) {
		printf("Client closed connection\n");
		closesocket(msg_sock);
		return -1;
	}
	return 0;
}


char* mysql_list() {
	char returnString[500] = "";
	MYSQL_RES *result;
	MYSQL_ROW result_row = NULL;
	MYSQL conn;
	char query[100] = "SELECT * FROM `phonetable`";
	mysql_init(&conn);
	if (!mysql_real_connect(&conn, "127.0.0.1", "root", truePwd, "networking", 3306, NULL, 0)) {
		mysql_close(&conn);
		return "Connect to database failed.\n";
	}
	mysql_query(&conn, query);
	

	
	result = mysql_store_result(&conn);
	if (result == NULL) {
		return "No data in the database.\n";
	}
	else {
		strcat(returnString, "=================================================\n");
		while (result_row = mysql_fetch_row(result)) {
			strcat(returnString, "||  ID: ");
			strcat(returnString, result_row[0]);
			strcat(returnString, "\t");
			strcat(returnString, "Name: ");
			strcat(returnString, result_row[1]);
			strcat(returnString, "\n");
		}
		strcat(returnString, "=================================================\n");
		return returnString;
	}
	mysql_close(&conn);
}

int mysql_insert(Infor person) {
	MYSQL conn;
	int result;
	char query[100] = "INSERT INTO `phonetable` VALUES (NULL, '";
	strcat(query, person.name);
	strcat(query, "', '");
	strcat(query, person.phone);
	strcat(query, "', '");
	strcat(query, person.fax);
	strcat(query, "', '");
	strcat(query, person.note);
	strcat(query, "')");

	mysql_init(&conn);
	if (!mysql_real_connect(&conn, "127.0.0.1", "root", truePwd, "networking", 3306, NULL, 0)) {
		mysql_close(&conn);
		return "Connect to database failed.\n";
	}

	result = mysql_query(&conn, query);
	mysql_close(&conn);
	return result;
}

char* mysql_select(char column[], char content[]) {
	char returnString[500] = "";
	MYSQL_RES *result = NULL;
	MYSQL_ROW result_row = NULL;
	MYSQL conn;
	char query[100] = "SELECT * FROM `phonetable` WHERE `";
	strcat(query, column);
	strcat(query, "`='");
	strcat(query, content);
	strcat(query, "'");
	mysql_init(&conn);
	if (!mysql_real_connect(&conn, "127.0.0.1", "root", truePwd, "networking", 3306, NULL, 0)) {
		mysql_close(&conn);
		return "Connect to database failed.\n";
	}
	mysql_query(&conn, query);
	result = mysql_store_result(&conn);
	result_row = mysql_fetch_row(result);
	if (result_row == NULL) {
		return "Person doesn't exist.\n";
	}
	else {
		strcat(returnString, "=================================================\n");
		strcat(returnString, "||  Name: ");
		strcat(returnString, result_row[1]);
		strcat(returnString, "\n");
		strcat(returnString, "||  Phone: ");
		strcat(returnString, result_row[2]);
		strcat(returnString, "\n");
		strcat(returnString, "||  Fax: ");
		strcat(returnString, result_row[3]);
		strcat(returnString, "\n");
		strcat(returnString, "||  Note: ");
		strcat(returnString, result_row[4]);
		strcat(returnString, "\n");
		strcat(returnString, "=================================================\n");
		return returnString;
	}
	mysql_close(&conn);
}

int mysql_delete(char id[]) {
	int result = NULL;
	MYSQL conn;

	char query[100] = "delete from `phonetable` where `id`='";
	strcat(query, id);
	strcat(query, "'");
	mysql_init(&conn);
	if (!mysql_real_connect(&conn, "127.0.0.1", "root", truePwd, "networking", 3306, NULL, 0)) {
		mysql_close(&conn);
		return "Connect to database failed.\n";
	}
	result = mysql_query(&conn, query);
	mysql_close(&conn);
	return result;
}

int mysql_modify(char id[], Infor person) {
	int result = NULL;
	MYSQL conn;
	char query[200] = "UPDATE phonetable SET name='";
	strcat(query, person.name);
	strcat(query, "', phone='");
	strcat(query, person.phone);
	strcat(query, "', fax='");
	strcat(query, person.fax);
	strcat(query, "', note='");
	strcat(query, person.note);
	strcat(query, "' WHERE id='");
	strcat(query, id);
	strcat(query, "'");

	mysql_init(&conn);
	if (!mysql_real_connect(&conn, "127.0.0.1", "root", truePwd, "networking", 3306, NULL, 0)) {
		mysql_close(&conn);
		return -1;
	}
	result = mysql_query(&conn, query);
	mysql_close(&conn);
	return result;
}
