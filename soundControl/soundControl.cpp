// soundControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <endpointvolume.h>
#include <iostream>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <Audioclient.h>
#include <Windows.h>
#include <TlHelp32.h>



#define EXIT_ON_ERROR(hr)  \
if (FAILED(hr)) { std::cout<< "error" << std::endl; goto Exit; }


static IAudioEndpointVolume *g_pEndptVol = NULL;
static IAudioClient * audioClient = NULL;
static IAudioSessionManager * audioManager = NULL;
static IAudioSessionManager2 * auMan2 = NULL;

bool printVolumeRange(IAudioEndpointVolume*);
bool changeVolume(IAudioEndpointVolume * endPtVol);
bool muteUnmute(IAudioEndpointVolume* endPtVol);
bool printChanelVolumes(IAudioEndpointVolume* endPtVol);
bool audioSessionManagerStuff(IAudioSessionManager * audioSessionManager);
HRESULT EnumSessions(IAudioSessionManager2* pSessionManager);

//  Forward declarations:
BOOL GetProcessName(DWORD & pid, WCHAR * retVal);
BOOL ListProcessModules(DWORD dwPID);
BOOL ListProcessThreads(DWORD dwOwnerPID);
void printError(TCHAR* msg);

int _tmain(int argc, _TCHAR* argv[])
{
	
	std::cout << "hi" << std::endl;
	char * in = new char[10];
	

	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;

	CoInitialize(NULL);


		// Get enumerator for audio endpoint devices.
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

		// Get default audio-rendering device.
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&g_pEndptVol);
	EXIT_ON_ERROR(hr)


		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **)&audioClient);
	EXIT_ON_ERROR(hr)

		hr = pDevice->Activate(__uuidof(IAudioSessionManager), CLSCTX_ALL, NULL, (void **)&audioManager);
	EXIT_ON_ERROR(hr)

		hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&auMan2);
	EXIT_ON_ERROR(hr)


		//GetProcessList();
	if (FAILED(EnumSessions(auMan2))){
		std::cout << "error" << std::endl;
		goto Exit;
	}


Exit:
	std::cin >> in; 
	return 0;
}

bool audioSessionManagerStuff(IAudioSessionManager * audioSessionManager){
	HRESULT hr;
	DWORD flags = 0;
	IAudioSessionControl * sessionControl = NULL;

	hr = audioSessionManager->GetAudioSessionControl(NULL, flags, &sessionControl);
	if (FAILED(hr)){
		std::cout << "audioSession error" << std::endl;
		return false;
	}

	LPWSTR ret = NULL;
	hr = sessionControl->GetDisplayName(&ret);
	if (FAILED(hr)){
		std::cout << "error" << std::endl;
		CoTaskMemFree(ret);
		return false;
	}
	std::wcout << ret << std::endl;
	CoTaskMemFree(ret);
	return true;
}

HRESULT EnumSessions(IAudioSessionManager2* pSessionManager)
{
	if (!pSessionManager)
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	int cbSessionCount = 0;
	DWORD pswSession = NULL;

	IAudioSessionEnumerator* pSessionList = NULL;
	IAudioSessionControl2* pSessionControl2 = NULL;
	IAudioSessionControl* pSessionControl = NULL;

	// Get the current list of sessions.
	FAILED(hr = pSessionManager->GetSessionEnumerator(&pSessionList));

	// Get the session count.
	FAILED(hr = pSessionList->GetCount(&cbSessionCount));







	for (int index = 0; index < cbSessionCount; index++)
	{
		//FAILED(hr = pSessionManager->GetAudioSessionControl(NULL, FALSE, &pSessionControl));
		FAILED(hr = pSessionList->GetSession(index, &pSessionControl));
//		SAFE_RELEASE(pSessionControl);
		FAILED(hr = pSessionControl->QueryInterface(
			__uuidof(IAudioSessionControl2), (void**)&pSessionControl2));

		if (FAILED(hr = pSessionControl2->GetProcessId(&pswSession))){
			continue;
		}
		WCHAR * name = new WCHAR[260];
		GetProcessName(pswSession, name);
		

		LPWSTR ret = NULL;
		hr = pSessionControl->GetDisplayName(&ret);
		if (FAILED(hr)){
			std::cout << "error" << std::endl;
			CoTaskMemFree(ret);
			return false;
		}

		wprintf_s(L"Session PID: %d\t", pswSession);

		if (wcslen(ret) == 0){
			wprintf_s(L"Session Name: %s\n", name);
		}
		else {
			std::cout << wcslen(ret);
			wprintf_s(L"Session Name: %s\n", ret);
		}
	}

done:
//	CoTaskMemFree(pswSession);
//	SAFE_RELEASE(pSessionControl);
//	SAFE_RELEASE(pSessionList);

	return hr;

}



