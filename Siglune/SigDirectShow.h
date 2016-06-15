#ifndef _SAIL_DSHOW_
#define _SAIL_DSHOW_

#include <dshow.h>

#define VOLUME_FULL     0L
#define VOLUME_SILENCE  -10000L

// Defaults used with audio-only files
#define DEFAULT_AUDIO_WIDTH     240
#define DEFAULT_AUDIO_HEIGHT    120
#define DEFAULT_VIDEO_WIDTH     320
#define DEFAULT_VIDEO_HEIGHT    240

#define WM_GRAPHNOTIFY  WM_USER+13

enum PLAYSTATE {Stopped, Paused, Running, Initial};
//
// Macros
//
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define JIF(x) if (FAILED(hr=(x))) \
    {ErrorOK("Failed"); return hr;}

#define LIF(x) if (FAILED(hr=(x))) \
    { ErrorOK("Failed"); }
/*
class MOVIEPLAYER8:public KEYEVENT{
protected:
	IGraphBuilder	*pGB;
	IMediaControl	*pMC;
	IMediaEventEx	*pME;
	IVideoWindow	*pVW;
	IBasicAudio		*pBA;
	IBasicVideo		*pBV;
	IMediaSeeking	*pMS;

	TCHAR			g_szFileName[MAX_PATH];
	BOOL			g_bAudioOnly;
	LONG			g_lVolume;
	DWORD			g_dwGraphRegister;
	PLAYSTATE		g_psCurrent;
	RECT2			rect2;
	
//	DWORD			px,py;

	double			val;//音量(０～１００)

	HRESULT			hr;

	HRESULT ToggleFullScreen(void);
	HRESULT ToggleMute(void);

public:
	HRESULT	PlayMovieInWindow(LPTSTR szFile,DWORD posx,DWORD posy,DWORD width = 0,DWORD height = 0);
	HRESULT InitPlayerWindow(void);
	HRESULT InitVideoWindow(int nMultiplier, int nDivider);
	HRESULT HandleGraphEvent(void);

	void	SetVolume(double vol);
	//void GetVolume();

	void	PaintAudioWindow(void);
	void	MoveVideoWindow(void);
	void	CheckVisibility(void);
	void	CloseInterfaces(void);

	void	PauseClip(void);
	void	StopClip(void);
	void	CloseClip(void);

	HRESULT	AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void	RemoveGraphFromRot(DWORD pdwRegister);

	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	MOVIEPLAYER8();
};
*/
#endif