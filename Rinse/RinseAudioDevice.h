#include "RinseInitXAudio2.h"

#if defined(_DALK_USE_XAUDIO_)
//CoInitialize（Ex）をしておくこと（お約束）
class CXAudio2 {
protected:
static	IXAudio2*					m_pXAudio;
static	IXAudio2MasteringVoice*		m_pXAudioMasteringVoice;
public:
static	BOOL Init();
static	void Release();
static	IXAudio2*&					GetDevice()			{ return m_pXAudio; }
static	IXAudio2MasteringVoice*&	GetMasteringVoice() { return m_pXAudioMasteringVoice; }
};

#else

typedef ALvoid /*AL_APIENTRY*/(*alBufferDataStaticProcPtr)(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

class COpenAL {
protected:
static	ALCdevice*  m_pDevice;
static	ALCcontext* m_pContext;
static	alBufferDataStaticProcPtr	m_pProc_alBufferDataStatic;

public:
static	BOOL Init();
static	BOOL Release();
//
static	ALvoid alBufferDataStatic(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
};

#endif