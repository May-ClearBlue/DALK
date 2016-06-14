#include <MultiStdafx.h>

#include "RimFile.h"

#if defined(_DALK_WINDOWS_)

//BOOL MakeSureDirectoryPathExists( PCSTR DirPath );
void ChangeFileExtent(std::string& filename,char* pFileExtent){
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];

	_splitpath_s( filename.data(), drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), NULL, 0 );

	filename.clear();

	filename += drive;
	filename += dir;
	filename += fname;
	filename += pFileExtent;
}

void ChangeFileExtent(char*& pFilename,char* pFileExtent){
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];

	_splitpath_s( pFilename, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), NULL, 0);
	
	::ZeroMemory( pFilename,strlen(pFilename) );
//Be Careful
	strcat_s(pFilename, 4, drive);
	strcat_s(pFilename, 4, dir);
	strcat_s(pFilename, 4, fname);
	strcat_s(pFilename, 4, pFileExtent);
}

void CPathString::SplitPath (LPCTSTR Path){
	_SPLITPATH( Path, m_Drive, m_Dir, m_Fname, m_Ext );

	m_Path = Path;
	m_DriveStr = m_Drive;
	m_DirStr = m_Dir;
	m_FnameStr = m_Fname;
	m_ExtStr = m_Ext;
}

LPITEMIDLIST CPathString::GetItemIDList(LPTSTR p_cFileStr){
	LPITEMIDLIST pIDL;
	LPSHELLFOLDER pDesktopFolder; 

	if( ::SHGetDesktopFolder( &pDesktopFolder ) != NOERROR )
		return NULL;

	OLECHAR       ochPath[MAX_PATH];
	ULONG         chEaten;  //?・ｽ・ｽ???????T?C?Y?・ｽ・ｽ・ｽ???????????・ｽ・ｽ?B
	ULONG         dwAttributes;     //???・ｽL?・ｽ・ｽ・ｽ???????????・ｽ・ｽ?B
	HRESULT       hRes;

#ifndef _DALK_WIDECHAR_MODE_
	::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, p_cFileStr, -1, ochPath, MAX_PATH );
#else
	_STRCPY(ochPath,  p_cFileStr);
#endif
	//?@?????・ｽcITEMIDLIST?・ｽ・ｽ・ｽ?????????・ｽ・ｽ?B
	hRes = pDesktopFolder->ParseDisplayName( NULL, NULL, ochPath, &chEaten, &pIDL, &dwAttributes);

	if( hRes != NOERROR )
	pIDL = NULL;

	pDesktopFolder->Release();

	return pIDL;
}

BOOL CPathString::GetDeskTopPath (HWND hWnd) {
	//BOOL res = FALSE;
	//LPITEMIDLIST idl;
	//if(::SHGetSpecialFolderLocation(hwnd,CSIDL_DESKTOPDIRECTORY,&idl) == NOERROR)
		//res = ::SHGetPathFromIDList(idl,buffer);
		//::CoTaskMemFree(idl);
	//return res;
	TCHAR buf[MAX_PATH];
	if(SHGetSpecialFolderPath(hWnd, buf, CSIDL_DESKTOPDIRECTORY,FALSE) == FALSE)
		return FALSE;
	SplitPath(buf);
	return TRUE;
}

BOOL CPathString::GetProgramListPath (HWND hWnd) {
//BOOL res = FALSE;
//LPITEMIDLIST idl;
//if(::SHGetSpecialFolderLocation(hwnd,CSIDL_PROGRAMS,&idl) == NOERROR)
	//res = ::SHGetPathFromIDList(idl,buffer);
	//::CoTaskMemFree(idl);
//return res;
	TCHAR buf[MAX_PATH];
	if(SHGetSpecialFolderPath(hWnd, buf, CSIDL_PROGRAMS,FALSE) == FALSE)
		return FALSE;
	SplitPath(buf);
	return TRUE;
}

