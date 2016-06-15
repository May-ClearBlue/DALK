#ifndef _RIM_SOUNDDECODER_
#define _RIM_SOUNDDECODER_

#include <MultiStdAfx.h>
#include "RimDataStream.h"

#if defined (_DALK_WINDOWS_)

typedef  WAVEFORMATEX _PCMFORMAT;
#define _PCM_CHANNELS(x)		x.nChannels
#define _PCM_BITS(x)			x.wBitsPerSample
#define _PCM_SAMPLERATE(x)		x.nSamplesPerSec

inline void SetPCMFormat(WAVEFORMATEX& wfex,DWORD samplingrate = 44100,WORD bitlate = 16,WORD channels = 1, WORD format = WAVE_FORMAT_PCM) {
	_ZEROINIT(wfex);
	wfex.wFormatTag      = format;
	wfex.nChannels       = channels;
	wfex.nSamplesPerSec  = samplingrate;
	wfex.wBitsPerSample  = bitlate;
	wfex.nBlockAlign     = bitlate * channels / 8;
	wfex.nAvgBytesPerSec = samplingrate * wfex.nBlockAlign;
}

#elif defined(_DALK_IOS_)

#include <AudioToolbox/ExtendedAudioFile.h>

#define WAVE_FORMAT_PCM 1

typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         /* format type */
    WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
    DWORD       nSamplesPerSec;     /* sample rate */
    DWORD       nAvgBytesPerSec;    /* for buffer estimation */
    WORD        nBlockAlign;        /* block size of data */
    WORD        wBitsPerSample;     /* number of bits per sample of mono data */
    WORD        cbSize;             /* the count in bytes of the size of */
                                    /* extra information (after cbSize) */
} WAVEFORMATEX, *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;

typedef AudioStreamBasicDescription _PCMFORMAT;
#define _PCM_CHANNELS(x)		x.mChannelsPerFrame
#define _PCM_BITS(x)			x.mBitsPerChannel
#define _PCM_SAMPLERATE(x)		x.mSampleRate

inline void SetPCMFormat(AudioStreamBasicDescription& outputFormat,DWORD samplingrate = 44100,WORD bitlate = 16,WORD channels = 1, DWORD format = kAudioFormatLinearPCM) {
	_ZEROINIT(outputFormat);
    outputFormat.mSampleRate		= samplingrate;
    outputFormat.mChannelsPerFrame	= channels;
    outputFormat.mFormatID			= format;
    outputFormat.mFramesPerPacket	= 1;
    outputFormat.mBytesPerFrame		= bitlate  / 8 * outputFormat.mChannelsPerFrame;
    outputFormat.mBytesPerPacket	= outputFormat.mBytesPerFrame * outputFormat.mFramesPerPacket;
    outputFormat.mBitsPerChannel	= bitlate;
	outputFormat.mFormatFlags		= kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
}

#endif

class ISoundDecoder {
protected:
	_PCMFORMAT		m_SourceFormat;
	_PCMFORMAT		m_DestFormat;
	size_t			m_TotalFrame;
	DWORD			m_DataBeginOffset;
	DWORD			m_LoopBeginFrame;
	bool			m_bLoop;
public:
virtual	~ISoundDecoder() { ; };
inline	size_t		CalcPCMBufferSize(DWORD Sec)	{ return _PCM_SAMPLERATE(m_DestFormat) * _PCM_CHANNELS(m_DestFormat) * _PCM_BITS(m_DestFormat) / 8 * Sec; }
inline	size_t		GetTotalFrame()		{ return m_TotalFrame; }
inline	float		GetTotalTime()		{ return (float)m_TotalFrame / (float)_PCM_SAMPLERATE(m_DestFormat); }
inline	size_t		GetTotalSize()		{ return m_TotalFrame * _PCM_BITS(m_DestFormat) / 8; }
inline	_PCMFORMAT& GetSourceFormat()	{ return m_SourceFormat; }
inline	_PCMFORMAT& GetDestFormat()		{ return m_DestFormat; }
virtual	float		TellTime()			{ return (float)TellFrame() / (float)_PCM_SAMPLERATE(m_DestFormat); }
virtual int			SeekTime(float time){ return SeekFrame( time * _PCM_SAMPLERATE(m_DestFormat) ); }
virtual	size_t		TellFrame() = 0;
virtual int			SeekFrame(size_t frame) = 0;
virtual	int			SetStream		( void* pStream = NULL ) = 0;
virtual	size_t		Decode			( void* pDestBuffer, DWORD DestSize, DWORD SrcSize ) = 0;
virtual	int			Close() = 0;
};

class CPCMDecoder : public ISoundDecoder {
protected:
		IDataStream*	m_pDataStream;
		size_t			m_DataSize;
		size_t			m_DataBeginPos;
public:
		int		SetDataStream	( IDataStream* pStream );
		int		SetStream		( void* pStream = NULL )												{ return SetDataStream((IDataStream*)pStream); }
		size_t	Decode			( void* pDestBuffer, DWORD DestSize, DWORD SrcSize )					{ return m_pDataStream->Read(DestSize, pDestBuffer, m_bLoop); }
		size_t	TellFrame()					{ return m_pDataStream->GetOffset() - m_DataBeginPos; }
		int		SeekFrame(size_t frame)		{ return m_pDataStream->Seek(m_DataBeginPos + ( frame * _PCM_BITS(m_SourceFormat) * _PCM_CHANNELS(m_SourceFormat) / 8 ), FILE_BEGIN); }
		int		Close()						{ m_pDataStream = NULL; return 0; }
};

#endif