#ifndef _RIM_EXTAUDIODECODER_
#define _RIM_EXTAUDIODECODER_

#include "RimSoundDecoder.h"

class CExtAudioStream : public IDataStream { 
public:
	CExtAudioStream();
	~CExtAudioStream() { Close(); }

protected:
    ExtAudioFileRef m_ExtAudioFileRef;
	SInt64			m_TotalFrame;
	_PCMFORMAT		m_Format;
public:

inline	int		Open			( LPCTSTR name )		 { return 0; }
		int		Open			( NSString* fileName );
		size_t	Seek			( size_t offset, DWORD type);
		DWORD	Read			( DWORD byte, void* pBuffer, bool bLoop);
		DWORD	Write			( DWORD byte, void* pBuffer, bool bLoop);
		size_t	GetOffset();
		int		GetTotalFrame	( SInt64& frame );
		int		GetFormat		( _PCMFORMAT& Format );
inline	size_t	GetSize()								{ return m_TotalFrame; }  //ToDo;
inline	int		SetFormat		( _PCMFORMAT& Format )	{ return ExtAudioFileSetProperty(m_ExtAudioFileRef, kExtAudioFileProperty_ClientDataFormat, sizeof(Format), &Format);	}
inline	int		Close()									{ return ExtAudioFileDispose(m_ExtAudioFileRef); }
inline	STREAM_TYPE	GetStreamType()						{ return STREAM_RESOURCE; }
};

class CExtAudioDecoder : public ISoundDecoder {
protected:
    CExtAudioStream* m_pExtAudioFileSteram;
public:
virtual	size_t		TellFrame()			{ return 0; }// ToDo;}
virtual int			SeekFrame(size_t frame) { return m_pExtAudioFileSteram->Seek(frame,0); }

	int		SetStream		( void* pStream = NULL );
inline	size_t	Decode			( void* pDestBuffer, DWORD DestSize, DWORD SrcSize )	{ return m_pExtAudioFileSteram->Read(DestSize, pDestBuffer, false); }
		int		Close()			{ return 0; }
	//pStream=NSSting
};
#endif
