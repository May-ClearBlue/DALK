#include "MultiStdAfx.h"

#include "SailArchiver.h"

int CArcMaker::AddFile(LPCTSTR Path, LPCTSTR Name) {
	SArcInfoData	InfoData;
	InfoData.Path = Path;

	if(Name == NULL){
		CPathString PathStr(Path);
//		char /*drive[_MAX_DRIVE],filedir[_MAX_DIR],*/fname[_MAX_FNAME],fext[_MAX_EXT];	
//		_splitpath(Path,NULL,NULL,fname,fext);
		InfoData.Name += PathStr.GetFileNameString();//fname;
		InfoData.Name += PathStr.GetExtentString();//fext;
	}
	else
		InfoData.Name = Name;

	m_InfoData.push_back(InfoData);

	return 0;
}

int	CArcMaker::AddDir(LPCTSTR Path){
	CFileFinder	Finder;
	Finder.Open(Path);

	do{
		if(Finder.IsDirectory() == SEARCH_DIR)
			continue;
		AddFile( Finder.GetFullFileName(),Finder.GetFileName() );

	}while(Finder.Next() != SEARCH_END);

	return 0;
}

int CArcMaker::Make(LPCTSTR Name){
	int FileNumber = 0;
//	strcpy(m_Header.Header,"AABBCCDDD");

	CFile	OutFile;
	OutFile.Create(Name,TRUE,TRUE);

	//Header
	OutFile.Write(8,"AABBCCDD");

	void* Buffer;
	
	_VALLOC(Buffer, 1024 * 1024);

	for(unsigned a = 0;a < m_InfoData.size(); a ++){
		CFile	InFile;

		InFile.Open( m_InfoData[a].Path.data() );

		m_InfoData[a].Info.Offset = OutFile.GetOffset();
		m_InfoData[a].Info.Size = InFile.GetFileSize();
		InFile.GetFileTime( NULL,NULL,&(m_InfoData[a].Info.FileTime) );
		
		DWORD	ReadSize,WriteSize;
		do{
			ReadSize	= InFile.Read(1024 * 1024,Buffer);
			WriteSize	= OutFile.Write(ReadSize,Buffer);
//			NameListOffset += WriteSize;
		}while(ReadSize != 0);	
	}

	//オフセットを保存
	size_t NameListOffset = OutFile.GetOffset();

	//ファイル数
	OutFile.Write(m_InfoData.size());

	size_t infosize = sizeof(SArcInfo) * m_InfoData.size();

	for(unsigned a = 0;a < m_InfoData.size(); a ++)
		infosize += (m_InfoData[a].Name.length() + 1);

	void* data;
	_VALLOC(data, infosize);

	SArcInfo* pInfo = (SArcInfo*)data;

	for(unsigned a = 0;a < m_InfoData.size(); a ++)
		*(pInfo++) = m_InfoData[a].Info;

	LPTSTR pNameList = (LPTSTR)pInfo;

	for(unsigned a = 0;a < m_InfoData.size(); a ++) {
		_STRCPY(pNameList, m_InfoData[a].Name.data());
		pNameList += (m_InfoData[a].Name.length() + 1);
	}

	CLZO lzo;
	void* pDest;
	size_t dest;
	_VALLOC(pDest, infosize);

	lzo.Compress(infosize, data, dest, pDest);

	//圧縮前サイズ
	OutFile.Write(infosize);
	//圧縮後サイズ
	OutFile.Write(dest);
	//圧縮データ
	OutFile.Write(dest, pDest);
	//ファイル末尾オフセット
	OutFile.Write(NameListOffset);

	_VFREE(data);
	_VFREE(pDest);

	return 0;
}


CArchiver::CArchiver(){
	dirnamelist = NULL;
	dirnameindex = NULL;
	dirnum = 0;
	//totaldatasize = 0;
	m_pFileList = NULL;
	//m_pFileIndex = NULL;
	m_pInfo = NULL;
	m_FileNumber = -1;
	//m_NameListOffset = -1;
	//currentnumber = -1;
}

