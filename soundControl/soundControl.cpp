// soundControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
/*
#include "soundControl.h"
#include "soundComponent.h"
#include "entry.h"
#include "server.h"

#include <vector>
*/
//#define EXIT_ON_ERROR(hr)  \
//if (FAILED(hr)) { std::cout << "error" << std::endl; exit(hr); }
// todo
// get list of all sound sessions
// with each session get the volume might want to keep a reference of the control to change volume.

/*
int _tmain(int argc, TCHAR* argv[])
{
	// start new thread to wait for socket connection.

	// get  master volume.
	// get volume for each session. (group if needed).

	initalise();

	SoundComponent sc= SoundComponent(g_pEndptVol);

	//initaliseSC(sc);

	Server s = Server(sc);
	s.run();




	

	//std::this_thread::sleep_for(std::chrono::milliseconds(30000));

	std::wcout << L"Master vol: " << sc.getMasterVol() << std::endl;

	float level;
	sc.master->GetMasterVolumeLevelScalar(&level);
	std::cout << sc.getMasterVolScalar() << std::endl;


	//changeSessionVol(sc.sessions[2].volume, 0.5);

//	muteUnmuteSession(sc.sessions[2].volume);
//	changeSessionVol(sc.sessions[2].volume, 0.4);




	for (Entry e : sc.sessions){
		std::wcout << e.name << " pid: " << e.pid << " vol: " << e.getMasterVolume() << std::endl;
		s.sendName(e.name, wcslen(e.name));
	}


	s.exit();
	int waitval = 0;
//	while waitval 
	std::cin >> waitval;
	return 0;
}
*/

/*
 * sends audio sessions to client
 */
void sendSessions(){
	// send the session info to the client: name, current vol, current mute, picture

}

void sendSessionsListUpdate(){
	// send update when new program is added,
}

void sendSessionUpdate(){
	// update to a single (at a time) session, i.e. windows muted/changed vol.
}

/*
* updates session with new settings from client
*/
void receiveSettings(){
	// make the change to windows
	// send the change back (i.e. currently -1 if error on updating e.t.c)

}
/*
int _tmain2(int argc, _TCHAR* argv[])
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



	for (int i = 0; i < 40; i++){
		if (FAILED(EnumSessions(auMan2, audioClient))){
			std::cout << "error" << std::endl;
			goto Exit;
		}
		std::cout << std::endl;

		Sleep(2000);

	}


		//GetProcessList();



Exit:
	std::cin >> in; 
	return 0;
}*/
/*
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
}*/
/*
HRESULT EnumSessions(IAudioSessionManager2* pSessionManager, IAudioClient * audioClient)
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

		ISimpleAudioVolume * audioVolume = NULL;


		LPCGUID * audioGuid = NULL;

		ISimpleAudioVolume * simpleAudioVol = NULL; 

		//audioClient->GetService(__uuidof(ISimpleAudioVolume), (void **) & simpleAudioVol);

	/ *	IAudioEndpointVolume * ev = NULL; 
		pSessionControl->QueryInterface(__uuidof(IAudioEndpointVolume), (void **)& ev);
		float level = 0;

		if (ev == NULL) std::cout << "ev was null" << std::endl;
		ev->GetMasterVolumeLevel(&level);
		* /
		
		pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)& simpleAudioVol);

		float level = 0;
		simpleAudioVol->GetMasterVolume(&level);
		

		std::cout << level << std::endl;
		//simpleAudioVol->

	//	pSessionManager->GetSimpleAudioVolume(audioGuid, FALSE, (void **)& audioVolume);
	}

//	CoTaskMemFree(pswSession);
//	SAFE_RELEASE(pSessionControl);
//	SAFE_RELEASE(pSessionList);

	return hr;

}


*/


/*
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
}*/
