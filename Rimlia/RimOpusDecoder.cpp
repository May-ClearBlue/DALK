#include "Multistdafx.h"

#include "RimOpusDecoder.h"

#ifndef _DALK_USE_OPUSFILE_

CVorbisDecoder::~CVorbisDecoder() {
 /* clean up and exit.  vorbis_info_clear() must be called last */
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
}

int CVorbisDecoder::SetDataStream(IDataStream* pStream){
	// Oggビットストリームを読むための ogg_sync_state 構造体の初期化
	ogg_sync_init( &oy );

	// libvorbisのOggレイヤーに4Kバッファを与える
	char *pBuffer = ogg_sync_buffer( &oy, 4096 );
	int Bytes = pStream->Read(4096,pBuffer,false);
	if( Bytes < 4096 )
		return 1;//if( ferror( fp ) != 0 ) return false; // ストリームにエラーがあるとfalseを返す
	
	if( ogg_sync_wrote( &oy, Bytes ) != 0 ) return 2; // バッファに書き込んだバイト数を伝える

	// 最初のページを取得
	if( ogg_sync_pageout( &oy, &og ) != 1 ) return 3;

	// ogg_stream_state 構造体の初期化とメモリの確保
	// シリアルナンバーを設定
	if( ogg_stream_init( &os, ogg_page_serialno( &og ) ) != 0 ) return 4;

	// 最初のページからヘッダを読み出してこのOggビットストリームが
	// 本当にVorvisデータなのか調べる

	// vorbis_info 構造体の初期化(ドキュメントに載ってない)
	vorbis_info_init( &vi );

	// vorbis_comment 構造体の初期化(ドキュメントに載ってない)
	vorbis_comment_init( &vc );

	// ビットストリームに完全なページを追加する
	if( ogg_stream_pagein( &os, &og ) < 0 ) return 5;

	// デコード用のrawデータパケットを作る
	if( ogg_stream_packetout( &os, &op ) != 1 ) return 6;

	// ??(ドキュメントに載ってない)
	if( vorbis_synthesis_headerin( &vi, &vc, &op ) < 0 ) return 7;

	// この時点で、このファイルがVorvisであることが確かめられたので
	// Oggビットストリームデコーダーを準備する。
	// コメントとコードブックヘッダを取得し、Vorvisデコーダを準備する。
	int i = 0;
	while( i < 2 ){
		while( i < 2 ){
			// ページの取得
			int Result = ogg_sync_pageout( &oy, &og );
			if( Result == 0 ) break; // さらにデータが必要
			// データの正当性についてはパケット出力段階でチェックする
			if( Result == 1 ){ 
				// ビットストリームに完全なページを追加する
				ogg_stream_pagein( &os, &og ); // そんなわけでエラーは無視する
				while( i < 2 ){
					// デコード用のrawデータパケットを作る
					Result = ogg_stream_packetout( &os, &op );
					if( Result == 0 ) break;
					if( Result < 0 ) return 8; // データが壊れてます

					// ??(ドキュメントに載ってない)
					vorbis_synthesis_headerin( &vi, &vc, &op );

					i++;
				}
			}
		}

		// バッファに読み込み
		pBuffer = ogg_sync_buffer( &oy, 4096 );
		Bytes = pStream->Read(4096,pBuffer,false);
		if( Bytes < 4096 )
			return 9;//if( ferror( fp ) != 0 ) return false; // ストリームにエラーがあるとfalseを返す
		

		if( Bytes == 0 && i < 2 ) return 10; // ヘッダに十分なファイル長がないです
		if( ogg_sync_wrote( &oy, Bytes ) != 0 ) return 11; // バッファに書き込んだバイト数を伝える
	}

	// コメント
	{
//		char **ptr = vc.user_comments;
//		while( *ptr ) ++ptr;
	}

	// 3つのヘッダの取得と解析が終わったのでVorbis packet->PCMデコーダを初期化する

	// ??(ドキュメントに載ってない)
	vorbis_synthesis_init( &vd, &vi );
	vorbis_block_init( &vd, &vb );

	m_pDataStream = pStream;
	return 0;
}