int CPathString::OpenFileBrowser(HWND hWnd, LPTSTR filter,LPTSTR caption, LPTSTR result, BOOL allow_any_files){
	//char buf[MAX_PATH];

	OPENFILENAME ofn;
	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= hWnd;
	ofn.hInstance			= NULL;
	ofn.lpstrFilter			= filter;									//?t?@?C?????t?B???^
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 1;
	ofn.lpstrFile			= result/*buf*/;							//?i?[?o?b?t?@
	ofn.nMaxFile			= MAX_PATH;									//?p?X????????
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= caption;									//?L???v?V????
	ofn.Flags				= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.nFileOffset			= 0;
	ofn.nFileExtension		= 0;
	ofn.lpstrDefExt			= NULL;
    ofn.lCustData			= NULL;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= 0;
	::GetOpenFileName(&ofn);
	return 0;
};

int CPathString::OpenDirBrowser(HWND hWnd, LPTSTR p_cSetStr, LPTSTR p_cRootStr, LPTSTR p_cCaptionStr, LPTSTR result, UINT p_uiFlags){
	LPMALLOC pMalloc;

	//?@IMalloc?C???^?[?t?F?C?X?・ｽ・ｽ???|?C???^?・ｽ・ｽ・ｽ?????????・ｽ・ｽ?B
	if( ::SHGetMalloc( &pMalloc ) != NOERROR )
	return 1;

	LPITEMIDLIST pIDL,pRootIDL;
	BROWSEINFO stBInfo;
	TCHAR chReturn[MAX_PATH] = _T("");

	pRootIDL = GetItemIDList( p_cRootStr );

	//?@?\?????・ｽ・ｽ・ｽ???????????・ｽ・ｽ?B
	stBInfo.pidlRoot		= NULL;				//pRootIDL;//???[?g?t?H???_???・ｽ・ｽ?B
	stBInfo.hwndOwner		= hWnd;				//?・ｽ}???_?C?A???O???e?E?B???h?E???n???h?????・ｽ・ｽ?B
	stBInfo.pszDisplayName	= chReturn;			//?I?・ｽ・ｽ・ｽ???????????t?H???_?・ｽ・ｽ・ｽ?・ｽe?・ｽ・ｽ?・ｽ・ｽ?・ｽ・ｽ???|?C???^???・ｽ・ｽ?B
	stBInfo.lpszTitle		= p_cCaptionStr;	//?・ｽ・ｽ?????・ｽ・ｽ???????・ｽ・ｽ?B
	stBInfo.ulFlags			= p_uiFlags;		//?t?H???_?????・ｽc?????・ｽ・ｽ?B
	stBInfo.lpfn			= (BFFCALLBACK)BrowseCallbackProc; //?v???V?[?W???・ｽ・ｽ???|?C???^???・ｽ・ｽ?B
	stBInfo.lParam			= (LPARAM)p_cSetStr;//?I?・ｽ・ｽ・ｽ?・ｽ・ｽ???t?H???_?・ｽ・ｽ???h?c???・ｽ・ｽ?B

	//?@?_?C?A???O?{?b?N?X?・ｽ・ｽ・ｽ?\?・ｽ・ｽ?????・ｽ・ｽ?B
	pIDL = ::SHBrowseForFolder( &stBInfo );
	//?@pidl?・ｽc???????t?H???_???l?[???X?y?[?X?・ｽc?・ｽ・ｽ?A?・ｽ・ｽ???????・ｽ・ｽ?|?C???^??
	//?@???????????・ｽ・ｽ?B?・ｽ}?????_????chReturn?・ｽc???I?・ｽ・ｽ・ｽ?????・ｽ・ｽ?t?H???_??????
	//?@?????????????????????A?・ｽ}???|?C???^?・ｽ・ｽ・ｽ???p?????・ｽ・ｽ?B
	if( pIDL != NULL ){
	//?@?t???p?X?・ｽ・ｽ・ｽ?????????・ｽ・ｽ?B
		if( ::SHGetPathFromIDList( pIDL, chReturn ) )
			_STRCPY(result, MAX_PATH, chReturn);

		pMalloc->Free( pIDL );
	}
	else
		return 1;
	//?@?????[?X?????・ｽ・ｽ?B
	if( pRootIDL != NULL )
		pMalloc->Free( pRootIDL );
	pMalloc->Release();

//?@?・ｽ・ｽ?????・ｽ・ｽ・ｽ?・ｽe?????・ｽ・ｽ?B
	SplitPath(chReturn);
	return 0;
}

