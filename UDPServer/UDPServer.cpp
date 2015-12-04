// UDPServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
Simple UDP Server
Silver Moon ( m00n.silv3r@gmail.com )
*/

#include<stdio.h>
#include<winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#include "soundComponent.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 2000  //Max length of buffer
#define PORT 27015   //The port on which to listen for incoming data

int main()
{

	
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	//keep listening for data
	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		PWSTR* out = new PWSTR[16];
		//InetNtop(AF_INET, &si_other.sin_addr, *out, 16);
		void *addr;
		
		char ipstr[INET6_ADDRSTRLEN];

		//struct sockaddr_in *ipv4 = (struct sockaddr_in *)si_other->ai_addr;
		addr = &(si_other.sin_addr);
		inet_ntop(AF_INET, addr, (PSTR)ipstr, sizeof(ipstr));
		//print details of the client/peer and the data received
		//printf("Received packet from %s:%d\n", out, ntohs(si_other.sin_port)); //inet_ntoa(si_other.sin_addr)
		printf("Data: %s\n", buf);
		memcpy(buf, "Stop", 3);
		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
