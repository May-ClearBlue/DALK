#ifndef _RIN_XAUDIO2_
#define _RIN_XAUDIO2_

#include "RinseAudioDevice.h"
#include <Rimlia/RimFileStream.h>

#if defined(_DALK_WINDOWS_)

#elif defined(_DALK_IOS_)
 #include <Rimlia/RimExtAudioDecoder.h>
 #import <AVFoundation/AVFoundation.h>
#endif


#ifdef _DALK_USE_OGG_
#include <Rimlia/RimOggDecoder.h>
#pragma message("-------------SoundDecoder--------------")
#pragma message("Enable:Ogg")
#pragma message("---------------------------------------")
#endif

#define _PCM_STREAMING_SIZE_ (44100 * 2)		//2sec
#define _AUDIO_QUEBUFFER_NUM_ 2

//バッファ管理クラス（ソースへの登録まで）
class IAudioBuffer {
public:
	IAudioBuffer() 
#if defined(_DALK_USE_OPENAL_)
	: m_BufferID(0)
#endif	
	{ 
		_ZEROINIT(m_BufferInfo);
	}
virtual	~IAudioBuffer() { Release(); }

protected:
    XAUDIO2_BUFFER		m_BufferInfo;
#if defined(_DALK_USE_OPENAL_)
	ALuint				m_BufferID;
#endif

public:
#if defined(_DALK_USE_OPENAL_)
inline	void		CreateInstance()				{ alGenBuffers(1, &m_BufferID); }
inline	void		SetFormat(_PCMFORMAT* pFormat)	{ m_BufferInfo.pContext = pFormat; } 
#endif
inline	const BYTE*		GetBufferPointer() { return m_BufferInfo.pAudioData; }
		void			Release();
		_RINSERESULT	SetBuffer		( void* pData, DWORD bytes, DWORD begin, DWORD end, BOOL isLoop = FALSE);
		_RINSERESULT	SubmitBuffer	( _AUDIOSOURCE pHandle, BOOL isStream = FALSE );
};

//ソースクラス
class CXAudioSource 
#if defined(_DALK_USE_XAUDIO_)
	: public IXAudio2VoiceCallback
#endif
{
public:
	CXAudioSource() :
		m_SourceInstance(NULL),
		m_State(AUDIOSTATE_INITIAL),
		m_Volume(1.0f),
		m_IsLoop(FALSE),
		m_BeginPos(0),
		m_EndPos(0),
		m_pStream(NULL),
		m_pDecoder(NULL),
		m_BufferSwitch(0)
    { }
protected:
	_AUDIOSOURCE	m_SourceInstance;
	AUDIO_STATE		m_State;
	float			m_Volume;
	BOOL			m_IsLoop;
	DWORD			m_BeginPos;
	DWORD			m_EndPos;

	IDataStream*	m_pStream;
	ISoundDecoder*	m_pDecoder;
	IAudioBuffer	m_Buffers[2];
	uint32_t		m_BufferSwitch;
public:
//共通なのでインライン化してある関数
		BOOL	Release();
		int		Load		( LPCTSTR name, void* pData = NULL );
		BOOL	PlayStatic	( BOOL IsLoop = FALSE );
		BOOL	PlayStream	( BOOL IsLoop = FALSE );

#if defined(_DALK_IOS_)
		int		Load		(NSString* name, void* pData = NULL);
#endif

inline	ISoundDecoder*	GetDecoder()									{ return m_pDecoder; }
inline	void			SetLoopPoint( DWORD begin, DWORD end )			{ m_BeginPos = begin; m_EndPos = end; }
inline	void			SetLoopMode	( BOOL isLoop )						{ m_IsLoop = isLoop; }

#if defined(_DALK_USE_XAUDIO_)
		AUDIO_STATE		GetState();
inline	_RINSERESULT	CreateSource( void* pContext = NULL )			{ return CXAudio2::GetDevice()->CreateSourceVoice( &m_SourceInstance, &m_pDecoder->GetDestFormat(), 0, 1.0f, (IXAudio2VoiceCallback*)pContext ); }
inline		float		GetVolume()					{ m_SourceInstance->GetVolume(&m_Volume); return m_Volume; }
inline	_RINSERESULT	SetVolume	( float vol )	{ return m_SourceInstance->SetVolume(vol); }
inline	_RINSERESULT	Start()						{ return m_SourceInstance->Start(); }
		BOOL			Stop();
		BOOL			Pause();
    void WINAPI	OnStreamEnd()											{ ; }
    void WINAPI	OnVoiceProcessingPassEnd()								{ ; }
    void WINAPI	OnVoiceProcessingPassStart	( UINT32 SamplesRequired )	{ ; }
	void WINAPI	OnBufferEnd					( void* pBufferContext )	{ ; }
    void WINAPI	OnBufferStart				( void* pBufferContext );
    void WINAPI	OnLoopEnd					( void* pBufferContext )	{ ; }
    void WINAPI	OnVoiceError				( void* pBufferContext, HRESULT Error) { ; }

#elif defined(_DALK_USE_OPENAL_)
inline	_RINSERESULT	GetPlayingTime(float& time)	{ alGetSourcef(m_SourceInstance, AL_SEC_OFFSET, &time); return 0;}
inline	AUDIO_STATE		GetState()					{ alGetSourcei(m_SourceInstance, AL_SOURCE_STATE, (ALint*)&m_State); return m_State; }
inline	_RINSERESULT	CreateSource()				{ alGenSources(1, &m_SourceInstance); return 0; }
inline	_RINSERESULT	SetVolume	( float vol )	{ alSourcef(m_SourceInstance, AL_GAIN, vol); m_Volume = vol; return 0;}
inline		float		GetVolume()					{ return m_Volume; }
inline	_RINSERESULT	Start()						{ alSourcePlay(m_SourceInstance); return 0; }
inline	BOOL			Stop();
inline	BOOL			Pause()						{ alSourcePause(m_SourceInstance); return TRUE;}
inline	_RINSERESULT	Rewind()					{ alSourceRewind(m_SourceInstance); return 0; }
inline	BOOL	IsActive()							{ return alIsSource(m_SourceInstance) == AL_TRUE ? TRUE : FALSE; }
#endif


//	BOOL SetStaticBuffer(void* pBuffer, DWORD Size, BOOL isStream);
};


