#include "stdafx.h"
#include "entry.h"

	float Entry::getMasterVolume(){
		float f;
		HRESULT hr = volume->GetMasterVolume(&f);
		if (FAILED(hr)){
			return 0;
		}
		return f;
	}

	Entry::Entry(ISimpleAudioVolume *v, DWORD pid, WCHAR n[MAX_NAME_LENGTH]) : volume(v), pid(pid) {
		name = n;
	}
