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

#include <cstdint>

#include <vector>


#include "soundComponent.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 2000  //Max length of buffer
#define PORT 27015   //The port on which to listen for incoming data

#define SPEAKER_PID 999999

// get rid of this shit. and pretend it never happened.
//std::vector<sockaddr_in> clients;
struct sockaddr_in si_other;
int slen;
SOCKET socket_;



void sendReply(SoundComponent& soundComponent ){
	printf("sending status");
	char retbuf[1000];
	DWORD pid = SPEAKER_PID;
	memcpy_s(&retbuf[0], 6, "Status", 6);
	memcpy_s(&retbuf[6], 4, (char*)&pid, 4);
	int v = soundComponent.getMasterVol() * 100; // this is in decibels.
	memcpy_s(&retbuf[10], 4, (char*)&v, 4);
	WCHAR * name = L"Speakers";
	int length = wcslen(name) * 2;
	memcpy_s(&retbuf[14], 500, (char*)&length, 4);
	memcpy_s(&retbuf[18], 500, name, wcslen(name) * 2);

	BOOL muted = soundComponent.getMasterMuted();

	memcpy_s(&retbuf[18 + wcslen(name) * 2], 1, &muted, 1);

	if (sendto(socket_, retbuf, 18 + length + 1, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	for (auto& s : soundComponent.sessions){
		char retbuf[1000];
		DWORD pid = s.pid;
		memcpy_s(&retbuf[0], 6, "Status", 6);
		memcpy_s(&retbuf[6], 4, (char*)&pid, 4);
		int v = s.getMasterVolume() * 100;
		memcpy_s(&retbuf[10], 4, (char*)&v, 4);
		WCHAR * name = s.name;
		int length = wcslen(name) * 2;
		memcpy_s(&retbuf[14], 500, (char*)&length, 4);
		memcpy_s(&retbuf[18], 500, name, wcslen(name) * 2);

		BOOL muted;
		s.volume->GetMute(&muted);

		memcpy_s(&retbuf[18 + wcslen(name) * 2], 1, &muted, 1);

		if (sendto(socket_, retbuf, 18 + length + 1, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

}

void processData(SoundComponent& soundComponent, char* buf, int recv_len, LPCGUID guid){
	if (strncmp(buf, "Hello", 5)==0){
		printf("message hello");

		
		sendReply(soundComponent);
		
	}
	else if (strncmp(buf, "Update", 5) == 0) {
		int pid = 0;
		memcpy_s(&pid, 4, (buf + 6), 4);
		pid = ntohl(pid);
		int vol = 0;
		memcpy_s(&vol, 4, (buf + 10), 4);
		vol = ntohl(vol);

		uint8_t muted;
		memcpy_s(&muted, 1, (buf + 14), 1);
		//int nameLength = 0;
		//memcpy_s(&nameLength, 4, (buf + 13), 4);
		//WCHAR name[260]; // max length for a name
		//memcpy_s(&nameLength, 260, (buf + 17), nameLength);

		if (pid == SPEAKER_PID){
			soundComponent.setMasterVol(static_cast<float>(-vol));
		}
		else{
			
			AudioSession* as = soundComponent.getAudioSession(pid);
			if (as != NULL){
				HRESULT res = as->volume->SetMasterVolume(static_cast<float>(vol)/100, guid);// find out what the null should be.
				if (res == S_OK){
					std::cout << "success on changing volume: " << static_cast<float>(vol)/100 << std::endl;
				}
				else{
					std::cout << "failed changing volume: " << static_cast<float>(vol) << std::endl;
					std::cout << "res: " << res << std::endl;
				}
				if (res == E_INVALIDARG){
					std::cout << "invalid argument on set volume: " << static_cast<float>(vol) << std::endl;
				}

				//std::cout << "setting mute to: " << muted << std::endl;
				as->volume->SetMute(static_cast<bool>(muted), guid);
			}
		}
	}
	memcpy(buf, "Stop", 3);
	//now reply the client with the same data

}


int main()
{
	
	GUID guid;
	HRESULT h = CoCreateGuid(&guid);
	if (h != S_OK){
		printf("dramas guid not created.");
	}
	else {
		printf("guid is: %d", guid);
	}
	LPCGUID lpcguid = static_cast<LPCGUID>(&guid);

	SoundComponent soundComponent(lpcguid);

	//soundComponent.setLPCGUID(lpcguid);

	SOCKET s;
	struct sockaddr_in server;
	int recv_len;
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
		socket_ = s; // todo fix
		processData(soundComponent, buf, recv_len, lpcguid);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
