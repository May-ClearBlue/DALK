#include "Multistdafx.h"

#include "RimFileStream.h"

int CFileStream::Open(LPCTSTR name) {
	int res = 0;

#ifdef _DALK_USE_STDSTREAM_	
	m_Stream.open(name, std::ios::binary |  std::ios_base::in);// | std::ios_base::in);

	if(!m_Stream.fail()) {
		m_Size = m_Stream.seekg(0, _IOS_END).tellg();
		m_Stream.seekg(0, _IOS_BEGIN);
	}
	else
		res = 1;
#else
	res = m_File.Open(name,TRUE,TRUE);

	if(res)
		m_Size = m_File.GetFileSize();
	else
		res = 0;
#endif

#if 0
	if(res == 0 ){
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( name, drive, dir, fname, ext );
		m_Name		= fname;
		m_TypeName	= ext;
		DalkString::ToUpper((char*)m_TypeName.data());

		if(m_TypeName.length ()> 0 && m_TypeName[0] == '.')
			m_TypeName.erase(0,1);
	}
#endif
	return res;
}

size_t CFileStream::Seek(size_t offset,DWORD type) {
#ifdef _DALK_USE_STDSTREAM_

	if(m_Stream.eof())
		m_Stream.clear();

	switch(type) {
	case FILE_BEGIN:
//	case SEEK_SET:
//	case std::ios::beg:
		m_Stream.seekg(offset, std::ios::beg);
		break;
	case FILE_CURRENT:
//	case SEEK_CUR:
//	case std::ios::cur:
		m_Stream.seekg(offset, std::ios::cur); 
		break;
	case FILE_END:
//	case SEEK_END:
//	case std::ios::end:
		m_Stream.seekg(offset, std::ios::end); 
		break;
	}

	if(!m_Stream.fail())
		m_Offset = m_Stream.tellg();
	else
		return  SIZE_MAX;
#else
	DWORD r = m_File.Seek(offset,type,NULL);

#endif
	return m_Offset;
}

DWORD CFileStream::Read(DWORD byte,void* pBuffer,bool bLoop) {
#ifdef _DALK_USE_STDSTREAM_
	m_Stream.read((char*)pBuffer, byte);
	return m_Stream.gcount();
#else
	DWORD readmax = GetSize() - GetOffset();

	if(byte <= readmax){
		byte = m_File.Read(byte,pBuffer);
		m_Offset += byte;
	}
	else {
		DWORD read1 = m_File.Read(readmax,pBuffer);
		m_File.SeekBegin(0);
		
		if(bLoop){
			DWORD read2 = m_File.Read(byte - read1,(char*)pBuffer + read1);
			byte = read1 + read2;
			m_Offset = read2;
		}
		else
			byte = read1;
	}
	return byte;
#endif
}

DWORD CFileStream::Write (DWORD byte,void* pBuffer,bool bLoop) {
#ifdef _DALK_USE_STDSTREAM_
	m_Stream.write((const char*)pBuffer, byte);
	return m_Stream.gcount();
#else
	DWORD writemax = GetSize() - GetOffset();

	if( byte <= writemax ) {
		byte = m_File.Write( byte, pBuffer );
		m_Offset += byte;
	}
	else {
		DWORD write1 = m_File.Write( writemax, pBuffer );
		m_File.SeekBegin(0);

		if( bLoop ) {
			DWORD write2 = m_File.Write( byte - write1, (char*)pBuffer + write1);
			byte = write1 + write2;
			m_Offset = write2;
		}
		else
			byte = write1;
	}
	return byte;
#endif
}

size_t CFileStream::GetOffset(){
#ifdef _DALK_USE_STDSTREAM_
	return m_Stream.tellg();
#else
	return m_File.SeekCurrent(0);
#endif
}

size_t CFileStream::GetSize(){
#ifdef _DALK_USE_STDSTREAM_
	return m_Size;
#else
	return m_File.GetFileSize();
#endif
}
