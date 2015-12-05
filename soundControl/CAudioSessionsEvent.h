#include <cstdio>
#include <audiopolicy.h>
//-----------------------------------------------------------
// Client implementation of IAudioSessionEvents interface.
// WASAPI calls these methods to notify the application when
// a parameter or property of the audio session changes.
//-----------------------------------------------------------


extern void sendReply(SoundComponent& soundComponent);

class CAudioSessionEvents : public IAudioSessionEvents
{
	LONG _cRef;
	LPCGUID _lpcguid;
	SoundComponent& soundComponent;

public:
	CAudioSessionEvents(LPCGUID lpcguid, SoundComponent& soundComponent) :
		_cRef(1), _lpcguid(lpcguid), soundComponent(soundComponent)
	{
	}

	~CAudioSessionEvents()
	{
	}

	// IUnknown methods -- AddRef, Release, and QueryInterface

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID  riid,
		VOID  **ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IAudioSessionEvents) == riid)
		{
			AddRef();
			*ppvInterface = (IAudioSessionEvents*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	// Notification methods for audio session events

	HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(
		LPCWSTR NewDisplayName,
		LPCGUID EventContext)
	{
		if (IsEqualGUID(*EventContext, *_lpcguid)){
			printf("OnDisplayNameChanged by self.\n");
		}
		else {
			printf("OnDisplayNameChanged\n");
			sendReply(soundComponent);
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnIconPathChanged(
		LPCWSTR NewIconPath,
		LPCGUID EventContext)
	{
		if (IsEqualGUID(*EventContext, *_lpcguid)){
			printf("OnIconPathChanged by self.\n");
		}
		else {
			printf("OnIconPathChanged\n");
			sendReply(soundComponent);
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		float NewVolume,
		BOOL NewMute,
		LPCGUID EventContext)
	{
		if (IsEqualGUID(*EventContext, *_lpcguid)){
			printf("OnSimpleVolumeChanged by self.\n");
		}
		else {
			printf("OnSimpleVolumeChanged\n");
			sendReply(soundComponent);
		}

		if (NewMute)
		{
			printf("MUTE\n");
		}
		else
		{
			printf("Volume = %d percent\n",
				(UINT32)(100 * NewVolume + 0.5));
		}
		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(
		DWORD ChannelCount,
		float NewChannelVolumeArray[],
		DWORD ChangedChannel,
		LPCGUID EventContext)
	{
		if (IsEqualGUID(*EventContext, *_lpcguid)){
			printf("OnChannelVolumeChanged by self.\n");
		}
		else {
			printf("OnChannelVolumeChanged\n");
			sendReply(soundComponent);
		}
	
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(
		LPCGUID NewGroupingParam,
		LPCGUID EventContext)
	{
		if (IsEqualGUID(*EventContext, *_lpcguid)){
			printf("ongroupParamChanged by self.\n");
		}
		else {
			printf("ongroupParamChanged\n");
			sendReply(soundComponent);
		}
		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnStateChanged(
		AudioSessionState NewState)
	{
		char *pszState = "?????";

		switch (NewState)
		{
		case AudioSessionStateActive:
			pszState = "active";
			break;
		case AudioSessionStateInactive:
			pszState = "inactive";
			break;
		}
		printf("New session state = %s\n", pszState);
		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		AudioSessionDisconnectReason DisconnectReason)
	{
		char *pszReason = "?????";

		switch (DisconnectReason)
		{
		case DisconnectReasonDeviceRemoval:
			pszReason = "device removed";
			break;
		case DisconnectReasonServerShutdown:
			pszReason = "server shut down";
			break;
		case DisconnectReasonFormatChanged:
			pszReason = "format changed";
			break;
		case DisconnectReasonSessionLogoff:
			pszReason = "user logged off";
			break;
		case DisconnectReasonSessionDisconnected:
			pszReason = "session disconnected";
			break;
		case DisconnectReasonExclusiveModeOverride:
			pszReason = "exclusive-mode override";
			break;
		}
		printf("Audio session disconnected (reason: %s)\n",
			pszReason);

		return S_OK;
	}
};

