#include "Multistdafx.h"
#include "RinseAudioSource.h"

BOOL CXAudioSource::Release() {
	if(m_SourceInstance != NULL) {
		Stop();

		_RINSERESULT r;
#if defined(_DALK_USE_XAUDIO_)
#if 0
		XAUDIO2_VOICE_STATE state = { 0 };

		do {
			_SLEEP(100);
			m_SourceInstance->GetState( &state );
		}while(state.BuffersQueued == 0);
#endif
		m_SourceInstance->DestroyVoice();
		m_SourceInstance = NULL;
#elif defined(_DALK_USE_OPENAL_)
		alDeleteSources(1, &m_SourceInstance);
		RIN_ERRORRETURNBOOL(r, "Failed@CXAudioSource::Release()");
		m_SourceInstance = 0;
#endif
	}

	delete m_pStream;
	delete m_pDecoder;
	m_pStream = NULL;
	m_pDecoder = NULL;

	m_Buffers[0].Release();
	m_Buffers[1].Release();

	m_State = AUDIOSTATE_INITIAL;

	return TRUE;
}

int CXAudioSource::Load( LPCTSTR name, void* pData ) {
	if(pData == NULL) {
		int r;
		m_pStream = new CFileStream;
		r = m_pStream->Open(name);		
		DALK_ERRORRETURNINT(r,"Failed@CXAudioSource::Load");
	}
	else
		m_pStream = (IDataStream*)pData;

	CPathString path(name);	
	string_t ex;
	ex = path.GetExtentStringBuf();

	if(ex == _T("wav") || ex == _T("WAV") || ex == _T(".wav") || ex == _T(".WAV") )
		m_pDecoder = new CPCMDecoder;
	else if(ex == _T("ogg") || ex == _T("OGG") || ex == _T(".ogg") || ex == _T(".OGG") )
		m_pDecoder = new CVorbisDecoder;

	return m_pDecoder->SetStream(m_pStream);
	SetVolume(1.0f);
}

#if defined(_DALK_USE_XAUDIO_)
AUDIO_STATE CXAudioSource::GetState() {
	if( m_SourceInstance == NULL )
		return AUDIOSTATE_INITIAL;

	if(m_State == AUDIOSTATE_PLAYING) {
		XAUDIO2_VOICE_STATE state = { 0 };
		m_SourceInstance ->GetState( &state );

		if(state.BuffersQueued > 0)
			return AUDIOSTATE_PLAYING;
		
		m_State = AUDIOSTATE_PAUSED;
	}
	//バッファがセットアップ済みかどうか
	return m_State;
}
#endif

