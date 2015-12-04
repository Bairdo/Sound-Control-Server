#include "stdafx.h"
#include "entry.h"

	float AudioSession::getMasterVolume(){
		float f;
		HRESULT hr = volume->GetMasterVolume(&f);
		if (FAILED(hr)){
			return 0;
		}
		return f;
	}

	AudioSession::AudioSession(ISimpleAudioVolume *v, DWORD pid, WCHAR n[MAX_NAME_LENGTH]) : volume(v), pid(pid) {
		name = n;
	}
