#include "Multistdafx.h"

#include "SigDirectShow.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "strmiids.lib")
#define WM_GRAPHNOTIFY  WM_USER+13

CMoviePlayer::CMoviePlayer() :
pGB(NULL),
pMC(NULL),
pME(NULL),
pVW(NULL),
pBA(NULL),
pBV(NULL),
pMS(NULL),
g_bAudioOnly(FALSE),
g_lVolume(VOLUME_FULL),
g_dwGraphRegister(0),
g_psCurrent(Stopped)

{ ; }

BOOL CMoviePlayer::Open(LPCTSTR name, HWND hParentWnd) {
// FilterGraphを生成
//	::CoCreateInstanceEx
	HRESULT _res;

	if( SUCCEEDED( ::CoCreateInstance(CLSID_FilterGraph, NULL,	CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID*)&m_pGraphBuilder)) )
	{
		m_pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMediaControl); // Graphを生成
		m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID *)&m_pMediaEvent);

//		m_pMediaControl->RenderFile(FILENAME);
		//	pMediaControl->RenderFile(FILENAME);
		if ( hParentWnd != NULL )
		{
			m_pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVideoWindow);
			m_pVideoWindow->put_Owner((OAHWND)hParentWnd);
			m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

			RECT rect;
			::GetClientRect(hParentWnd, &rect);
			m_pVideoWindow->SetWindowPosition(0, 0, rect.right - rect.left, rect.bottom - rect.top);

			m_hWnd = hParentWnd;
		}
	}

	return TRUE;
}

void CMoviePlayer::Run()
{
	if (m_hWnd == NULL && m_pVideoWindow != NULL)
	{
		m_pVideoWindow->SetWindowForeground(OATRUE);
		m_pVideoWindow->put_Visible(OATRUE);
	}
	m_pMediaControl->Run();
}

int CMoviePlayer::GetState(LONG _timeout)
{
	return m_pMediaControl->GetState(_timeout, NULL);
}

HRESULT CMoviePlayer::PlayMovieInWindow(HWND hWnd, LPTSTR szFile, DWORD posx, DWORD posy, DWORD width, DWORD height){
	WCHAR wFile[MAX_PATH];

    // Clear open dialog remnants before calling RenderFile()
    //UpdateWindow(hwnd);

#ifndef UNICODE
    MultiByteToWideChar(CP_ACP, 0, szFile, -1, wFile, MAX_PATH);
#else
    lstrcpy(wFile, szFile);
#endif
	// Get the interface for DirectShow's GraphBuilder
	hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGB);
	if(hr)return hr;

	// Have the graph construct its the appropriate graph automatically
	pGB->RenderFile(wFile, NULL);

	// QueryInterface for DirectShow interfaces
	pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
	pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME);
	pGB->QueryInterface(IID_IMediaSeeking, (void **)&pMS);

	// Query for video interfaces, which may not be relevant for audio files
	pGB->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	pGB->QueryInterface(IID_IBasicVideo, (void **)&pBV);

	// Query for audio interfaces, which may not be relevant for video-only files
	pGB->QueryInterface(IID_IBasicAudio, (void **)&pBA);

	// Is this an audio-only file (no video component)?
	CheckVisibility();

	if(!g_bAudioOnly){
		pVW->put_Owner((OAHWND)hWnd);
		pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	}
	// Have the graph signal event via window callbacks for performance
	pME->SetNotifyWindow((OAHWND)hWnd, WM_GRAPHNOTIFY, 0);

//	RECT rect;
//	GetClientRect(hwnd, &rect);

//	SetRECT2(&rect2,posx,posy,width,height);

//	pVW->SetWindowPosition(rect.left + posx, rect.top + posy, rect.left + width, rect.top + height);
	pVW->put_MessageDrain( (OAHWND)hWnd );

	//メッセージをhwndに通知するように
	// Let's get ready to rumble!

#ifdef REGISTER_FILTERGRAPH
	hr = AddGraphToRot(pGB, &g_dwGraphRegister);
	if (FAILED(hr))
		g_dwGraphRegister = 0;
#endif
	// Run the graph to play the media file
	pMC->Run();

//	ResetAllEvent();
	g_psCurrent=Running;

	SetVolume(15);

//	while(::WaitForMultipleObjects(KEYEVENT_MAX,event,FALSE,100) == WAIT_TIMEOUT){
		//HRESULT ddf = pVW->put_BorderColor(RGB(255,255,0) );
		//InvalidateRect(hwnd,NULL,TRUE);
		//::SendMessage(hwnd,WM_PAINT,0,0);
//	}

	//::SetWindowLong(hwnd,GWL_EXSTYLE,wexs);
	//::SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

	m_hWnd = hWnd;

	return hr;
}

