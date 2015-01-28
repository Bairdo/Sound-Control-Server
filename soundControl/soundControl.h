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
#include "general.h"
#include "entry.h"








bool printChanelVolumes(IAudioEndpointVolume* endPtVol);
//bool audioSessionManagerStuff(IAudioSessionManager * audioSessionManager);
HRESULT EnumSessions(IAudioSessionManager2* pSessionManager, IAudioClient* audioClient);

void getEntrys(std::vector<Entry>& list);

//  Forward declarations:
BOOL GetProcessName(DWORD & pid, WCHAR * retVal);
BOOL ListProcessModules(DWORD dwPID);
BOOL ListProcessThreads(DWORD dwOwnerPID);
void printError(TCHAR* msg);