BOOL CArchiver::Close(){
	if(m_pList){
//		delete[] m_pFileList;
		_VFREE(m_pList);
	}
//	if(m_pFileIndex){
//		delete[] m_pFileIndex;
//		m_pFileIndex	= NULL;
//	}
	if(dirnamelist){
		delete[] dirnamelist;
		dirnamelist		= NULL;
	}
	if(dirnameindex){
		delete[] dirnameindex;
		dirnameindex	= NULL;
	}

	if( m_File.Close() ){
		m_FileNumber = -1;
		//m_NameListOffset = -1;

		//totaldatasize	= 0;
		dirnum			= 0;
		nametable.clear();
		return TRUE;
	}
	else
        return FALSE;
}
/*
int ACHV::GetFileNumber(const char *name){
//	hash_map<string,ACHVINFO>::iterator hasher = nametable.find(name);
//	return (hasher == nametable.end() ? -1 : (*hasher).second );
	

	for(unsigned int res = 0;res < achvheader.filenum;res ++){
		if(strcmp(filenameindex[res],name) == 0)
			return res;
	}
	return -1;
}
*/

int CArchiver::GetExtraInfo	(LPCTSTR filename) {
	hasher = nametable.find( filename );
	
	if ( hasher != nametable.end() )
		{ return (*hasher).second->ExtraInfo; }
	else
		{ return -1; }

}

int CArchiver::Open(LPCTSTR name) {
	hasher = nametable.find(name);
	
	if ( hasher != nametable.end() ) {
		m_File.SeekBegin( (*hasher).second->Offset );

		CPathString PathStr(name);

		m_Name		= name;
		m_TypeName	= PathStr.GetExtentString();

		if(m_TypeName.length ()> 0 && m_TypeName[0] == '.')
			m_TypeName.erase(0,1);

		return 0;
	}
   
	//return (hasher == nametable.end() ? INVALID_SET_FILE_POINTER : m_File.MovePointer( (*hasher).second->offset,FILE_BEGIN) );
	return 1;
}

