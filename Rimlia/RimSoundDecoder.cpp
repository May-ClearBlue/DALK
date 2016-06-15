#include <MultiStdAfx.h>

#include "RimSoundDecoder.h"

int	CPCMDecoder::SetDataStream (IDataStream* pStream) {
	DWORD ChunkSize = 0;
	char ChunkData[5] = "\0\0\0\0";

	int res;

	res = pStream->CompareString("RIFF");
	DALK_ERRORRETURNINT(res,_TEXT("WAVファイルではない(不正なRIFFチャンク)"));

	pStream->Seek(4,FILE_CURRENT);
	//datafile->MovePointer(4);

	res = pStream->CompareString("WAVE");
	DALK_ERRORRETURNINT(res,_TEXT("WAVファイルではない(不正なWAVEチャンク)"));

	res = pStream->CompareString("fmt ");
	DALK_ERRORRETURNINT(res,_TEXT("WAVファイルではない(不正なfmtチャンク)"));

	pStream->Read(ChunkSize);

	WAVEFORMATEX wfex;

	if( ChunkSize > sizeof(WAVEFORMATEX) )
		{ DALK_ERRORRETURN(RIM_ERROR_FORMAT, _TEXT("WAVヘッダのサイズが違う")); }
	
	pStream->Read(ChunkSize,&wfex,false);

	if(wfex.wFormatTag == WAVE_FORMAT_PCM)
		wfex.cbSize = 0;

	SetPCMFormat(m_SourceFormat, wfex.nSamplesPerSec, wfex.wBitsPerSample, wfex.nChannels);

//	if( ChunkSize > sizeof(m_SourceFormat) )
//		{ DALK_ERRORRETURN(RIM_ERROR_FORMAT, "WAVヘッダのサイズが違う"); }
//	pStream->Read(ChunkSize,&m_SourceFormat,false);

//	if(m_SourceFormat.wFormatTag == WAVE_FORMAT_PCM)
//		m_SourceFormat.cbSize = 0;
	
	pStream->Read(4,ChunkData,false);

	if( strcmp(ChunkData, "PAD ") == 0){
		pStream->Read(ChunkSize);
		pStream->Seek(ChunkSize,FILE_CURRENT);
		pStream->Read(ChunkSize);
	}

	if(strcmp(ChunkData,"fact") == 0){
		pStream->Read(ChunkSize);
		pStream->Seek(ChunkSize,FILE_CURRENT);
		pStream->Read(ChunkSize);
	}

	res = strcmp(ChunkData,"data");
	DALK_ERRORRETURNINT(res, _TEXT("WAVファイルではない(不正なdataチャンク)"));

	pStream->Read(m_DataSize);
	m_TotalFrame = m_DataSize / _PCM_CHANNELS(m_SourceFormat) / (_PCM_BITS(m_SourceFormat) / 8);

	m_DataBeginPos = pStream->GetOffset();
	m_pDataStream = pStream;

//生Wavは変換をしない（Dest=Souce）
	m_DestFormat = m_SourceFormat;

//	SetDestFormat(m_SourceFormat.nSamplesPerSec, m_SourceFormat.wBitsPerSample, m_SourceFormat.nChannels);

	return 0;
}