#if defined(_DALK_IOS_)
//AVAudioPlayerはフォーマットの情報をチャンネルくらいしか持ってないみたいなので、周波数は今のところ決め打ち。
#define _BGM_SAMPLING_RATE_ 44100
#define _SE_SAMPLING_RATE_ 44100
#define _VOICE_SAMPLING_RATE_ 44100

class CAVAudioPlayer {
public:
	CAVAudioPlayer() :
		m_SamplingRate(_BGM_SAMPLING_RATE_),
		m_State(AUDIOSTATE_INITIAL),
		m_Volume(1.0f),
		m_BeginPos(0),
		m_EndPos(0)
	  { ; }
protected:
const	DWORD		m_SamplingRate;
	AUDIO_STATE		m_State;
	float			m_Volume;
	DWORD			m_BeginPos;
	DWORD			m_EndPos;
	AVAudioPlayer*	m_Player;
public:
inline	void			Load	( NSString* name, void* pData = NULL ){
        @autoreleasepool {

		NSURL *soundFile = [NSURL fileURLWithPath: [[NSBundle mainBundle]pathForResource:name ofType:nil]];
		m_Player = [[AVAudioPlayer alloc] initWithContentsOfURL:soundFile error:nil];
		//m_Player.delegate = self;
		m_Player.numberOfLoops = -1;
		[m_Player prepareToPlay];
        }
	}
inline  void        PlayStream(){
		[m_Player play];
        m_State = AUDIOSTATE_PLAYING;
    }

inline	void        Release()				{ Stop(); m_State = AUDIOSTATE_INITIAL; }
inline	void		SetLoopMode(BOOL isLoop){ m_Player.numberOfLoops = isLoop ? -1 : 0; }
inline	AUDIO_STATE	GetState()				{ return m_Player.playing ? AUDIOSTATE_PLAYING : m_State; }
inline	void        SetVolume( float vol )	{ m_Player.volume = vol; }
inline	float       GetVolume()				{ float v = m_Player.volume; return v;}
inline	void        Stop()					{ [m_Player stop]; m_State = AUDIOSTATE_STOPPED; }
inline	void        Pause()					{ [m_Player pause]; m_State = AUDIOSTATE_PAUSED; }
inline	float		GetTotalTime()			{ float v = m_Player.duration; return v; }
inline	float		GetCurrentTime()		{ float v = m_Player.currentTime; return v; }


};
#endif

#endif
