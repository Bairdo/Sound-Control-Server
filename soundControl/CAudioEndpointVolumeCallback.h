
#include <windows.h>
#include <commctrl.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#define IDC_SLIDER_VOLUME      1001
#define IDC_CHECK_MUTE         1002
#define IDC_STATIC_MINVOL      1003
#define IDC_STATIC_MAXVOL      1004

#include "soundComponent.h"

extern void sendReply(SoundComponent& soundComponent);

// Dialog handle from dialog box procedure
//extern HWND g_hDlg;



// Maximum volume level on trackbar
#define MAX_VOL  100

#define SAFE_RELEASE(punk)  \
if ((punk) != NULL)  \
{ (punk)->Release(); (punk) = NULL; }

//-----------------------------------------------------------
// Client implementation of IAudioEndpointVolumeCallback
// interface. When a method in the IAudioEndpointVolume
// interface changes the volume level or muting state of the
// endpoint device, the change initiates a call to the
// client's IAudioEndpointVolumeCallback::OnNotify method.
//-----------------------------------------------------------
class CAudioEndpointVolumeCallback : public IAudioEndpointVolumeCallback
{
	LONG _cRef;
	SoundComponent& soundComponent;
	// Client's proprietary event-context GUID
	GUID g_guidMyContext;

public:
	CAudioEndpointVolumeCallback(GUID guid, SoundComponent& soundComponent) :
		_cRef(1), g_guidMyContext(guid), soundComponent(soundComponent)
	{
	}

	~CAudioEndpointVolumeCallback()
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

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IAudioEndpointVolumeCallback) == riid)
		{
			AddRef();
			*ppvInterface = (IAudioEndpointVolumeCallback*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	// Callback method for endpoint-volume-change notifications.

	HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
	{
		if (pNotify == NULL)
		{
			return E_INVALIDARG;
		}
		/*if (g_hDlg != NULL && pNotify->guidEventContext != g_guidMyContext)
		{
			PostMessage(GetDlgItem(g_hDlg, IDC_CHECK_MUTE), BM_SETCHECK,
				(pNotify->bMuted) ? BST_CHECKED : BST_UNCHECKED, 0);

			PostMessage(GetDlgItem(g_hDlg, IDC_SLIDER_VOLUME),
				TBM_SETPOS, TRUE,
				LPARAM((UINT32)(MAX_VOL*pNotify->fMasterVolume + 0.5)));
		}*/

		if (IsEqualGUID(pNotify->guidEventContext, g_guidMyContext)){
			printf("AudioEndpoint changed by self.\n");
		}
		else {
			printf("AudioEndpoint changed.\n");
			sendReply(soundComponent);
		}
		return S_OK;
	}
};