DWORD CArchiver::Seek(LONG offset,DWORD type){ 
	switch(type){
	case FILE_BEGIN:
		return m_File.SeekBegin( (*hasher).second->Offset + offset );
		break;
	case FILE_CURRENT:
		return m_File.SeekCurrent( (*hasher).second->Offset );
		break;
	case FILE_END:
		return m_File.SeekEnd( (*hasher).second->Size + (*hasher).second->Offset - offset );
		break;
	}
	return INVALID_SET_FILE_POINTER;
}
/*
DWORD CArchiver::MovePointer(LONG address,DWORD type,LONG *addresshigh){
	DWORD res;

	switch(type){
	case FILE_BEGIN:
		res =  BINFILE::MovePointer(achvinfo[currentnumber].offset + address,type,addresshigh);
		break;
	case FILE_CURRENT:
		res = BINFILE::MovePointer(address,FILE_CURRENT);
		break;
	case FILE_END:
		res = BINFILE::MovePointer(achvinfo[currentnumber].offset + achvinfo[currentnumber].datasize + address);
		break;
	}

	if(res != INVALID_SET_FILE_POINTER)
		return res - achvinfo[currentnumber].offset;

	return res;
}
*/
int CArchiver::Init(LPCTSTR name){
	int res;

	//オープン済みの場合は内部を検索
	//if(handle)
	//	return (SetFilePointer(Name) != INVALID_SET_FILE_POINTER) ? 0 : 5;

	//currentnumber			= -1;
	//achvheader.filenum		= 0;
	//achvheader.namelistsize = 0;

	if( res = m_File.Open(name) )
		return res;

	TCHAR header[10] = { 0 };

	m_File.Read(8 * sizeof(TCHAR), header);

#if 0
	if( strcmp(MARCID,m_Header.Header) )
        return 1;
#endif
	//DWORD dirdatasize;

	//m_File.Read(dirnum);
	//m_File.Read(dirdatasize);

	//dirnamelist = new char[dirdatasize];
	//dirnameindex= new char*[dirnum];

	size_t dataend,srcsize,destsize;

	m_File.Seek(-1 * (int)(sizeof(int)),FILE_END);
	m_File.Read(dataend);

	m_File.Seek(dataend,FILE_BEGIN);
	m_File.Read(m_FileNumber);
	m_File.Read(destsize);
	m_File.Read(srcsize);

	void* pSrc;
	_VALLOC(pSrc, srcsize);

	m_File.Read(srcsize,pSrc);
	
	_VALLOC(m_pList, destsize);

	CLZO lzo;

	lzo.Decompress(srcsize,pSrc,destsize,m_pList,false);

	_VFREE(pSrc);

	m_pInfo		= (SArcInfo*)m_pList;
	m_pFileList	= (LPCTSTR)(m_pInfo + m_FileNumber);

	//VCのhash_mapにはない	
#if 0
	nametable.resize(m_Header.FileNumber);
#endif	

	LPTSTR temp = (LPTSTR)m_pFileList;

	for(int a = 0;a < m_FileNumber;a ++){
		nametable[temp] = &m_pInfo[a]/*.offset*/;
		//m_pFileIndex[a] = temp;
		temp += _STRLEN(temp) + 1;
	}

	hasher = nametable.end();

	return 0;
}
/*
int CArchiver::AddFile(CFile* File){
	sArcInfo info;
	info.bArchived = false;
	info.datasize = File->GetFileSize();
	info.offset = 0;
	File->GetFileTime(NULL,NULL,&info.filetime);
}
*/

/*
DWORD CArchiver::SetFilePointer(const unsigned int number){
	DWORD res = BINFILE::MovePointer(GetOffset(number),FILE_BEGIN);

	if(res != INVALID_SET_FILE_POINTER)
		currentnumber = number;
	return res;
}
*/

