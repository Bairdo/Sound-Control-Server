#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>


struct Connection {
private:
	SOCKET s;

	std::thread * rec = NULL;
	// because connection is creeated on a thread, it handles send.

	void run();


public:
	Connection(SOCKET s);
	Connection(const Connection & c);

	void join();
	int disconnect();

	int sendName(WCHAR * name, int length);
	void receive();

	void sendData(WCHAR * name, int length);

};