HRESULT CMoviePlayer::InitVideoWindow(int nMultiplier, int nDivider){
	LONG lHeight, lWidth;
	hr = S_OK;
	RECT rect;

	if (!pBV)
		return S_OK;

    // Read the default video size
    JIF(pBV->GetVideoSize(&lWidth, &lHeight));

    // Account for requests of normal, half, or double size
    lWidth  = lWidth  * nMultiplier / nDivider;
    lHeight = lHeight * nMultiplier / nDivider;

    SetWindowPos(m_hWnd,NULL,0,0,lWidth,lHeight,SWP_NOMOVE | SWP_NOOWNERZORDER);

    int nTitleHeight  = GetSystemMetrics(SM_CYCAPTION);
    int nBorderWidth  = GetSystemMetrics(SM_CXBORDER);
    int nBorderHeight = GetSystemMetrics(SM_CYBORDER);

    // Account for size of title bar and borders for exact match
    // of window client area to default video size
    SetWindowPos(m_hWnd, NULL, 0, 0, lWidth + 2*nBorderWidth,lHeight + nTitleHeight + 2*nBorderHeight,SWP_NOMOVE | SWP_NOOWNERZORDER);

    GetClientRect(m_hWnd, &rect);
    JIF(pVW->SetWindowPosition(rect.left, rect.top, rect.right, rect.bottom));
    return hr;
}

void CMoviePlayer::SetVolume(double vol){
	val = vol;
	double v =  ( 20.0 * log10( val / 100.0 ) ) * 100.0;
	if(v > 0.0)
		v = 0.0;
	if(v < -10000.0)
		v = -10000.0;
	pBA->put_Volume( (long) v);
}

HRESULT CMoviePlayer::InitPlayerWindow(void){
	// Reset to a default size for audio and after closing a clip
	SetWindowPos(m_hWnd, NULL,0,0,DEFAULT_AUDIO_WIDTH,DEFAULT_AUDIO_HEIGHT,SWP_NOMOVE | SWP_NOOWNERZORDER);
    return S_OK;
}

void CMoviePlayer::MoveVideoWindow(void){
    // Track the movement of the container window and resize as needed
    if(pVW){
        RECT client;

        GetClientRect(m_hWnd, &client);
        hr = pVW->SetWindowPosition(client.left,client.top,client.right,client.bottom);
    }
}

void CMoviePlayer::CheckVisibility(void){
    long lVisible;

    g_bAudioOnly = FALSE;

    if ((!pVW) || (!pBV)){
		g_bAudioOnly = TRUE;
		return;
    }

    hr = pVW->get_Visible(&lVisible);
    if (FAILED(hr)){
        // If this is an audio-only clip, get_Visible() won't work.
        //
        // Also, if this video is encoded with an unsupported codec,
        // we won't see any video, although the audio will work if it is
        // of a supported format.
        //
        if (hr == E_NOINTERFACE)
			g_bAudioOnly = TRUE;
    }
}

void CMoviePlayer::PauseClip(void){
    if (!pMC)
        return;

    // Toggle play/pause behavior
    if((g_psCurrent == Paused) || (g_psCurrent == Stopped)){
        hr = pMC->Run();
        g_psCurrent = Running;
    }
    else{
        hr = pMC->Pause();
        g_psCurrent = Paused;
    }
    //UpdateMainTitle();
}

void CMoviePlayer::StopClip(void){
    if ((!pMC) || (!pMS))
        return;

    // Stop and reset postion to beginning
    if((g_psCurrent == Paused) || (g_psCurrent == Running)){
        LONGLONG pos = 0;
        hr = pMC->Stop();
        g_psCurrent = Stopped;

        hr = pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
            NULL, AM_SEEKING_NoPositioning);

        // Display the first frame to indicate the reset condition
        hr = pMC->Pause();
    }
}

void CMoviePlayer::CloseClip(){
    if(pMC)
        hr = pMC->Stop();

    g_psCurrent = Stopped;
    g_bAudioOnly = TRUE;

    CloseInterfaces();

    // Clear file name to allow selection of new file with open dialog
    g_szFileName[0] = L'\0';

    // No current media state
    g_psCurrent = Initial;

    RECT rect;
	::GetClientRect(m_hWnd, &rect);
	::InvalidateRect(m_hWnd, &rect, TRUE);
}