// 返値: -1: エラー 0: 再生終了 1: エラー無し
BOOL CVorbisDecoder::Decode(void* pDestBuffer,DWORD& DestSize,DWORD SrcSize){
	int eos = 1;
	int Counter = 0;

	DestSize = 0;

	// 以下ひたすらデコード
	while( eos ){
		int Result = ogg_sync_pageout( &oy, &og );
		if( Result == 0 ) break; // さらにデータが必要
		if( Result < 0 ) return -1; // データが壊れてます
		else{
			// ビットストリームに完全なページを追加する
			ogg_stream_pagein( &os, &og ); // ここでは安全にエラーを無視できる
			while(1){
				// デコード用のrawデータパケットを作る
				Result = ogg_stream_packetout( &os, &op );
				if( Result == 0 ) break; // さらにデータが必要
				if( Result < 0 ) return -1; // データが壊れてます
				else{
					// パケットを取得したのでデコードする
					float **ppPCM;
					int Samples;
					
					// ??(ドキュメントに載ってない)
					if( vorbis_synthesis( &vb, &op ) == 0 ) // test for success!
					vorbis_synthesis_blockin( &vd, &vb );

					// **pcm はマルチチャンネルのfloat列である。
					// ステレオの時は pcm[0] が左、pcm[1] が 右になる。
					// samples は各チャンネルのサイズである。
					// +-1のfloatデータに変換して書き出す
					while( ( Samples = vorbis_synthesis_pcmout( &vd, &ppPCM ) ) > 0 ){
						int ConvSize = 4096 / vi.channels;
						Samples = ( Samples < ConvSize ? Samples : ConvSize ); 

						// floatデータを16 bit signed ints (host order)に変換してクリッピングする
						//float VolumeTmp = Volume;
						for( int i = 0; i < vi.channels; i++ ){
							//Volume = VolumeTmp;
							ogg_int16_t *pBuffer = ConvBuffer + i;
							float *pMono = ppPCM[i];
							for( int j = 0; j < Samples; j++ ){
								int Val= (int)( pMono[j] * 32767.f );
								// クリッピングに注意しましょう
								if( Val > 32767 )  Val = 32767;
								if( Val < -32768 ) Val = -32768;
								//Val = (int)( (float)Val * Volume );
								*pBuffer = Val;
								pBuffer += vi.channels;
							}
						}

						// waveOut用バッファに書き込み
						if( Counter + Samples * 2 * vi.channels < BUFFERSIZE ){
							//memcpy( WaveHdr[DecodePage].lpData + Counter, ConvBuffer, Samples * 2 * vi.channels );
							Counter += Samples * 2 * vi.channels;
						}

						// ??(ドキュメントに載ってない)
						vorbis_synthesis_read( &vd, Samples ); // どれだけのサンプル数を使ったかlibvorbisに伝える
					}
				}
			}

			// このページがストリームの終わりか調べる
			if( ogg_page_eos( &og ) ) eos = 0;
		}
	}

	if( eos == 0 ){
		/*
		// 再生終了を待つ
		DWORD MMTimeOld = 0;
		for( int i = 0; i < 3000; i++ ){ // 最大30秒待つ
			MMTIME MMTime;
			MMTime.wType = TIME_BYTES;
			//if( waveOutGetPosition( hWaveOut, &MMTime, sizeof( MMTIME ) ) != MMSYSERR_NOERROR ) break;
			if( MMTimeOld == MMTime.u.sample ) break;
			MMTimeOld = MMTime.u.sample;
			Sleep(10);
		}
		*/
		//fseek( fp, 0, SEEK_SET ); // ファイル先頭に巻き戻し（一応動くが要検証!!）
	}

	// バッファに読み込み
	char *pBuffer = ogg_sync_buffer( &oy, 4096 );
	DWORD Bytes = m_pDataStream->Read(4096,pBuffer,false);
	if( Bytes < 4096 )
		return FALSE;//if( ferror( fp ) != 0 ) return -1; // ストリームにエラーがあると-1返す
	if( ogg_sync_wrote( &oy, Bytes ) != 0 )
		return FALSE;//return -1; // バッファに書き込んだバイト数を伝える

// printf("Decodeing...%d\n",Counter);
/*
	// waveOutにデータを書き出す
	if( Counter != 0 ){
		if( waveOutUnprepareHeader( hWaveOut, &WaveHdr[DecodePage], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ){
			return -1;
		}

		WaveHdr[DecodePage].dwBytesRecorded	= 0;
		WaveHdr[DecodePage].dwFlags			= 0;
		WaveHdr[DecodePage].dwLoops			= 0;
		WaveHdr[DecodePage].dwUser			= 0;
		WaveHdr[DecodePage].lpNext			= 0;
		WaveHdr[DecodePage].reserved		= 0;

		WaveHdr[DecodePage].dwBufferLength	= Counter;

		if( waveOutPrepareHeader( hWaveOut, &WaveHdr[DecodePage], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ) return -1;
		if( waveOutWrite( hWaveOut, &WaveHdr[DecodePage], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ) return -1;

		// ページを切り替える
		++DecodePage;
		if( DecodePage >= MAXBUFFER ) DecodePage = 0;
	}
	else return -1;
*/
	return eos;
}
#else