bool muteUnmute(IAudioEndpointVolume* endPtVol){
	BOOL mute;
	HRESULT hr;
	hr = g_pEndptVol->GetMute(&mute);
	if (FAILED(hr)) return false;
	hr = g_pEndptVol->SetMute(!mute, NULL);
	if (FAILED(hr)) return false;
	std::cout << "mute set: " << mute << std::endl;
}

bool changeVolume(IAudioEndpointVolume * endPtVol){
	HRESULT hr;

	int steps;
	std::cin >> steps;
	if (steps < 0){
		for (int i = steps; i < 0; i++){
			hr = endPtVol->VolumeStepDown(NULL);
			if (FAILED(hr)) return false;

		}
	}
	else{
		for (int i = 0; i < steps; i++){
			hr = endPtVol->VolumeStepUp(NULL);
			if (FAILED(hr)) return false;

		}
	}
	std::cout << "changed vol by: " << steps << " steps." << std::endl;
}



/*
 * returns false on failure.
*/
bool printVolumeRange(IAudioEndpointVolume * endPtVol){
	float  min = 0;
	float max = 0;
	float inc = 0;
	HRESULT hr;
	hr = g_pEndptVol->GetVolumeRange(&min, &max, &inc);
	if (FAILED(hr)) return false;

	std::cout << "Min db: " << min << " Max db: " << max << " increment db: " << inc << std::endl;
	return true;
}


bool printChanelVolumes(IAudioEndpointVolume* endPtVol){
	HRESULT hr;

	UINT channelCount = 0;

	hr = endPtVol->GetChannelCount(&channelCount);
	if (FAILED(hr)){
		return false;
	}
	std::cout << "Channel Count: " << channelCount << std::endl;

	for (UINT i = 0; i < channelCount; i++){
		float level = 0;
		hr = endPtVol->GetChannelVolumeLevel(i, &level);
		if (FAILED(hr)){
			return false;
		}
		std::cout << "Channel: " << i << " db: " << level << std::endl;
	}
	return true;
}


/*
 * retName the returned name of the process.
 */
BOOL GetProcessName(DWORD& pid, WCHAR * retName)
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
		return(FALSE);
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		printError(TEXT("Process32First")); // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(FALSE);
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL){}
		//	printError(TEXT("OpenProcess"));
		else
		{
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
				printError(TEXT("GetPriorityClass"));
			CloseHandle(hProcess);
		}

		//std::cout << pe32.th32ProcessID << " " << pid << std::endl;

		if (pe32.th32ProcessID == pid){
			//_tprintf(TEXT("\n\n====================================================="));
		//	_tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
			//_tprintf(TEXT("\n-------------------------------------------------------"));

			//_tprintf(TEXT("\n  Process ID        = 0x%08X"), pe32.th32ProcessID);
			//wcsncpy_s(retName, pe32.szExeFile, 260);
			wcsncpy_s(retName, 260, pe32.szExeFile, 260);
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(TRUE);
}


BOOL ListProcessModules(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot (of modules)"));
		return(FALSE);
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &me32))
	{
		printError(TEXT("Module32First"));  // show cause of failure
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return(FALSE);
	}

	// Now walk the module list of the process,
	// and display information about each module
	do
	{
		_tprintf(TEXT("\n\n     MODULE NAME:     %s"), me32.szModule);
		_tprintf(TEXT("\n     Executable     = %s"), me32.szExePath);
		_tprintf(TEXT("\n     Process ID     = 0x%08X"), me32.th32ProcessID);
		_tprintf(TEXT("\n     Ref count (g)  = 0x%04X"), me32.GlblcntUsage);
		_tprintf(TEXT("\n     Ref count (p)  = 0x%04X"), me32.ProccntUsage);
		_tprintf(TEXT("\n     Base address   = 0x%08X"), (DWORD)me32.modBaseAddr);
		_tprintf(TEXT("\n     Base size      = %d"), me32.modBaseSize);

	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return(TRUE);
}

BOOL ListProcessThreads(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return(FALSE);

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if (!Thread32First(hThreadSnap, &te32))
	{
		printError(TEXT("Thread32First")); // show cause of failure
		CloseHandle(hThreadSnap);          // clean the snapshot object
		return(FALSE);
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			_tprintf(TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID);
			_tprintf(TEXT("\n     Base priority  = %d"), te32.tpBasePri);
			_tprintf(TEXT("\n     Delta priority = %d"), te32.tpDeltaPri);
			_tprintf(TEXT("\n"));
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return(TRUE);
}

void printError(TCHAR* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	_tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}