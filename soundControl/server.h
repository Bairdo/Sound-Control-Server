#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <vector>

#include "connection.h"
#include "soundControl.h"
//#include "soundComponent.h"

#pragma comment (lib, "Ws2_32.lib")

struct SoundComponent;


struct Server {

private:
	// Fields
	WSADATA wsaData;
	struct addrinfo * result = NULL;

	SoundComponent * sc;

	std::vector<Connection> connections;
	std::thread t;


	
	/*
	
	

	Initialize Winsock.
	Create a socket.
	Bind the socket.
	Listen on the socket for a client.
	Accept a connection from a client.
	Receive and send data.
	Disconnect.

	*/
public:
	// Constructors
	Server();
	
	//Methods
	int intialiseWinSock();
	int createSocket(SOCKET & ListenSocket);
	int bindSocket(SOCKET& ListenSocket);
	int listenSocket(SOCKET &ListenSocket);
	int acceptConnection(SOCKET & ListenSocket, SOCKET & ClientSocket);

	int sendName(WCHAR* name, int length);

	int run();

	void exit();

	void getConnections(SOCKET& ListenSocket);
};