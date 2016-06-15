#include "MultiStdafx.h"
#include "RimACMDecoder.h"

void CACMDecoder::SetDestFormat(DWORD SamplingRate, WORD BitRate, WORD Channels) {
	SetPCMFormat(m_DestFormat, SamplingRate, BitRate, Channels);

//acmドライバの検索
	MMRESULT res = ::acmFormatSuggest( NULL, &m_SourceFormat, &m_DestFormat, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG);
	//DALK_ERRORRETURNINT(res,"Failed@acmFormatSuggest");
	
	res = ::acmStreamOpen( &m_hACMStream, NULL, &m_SourceFormat, &m_DestFormat, NULL, NULL, 0, ACM_STREAMOPENF_NONREALTIME );
}

#if 0
int	CACMDecoder::SetDataStream( IDataStream* pStream ) {
	DWORD ChunkSize = 0;
	char ChunkData[5] = "\0\0\0\0";

	if( pStream->CompareString("RIFF") )
		return CLogReporter::Return("WAVファイルではない(不正なRIFFチャンク)",1);

	pStream->Seek(4,FILE_CURRENT);

	if( pStream->CompareString("WAVE") )
		return CLogReporter::Return("WAVファイルではない(不正なWAVEチャンク)",2);

	if( pStream->CompareString("fmt ") )
		return CLogReporter::Return("WAVファイルではない(不正なfmtチャンク)",3);

	pStream->Read(ChunkSize);

	(void*&)m_pACMFormat = HeapAlloc( ChunkSize );

	if( m_pACMFormat == NULL )
		{ return CLogReporter::Return("WAVヘッダ用のメモリ確保ができなかった", 4); }
		//return CLogReporter::Return("対応していないWavフォーマット",-1);

	pStream->Read( ChunkSize, m_pACMFormat, false );

	if( m_pACMFormat->wFormatTag == WAVE_FORMAT_PCM )
		m_pACMFormat->cbSize = 0;
	
	pStream->Read(4,ChunkData,false);

	if( strcmp(ChunkData, "PAD ") == 0) {
		pStream->Read(ChunkSize); 
		pStream->Seek(ChunkSize,FILE_CURRENT);
		pStream->Read(ChunkSize);
	}

	if(strcmp(ChunkData,"fact") == 0) {
		pStream->Read(ChunkSize);
		pStream->Seek(ChunkSize,FILE_CURRENT);
		pStream->Read(ChunkSize);
	}

	if( strcmp(ChunkData,"data") )
		return CLogReporter::Return("WAVファイルではない(不正なdataチャンク)",4);

	pStream->Read(m_DataSize);

	m_DataBeginPos = pStream->GetOffset();
	m_pDataStream = pStream;

	//acmドライバの検索
	if( ::acmFormatSuggest( NULL, &pPCMFormat, &m_DestFormat, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG) )
		CLogReporter::Report("acmFormatSuggestに失敗");
	
	m_sSourceFormat = pPCMFormat;

	if( ::acmStreamOpen( &m_hACMStream, NULL, &m_sSourceFormat, &m_DestFormat, NULL, NULL, 0, ACM_STREAMOPENF_NONREALTIME ) )
		return;


	return 0;
}
#endif

size_t CACMDecoder::Decode( void* pDestBuffer, DWORD DestSize, DWORD SrcSize ) {
	ACMSTREAMHEADER ACMHeader = { 0 };

	MMRESULT res;
	res = ::acmStreamSize( m_hACMStream, DestSize, &SrcSize, ACM_STREAMSIZEF_DESTINATION );
	DALK_ERRORRETURNINT(res,"Failed@acmStreamSize");
	//::acmStreamSize(hacmstream,srcsize,&destsize,ACM_STREAMSIZEF_SOURCE);  

	ACMHeader.cbStruct		= sizeof(ACMHeader);
	ACMHeader.pbSrc			= (BYTE*)m_SrcBuffer.GetPointer();
	ACMHeader.cbSrcLength	= m_pDataStream->Read( SrcSize, ACMHeader.pbSrc, false );
	ACMHeader.pbDst			= (BYTE*)pDestBuffer;
	ACMHeader.cbDstLength	= DestSize;

	res = ::acmStreamPrepareHeader(m_hACMStream,&ACMHeader, 0);
	DALK_ERRORRETURNINT(res,"Failed@acmStreamPrepareHeader");

	res = ::acmStreamConvert(m_hACMStream, &ACMHeader, ACM_STREAMCONVERTF_BLOCKALIGN);
	DALK_ERRORRETURNINT(res,"Failed@acmStreamConvert");

	res  = ::acmStreamUnprepareHeader( m_hACMStream, &ACMHeader, 0);
	DALK_ERRORRETURNINT(res,"Failed@acmStreamUnprepareHeader");

	return ACMHeader.cbDstLengthUsed;

#if 0
	dataremain -= ACMHeader.cbSrcLength;

	memcpy(readbuffer,(BYTE*)readbuffer + ACMHeader.cbSrcLengthUsed, ACMHeader.cbSrcLength - ACMHeader.cbSrcLengthUsed);

	quebufremain += ACMHeader.cbDstLengthUsed;

	return ACMHeader.cbDstLengthUsed;
#endif
}

int CACMDecoder::Close() {
	if( m_hACMStream != NULL ) {
		::acmStreamClose( m_hACMStream, 0 );
		m_hACMStream = NULL;
	}
	return 0;
}
