#include "stdafx.h"
#include "connection.h"
#include <iostream>

namespace{
	static const int DEFAULT_BUFLEN = 512;
	static const uint8_t VOLUME = 1;
	static const uint8_t NAME = 2;
	static const uint8_t IMAGE = 3;
	static const uint8_t UPDATE = 4;
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

void Connection::sendData(char * name, int length){
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	std::cout << "sending: " << name << std::endl;
	printf("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", name[0], name[1], name[2], name[3], name[4], name[5], name[6], name[7], name[8], name[9], name[10], name[11], name[12], name[13], name[14], name[15]);
	printf("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x", name[16], name[17], name[18], name[19], name[20], name[21], name[22], name[23], name[24], name[25], name[26], name[27], name[28], name[29], name[30], name[31]);
	
	iSendResult = send(s, name, length, 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();

	}

	printf("sent\n");

}

/* l is the length of name.*/
int Connection::sendName(WCHAR * name, int l){
	BYTE * out = new BYTE[1024];
	int32_t length = l * 2; //wchar is 2 bytes.
	out[0] = NAME;
	memcpy_s(&out[1], 4, &length, 4);
	memcpy_s(&out[5], 1000, name, length);
	std::wcout << "actual name: " << name << std::endl;
	
	sendData((char *)out, length + 5);
	return length*2;
}

void Connection::sendVol(float vol){
	BYTE out[1024];
	int32_t length = 4;
	out[0] = VOLUME;
	memcpy_s(&out[1], 4, &length, 4);
	memcpy_s(&out[5], 4, &vol, 4);
	//swprintf_s(out, 1024, L"%d%d%d", VOLUME, length ,vol);
	std::cout << "actual vol: " << vol << std::endl;

	sendData((char*)out, 9);
	
}

void Connection::sendUpdate(long pid, float vol, WCHAR* name){

	BYTE out[1024];
	uint32_t length = wcsnlen_s(name, 260) * 2;

	out[0] = UPDATE;

	memcpy_s(&out[1], 4, &pid, 4);
	memcpy_s(&out[5], 4, &vol, 4);
	memcpy_s(&out[9], 4, &length, 4);

	memcpy_s(&out[13], 1000, name, length);

	std::wcout << L"pid: " << pid << L" vol: " << vol << L" Name: " << name << std::endl;

	sendData((char*) out, length + 13);

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


