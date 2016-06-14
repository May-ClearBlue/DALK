#ifndef _SAIL_DATA_STREAM_
#define _SAIL_DATA_STREAM_

#include "DalkStdio.h"
#include "RimHeap.h"
#include "RimFile.h"

//CopmareStringのバッファサイズ
#define CHUNK_BUFFER_SIZE 15

enum STREAM_TYPE {
	STREAM_MEMORY,
	STREAM_FILE,
	STREAM_PACKFILE,
#if defined(_DALK_IOS_)
	STREAM_RESOURCE		//iOS
#endif
};

class IDataStream {
public:
//	IDataStream(const IDataStream& obj){;}
//	IDataStream(LPCTSTR name, void* pTemp);
	IDataStream();
	virtual ~IDataStream() { ; };

		//BOOL	ReadEx(DWORD byte,void* pBuffer,DWORD& ReadSize);

protected:
		BOOL	m_bLoop;
		size_t	m_Offset;
		string_t	m_Name;
		string_t	m_TypeName;
public:
virtual	int		Open	( LPCTSTR name )							= 0;
#if defined(_DALK_IOS_)
virtual	int		Open	( NSString* fileName )						= 0;
#endif
virtual	size_t	Seek	( size_t offset, DWORD type )				= 0;
virtual	DWORD 	Read	( DWORD byte, void* pBuffer, bool bLoop )	= 0;	//読み込みは32bitに制限
virtual	size_t	GetOffset()						= 0;
virtual	size_t	GetSize()						= 0;
virtual	DWORD	Write	( DWORD byte,void* pBuffer,bool bLoop)		= 0;	//書き込みも32bit幅まで
int		CompareString(char* str);									//チャンク用、基本的に４文字（余裕を見て１５文字までバッファを取った）
virtual	STREAM_TYPE	GetStreamType()	= 0;
//inline	string_t&	GetName()				{ return m_Name; }
//inline	string_t&	GetTypeName()			{ return m_TypeName; }
template <class P>size_t Read(P& var)	{ return Read(sizeof(var),&var,false); }
};

class MemoryStream : public IDataStream {
protected:
	BYTE*	m_pBase;
	DWORD	m_DataSize;
public:
	int		SetData	( void* pData, DWORD Size );
	int		Open	( LPCTSTR name )			{ return 1; }
#if defined(_DALK_IOS_)
	int		Open	( NSString* fileName )		{ return 1; };
#endif
	size_t	Seek	( size_t offset, DWORD type );
	DWORD 	Read	( DWORD byte, void* pBuffer, bool bLoop );
	DWORD	Write	( DWORD byte, void* pBuffer, bool bLoop );
	size_t	GetOffset()	{ return m_Offset; }
	size_t	GetSize()	{ return m_DataSize; }
//	string_t&	GetName()				{ return m_Name; }
//	string_t&	GetTypeName()			{ return m_TypeName; }
	STREAM_TYPE	GetStreamType()			{ return STREAM_MEMORY; }
//template <class P>DWORD Read(P& var)	{ return Read(sizeof(var),&var,false); }
	MemoryStream() { ; }
};

#endif