#ifndef _SAIL_FILESTREAM_
#define _SAIL_FILESTREAM_

#define _DALK_USE_STDSTREAM_

#ifdef _DALK_USE_STDSTREAM_
#include <iostream>
#include <fstream>
#else
#include "RimFile.h"
#endif
#include "RimDataStream.h"

class CFileStream : public IDataStream {
public:
//	CFileStream(const CFileStream& obj){ }
	CFileStream() :
	  m_Size(0)
	{ ; }

	CFileStream(LPCTSTR name){ Open(name); }
#if defined(_DALK_IOS_)
	CFileStream(NSString* name){ Open(name); }
#endif

protected:
#ifdef _DALK_USE_STDSTREAM_
	_FSTREAM	m_Stream;
#else
	CFile	m_File;
#endif
	size_t	m_Size;
public:
		int		Open	( LPCTSTR name );
#if defined(_DALK_IOS_)
inline	int		Open	( NSString* fileName )		{ return Open([fileName UTF8String]); }
#endif
		size_t	Seek	( size_t offset, DWORD type );
		DWORD	Read	( DWORD byte, void* pBuffer, bool bLoop );
		DWORD	Write	( DWORD byte, void* pBuffer, bool bLoop );
		size_t	GetOffset();
		size_t	GetSize();
STREAM_TYPE	GetStreamType()	{ return STREAM_FILE; }
//	CFileStream(){;}
};

#endif