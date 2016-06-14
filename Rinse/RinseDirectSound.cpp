#include "Multistdafx.h"

#include "SigDirectSound.h"

BOOL CDirectSound::CreateDevice ( LPGUID pGuid, const char* pDeviceName, const char* pDriverName ) {
	if( FAILED(DirectSoundCreate8(/*pGuid*/NULL,&m_pDirectSound,NULL) ) )
		return FALSE;

	m_DeviceName = pDeviceName;
	m_DriverName = pDriverName;

	string Mes = "サウンドデバイスを初期化:デバイス名=";
	Mes += pDeviceName;
	Mes += "\n";
	Mes += " ドライバ名=";
	Mes += pDriverName;
	Mes += "\n";

	::OutputDebugString(Mes.data());
	return TRUE;
}

BOOL CDirectSound::Release() {
	SafeRelease( m_pDirectSound );
	::OutputDebugString("DirectSoundを解放\n");
	return TRUE;
}

BOOL CDirectSound::SetCooperativeLevel( HWND hWnd, DWORD Level ) {
	return SUCCEEDED( m_pDirectSound->SetCooperativeLevel ( hWnd, Level/*DSSCL_PRIORITY*/) );
}

BOOL CALLBACK EnumDirectSoundCallback( LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext ) {
	CDirectSound* pDirectSound = (CDirectSound*)lpContext;

	GUID DefaultGuid;

	GetDeviceID( &DSDEVID_DefaultPlayback, &DefaultGuid );

	if( (lpGuid != NULL) && *lpGuid == DefaultGuid ) {
		pDirectSound->CreateDevice( lpGuid, lpcstrDescription, lpcstrModule);
		return FALSE;
	}
	return TRUE;
}

BOOL CPrimarySoundBuffer::CreateBuffer( HWND hWnd ) {
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(dsbd) );
	dsbd.dwSize        = sizeof(dsbd);
	dsbd.dwFlags       = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;
	
	if( SUCCEEDED( g_pDirectSound->GetInstance()->CreateSoundBuffer( &dsbd, &m_pSoundBuffer, NULL ) ) ){
		SetFormat(44100,16,2);
		m_pSoundBuffer->GetVolume( &m_InitialVolume );
		g_pDirectSound->GetInstance()->SetCooperativeLevel( hWnd, DSSCL_PRIORITY );
		SetHWND( hWnd );
		return TRUE;
	}
	return FALSE;
}

BOOL CPrimarySoundBuffer::ReleaseBuffer() {
	if ( m_pSoundBuffer ) {
		m_pSoundBuffer->Stop();
		SafeRelease(m_pSoundBuffer);//->Release();
		::OutputDebugString("サウンドプライマリバッファを解放\n");
	}
	return TRUE;
}

BOOL CPrimarySoundBuffer::SetFormat(DWORD SamplingLate,WORD BitLate,WORD Channels){
	//強調レベルを上げないと変更不可
	WAVEFORMATEX wfex;

	SetPCMFormat(wfex,SamplingLate,BitLate,Channels);

	return SUCCEEDED( m_pSoundBuffer->SetFormat(&wfex) );
}

CDirectSoundBuffer::CDirectSoundBuffer(){
	m_pSoundBuffer	= NULL;
	m_pSoundNotify	= NULL;
	SetPCMFormat(m_oFormat,44100,16,1);
	m_bPlaying		= FALSE;
	m_BufferSize	= 0;
}

BOOL CDirectSoundBuffer::Play(BOOL bLoop){
	m_oNotifyEvent.Off();

	if(m_pSoundBuffer == NULL)
		return FALSE;

	return SUCCEEDED( m_pSoundBuffer->Play(0,0,bLoop ? DSBPLAY_LOOPING : 0) );
}

BOOL CDirectSoundBuffer::Stop(){
	if(m_pSoundBuffer == NULL)
		return FALSE;

	return SUCCEEDED( m_pSoundBuffer->Stop() );
}

BOOL CDirectSoundBuffer::Create(DWORD Size){
	DSBUFFERDESC dsbd;
	::ZeroMemory( &dsbd, sizeof(dsbd) );
	dsbd.dwSize        = sizeof(dsbd);
	dsbd.dwFlags       = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
	dsbd.dwBufferBytes = Size;
	dsbd.lpwfxFormat   = &m_oFormat;
		
	LPDIRECTSOUNDBUFFER pTempBuffer = NULL;

	if( FAILED( g_pDirectSound->GetInstance()->CreateSoundBuffer(&dsbd, &pTempBuffer,NULL) ) )
		return FALSE;

	if( SUCCEEDED( pTempBuffer->QueryInterface( IID_IDirectSoundBuffer8, (void **)&m_pSoundBuffer ) ) ){
		if( FAILED(m_pSoundBuffer->QueryInterface(IID_IDirectSoundNotify8,(LPVOID*)&m_pSoundNotify) ) )
			::OutputDebugString("SoundNotify作成に失敗\n");
	}

	SafeRelease(pTempBuffer);

	m_BufferSize = Size;
/*
	Lock(0,buffersize);
	::ZeroMemory(decodebuffer,buffersize);
	Unlock();

	UpdateVolume();
*/
	return TRUE;
}

void CDirectSoundBuffer::Release(){
	SafeRelease(m_pSoundNotify);
	SafeRelease(m_pSoundBuffer);
}

void CDirectSoundBuffer::SetFormat(DWORD SamplingLate,WORD BitLate,WORD Channels){
    SetPCMFormat(m_oFormat,SamplingLate,BitLate,Channels);
}

BOOL CDirectSoundBuffer::SetPlayPos(DWORD Offset){
	return SUCCEEDED( m_pSoundBuffer->SetCurrentPosition(Offset) );
}

BOOL CDirectSoundBuffer::SetNotifyPos(DWORD Offset){
	DSBPOSITIONNOTIFY dsbn = { Offset,m_oNotifyEvent.GetHandle() };

	if(m_pSoundNotify == NULL)
		return FALSE;

	return SUCCEEDED( m_pSoundNotify->SetNotificationPositions(1,&dsbn) );
}