int COpusDecoder::SetDataStream(IDataStream* pStream){
	//Oggを開く
	//srcfile = file;
	//databeginoffset = file->GetOffset();
	//m_totalfilesize = file->GetFileSize();

	int errorNum = 0;

	m_pDecoderInstance =  opus_decoder_create(m_DestFormat.nSamplesPerSec, m_DestFormat.nChannels, &errorNum);

	if(errorNum < 0)
	{
		fprintf(stderr, "failed to create decoder: %s\n", opus_strerror(errorNum));
		return EXIT_FAILURE;
	}

	while (1)
	{
		int i;
		unsigned char pcm_bytes[MAX_FRAME_SIZE * m_DestFormat.nChannels * 2];

		int frame_size;

		/* Read a 16 bits/sample audio frame. */
		fread(pcm_bytes, sizeof(short) * m_DestFormat.nChannels, FRAME_SIZE, m_InputBuffer);

		if (feof(fin))
			break;

		int nbBytes;

		/* Decode the data. In this example, frame_size will be constant because
		the encoder is using a constant frame size. However, that may not
		be the case for all encoders, so the decoder must always check
		the frame size returned. */

		frame_size = opus_decode(m_pDecoderInstance, m_packetBuffer, nbBytes, m_OutputBuffer, MAX_FRAME_SIZE, 0);
		if (frame_size<0)
		{
			fprintf(stderr, "decoder failed: %s\n", opus_strerror(frame_size));
			return EXIT_FAILURE;
		}

		/* Convert to little-endian ordering. */
		for (i = 0;i < m_DestFormat.nChannels * frame_size; i++)
		{
			pcm_bytes[2 * i] = m_OutputBuffer[i] & 0xFF;
			pcm_bytes[2 * i + 1] = (m_OutputBuffer[i] >> 8) & 0xFF;
		}
		/* Write the decoded audio to file. */
		fwrite(pcm_bytes, sizeof(short), frame_size * m_DestFormat.nChannels, m_OutputBuffer);
	}

	m_pDataStream = pStream;
	//loading = TRUE;

	SetPCMFormat(m_DestFormat,info->rate,16,info->channels);
	SetPCMFormat(m_SourceFormat,info->rate,16,info->channels);
//	m_DataSize = (DWORD)ov_pcm_total(&vf, -1) * info->channels * 2;
	m_TotalFrame = ov_pcm_total(&vf, -1);
	return true;
}

size_t CVorbisDecoder::Decode(void* pDestBuffer,DWORD DestSize,DWORD SrcSize) {
	//DestSizeにはあらかじめ総データ量が入っていること
	DWORD DecodedSize = 0;
	char* pBufferPos = (char*)pDestBuffer;

	while( DestSize > (DecodedSize + 4096) ) {
#ifdef _DALK_USE_TREMOR_
        int Size = ov_read(&m_vf, pBufferPos + DecodedSize, 4096/*SrcSize*/, NULL);
#else
        int Size = ov_read(&m_vf, pBufferPos + DecodedSize, 4096/*SrcSize*/, 0, 2, 1, NULL);
#endif
		switch( Size ) {
		case OV_HOLE:
		case OV_EBADLINK:
			return 0;
		case 0:
			ov_pcm_seek(&m_vf, 0);
			break;
		default:
			DecodedSize += Size;
			break;
		}
	}
	//DestSize = Size > 0 ? Size : 0;
    
	return DecodedSize;
}

int CVorbisDecoder::Close(){
	if ( m_bInit == false )
		return 0;
	int res = ov_clear(&m_vf);

	if(res == 0)
		m_pDataStream = NULL;

	m_bInit = true;

	return res;
}

/**
 * Vorbisfileからのコールバックを受けてファイルから指定されたサイズを読みとります。
 */
size_t CVorbisDecoder::CallbackRead(void *pBuffer, size_t Size, size_t nmemb, void *pStream){
	IDataStream* lpStream = (IDataStream*)pStream;
	return lpStream->Read( (DWORD)(Size * nmemb),pBuffer,false);
}
/**
 * Vorbisfileからのコールバックを受けてファイルポインタを移動します。
 */
int CVorbisDecoder::CallbackSeek(void *pStream, ogg_int64_t Offset, int Whence){
	IDataStream* lpStream = (IDataStream*)pStream;
	
	DWORD result;

	switch(Whence){
	case SEEK_CUR:
		result = lpStream->Seek( (DWORD)Offset ,FILE_CURRENT );
		break;
	case SEEK_END:
		result = lpStream->Seek( (DWORD)Offset ,FILE_END );
		break;
	case SEEK_SET:
		result = lpStream->Seek( (DWORD)Offset, FILE_BEGIN );
		break;
	default:
		return -1;
	}
	return (result == 0xffffffff) ? -1 : 0;
}

int CVorbisDecoder::CallbackClose(void *pStream){
	return 0;
	//COggWin32File *file = (COggWin32File *)datasource;
	//return  ( file->GetBINFILE()->IsArchive() ) ? file->GetBINFILE()->Close() : EOF;
}

//データの相対位置
long CVorbisDecoder::CallbackTell(void *pStream){
	IDataStream* lpStream = (IDataStream*)pStream;
	
	return lpStream->GetOffset();
}

ov_callbacks CVorbisDecoder::oggCallbacks = {
	CVorbisDecoder::CallbackRead,
	CVorbisDecoder::CallbackSeek,
	CVorbisDecoder::CallbackClose,
	CVorbisDecoder::CallbackTell
};

/*
bool COggFileReader::Seek(unsigned long sample)
{
	if(!IsOpen()){
		return false;//開いていない
	}

	if(ov_pcm_seek(&m_vf, sample)){
		return false;
	}

	m_terminated = false;

	return true;
}
*/
#endif