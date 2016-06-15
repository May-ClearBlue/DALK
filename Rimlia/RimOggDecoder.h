#ifndef __VORBISDRIVER__
#define __VORBISDRIVER__

#define _DALK_USE_VORBISFILE_
#ifdef _DALK_IOS_
 #define _DALK_USE_TREMOR_
#endif

#include <MultiStdAfx.h>
#include "RimSoundDecoder.h"

#ifdef _WIN32 /* We need the following two to set stdin/stdout to binary */
#include <io.h>
#include <fcntl.h>
#endif

#ifdef _DALK_USE_VORBISFILE_
 #ifdef _DALK_USE_TREMOR_
  #include <Tremor/ivorbisfile.h>
 #else
  #include <vorbis/codec.h>
  #include <vorbis/vorbisfile.h>
 #endif
#endif

#include <stdio.h>

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
#define BUFFERSIZE	128 * 1024	// 経験則で64Kあれば良さそうなので余裕を持って128k確保

#ifndef _DALK_USE_VORBISFILE_
//WAVEDECODER:PCMデコーダーのベース。ReadHeaderで初期化、Decodeでデコード。
//この二つをオーバーライドすること。
class CVorbisDecoder : public ISoundDecoder {
public:
    CVorbisDecoder(){}
	~CVorbisDecoder();

protected:
	vorbis_dsp_state	vd; // central working state for the packet->PCM decoder
	vorbis_block		vb; // local working space for packet->PCM decode
	ogg_sync_state		oy; // sync and verify incoming physical bitstream
	ogg_page			og; // one Ogg bitstream page.  Vorbis packets are inside
	ogg_stream_state	os; // take physical pages, weld into a logical stream of packets
	ogg_packet			op; // one raw packet of data for decode
	vorbis_info			vi; // struct that stores all the static vorbis bitstream settings
	vorbis_comment		vc; // struct that stores all the bitstream user comments

	ogg_int16_t ConvBuffer[4096]; // take 8k out of the data segment, not the stack
public:
//virtual	void	SetSourceFormat(WAVEFORMATEX& rPCMFormat){ m_sSourceFormat = rPCMFormat; }
	int		SetDataStream( IDataStream* pStream );
	BOOL	Decode( void* pDestBuffer, DWORD& DestSize, DWORD SrcSize );
};
#else
class CVorbisDecoder : public ISoundDecoder {
public:
	CVorbisDecoder()	{ m_bInit = false; }
	~CVorbisDecoder()	{ Close(); }
private:
	IDataStream*	m_pDataStream;
	bool			m_bInit;
	OggVorbis_File	m_vf;
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
