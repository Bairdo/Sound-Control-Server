#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <endpointvolume.h>
#include <iostream>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <Audioclient.h>

#include <TlHelp32.h>

#include "general.h"

struct Entry {
	ISimpleAudioVolume * volume;
	WCHAR * name;
	DWORD pid;


	float getMasterVolume();
	Entry(ISimpleAudioVolume *v, DWORD pid, WCHAR n[MAX_NAME_LENGTH]);

};