BOOL CXAudioSource::Stop() {
    _DEBUGPRINTF("AXAudioSource::Stop");
#if defined(_DALK_USE_XAUDIO_)
	if (GetState() == AUDIOSTATE_PLAYING && SUCCEEDED(m_SourceInstance->Stop(0)) && SUCCEEDED(m_SourceInstance->FlushSourceBuffers())) {
#elif defined(_DALK_USE_OPENAL_)
	alSourceStop(m_SourceInstance);
	if(alGetError() == 0 ) {
        alSourceRewind(m_SourceInstance);
#endif
		m_pDecoder->SeekFrame(0);
		m_State = AUDIOSTATE_STOPPED;
		return TRUE;
	}
	return FALSE;
}

#if defined(_DALK_USE_XAUDIO_)
BOOL CXAudioSource::Pause() {
	if( GetState() == AUDIOSTATE_PLAYING && SUCCEEDED(m_SourceInstance->Stop(0)) ) {
		m_State = AUDIOSTATE_PAUSED;
		return TRUE;
	}
	return FALSE;
}

void CXAudioSource::OnBufferStart( void* pBufferContext ) {
	BYTE* pBuffer = (BYTE*)m_Buffers[m_BufferSwitch].GetBufferPointer();

	DWORD BufSize = m_pDecoder->CalcPCMBufferSize(1);

	size_t size = m_pDecoder->Decode(pBuffer, BufSize, BufSize);
#if 0
	int remain = BufSize - size;
	
	if(size < _PCM_STREAMING_SIZE_ && remain > 4096) {
		size_t temp = m_pDecoder->Decode(pBuffer+size, remain, remain);

		if(temp != 0)
			size += temp;
		else {
			int res = m_pDecoder->SeekFrame(m_BeginPos);
			
			temp = m_pDecoder->Decode(pBuffer+size, remain, remain);
			size += temp;
		}
	}
#endif
	m_Buffers[m_BufferSwitch].SetBuffer(pBuffer, size, 0, 0);
	m_Buffers[m_BufferSwitch].SubmitBuffer(m_SourceInstance);

	m_BufferSwitch ^= 1;
}
#endif

BOOL CXAudioSource::PlayStatic(BOOL IsLoop) {
	size_t FrameNum = m_pDecoder->GetTotalFrame() + m_BeginPos;
	size_t BufSize  = m_pDecoder->GetTotalSize() - m_EndPos;

	_RINSERESULT res;

	res = CreateSource();
	RIN_ERRORRETURNBOOL(res, _TEXT("Failed:CreateSource@PlayStatic"));
	
	void* pBuffer;
	_VALLOC(pBuffer, BufSize);

	m_pDecoder->SeekFrame(m_BeginPos);

	size_t size = m_pDecoder->Decode(pBuffer, BufSize, BufSize);

#if defined(_DALK_USE_OPENAL_)
	m_Buffers[0].CreateInstance();
	RIN_ERRORRETURNBOOL(res, _TEXT("Failed:SetFormat@PlayStream"));
	m_Buffers[0].SetFormat(&m_pDecoder->GetDestFormat());
#endif
	res = m_Buffers[0].SetBuffer( pBuffer, size, 0, 0, IsLoop);
    RIN_ERRORCHECK(res,"Failed@IAudioBuffer::SetBuffer");
	res = m_Buffers[0].SubmitBuffer( m_SourceInstance, FALSE );
    RIN_ERRORCHECK(res,"Failed@IAudioBuffer::SubmitBuffer");
    
    res = SetVolume(1.0f);
	RIN_ERRORRETURNBOOL(res, _TEXT("Failed:SetVolume@PlayStatic"));
	res = Start();
	RIN_ERRORRETURNBOOL(res, _TEXT("Failed:Start@PlayStatic"));

//ストリームしないときはSwitchを無効に
	m_BufferSwitch = -1;
	m_State = AUDIOSTATE_PLAYING;

	return TRUE;
}

BOOL CXAudioSource::PlayStream(BOOL IsLoop) {
	_RINSERESULT res;

#if defined(_DALK_USE_XAUDIO_)
	res = CreateSource(this);
#elif defined(_DALK_USE_OPENAL_)
	res = CreateSource();
#endif
	RIN_ERRORRETURN(res, _TEXT("Failed:CreateSource@PlayStatic"));

	DWORD BufSize = m_pDecoder->CalcPCMBufferSize(1);

	m_pDecoder->SeekFrame(m_BeginPos);

	for(int a  = 0;a < 2;a ++) {
		void* pBuffer;
		_VALLOC(pBuffer, BufSize);

		size_t size = m_pDecoder->Decode(pBuffer, BufSize, BufSize);

#if defined(_DALK_IOS_)
		m_Buffers[0].CreateInstance();
		RIN_ERRORRETURNBOOL(res, _TEXT("Failed:SetFormat@PlayStream"));
		m_Buffers[a].SetFormat(&m_pDecoder->GetDestFormat());
#endif
		m_Buffers[a].SetBuffer( pBuffer, size, 0, 0, FALSE );
		RIN_ERRORRETURNBOOL(res, _TEXT("Failed:CreateBuffer@PlayStream"));

		m_Buffers[a].SubmitBuffer( m_SourceInstance, FALSE );
		RIN_ERRORRETURNBOOL(res, _TEXT("Failed:SubmitBuffer@PlayStream"));
	}

	res = Start();
	RIN_ERRORRETURN(res, _TEXT("Failed:CreateSource@PlayStream"));

	m_BufferSwitch = 1;
	m_State = AUDIOSTATE_PLAYING;

	return TRUE;
}

#ifdef _DALK_IOS_
_RINSERESULT CXAudioSource::Load(NSString* name, void* pData) {
	m_pStream = new CExtAudioStream;
	m_pDecoder= new CExtAudioDecoder;

	((CExtAudioStream*)m_pStream)->Open(name);
	return m_pDecoder->SetStream(m_pStream);
	SetVolume(1.0f);
}
#endif