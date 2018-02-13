#ifndef __OPUSDRIVER__
#define __OPUSDRIVER__

#define _DALK_USE_OPUSFILE_
#ifdef _DALK_IOS_
 #define _DALK_USE_TREMOR_
#endif

#include <MultiStdAfx.h>
#include "RimSoundDecoder.h"

#ifdef _WIN32 /* We need the following two to set stdin/stdout to binary */
#include <io.h>
#include <fcntl.h>
#endif

#include <stdio.h>

#include "opus.h"

// OggVorbis ライブラリ
#ifdef _DALK_DEBUG_BUILD_
//#pragma	comment( lib, "libvorbis_static.lib" )
//#pragma	comment( lib, "libogg_static.lib" )
#ifdef _USE_VORBISFILE_
#pragma comment(lib, "libvorbisfile_static.lib")
#endif
#else
//#pragma	comment( lib, "libvorbis_static.lib" )
//#pragma	comment( lib, "libogg_static.lib" )
#ifdef _USE_VORBISFILE_
#pragma comment(lib, "libvorbisfile_static.lib")
#endif
#endif

#define MAXBUFFER	4

#define FRAME_SIZE 960
#define MAX_FRAME_SIZE 6*960
#define MAX_PACKET_SIZE (3*1276)

#ifndef _DALK_USE_VORBISFILE_
//WAVEDECODER:PCMデコーダーのベース。ReadHeaderで初期化、Decodeでデコード。
//この二つをオーバーライドすること。
class COpusDecoder : public ISoundDecoder {
public:
    COpusDecoder(){}
	~COpusDecoder();

protected:
	OpusDecoder * m_pDecoderInstance;
	IDataStream*	m_pDataStream;

	opus_int16 m_InputBuffer[FRAME_SIZE*2];
	opus_int16 m_OutputBuffer[MAX_FRAME_SIZE*2];
	unsigned char m_packetBuffer[MAX_PACKET_SIZE];

public:
//virtual	void	SetSourceFormat(WAVEFORMATEX& rPCMFormat){ m_sSourceFormat = rPCMFormat; }
	int		SetDataStream( IDataStream* pStream );
	BOOL	Decode( void* pDestBuffer, DWORD& DestSize, DWORD SrcSize );
};

#else
class COpusDecoder : public ISoundDecoder {
public:
	COpusDecoder()	{ m_bInit = false; }
	~COpusDecoder()	{ Close(); }
private:
	IDataStream*	m_pDataStream;
	bool			m_bInit;
	OpusDecoder*	m_DecoderInstance;
//	OggVorbis_File	m_vf;
	//DWORD			m_totalSamples;
	//DWORD			m_totalfilesize;
	//BOOL			loading;
public:
	int		SetDataStream	( IDataStream* pStream );
	int		SetStream		( void* pStream = NULL )												{ return SetDataStream((IDataStream*)pStream); }
	void	SetDestFormat	( DWORD SamplingRate = 44100, WORD BitRate = 16, WORD Channels = 1  )	{ SetPCMFormat(m_DestFormat, SamplingRate, BitRate, Channels); }
	float	TellTime()							{ return ov_time_tell(&m_vf); }
	size_t	TellFrame()							{ return ov_pcm_tell(&m_vf); }
	int		SeekFrame		( size_t frame )	{ return ov_pcm_seek( &m_vf, frame); }
	int		SeekTime		( float time )		{ return ov_time_seek( &m_vf, time); }

size_t	Decode		( void* pDestBuffer, DWORD DestSize, DWORD SrcSize );
int		Close();

public:
static ov_callbacks oggCallbacks;
static size_t	CallbackRead  ( void *pBuffer, size_t Size, size_t nmemb, void *pStream );
static int		CallbackSeek  ( void *pStream, ogg_int64_t Offset, int Whence );
static int		CallbackClose ( void *pStream );
static long		CallbackTell  ( void *pStream );
};
#endif

#endif // __VORBISDRIVER__
