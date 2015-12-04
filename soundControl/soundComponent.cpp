#include "stdafx.h"
#include "soundComponent.h"



#define PRINT_ON_ERROR(hr) \
if (FAILED(hr)) { std::cout << "error: " << hr << std::endl; return; }


SoundComponent::SoundComponent(){
	initalise();
	sessions = std::vector<AudioSession>();
	getEntrys(sessions);
}

SoundComponent::SoundComponent(Server * s) : server(s){
	initalise();
	sessions = std::vector<AudioSession>();
	getEntrys(sessions);
}


void SoundComponent::initalise(){
	CoInitialize(NULL);

	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;

	// Get enumerator for audio endpoint devices.
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator);
	PRINT_ON_ERROR(hr)

		// Get default audio-rendering device.
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	PRINT_ON_ERROR(hr)

		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&master);
	PRINT_ON_ERROR(hr)


		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **)&audioClient);
	PRINT_ON_ERROR(hr)

		hr = pDevice->Activate(__uuidof(IAudioSessionManager), CLSCTX_ALL, NULL, (void **)&audioManager);
	PRINT_ON_ERROR(hr)

		hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&auMan2);
	PRINT_ON_ERROR(hr)

}


void SoundComponent::getEntrys(std::vector<AudioSession>& list){


	ISimpleAudioVolume *simpleVol = NULL;



	if (!auMan2)
	{
		PRINT_ON_ERROR(E_INVALIDARG);
	}

	HRESULT hr = S_OK;

	int cbSessionCount = 0;
	DWORD pswSession = NULL;

	IAudioSessionEnumerator* pSessionList = NULL;
	IAudioSessionControl2* pSessionControl2 = NULL;
	IAudioSessionControl* pSessionControl = NULL;

	// Get the current list of sessions.
	FAILED(hr = auMan2->GetSessionEnumerator(&pSessionList));

	// Get the session count.
	FAILED(hr = pSessionList->GetCount(&cbSessionCount));




	for (int index = 0; index < cbSessionCount; index++)
	{
		FAILED(hr = pSessionList->GetSession(index, &pSessionControl));
		FAILED(hr = pSessionControl->QueryInterface(
			__uuidof(IAudioSessionControl2), (void**)&pSessionControl2));

		if (FAILED(hr = pSessionControl2->GetProcessId(&pswSession))){
			continue;
		}


		WCHAR * name = new WCHAR[MAX_NAME_LENGTH]();
		BOOL result = GetProcessName(pswSession, name);
		if (!result){
			continue;
		}
		//LPWSTR ret = NULL;
		/*	hr = pSessionControl->GetDisplayName(&ret);
		if (FAILED(hr)){
		std::cout << "error" << std::endl;
		CoTaskMemFree(ret);
		return;
		}

		if (wcslen(ret) != 0){
		name = ret;

		}*/

		HRESULT ret = pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)& simpleVol);

		if (FAILED(hr)){
			continue;
		}
		list.emplace_back(simpleVol, pswSession, name);
	}
}

/*
* retName the returned name of the process.
*/
BOOL SoundComponent::GetProcessName(DWORD& pid, WCHAR * retName)
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

float SoundComponent::setMasterVol(float vol){
	float level = 0;
	HRESULT hr = master->SetMasterVolumeLevel(vol, NULL);
	if (FAILED(hr)) return -1;
	return level;
}

float SoundComponent::getMasterVol(){
	float level = 0;
	HRESULT hr = master->GetMasterVolumeLevelScalar(&level);
	if (FAILED(hr)) return -1;
	return level;
}

float SoundComponent::getMasterVolScalar(){
	float level = 0;
	HRESULT hr = master->GetMasterVolumeLevelScalar(&level);
	if (FAILED(hr)) return -1;
	return level;
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

AudioSession* SoundComponent::getAudioSession(int pid){
	for (auto& as : sessions){
		if (as.pid == pid){
			return &as;
		}
	}
	return NULL;
}