#include "Multistdafx.h"

#include "RinseAudioDevice.h"

IXAudio2*					CXAudio2::m_pXAudio;
IXAudio2MasteringVoice*		CXAudio2::m_pXAudioMasteringVoice;

BOOL CXAudio2::Init() {
	UINT32 flags = 0;
#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	HRESULT res;

	res = ::XAudio2Create(&m_pXAudio,flags);
	RIN_ERRORRETURNBOOL(res,_TEXT("Failed@XAudio2Create"));

	res = m_pXAudio->CreateMasteringVoice(&m_pXAudioMasteringVoice);
	RIN_ERRORRETURNBOOL(res,_TEXT("Failed@CreateMasteringVoice"));

	_DEBUGPRINTF(_TEXT("XAudio2を初期化"));

	return TRUE;
}

void CXAudio2::Release() {
	if(m_pXAudioMasteringVoice != NULL) {
        m_pXAudioMasteringVoice->DestroyVoice();
        m_pXAudioMasteringVoice = NULL;
    }
	SafeRelease(m_pXAudio);
	_DEBUGPRINTF(_TEXT("XAudio2を解放"));
}
