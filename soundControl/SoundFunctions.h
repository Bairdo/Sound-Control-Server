#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <endpointvolume.h>
#include <iostream>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <Audioclient.h>


bool muteUnmute(IAudioEndpointVolume* endPtVol);
bool changeVolume(IAudioEndpointVolume * endPtVol);


/*
* returns false on failure.
*/
bool printVolumeRange(IAudioEndpointVolume * endPtVol);
bool printChanelVolumes(IAudioEndpointVolume* endPtVol);

/*
* returns true if successful.
*/
bool changeSessionVol(ISimpleAudioVolume* av, float level);
bool muteUnmuteSession(ISimpleAudioVolume * av);