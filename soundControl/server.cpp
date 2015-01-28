#include "stdafx.h"

#include "server.h"
#include "soundComponent.h"

#include <iostream>



namespace {
	static const PCSTR DEFAULT_PORT = "27015";
}


Server::Server() {
	sc = new SoundComponent(this);
}


	/*


	Initialize Winsock.
	Create a socket.
	Bind the socket.
	Listen on the socket for a client.
	Accept a connection from a client.
	Receive and send data.
	Disconnect.

	*/
	int Server::intialiseWinSock(){
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
			return 1;
		}

		return 0;

	}

	int Server::createSocket(SOCKET & ListenSocket){
		struct addrinfo *ptr = NULL, hints;

		ZeroMemory(&hints, sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (ListenSocket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		return 0;
	}


	int Server::bindSocket(SOCKET& ListenSocket){

		// Setup the TCP listening socket
		int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);
		return 0;
	}


	int Server::listenSocket(SOCKET &ListenSocket){
		if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
			printf("Listen failed with error: %ld\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	int Server::acceptConnection(SOCKET & ListenSocket, SOCKET & ClientSocket){
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 0;
		}
		return 1;
	}


	void Server::getConnections(SOCKET& ListenSocket ){
		SOCKET ClientSocket = INVALID_SOCKET;
		while (acceptConnection(ListenSocket, ClientSocket)){
			std::cout << "Accepted connection" << std::endl;
			connections.emplace_back(ClientSocket);
			sc->sendNames();
		}
	}

	

	int Server::run(){

		

		struct addrinfo * result = NULL;
		SOCKET ListenSocket = INVALID_SOCKET;
		
		std::cout << "running" << std::endl;
		intialiseWinSock();
		std::cout << "creating" << std::endl;
		createSocket(ListenSocket);
		std::cout << "binding" << std::endl;
		bindSocket(ListenSocket);
		std::cout << "listening" << std::endl;
		listenSocket(ListenSocket);
		std::cout << "accepting" << std::endl;
		
		t = std::thread(&Server::getConnections, this, ListenSocket);

		//std::cout << "receiving/sending" << std::endl;
		//receiveSend(ClientSocket);
	//	std::cout << "disconnecting" << std::endl;
	//	disconnect(ClientSocket);
		
	//	while (true){
		
			//accept a connection;
			//create a thread to deal with the client;


		//}
		return 0;
	}
	

	void Server::exit(){
		for (size_t i = 0; i < connections.size(); i++){
			std::cout << "waiting for thread to be done" << std::endl;
			connections[i].join();
		}
		std::cout << "waiting for user to quit" << std::endl;
	}

	int Server::sendName(WCHAR * name, int length){

		for (Connection& c : connections){
			c.sendName(name, length);
		}
		return length;
	}
