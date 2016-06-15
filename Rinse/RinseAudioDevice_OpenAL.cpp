#include <MultiStdAfx.h>
#include "RinseAudioDevice.h"

ALCdevice*  COpenAL::m_pDevice;
ALCcontext* COpenAL::m_pContext;
alBufferDataStaticProcPtr	COpenAL::m_pProc_alBufferDataStatic;

BOOL COpenAL::Init() {
	// OpneALデバイスを開く
	m_pDevice = alcOpenDevice(NULL);

    // OpenALコンテキストを作成して、カレントにする
	m_pContext = alcCreateContext(m_pDevice, NULL);

	BOOL res = alcMakeContextCurrent(m_pContext) == AL_TRUE ? TRUE : FALSE;

	_DEBUGPRINTF("OpenALを初期化");

	//OpenAL拡張
	m_pProc_alBufferDataStatic = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");

	return res;
}

BOOL COpenAL::Release() {
	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_pContext);
	_DEBUGPRINTF("OpenALを解放");
	return alcCloseDevice( m_pDevice ) == AL_TRUE ? TRUE : FALSE;
}

ALvoid COpenAL::alBufferDataStatic(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq) {
#ifdef _DALK_DEBUG_BUILD_
	if(m_pProc_alBufferDataStatic)
		m_pProc_alBufferDataStatic(bid, format, data, size, freq);
	else
		_DEBUGPRINTF("None:alBufferDataStatic");
#else
	m_pProc_alBufferDataStatic(bid, format, data, size, freq);
#endif
}
