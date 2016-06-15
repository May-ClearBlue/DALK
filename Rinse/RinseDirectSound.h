#ifndef _SAIL_DIRECTSOUND_
#define _SAIL_DIRECTSOUND_

#include "Rinse/RinseInitXAudio2.h"

//#define DIRECTSOUND_VERSION 0x0800 
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

#include "SigBase.h"
#include "Singleton.h"
#include "Rimlia\RimSignal.h"

#include <mmsystem.h>
#include <dsound.h>
#include <hash_map>
//ds->SetCooperativeLevel(Window->hwnd,DSSCL_PRIORITY);
//ds->SetCooperativeLevel(Window->hwnd,DSSCL_EXCLUSIVE);
//Set primary buffer format to 22kHz and 16-bit output
//primary->SetFormat(&wfx);
//static HRESULT lastdsounderror;
//inline HRESULT CreateDirectSoundNotify(LPDIRECTSOUNDBUFFER &dsb,LPDIRECTSOUNDNOTIFY &dsn){ return dsb->QueryInterface(IID_IDirectSoundNotify,(void **)&dsn); }

BOOL CALLBACK EnumDirectSoundCallback( LPGUID lpGuid,LPCSTR lpcstrDescription,LPCSTR lpcstrModule,LPVOID lpContext );

//--------------------デバイス(Singleton)
class CDirectSound {
private:
	LPDIRECTSOUND8	m_pDirectSound;
	string			m_DeviceName;
	string			m_DriverName;
public:
		CDirectSound()	{ m_pDirectSound = NULL; }
		~CDirectSound()	{ Release(); }

LPDIRECTSOUND8&	GetInstance()	{ return m_pDirectSound; }
inline	void	Initialize()	{ DirectSoundEnumerate( EnumDirectSoundCallback, this ); }
		BOOL	CreateDevice		( LPGUID pGuid, LPCTSTR pDeviceName, LPCTSTR pDriverName );
		BOOL	Release();
		BOOL	SetCooperativeLevel	( HWND hWnd, DWORD Level );
friend class CSingleton<CDirectSound>;
};

//--------------------プライマリバッファ(Singleton)
class CPrimarySoundBuffer : public CDirectXBase {
private:
	LPDIRECTSOUNDBUFFER	m_pSoundBuffer;
	LONG				m_InitialVolume;
public:
	CPrimarySoundBuffer()	{ m_pSoundBuffer = NULL; m_InitialVolume = 0; }
	~CPrimarySoundBuffer()	{ ReleaseBuffer(); }

	BOOL	CreateBuffer( HWND hWnd );
	BOOL	ReleaseBuffer();
	BOOL	SetFormat( DWORD SamplingLate, WORD BitLate, WORD Channels );
friend class CSingleton<CPrimarySoundBuffer>;
};

//シングルトン宣言
static CSingleton<CDirectSound> g_pDirectSound;
static CSingleton<CPrimarySoundBuffer> g_pPrimarySoundBuffer;

//--------------------サウンドバッファ基底クラス
class CDirectSoundBuffer {
protected:
//static	CDirectSound*			ms_pDirectSound;
//static	CPrimarySoundBuffer*	ms_pPrimaryBuffer;
		LPDIRECTSOUNDBUFFER8	m_pSoundBuffer;
		LPDIRECTSOUNDNOTIFY8	m_pSoundNotify;
		WAVEFORMATEX			m_oFormat;
		BOOL					m_bPlaying;
		DWORD					m_BufferSize;
		CSignal					m_oNotifyEvent;
		//Lock用
		DWORD					m_BufferSize1,m_BufferSize2;
		void					*m_DecodeBuffer1,*m_DecodeBuffer2;
public:
			CDirectSoundBuffer();
virtual		~CDirectSoundBuffer() { Release(); }
//static	void	SetDevice(	CDirectSound* pDirectSound, CPrimarySoundBuffer* pPrimarySoundBuffer ) { ms_pDirectSound = pDirectSound; ms_pPrimaryBuffer = pPrimarySoundBuffer; }
//
inline	int		WaitNotify(DWORD time)			{ return m_oNotifyEvent.Wait(time); }
inline	BOOL	IsPlaying()						{ return m_bPlaying; }
inline	DWORD	SoundBufferSize(DWORD sec)		{ return m_oFormat.nAvgBytesPerSec; }
virtual	BOOL	Play		(BOOL bLoop);
virtual	BOOL	Stop();
		BOOL	Create		(DWORD Size);
virtual	void	Release();
		void	SetFormat	(DWORD SamplingLate,WORD BitLate,WORD Channels);
		BOOL	SetPlayPos	(DWORD Offset = 0);
		BOOL	SetNotifyPos(DWORD Offset = DSBPN_OFFSETSTOP);
virtual	BOOL	SetVolume	(long Vol)			{ return SUCCEEDED( m_pSoundBuffer->SetVolume(Vol) ); }
virtual	BOOL	GetVolume	(long* Vol)			{ return SUCCEEDED( m_pSoundBuffer->GetVolume(Vol) ); }
virtual	BOOL	SetPan		(long Pan)			{ return SUCCEEDED( m_pSoundBuffer->SetPan(Pan) ); }
		void	Lock(DWORD Pos,DWORD Size)		{ m_pSoundBuffer->Lock(Pos,Size,&m_DecodeBuffer1,&m_BufferSize1,&m_DecodeBuffer2,&m_BufferSize2,0);}
		void	Lock(DWORD Pos)					{ m_pSoundBuffer->Lock(Pos,m_BufferSize,&m_DecodeBuffer1,&m_BufferSize1,&m_DecodeBuffer2,&m_BufferSize2,m_BufferSize);}
		void	Unlock()						{ m_pSoundBuffer->Unlock(m_DecodeBuffer1,m_BufferSize1,m_DecodeBuffer2,m_BufferSize2);}
};

#endif