#include "stdafx.h"
#include "SoundFunctions.h"

// todo these endptvol s were g_gptend or what everthe fuck

bool muteUnmute(IAudioEndpointVolume* endPtVol){
	BOOL mute;
	HRESULT hr;
	hr = endPtVol->GetMute(&mute);
	if (FAILED(hr)) return false;
	hr = endPtVol->SetMute(!mute, NULL);
	if (FAILED(hr)) return false;
	std::cout << "mute set: " << mute << std::endl;
	return true;
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
	return true;
}



/*
* returns false on failure.
*/
bool printVolumeRange(IAudioEndpointVolume * endPtVol){
	float  min = 0;
	float max = 0;
	float inc = 0;
	HRESULT hr;
	hr = endPtVol->GetVolumeRange(&min, &max, &inc);
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
* returns true if successful.
*/
bool changeSessionVol(ISimpleAudioVolume* av, float level){
	HRESULT hr = av->SetMasterVolume(level, NULL);
	if (FAILED(hr)){
		return false;
	}
	return true;

}

bool muteUnmuteSession(ISimpleAudioVolume * av){
	BOOL muted;
	HRESULT hr = av->GetMute(&muted);
	if (FAILED(hr)){
		return false;
	}

	hr = av->SetMute(!muted, NULL);
	if (FAILED(hr)){
		return false;
	}
	return true;

}