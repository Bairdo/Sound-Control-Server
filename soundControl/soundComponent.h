#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <endpointvolume.h>
#include <iostream>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <Audioclient.h>

#include <TlHelp32.h>
#include <vector>
#include "entry.h"

struct Server;

struct SoundComponent {

private:
	Server * server;

	
	IAudioClient * audioClient = NULL;
	IAudioSessionManager * audioManager = NULL;
	IAudioSessionManager2 * auMan2 = NULL;

	void initalise();
	void getEntrys(std::vector<Entry>& list);
	BOOL GetProcessName(DWORD& pid, WCHAR * retName);

public:
	// Fields
	IAudioEndpointVolume * master = NULL;
	std::vector<Entry> sessions;

	// constructors
	SoundComponent(Server * s);
	

	// MEthods
	float getMasterVol();
	float getMasterVolScalar();

	void sendNames();
	
};
