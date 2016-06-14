#ifndef _RIN_INIT_
#define _RIN_INIT_

#include "MultiStdAfx.h"
#include <Rimlia/RimSoundDecoder.h>

#if defined(_DALK_USE_XAUDIO_)
 #include <xaudio2.h>

 #ifdef _DALK_DEBUG_BUILD_
  #pragma comment( lib, "xapobased.lib" )
 #else
  #pragma comment( lib, "xapobase.lib" )
 #endif

#else
 #ifdef _DALK_WINDOWS_
  #include <al.h>
  #include <alc.h>
 #else
  #include <OpenAL/OpenAL.h>
 #endif
// Flags
//#define XAUDIO2_END_OF_STREAM           0x0040        // Used in XAUDIO2_BUFFER.Flags
//#ifndef UINT32
//#define UINT32 DWORD
//#endif
#endif

//DirectX-OpenAL共通化型
#if defined(_DALK_USE_XAUDIO_)
    typedef	HRESULT					_RINSERESULT;
    typedef IXAudio2SourceVoice*	_AUDIOSOURCE;

	enum AUDIO_STATE {
		AUDIOSTATE_INITIAL,
		AUDIOSTATE_PLAYING,
		AUDIOSTATE_PAUSED,
		AUDIOSTATE_STOPPED
	};
#else
	typedef	ALint	_RINSERESULT;
	typedef ALuint	_AUDIOSOURCE;

	enum  AUDIO_STATE {
		AUDIOSTATE_INITIAL = AL_INITIAL,
		AUDIOSTATE_PLAYING = AL_PLAYING,
		AUDIOSTATE_PAUSED = AL_PAUSED,
		AUDIOSTATE_STOPPED = AL_STOPPED
	};

//from XAUDIO2
	// Numeric values with special meanings
	#define XAUDIO2_COMMIT_NOW              0             // Used as an OperationSet argument
	#define XAUDIO2_COMMIT_ALL              0             // Used in IXAudio2::CommitChanges
	#define XAUDIO2_INVALID_OPSET           (UINT32)(-1)  // Not allowed for OperationSet arguments
	#define XAUDIO2_NO_LOOP_REGION          0             // Used in XAUDIO2_BUFFER.LoopCount
	#define XAUDIO2_LOOP_INFINITE           255           // Used in XAUDIO2_BUFFER.LoopCount
	#define XAUDIO2_DEFAULT_CHANNELS        0             // Used in CreateMasteringVoice
	#define XAUDIO2_DEFAULT_SAMPLERATE      0             // Used in CreateMasteringVoice

	// Flags
	#define XAUDIO2_DEBUG_ENGINE            0x0001        // Used in XAudio2Create on Windows only
	#define XAUDIO2_VOICE_NOPITCH           0x0002        // Used in IXAudio2::CreateSourceVoice
	#define XAUDIO2_VOICE_NOSRC             0x0004        // Used in IXAudio2::CreateSourceVoice
	#define XAUDIO2_VOICE_USEFILTER         0x0008        // Used in IXAudio2::CreateSource/SubmixVoice
	#define XAUDIO2_VOICE_MUSIC             0x0010        // Used in IXAudio2::CreateSourceVoice
	#define XAUDIO2_PLAY_TAILS              0x0020        // Used in IXAudio2SourceVoice::Stop
	#define XAUDIO2_END_OF_STREAM           0x0040        // Used in XAUDIO2_BUFFER.Flags
	#define XAUDIO2_SEND_USEFILTER          0x0080        // Used in XAUDIO2_SEND_DESCRIPTOR.Flags

	// Default parameters for the built-in filter
	#define XAUDIO2_DEFAULT_FILTER_TYPE     LowPassFilter
	#define XAUDIO2_DEFAULT_FILTER_FREQUENCY XAUDIO2_MAX_FILTER_FREQUENCY
	#define XAUDIO2_DEFAULT_FILTER_ONEOVERQ 1.0f

    #ifndef UINT32
        #define UINT32 DWORD
    #endif
    // Used in IXAudio2SourceVoice::SubmitSourceBuffer
	struct XAUDIO2_BUFFER
	{
		UINT32 Flags;                       // Either 0 or XAUDIO2_END_OF_STREAM.
		UINT32 AudioBytes;                  // Size of the audio data buffer in bytes.
		const BYTE* pAudioData;             // Pointer to the audio data buffer.
		UINT32 PlayBegin;                   // First sample in this buffer to be played.
		UINT32 PlayLength;                  // Length of the region to be played in samples,
											//  or 0 to play the whole buffer.
		UINT32 LoopBegin;                   // First sample of the region to be looped.
		UINT32 LoopLength;                  // Length of the desired loop region in samples,
											//  or 0 to loop the entire buffer.
		UINT32 LoopCount;                   // Number of times to repeat the loop region,
											//  or XAUDIO2_LOOP_INFINITE to loop forever.
		void* pContext;                     // Context value to be passed back in callbacks.
	};
#endif

//エラーチェック用マクロ。RELEASEは何もしない
#ifdef _DALK_DEBUG_BUILD_
#if defined(_DALK_USE_XAUDIO_)
 #define RIN_ERRORCHECK(hr,mes)\
   if(FAILED(hr))\
 	_DEBUGPRINTF(mes);

 #define RIN_ERRORRETURN(hr,mes)\
 	if(FAILED(hr)){\
 		_DEBUGPRINTF(mes);\
		return hr;\
	}

 #define RIN_ERRORRETURNBOOL(hr,mes)\
	if(FAILED(hr)){\
		_DEBUGPRINTF(mes);\
		return 	FALSE;\
	}

#elif defined(_DALK_USE_OPENAL_)
 #define RIN_ERRORCHECK(hr,mes)\
   if((hr = alGetError()) != AL_NO_ERROR)\
	_DEBUGPRINTF(mes);

 #define RIN_ERRORRETURN(hr,mes)\
	if((hr = alGetError()) != AL_NO_ERROR){\
		_DEBUGPRINTF(mes);\
		return hr;\
	}

 #define RIN_ERRORRETURNBOOL(hr,mes)\
	if((hr = alGetError()) != AL_NO_ERROR){\
		_DEBUGPRINTF(mes);\
		return 	FALSE;\
	}
#endif
#else
 #define RIN_ERRORCHECK(hr,mes) 
 #define RIN_ERRORRETURN(hr,mes) 
 #define RIN_ERRORRETURNBOOL(hr,mes) 
#endif

#endif