void CMoviePlayer::CloseInterfaces(void){
    // Relinquish ownership (IMPORTANT!) after hiding video window
    if(pVW){
        hr = pVW->put_Visible(OAFALSE);
        hr = pVW->put_Owner(NULL);
    }
#ifdef REGISTER_FILTERGRAPH
    if (g_dwGraphRegister){
        RemoveGraphFromRot(g_dwGraphRegister);
        g_dwGraphRegister = 0;
    }
#endif
    SAFE_RELEASE(pME);
    SAFE_RELEASE(pMS);
    SAFE_RELEASE(pMC);
    SAFE_RELEASE(pBA);
    SAFE_RELEASE(pBV);
    SAFE_RELEASE(pVW);
    SAFE_RELEASE(pGB);
}

#ifdef REGISTER_FILTERGRAPH
HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister){
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[128];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}

void MOVIEPLAYER8::RemoveGraphFromRot(DWORD pdwRegister){
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}
#endif

HRESULT CMoviePlayer::ToggleMute(void){
    hr = S_OK;

    if ((!pGB) || (!pBA))
        return S_OK;

    // Read current volume
    hr = pBA->get_Volume(&g_lVolume);
    if (hr == E_NOTIMPL)
		return S_OK;
    else if (FAILED(hr))
		return hr;

    // Switch volume levels
    if (g_lVolume == VOLUME_FULL)
        g_lVolume = VOLUME_SILENCE;
    else
        g_lVolume = VOLUME_FULL;

    // Set new volume
    JIF(pBA->put_Volume(g_lVolume));

    //UpdateMainTitle();
    return hr;
}

HRESULT CMoviePlayer::ToggleFullScreen(void){
    hr = S_OK;
    LONG lMode;
    static HWND hDrain=0;

    // Don't bother with full-screen for audio-only files
    if ((g_bAudioOnly) || (!pVW))
        return S_OK;

    if (!pVW)
        return S_OK;

    // Read current state
    JIF(pVW->get_FullScreenMode(&lMode));

    if (lMode == OAFALSE){
        // Save current message drain
        LIF(pVW->get_MessageDrain((OAHWND *) &hDrain));

        // Set message drain to application main window
        LIF(pVW->put_MessageDrain((OAHWND) m_hWnd));

        // Switch to full-screen mode
        lMode = OATRUE;
        JIF(pVW->put_FullScreenMode(lMode));
    }
    else
    {
        // Switch back to windowed mode
        lMode = OAFALSE;
        JIF(pVW->put_FullScreenMode(lMode));

        // Undo change of message drain
        LIF(pVW->put_MessageDrain((OAHWND) hDrain));

        // Reset video window
        LIF(pVW->SetWindowForeground(-1));

		// Reclaim keyboard focus for player application
		// UpdateWindow(ghApp);
		::SetForegroundWindow(m_hWnd);
		::SetFocus(m_hWnd);
    }
    return hr;
}

HRESULT CMoviePlayer::HandleGraphEvent(void){
    LONG evCode;
	LONG_PTR  evParam1, evParam2;
    hr = S_OK;

	while(SUCCEEDED(pME->GetEvent(&evCode, &evParam1, &evParam2, 0))){
        // Spin through the events
        hr = pME->FreeEventParams(evCode, evParam1, evParam2);

		if(EC_COMPLETE == evCode){
            LONGLONG pos=0;
//			::SetEvent(event[KEY_RESERVED1]);
			return hr;
            // Reset to first frame of movie
        }
    }
    return hr;
}

LRESULT CMoviePlayer::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static PAINTSTRUCT ps;

	switch(message){
	case WM_PAINT:
		::BeginPaint(hwnd,&ps);
		if(pVW){
			RECT crect,vwrect,trect;
			::GetClientRect(hwnd,&crect);
			crect.right -= crect.left;crect.left = 0;crect.bottom -= crect.top;crect.top = 0;

			pVW->GetWindowPosition(&vwrect.left,&vwrect.top,&vwrect.right,&vwrect.bottom);
			POINT pt;pt.x = vwrect.left;pt.y = vwrect.top;
			ScreenToClient(hwnd,&pt);

			SetRect(&trect,crect.left,crect.top,crect.right,pt.y);
			FillRect( ps.hdc,&trect,(HBRUSH)GetStockObject(BLACK_BRUSH) );

			SetRect(&trect,crect.left,vwrect.bottom + pt.y,crect.right,crect.bottom);
			FillRect( ps.hdc,&trect,(HBRUSH)GetStockObject(BLACK_BRUSH) );
		}
		::EndPaint(hwnd,&ps);
		return 1;
	case WM_GRAPHNOTIFY:
		HandleGraphEvent();
		return 1;
	// Resize the video when the window changes
	case WM_MOVE:
	case WM_SIZE:
		if (!g_bAudioOnly && pVW){
			RECT rect;
			::GetClientRect(m_hWnd, &rect);
			pVW->SetWindowPosition(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		return 1;
		}
	}
	return 0;
}
