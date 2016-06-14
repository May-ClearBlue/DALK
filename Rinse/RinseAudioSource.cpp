#include "MultiStdAfx.h"

#include "RinseAudioSource.h"

void IAudioBuffer::Release() { 
#if defined(_DALK_USE_OPENAL_)
	if(m_BufferID != 0) {
		alDeleteBuffers(1, &m_BufferID);
        m_BufferID = 0;
    }
#endif

	_VFREE((BYTE*&)m_BufferInfo.pAudioData);
	_ZEROINIT(m_BufferInfo);
}

_RINSERESULT IAudioBuffer::SetBuffer( void* pData, DWORD bytes, DWORD begin, DWORD end, BOOL isLoop) {
//#if defined(_DALK_IOS_)
//	Release();
//#endif
	m_BufferInfo.AudioBytes = bytes;
	m_BufferInfo.pAudioData = (BYTE*)pData;

	if(isLoop) {
//			m_Buffer.Flags		= 0;
		m_BufferInfo.LoopCount	= XAUDIO2_LOOP_INFINITE;
		m_BufferInfo.LoopBegin	= begin;
		m_BufferInfo.LoopLength	= end - begin;		
	}
	else {
//			m_Buffer.Flags		= XAUDIO2_END_OF_STREAM;
		m_BufferInfo.LoopCount	= 0;
		m_BufferInfo.PlayBegin	= begin;
		m_BufferInfo.PlayLength	= end - begin;
	}

#if defined(_DALK_USE_OPENAL_)
	//OpenALはpContextにPCMFORMATを入れておく
    _RINSERESULT r;
	ALenum Format;

	_PCMFORMAT _Format = *((_PCMFORMAT*)m_BufferInfo.pContext);

	if( _PCM_CHANNELS(_Format) == 1 )
		Format = AL_FORMAT_MONO16;
	else
		Format = AL_FORMAT_STEREO16;

	COpenAL::alBufferDataStatic( m_BufferID, Format, (void*)(m_BufferInfo.pAudioData+begin), m_BufferInfo.AudioBytes-begin/*end*/,  _PCM_SAMPLERATE(_Format) );
    RIN_ERRORCHECK(r,"Failed@IAudioBuffer::SubmitBuffer");

	return 0;
#else
	return S_OK;
#endif
}

//ToDo:ストリームでLOOPさせないとき（いらない気もする）
_RINSERESULT IAudioBuffer::SubmitBuffer	( _AUDIOSOURCE pHandle, BOOL isStream ) {
	m_BufferInfo.Flags = m_BufferInfo.Flags ? XAUDIO2_END_OF_STREAM : 0;

#if defined(_DALK_USE_XAUDIO_)
	return pHandle->SubmitSourceBuffer(&m_BufferInfo);
#elif defined(_DALK_USE_OPENAL_)
	_RINSERESULT r;
	if(isStream)
		alSourceQueueBuffers(pHandle, 1, &m_BufferID);
	else {
		alSourcei(pHandle, AL_BUFFER, m_BufferID);
		RIN_ERRORCHECK(r,"Failed@IAudioBuffer::SubmitBuffer");
		alSourcei(pHandle, AL_LOOPING, m_BufferInfo.LoopCount == XAUDIO2_LOOP_INFINITE ? AL_TRUE : AL_FALSE);
		return 0;
	}
#endif
}