BOOL CPathString::CreateShortcut(LPCTSTR szLinkFile, LPCTSTR workdir){
	IShellLink* pShellLink = NULL;
	IPersistFile* pPersistFile = NULL;

	if(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,IID_IShellLink, (void**)&pShellLink) != S_OK)
		return FALSE;

	if(pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile) != S_OK)
		return FALSE;
	//???????・ｽc???s???・ｽ・ｽ????

	if(pShellLink->SetPath(m_Path.data()) != S_OK){
	//?p?X???????・ｽc???s???・ｽ・ｽ????
		pShellLink->Release();
		return FALSE;
	}

	if(workdir)
		pShellLink->SetWorkingDirectory(workdir);
	
//OLECHAR・ｽﾍ奇ｿｽ{・ｽI・ｽ・ｽWCHAR・ｽ・ｽdefine・ｽﾈのゑｿｽUnicode・ｽ・ｽ・ｽ墲ｹ
#ifdef _DALK_WIDECHAR_MODE_
	if( pPersistFile->Save(szLinkFile, TRUE) != S_OK ) {
#else
	WCHAR wBuf[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, szLinkFile, -1, wBuf, MAX_PATH);	
	if(pPersistFile->Save(wBuf, TRUE) != S_OK) {
#endif
		pShellLink->Release();
		pPersistFile->Release();
		return FALSE;
	}

	pShellLink->Release();
	pPersistFile->Release();

	return TRUE;
}

int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData ) {
	if(uMsg == BFFM_INITIALIZED) {
		::SetWindowText( hwnd, _T("・ｽt・ｽ@・ｽC・ｽ・ｽ・ｽ・ｽI・ｽ・ｽ?0"));
		::SendMessage( hwnd, BFFM_SETSELECTION,TRUE, lpData );
	}
	else{
		TCHAR chText[MAX_PATH];
		if( ::SHGetPathFromIDList( (LPITEMIDLIST)lParam, chText ) )
			::SendMessage( hwnd, BFFM_SETSTATUSTEXT, TRUE, (LPARAM)chText );
	}
	return 0;
}
#elif defined(_DALK_IOS_)
void CPathString::SplitPath (LPCTSTR Path) {
    _STRING str(Path);
	SplitPath(_GETSTRING(str));
}

void CPathString::SplitPath (NSString* Path){
	m_Path = [Path copy];
	m_FnameStr = [m_Path.GetNSString() lastPathComponent]/*[fileName stringByDeletingPathExtension]*/;
	m_ExtStr   = [m_Path.GetNSString() pathExtension];
	m_DirStr   = [m_Path.GetNSString() stringByDeletingLastPathComponent];
	m_DriveStr = @"";

	m_Ext = m_ExtStr.data();
	m_Fname =  m_FnameStr.data();
	m_Dir = m_DirStr.data();
}

#endif

#if defined(_DALK_WINDOWS_)
CFileFinder::CFileFinder(){
	m_bIgnoredir		= FALSE;
	m_hFileSearch		= NULL;
	m_pPath				= NULL;
	//filename			= NULL;
	m_pLowerDirectory	= NULL;
}

BOOL CFileFinder::Close(){
	if(m_hFileSearch != INVALID_HANDLE_VALUE){
		if( ::FindClose(m_hFileSearch) ){
			m_hFileSearch = INVALID_HANDLE_VALUE;
			SafeDeletes(m_pPath);
		}
		else
			return FALSE;
	}
	return TRUE;
}

LPCTSTR	CFileFinder::GetFullFileName(){
	if(m_pLowerDirectory)
		return m_pLowerDirectory->GetFullFileName();
	
	m_fullfilename = m_pPath;
	m_fullfilename += m_findata.cFileName;

	return m_fullfilename.data();
}

LPCTSTR	CFileFinder::GetLastFileName(){
	if(m_pLowerDirectory)
		return m_pLowerDirectory->GetFileName();
	
	return m_findata.cFileName;
}

LPCTSTR	CFileFinder::GetFileName(string_t& str){
	str += m_findata.cFileName;

	if(m_pLowerDirectory){
		str += _T("\\");
		m_pLowerDirectory->GetFileName(str);
		return str.data();
	}
	else
		return NULL;
}

