#ifndef _SAIL_QUEBUFFER_
#define _SAIL_QUEBUFFER_

#include "SailDataStream.h"

class CQueBuffer{
protected:
	BYTE*	m_Pointer;
	int		m_Size;
	int		m_MaxSize;
	int		m_BeginPos;
	int		m_EndPos;

public:
void	Create(int Size)	{ m_Pointer = new BYTE[Size]; m_MaxSize = Size; m_Size = 0; m_BeginPos = 0; m_EndPos = 0; }
void	Release()			{
	if ( m_Pointer != NULL ){
		delete[] m_Pointer;
		m_Pointer = NULL;
	}
	m_MaxSize = 0; m_Size = 0; m_BeginPos = 0; m_EndPos = 0;
	}
int		GetSize()			{ return m_Size; }
int		GetMaxSize()		{ return m_MaxSize; }
BYTE*	GetPointer()		{ return m_Pointer; }
void	SetSize(int Size)	{ m_Size = Size; }
void	SetEndPos(int Pos)	{ m_EndPos = Pos; }
void	Push(int Size,void* Pointer){
	if( (m_EndPos + Size) > m_MaxSize ){
		int Size1 = m_MaxSize - m_EndPos;
		int Size2 = Size - Size1;

		memcpy( m_Pointer + m_EndPos, Pointer					, Size1);
		memcpy( m_Pointer			, (BYTE*)Pointer + Size1	, Size2);
		m_EndPos = Size2;
	}
	else{
		memcpy(m_Pointer + m_EndPos,Pointer,Size);
		m_EndPos += Size;
	}
	m_Size += Size;
}

void	Pop(int Size,void* Pointer){
	if( (m_BeginPos + Size) > m_MaxSize ){
		int Size1 = m_MaxSize - m_BeginPos;
		int Size2 = Size - Size1;

		memcpy( Pointer					,m_Pointer + m_BeginPos	,Size1);
		memcpy( (BYTE*)Pointer + Size1	,m_Pointer				,Size2);
		m_BeginPos = Size2;
	}
	else{
		memcpy(Pointer,m_Pointer + m_BeginPos,Size);
		m_BeginPos += Size;
	}
	m_Size -= Size;
}
	CQueBuffer(){
		m_Pointer = NULL;
		m_Size = 0;
		m_BeginPos = 0;
		m_EndPos = 0;
	}
	~CQueBuffer() { Release(); }
};

#endif