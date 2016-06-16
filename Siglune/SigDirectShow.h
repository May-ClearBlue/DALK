#ifndef _SAIL_DSHOW_
#define _SAIL_DSHOW_

#include <MultiStdAfx.h>
#include <dshow.h>

#define VOLUME_FULL     0L
#define VOLUME_SILENCE  -10000L

// Defaults used with audio-only files
#define DEFAULT_AUDIO_WIDTH     320
#define DEFAULT_AUDIO_HEIGHT    180
#define DEFAULT_VIDEO_WIDTH     640
#define DEFAULT_VIDEO_HEIGHT    360

#define WM_GRAPHNOTIFY  WM_USER+13
//
// Macros
//
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define JIF(x) if (FAILED(hr=(x))) \
    { _DEBUGPRINTF("Failed"); return hr;}

#define LIF(x) if (FAILED(hr=(x))) \
    { _DEBUGPRINTF("Failed"); }

class CMoviePlayer {
public:
	CMoviePlayer();

protected:
	enum PLAYSTATE { Stopped, Paused, Running, Initial };

	HWND			m_hWnd;
	IGraphBuilder*	m_pGraphBuilder;
	IMediaControl*	m_pMediaControl;
	IMediaEvent*	m_pMediaEvent;
	IVideoWindow*	m_pVideoWindow;
	LONG			m_EventCode;

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
	RECT			rect2;
	
//	DWORD			px,py;

	double			val;//音量(０～１００)

	HRESULT			hr;

	HRESULT ToggleFullScreen(void);
	HRESULT ToggleMute(void);

public:
	BOOL	Open( LPCTSTR name, HWND hParentWnd = NULL );
	void	Run();
	int		GetState(LONG _timeout = 0);

	HRESULT	PlayMovieInWindow(HWND hWnd, LPTSTR szFile,DWORD posx,DWORD posy,DWORD width = 0,DWORD height = 0);
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
};

#endif