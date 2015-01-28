#include "stdafx.h"
#include "connection.h"
#include <iostream>

namespace{
	static const int DEFAULT_BUFLEN = 512;
}

void work(SOCKET& s){
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	iSendResult = send(s, "qwerty\n", 7, 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();

	}
	printf("sent\n");

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(s, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(s, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(s);
				WSACleanup();

			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(s);
			WSACleanup();

		}

	} while (iResult > 0);
}


Connection::Connection(SOCKET s) : s(s) {
	//t = new std::thread(work, s);
	rec = new std::thread(&Connection::receive, this);
}

Connection::Connection(const Connection & c){
	std::cout << "Copying connection" << std::endl;
	s = c.s;
	rec = c.rec;
}

void Connection::sendData(WCHAR * name, int length){
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	std::wcout << L"sending: " << name << std::endl;

	iSendResult = send(s, (char*)name, length * 2, 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();

	}

	iSendResult = send(s, (char*)L"\n", 2, 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();

	}
	printf("sent\n");

}

/* returns number of bytes sent*/
int Connection::sendName(WCHAR * name, int length){
	sendData(name, length);
	return length*2;
}

void Connection::join(){
	if (rec != NULL)
		rec->join();
	disconnect();
}

int Connection::disconnect(){
	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(s, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	std::cout << "disconnected" << std::endl;
	// cleanup
	closesocket(s);
	WSACleanup();
	return 0;
}


void Connection::receive(){
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	do {

		iResult = recv(s, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(s);
			WSACleanup();

		}

	} while (iResult > 0);
}
