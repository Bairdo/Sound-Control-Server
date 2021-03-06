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

	LPCGUID lpcguid;

	IAudioSessionEvents* newNotifications;
	IAudioEndpointVolumeCallback * audioEndpopointCallback;

	void initalise();
	void getEntrys(std::vector<AudioSession>& list);
	BOOL GetProcessName(DWORD& pid, WCHAR * retName);

	

public:
	// Fields
	IAudioEndpointVolume * master = NULL;
	std::vector<AudioSession> sessions;

	// Constructors
	SoundComponent(Server * s);
	SoundComponent(LPCGUID lpcguid);

	// Methods
	float getMasterVol();
	float getMasterVolScalar();

	float setMasterVol(float vol);

	BOOL getMasterMuted();
	BOOL setMasterMuted(BOOL muted);

	void updateEntrys();

	AudioSession* getAudioSession(int pid);

};