/*
BOOL CopyFile3(ACHV& src,LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists,HWND hfilesize,HWND htotalsize,HWND dialogwnd,HWND dialogparent){
	static BYTE buffer[_COPYFILEBUFFERSIZE_];
	::ZeroMemory(buffer,_COPYFILEBUFFERSIZE_);

	BINFILE dest;
	string errormes;

	if( src.Open(lpExistingFileName) ){
		errormes = lpExistingFileName;
		errormes += "をオープンできません";
		ErrorOK( errormes.data() );
		return FALSE;
	}

	if(bFailIfExists)
		bFailIfExists = FALSE;
	else
		bFailIfExists = TRUE;

	if( dest.Create(lpNewFileName,bFailIfExists) ){
		errormes = lpNewFileName;
		errormes += "を作成できません";
		ErrorOK( errormes.data() );
		src.Close();
		return FALSE;
	}
	
	DWORD totalsize = src.GetFileSize();

	::SendMessage(hfilesize,PBM_SETRANGE32,0,totalsize);
	::SendMessage(hfilesize,PBM_SETPOS,0,1);

	while(totalsize >= _COPYFILEBUFFERSIZE_){
		src.Read(_COPYFILEBUFFERSIZE_,buffer);
		::Sleep(80);
		dest.Write(_COPYFILEBUFFERSIZE_,buffer);
		::Sleep(80);
		::SendMessage(hfilesize,PBM_DELTAPOS,_COPYFILEBUFFERSIZE_,0);
		::RedrawWindow(hfilesize,NULL,NULL,0);
		::SendMessage(htotalsize,PBM_DELTAPOS,_COPYFILEBUFFERSIZE_,0);
		::RedrawWindow(htotalsize,NULL,NULL,0);
		totalsize -= _COPYFILEBUFFERSIZE_;
		::RedrawWindow(dialogwnd,NULL,NULL,0);
		::RedrawWindow(dialogparent,NULL,NULL,0);
		//::ValidateRect(dialogwnd,NULL);
	}

	if( totalsize > 0){
		src.Read(totalsize,buffer);
		dest.Write(totalsize,buffer);
		::SendMessage(hfilesize,PBM_DELTAPOS,totalsize,0);
		::SendMessage(htotalsize,PBM_DELTAPOS,totalsize,0);
	}

	dest.SetFileTime(src.GetFileTime(),NULL,NULL);
	dest.Close();

	//::SetFileAttributes(lpNewFileName,::GetFileAttributes(lpExistingFileName) );

	return TRUE;
}

void ACHVCOPY::Init(ACHV* achv,HWND hfile,HWND htotal,HWND dlg,HWND dlgparent){
	src = achv;
	hfilesize = hfile;
	htotalsize= htotal;
	dialogwnd= dlg;
	dialogparent= dlgparent;
}

BOOL ACHVCOPY::Copy(LPCTSTR srcfile,LPCTSTR destfile,BOOL overwrite){
	lpExistingFileName	= srcfile;
	lpNewFileName		= destfile;
	bFailIfExists		= overwrite;
	ResetEvent();
	BeginThread();
	WaitEvent(INFINITE);
	CloseThread();
	return TRUE;
}

void ACHVCOPY::ThreadProc(){
	static BYTE buffer[_COPYFILEBUFFERSIZE_];
	::ZeroMemory(buffer,_COPYFILEBUFFERSIZE_);

	BINFILE dest;
	string errormes;

	if( src->Open( lpExistingFileName.data() ) ){
		errormes = lpExistingFileName;
		errormes += "をオープンできません";
		ErrorOK( errormes.data() );
		return;
	}

	if(bFailIfExists)
		bFailIfExists = FALSE;
	else
		bFailIfExists = TRUE;

	if( dest.Create( lpNewFileName.data(),bFailIfExists ) ){
		errormes = lpNewFileName;
		errormes += "を作成できません";
		ErrorOK( errormes.data() );
		//src.Close();
		return;// FALSE;
	}
	
	DWORD totalsize = src->GetFileSize();

//	::SendMessage(hfilesize,PBM_SETRANGE32,0,totalsize);
//	::SendMessage(hfilesize,PBM_SETPOS,0,1);

	while(totalsize >= _COPYFILEBUFFERSIZE_){
		src->Read(_COPYFILEBUFFERSIZE_,buffer);
		::Sleep(80);
		dest.Write(_COPYFILEBUFFERSIZE_,buffer);
		::Sleep(80);
		::SendMessage(hfilesize,PBM_DELTAPOS,_COPYFILEBUFFERSIZE_,0);
		::RedrawWindow(hfilesize,NULL,NULL,0);
		::SendMessage(htotalsize,PBM_DELTAPOS,_COPYFILEBUFFERSIZE_,0);
		::RedrawWindow(htotalsize,NULL,NULL,0);
		totalsize -= _COPYFILEBUFFERSIZE_;
		::RedrawWindow(dialogwnd,NULL,NULL,0);
		::RedrawWindow(dialogparent,NULL,NULL,0);
		//::ValidateRect(dialogwnd,NULL);
	}

	if( totalsize > 0){
		src->Read(totalsize,buffer);
		dest.Write(totalsize,buffer);
		::SendMessage(hfilesize,PBM_DELTAPOS,totalsize,0);
		::SendMessage(htotalsize,PBM_DELTAPOS,totalsize,0);
	}

	dest.SetFileTime(src->GetFileTime(),NULL,NULL);
	dest.Close();

	SetEvent();

	//::SetFileAttributes(lpNewFileName,::GetFileAttributes(lpExistingFileName) );

	return;// TRUE;
}
*/