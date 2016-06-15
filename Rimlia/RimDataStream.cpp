#include "Multistdafx.h"

#include "RimDataStream.h"

//#include "RimFile.h"

IDataStream::IDataStream() :
		m_bLoop(FALSE)
,		m_Offset(0)
,		m_Name(_T(""))
,		m_TypeName(_T(""))
{ }

/*
IDataStream::IDataStream(LPCTSTR name, void* temp) :
		m_bLoop(FALSE)
,		m_Offset(0)
,		m_Name(_T(""))
,		m_TypeName(_T(""))
{ this->Open(name); }
*/

int IDataStream::CompareString(char* str) {
	char buf[CHUNK_BUFFER_SIZE] = { 0 };

//stdio native code
	size_t len = strlen(str);

	Read(len, buf, false);

//stdio native code
	int res = strcmp(buf, str);

	return res;
}

DWORD MemoryStream::Read(DWORD byte,void* pBuffer,bool bLoop) {
	size_t readSize = byte;

	if( (m_Offset + byte) > m_DataSize)
		readSize = m_DataSize - m_Offset;

	_MEMCPY( pBuffer, m_pBase + m_Offset, readSize);

	m_Offset += readSize;

	return readSize;
}

DWORD MemoryStream::Write(DWORD byte,void* pBuffer,bool bLoop) {
	unsigned int writeSize = byte;

	if( (m_Offset + byte) > m_DataSize)
		writeSize = m_DataSize - m_Offset;

	_MEMCPY( m_pBase + m_Offset, pBuffer, writeSize );

	m_Offset += writeSize;

	return writeSize;
}

int MemoryStream::SetData(void* pData,DWORD Size) {
	if(pData == NULL) {
		void* p;
		_VALLOC(p, Size);
		m_pBase = (BYTE*)p;
	}
	else
        m_pBase = (BYTE*)pData;

	m_DataSize = Size;
	m_Offset = 0;

	return 0;
}

size_t MemoryStream::Seek(size_t offset,DWORD type) {
	switch(type) {
	case FILE_BEGIN:
		m_Offset = offset;
		break;
	case FILE_CURRENT:
		m_Offset += offset;
		break;
	case FILE_END:
		m_Offset = GetSize() + offset;
		break;
	}
	return 0;
}
/*
BOOL IDataStream::ReadEx(DWORD byte,void* pBuffer,DWORD& ReadSize){
	ReadSize = 0;

	if( (byte + GetOffset()) > GetSize() ){
		byte -= ( GetSize() - GetOffset() );

		ReadSize += Read(GetSize() - GetOffset(),pBuffer,false);

		if(m_bLoop){
			Seek(0,FILE_CURRENT);
			ReadSize += Read(byte);
		}
		return TRUE;
	}

	ReadSize = Read(byte,pBuffer);

	return FALSE;

}
*/