LPCTSTR	CFileFinder::GetFileName(){
	if(m_pLowerDirectory){
		m_filename.clear();
		GetFileName(m_filename);
		return m_filename.data();
	}
	return m_findata.cFileName;

}

int CFileFinder::IsDirectory(){
	if(m_pLowerDirectory)
		return m_pLowerDirectory->IsDirectory();

	return (m_findata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? SEARCH_DIR : SEARCH_FILE;

}
int CFileFinder::Next(){
	if(m_pLowerDirectory){
		int res =  m_pLowerDirectory->Next();
		if(res == SEARCH_END){
			delete m_pLowerDirectory;
			m_pLowerDirectory = NULL;
			goto NEXTMAIN;
		}
		else
			return res;
	}

	if( IsDirectory() == SEARCH_DIR && m_bIgnoredir != FALSE){
		string_t m_pLowerDirectorym_pPath = m_pPath;
		m_pLowerDirectorym_pPath += m_findata.cFileName;
		m_pLowerDirectorym_pPath += _T("\\");
		m_pLowerDirectorym_pPath += m_searchfilename;
		m_pLowerDirectory = new CFileFinder;
		m_pLowerDirectory->Open( m_pLowerDirectorym_pPath.data(), TRUE );
		return m_pLowerDirectory->IsDirectory();
	}

NEXTMAIN:
	do{
		if(::FindNextFile(m_hFileSearch,&m_findata) == 0){
			if(::GetLastError() == ERROR_NO_MORE_FILES)
				return SEARCH_END;
		}
	}while( _STRCMP (m_findata.cFileName, _T(".")) == 0 || _STRCMP(m_findata.cFileName, _T("..")) == 0 );

	return IsDirectory();
}

BOOL CFileFinder::Open(LPCTSTR name,BOOL ignoreflag){
	m_pPath = NULL;
//	filename = m_findata.cFileName;
	if( ( m_hFileSearch = ::FindFirstFile(name,&m_findata) ) == INVALID_HANDLE_VALUE)
		return FALSE;

	unsigned int len = 0;
	unsigned int temp = 0; 

	while(temp < _tcslen(name) ){
		name[temp] < 0 ? temp += 2 : temp ++;

		if(name[temp] == '\\')
			len = temp;
	}

	m_searchfilename = name + len + 1;

	m_pPath = new TCHAR[len + 2];
	_ZEROINITPTR( m_pPath, sizeof(TCHAR) * (len + 2));
	_STRNCPY(m_pPath, strlen(m_pPath), name, len + 1);

//???t?H???_???・ｽ・ｽ???????????[?v?・ｽc?????????・ｽ・ｽ?????C???B
	while( _STRCMP(m_findata.cFileName, _T(".")) == 0 || _STRCMP(m_findata.cFileName, _T("..")) == 0 )
		::FindNextFile(m_hFileSearch, &m_findata);

//	res = m_pPath;
//	res += filename;
	m_bIgnoredir = ignoreflag;

	return TRUE;
}

CFile::CFile() :
	m_hFile(NULL)
,	m_bReadonly(FALSE)
,	m_bLocked(FALSE)
,	m_bOverlap(false)
{
	_ZEROINIT(m_Overlap);
}

BOOL CFile::Close() {
	DWORD readsize = 0;
	if(IsAsyncComplete(readsize) == -1)
		GetOverlappedResult(m_hFile, &m_Overlap, &readsize, TRUE);

	return SafeClose(m_hFile);
}

BOOL CFile::Lock(const DWORD begin, DWORD end){
	if(m_bLocked == TRUE)
		return TRUE;
	if(end = 0xFFFFFFFF)
		end = GetFileSize();  

	if( ::LockFile(m_hFile,begin,0,end,0) ){
		m_lockbegin = begin;
		m_lockend	= end;
		m_bLocked	= TRUE;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFile::Unlock(){
	if(m_bLocked == FALSE)
		return TRUE;

	if(	::UnlockFile( m_hFile,m_lockbegin,0,m_lockend,0) ){
		m_lockbegin = 0;
		m_lockend = 0;
		m_bLocked = FALSE;
		return TRUE;
	}
	return FALSE;

}

int CFile::Create(LPCTSTR pFilename,const BOOL bOverwrite,/*const BOOL bInitialize,*/const BOOL bOverload){
	::SetLastError(0);

	if(m_hFile != NULL && bOverload == FALSE)
		return 1;

	Close();
	
	DWORD flag = FILE_ATTRIBUTE_NORMAL;

	//	async = sync_mode;
//	if(sync_mode == TRUE)
//		flag |= FILE_FLAG_OVERLAPPED;

	DWORD accessmode = CREATE_NEW;

	if(bOverwrite == TRUE)
		accessmode = /*bInitialize ? TRUNCATE_EXISTING :*/ CREATE_ALWAYS;

	if( (m_hFile = ::CreateFile(pFilename, GENERIC_READ | GENERIC_WRITE,0,NULL,accessmode,flag,NULL)) == INVALID_HANDLE_VALUE ){
		m_hFile = NULL;
	
		DWORD err = GetLastError();

		if(err == ERROR_ALREADY_EXISTS || err == ERROR_FILE_EXISTS)
			return 2;//???・ｽc?・ｽ・ｽ???・ｽ・ｽ???t?@?C??
		return 3;//????????????
	}
	m_filename = pFilename;

	return 0;
}

size_t CFile::GetFileSize(DWORD *pResulthigh) {
	LARGE_INTEGER  r;
	if(::GetFileSizeEx(m_hFile,&r)) {
#ifdef _WIN64
			return r.QuadPart;
#else
			return r.LowPart;
#endif
	}
	else
		return -1;
}

DWORD CFile::Seek(LONG address, DWORD type, LONG *pAddresshigh) {
	DWORD res = ::SetFilePointer(m_hFile,address,pAddresshigh,type);
	if(res != INVALID_SET_FILE_POINTER)
		return res;
	//else if(addresshigh == NULL || GetLastError() != NO_ERROR)
	//	throw FAILED_MOVEPOINTER;
	return res;
}

int CFile::Open(LPCTSTR pFilename,const BOOL bFileexist,const BOOL bOverload,const BOOL bShare, bool bOverlap){
//1 = ?I?[?o?[???[?h???s
//2 = ?・ｽ・ｽ?????????t?@?C??
//3 = ??????
	if(m_hFile != NULL && bOverload == FALSE)
		return RIM_ERROR_FILEHANDLE_OVERWRITE;

	Close();

	DWORD flag = FILE_ATTRIBUTE_NORMAL;

	m_bOverlap = bOverlap;

	if(bOverlap)
		flag |= FILE_FLAG_OVERLAPPED;
	
	DWORD accessmode = OPEN_ALWAYS;
	if(bFileexist == TRUE)
		accessmode = OPEN_EXISTING;
	
	DWORD attribute = GENERIC_READ | GENERIC_WRITE;
	m_bReadonly = FALSE;

	if(::GetFileAttributes(pFilename) & FILE_ATTRIBUTE_READONLY){
		m_bReadonly = TRUE;
		attribute = GENERIC_READ;
	}

	if(bShare)
		attribute |= FILE_SHARE_READ;

	::SetLastError(0);
	
	m_hFile = ::CreateFile(pFilename,attribute,FILE_SHARE_READ,NULL,accessmode,flag,NULL);

	if(m_hFile == INVALID_HANDLE_VALUE){
		_DEBUGPRINTF(_TEXT("File not Found"));
		//::MessageBox(NULL, _T("File not Found"), pFilename, MB_OK | MB_TOPMOST );
		if(::GetLastError() == ERROR_FILE_NOT_FOUND )
			return RIM_ERROR_FILE_NOT_FOUND;
		else
			return RIM_ERROR_FILE_FAILED_OPEN;
    }
	m_filename = pFilename;

	return 0;
}

DWORD CFile::Read(const DWORD bytes,void* pBuffer){
	DWORD readsize;

	::ReadFile (m_hFile,pBuffer,bytes, &readsize,NULL);

	return readsize;
}

DWORD CFile::Write(DWORD bytes, const void* pBuffer){
	DWORD writesize;
	::WriteFile(m_hFile, pBuffer, bytes, &writesize,NULL);
	return writesize;
}

int CFile::ReadAsync(const DWORD bytes, void* pBuffer, DWORD& readsize, const DWORD offset, const DWORD offsethigh) {
	if(!m_bOverlap)
		return -1;
	else if(HasOverlappedIoCompleted(&m_Overlap) == FALSE)
		return -2;

	_ZEROINIT(m_Overlap);

	m_Overlap.Offset = offset;
	m_Overlap.OffsetHigh = offsethigh;             // ?K?v?・ｽc???・ｽ・ｽ???Z?b?g
	m_Overlap.hEvent = NULL;               // ?K?v?・ｽc???・ｽ・ｽ???Z?b?g

	if(::ReadFile( m_hFile, pBuffer, bytes, &readsize, &m_Overlap ))
		return 0;

	//???s???・ｽ・ｽ???????[???N???A????????
	_ZEROINIT(m_Overlap);

	return 1;
}


int CFile::WriteAsync(const DWORD bytes, void* pBuffer, DWORD& readsize, const DWORD offset, const DWORD offsethigh) {
	if(!m_bOverlap)
		return -1;
	else if(HasOverlappedIoCompleted(&m_Overlap) == FALSE)
		return -2;

	_ZEROINIT(m_Overlap);

	m_Overlap.Offset = offset;
	m_Overlap.OffsetHigh = offsethigh;
	m_Overlap.hEvent = NULL;              

	if(::WriteFile( m_hFile, pBuffer, bytes, &readsize, &m_Overlap ))
		return 0;

	_ZEROINIT(m_Overlap);

	return 1;
}

int CFile::IsAsyncComplete(DWORD &readsize) {
	if(!m_bOverlap)
		return 1;
	
	if(GetOverlappedResult(m_hFile, &m_Overlap, &readsize, FALSE))
		return 0;

	return -1;
}

#else
CFile::CFile() {
	m_hFile = NULL;
}

BOOL CFile::Close() {
	int t = ::fclose(m_hFile);

    if(t == 0)
		m_hFile = NULL;

	return t != 0 ? TRUE : FALSE;
}

int CFile::Create(LPCTSTR pFilename,const BOOL bOverwrite,/*const BOOL bInitialize,*/const BOOL bOverload) {
    _FOPEN(m_hFile, pFilename, _T("r+b"));
	if(m_hFile == NULL)
		return errno;
//ToDo:
	int r = stat(pFilename, &m_Stat);

	m_filename = pFilename;
	return 0;
}

DWORD CFile::Seek(LONG address,DWORD type,LONG *pAddresshigh) {
//SEEK_SET
//SEEK_CUR
//SEEK_END
	return ::fseek(m_hFile, address, type);
}

int CFile::Open(LPCTSTR pFilename,const BOOL bFileexist, BOOL bOverload, const BOOL bShare, bool bOverlap) {
	if(m_hFile != NULL && bOverload == false)
		return 1;

	Close();

	string_t mode = _T("rb");

	_FOPEN(m_hFile, pFilename,mode.data());

	if (m_hFile == NULL)
		return 1;

	int r = stat(pFilename, &m_Stat);

	m_filename = pFilename;

	return 0;
}

DWORD CFile::Read(const DWORD bytes,void* pBuffer){
	DWORD readsize;
	readsize = fread (pBuffer, bytes, 1, m_hFile); 
	return readsize;
}

DWORD CFile::Write(DWORD bytes,const void* pBuffer){
	DWORD writesize;
	writesize = fwrite (pBuffer, bytes, 1, m_hFile); 
	return writesize;
}

#endif


DWORD CFile::ReadAll(void*& pBuffer,int usemethod) {
	DWORD fsize = GetFileSize();

	if(pBuffer == NULL)
		_VALLOC(pBuffer, fsize);

	return Read(fsize,pBuffer);
}

DWORD CFile::ReadString(char*& pBuffer){
	DWORD fsize = GetFileSize();

	//Read(fsize);

	if(pBuffer == NULL)
		pBuffer = new char[fsize + 1];

	_ZEROINITPTR(pBuffer,fsize + 1);

	return Read(fsize,pBuffer);
}

DWORD CFile::ReadString(string& str){
	DWORD fsize = GetFileSize();

	//Read(readsize);

	if(fsize){
		str.resize(fsize + 1,'\0');
		return Read( fsize,(void*)str.data() );
	}

	str.clear();
	return 0;
}