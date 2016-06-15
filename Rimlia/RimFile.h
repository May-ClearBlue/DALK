#ifndef _RIM_FILE_
#define _RIM_FILE_

#include "../MultiStdafx.h"
#include "RimHeap.h"

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DALK_WINDOWS_
#include <commctrl.h> 
#include <commdlg.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <imagehlp.h>

#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"uuid.lib")
#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib, "shlwapi.lib")
//#pragma comment(lib,"uuid2.lib")

#pragma warning(push)
#pragma warning( disable : 4267 )

//#define SEEK_SET FILE_BEGIN
//#define SEEK_CUR FILE_CURRENT
//#define SEEK_END FILE_END

#else
#include <unistd.h>
#define FILE_BEGIN SEEK_SET
#define FILE_CURRENT SEEK_CUR 
#define FILE_END SEEK_END 
#endif

#define _IOS_BEGIN std::ios::beg
#define _IOS_CURRENT std::ios::beg
#define _IOS_END std::ios::beg


#ifdef _DALK_IOS_
//#include <sys/errno.h>
#endif


//HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall
//アプリの追加と削除の情報
//サブキー:DisplayName（文字列）リストに表示する文字列(アプリケーション名など) 
//サブキー:UninstallString（文字列）アンインストールプログラム等を実行するコマンドライン文字列
//                             具体的には、"C:\WINDOWS\UNINST.EXE"といった感じでフ
//                             ルパスで指定してやります。

//Windowsのレジストリに関する規約で、HKEY_LOCAL_MACHINES\SOFTWARE\に、
//社名のキーを作り、その下に製品名のキーを作り、
//そこにアプリケーションごとの設定を保存することになっている。

//HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Desktop\General\Wallpaper

//BOOL PathFileExists(LPCTSTR Path);
//MakeSureDirectoryPathExists(PCSTR DirPath);
#ifdef _DALK_WINDOWS_
void ChangeFileExtent(string_t& filename,char* pFileExtent);
void ChangeFileExtent(char*& pFilename,char* pFileExtent);
#endif

class CPathString {
public:
	CPathString(){ ; }
	CPathString(LPCTSTR p){ SplitPath(p); }
#if defined(_DALK_IOS_)
	CPathString(NSString* p){ SplitPath(p); }
#endif

protected:
#if defined(_DALK_WINDOWS_)
	TCHAR	m_Drive	[_MAX_DRIVE];
	TCHAR	m_Dir	[_MAX_DIR];
	TCHAR	m_Fname	[_MAX_FNAME];
	TCHAR	m_Ext	[_MAX_EXT];
#elif defined(_DALK_IOS_)
	LPCTSTR	m_Drive;
	LPCTSTR m_Dir;
	LPCTSTR m_Fname;
	LPCTSTR m_Ext;
#endif
	_STRING	m_DriveStr;
	_STRING	m_DirStr;
	_STRING	m_FnameStr;
	_STRING	m_ExtStr;

	_STRING	m_Path;

	void            SplitPath		(LPCTSTR Path);
#if defined(_DALK_WINDOWS_)
	LPITEMIDLIST	GetItemIDList	(LPTSTR p_cFileStr);
#elif defined(_DALK_IOS_)
	void			SplitPath		(NSString* Path);
#endif

public:
void	operator=(LPCTSTR Path)					{ SplitPath( Path ); }
inline	const _STRING&	GetExtentString()				{ return m_ExtStr; }
inline	const _STRING&	GetFileNameString()				{ return m_FnameStr; }
inline	const _STRING&	GetDirectoryString()			{ return m_DirStr; }
inline	const _STRING&	GetDriveString()				{ return m_DriveStr; }
inline	LPCTSTR	GetExtentStringBuf()			{ return m_Ext; }
inline	LPCTSTR	GetFileNameStringBuf()			{ return m_Fname; }
inline	LPCTSTR	GetDirectoryStringBuf()			{ return m_Dir; }
inline	LPCTSTR	GetDriveStringBuf()				{ return m_Drive; }


#if defined(_DALK_WINDOWS_)
	BOOL	GetDeskTopPath (HWND hWnd);
	BOOL	GetProgramListPath (HWND hWnd);
	//BOOL	GetProgramFilesPath(){ return ReadRegistry(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion","ProgramFilesDir",(LPBYTE)buffer); }
	int		OpenFileBrowser	(HWND hWnd, LPTSTR filter, LPTSTR title, LPTSTR result, BOOL allow_any_files);
	int		OpenDirBrowser	(HWND hWnd, LPTSTR p_cSetStr, LPTSTR p_cRootStr, LPTSTR p_cCaptionStr, LPTSTR result, UINT p_uiFlags = BIF_RETURNONLYFSDIRS);
	BOOL	CreateShortcut	(/*LPCSTR szExeFile,*/LPCTSTR szLinkFile, LPCTSTR workdir = NULL);

	//char*	FileName()						{ return m_Fname; }
	void	ChangeExtent(LPCTSTR ext)	{ _STRCPY(m_Ext, ext); m_ExtStr = ext; }
#elif defined(_DALK_IOS_)
	void	ChangeExtent(NSString* ext)	{ [m_Path.GetNSString() stringByDeletingPathExtension];  [m_Path.GetNSString() stringByAppendingPathExtension:ext]; }
#endif
};

#ifdef _DALK_WINDOWS_
int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

enum { SEARCH_FAILED,SEARCH_END,SEARCH_DIR,SEARCH_FILE };

class CFileFinder {
protected:
	BOOL			m_bIgnoredir;
	WIN32_FIND_DATA	m_findata;
	HANDLE			m_hFileSearch;
	TCHAR*			m_pPath;
	string_t		m_filename;
	string_t		m_fullfilename;
	string_t		m_searchfilename;
	string_t		m_nametemp;
	CFileFinder*	m_pLowerDirectory;
public:
		CFileFinder();
virtual	~CFileFinder()				{ Close(); }

		BOOL		Close();
		LPCTSTR		GetFileName();
		LPCTSTR		GetFileName	(string_t& filename);
		LPCTSTR		GetLastFileName();
		LPCTSTR		GetFullFileName();
inline	HANDLE		GetHandle()			{ return m_hFileSearch; }	
inline	LPCTSTR		GetPath()			{ return m_pPath; }
inline	string_t&	GetString()			{ return m_fullfilename; }
		int			IsDirectory();
		int			Next();
		BOOL		Open		(LPCTSTR name,BOOL ignoreflag = FALSE);
};
#endif

//バイナリファイル操作クラス
class CFile{
protected:
#ifdef _DALK_WINDOWS_
	HANDLE	m_hFile;
	DWORD	m_lockbegin,m_lockend;
	BOOL	m_bLocked;
	BOOL	m_bReadonly;
	bool	m_bOverlap;
	OVERLAPPED m_Overlap;
#else
	FILE*	m_hFile;
	struct stat	m_Stat;
#endif
	string_t	m_filename;

public:
		CFile();
virtual	~CFile(){ Close(); }

		BOOL	Lock(const DWORD begin = 0,DWORD end = 0xffffffff);
		BOOL	Unlock();

		int		Create(LPCTSTR  Filename, const BOOL bOverwrite = FALSE, /*const BOOL bInitialize = FALSE,*/const BOOL bOverload = FALSE);
//		int		Create(NSSring* Filename,const BOOL bOverwrite = FALSE,/*const BOOL bInitialize = FALSE,*/const BOOL bOverload = FALSE);
#ifdef _DALK_WINDOWS_
inline	BOOL	GetFileTime	(LPFILETIME pCreate,LPFILETIME pLastaccess,LPFILETIME pLastwrite)					{ return ::GetFileTime(m_hFile,pCreate,pLastaccess,pLastwrite); }
inline	BOOL	SetFileTime	(const FILETIME* pCreate,const FILETIME* pLastaccess,const FILETIME* pLastwrite)	{ return ::SetFileTime(m_hFile,pCreate,pLastaccess,pLastwrite); }
inline	BOOL	SetEOF()								{ return ::SetEndOfFile(m_hFile); }
inline	HANDLE	GetHandle()								{ return m_hFile; }
		size_t	GetFileSize	(DWORD *pResulthigh = NULL)	;
#else
//ToDo:Unicode版はないので（C規格外）char版のみにする
//inline 	void	GetStat()								{ stat(m_filename.data(), &m_Stat); }
inline	bool	GetFileTime	(int type){ return true; }	//(LPFILETIME pCreate,LPFILETIME pLastaccess,LPFILETIME pLastwrite)					return ::GetFileTime(m_hFile,pCreate,pLastaccess,pLastwrite); }
inline	bool	SetFileTime	(int type){ return true; } //const FILETIME* pCreate,const FILETIME* pLastaccess,const FILETIME* pLastwrite)	{ return TRUE; }//::SetFileTime(m_hFile,pCreate,pLastaccess,pLastwrite); }
inline	bool	SetEOF()								{ return true; }//::(m_hFile); }//int truncate(const char *path， off_t length); }
inline	FILE*	GetHandle()								{ return m_hFile; }
inline	DWORD	GetFileSize	(DWORD *pResulthigh = NULL)	{ return m_Stat.st_size; } //::GetFileSize(m_hFile,pResulthigh); }

//inline	DWORD	SeekBegin	(LONG address,LONG *pAddresshigh = NULL) { return Seek(address,SEEK_SET,pAddresshigh); }
//inline	DWORD	SeekCurrent	(LONG address,LONG *pAddresshigh = NULL) { return Seek(address,SEEK_CUR,pAddresshigh); }
//inline	DWORD	SeekEnd		(LONG address,LONG *pAddresshigh = NULL) { return Seek(address,SEEK_END,pAddresshigh); }
//inline	DWORD	GetOffset	(LONG *pAddresshigh = NULL)	{ return Seek(0,SEEK_CUR,pAddresshigh); }
#endif

inline	DWORD	SeekBegin	(LONG address,LONG *pAddresshigh = NULL) { return Seek(address, FILE_BEGIN, pAddresshigh); }
inline	DWORD	SeekCurrent	(LONG address,LONG *pAddresshigh = NULL) { return Seek(address, FILE_CURRENT, pAddresshigh); }
inline	DWORD	SeekEnd		(LONG address,LONG *pAddresshigh = NULL) { return Seek(address, FILE_END, pAddresshigh); }
inline	DWORD	GetOffset	(LONG *pAddresshigh = NULL)	{ return Seek(0, FILE_CURRENT, pAddresshigh); }
inline	DWORD	GetBeginOffset()						{ return 0; }
inline	LPCTSTR	GetFileName()							{ return m_filename.data(); }

		DWORD	Seek		(LONG address,DWORD type,LONG *pAddresshigh = NULL);

		BOOL	Close();
		int		Open		( LPCTSTR pFilename, const BOOL bFileexist = TRUE, BOOL bOverload = TRUE, const BOOL bShare = FALSE, bool bOverlap = false);
///		int		Open		( NSString* pFilename, const BOOL bFileexist = TRUE, BOOL bOverload = TRUE, const BOOL bShare = FALSE, bool bOverlap = false);

		DWORD	Read		(const DWORD bytes,void* pBuffer);
		DWORD	ReadAll		(void*& pBuffer,int usemethod = 0);

		DWORD	Write		(const DWORD bytes, const void* pBuffer);

		int		ReadAsync	(const DWORD bytes, void* pBuffer, DWORD& readsize, const DWORD offset = 0, const DWORD offsethigh = 0);
		int		WriteAsync	(const DWORD bytes, void* pBuffer, DWORD& readsize, const DWORD offset = 0, const DWORD offsethigh = 0);
		int		IsAsyncComplete(DWORD &readsize);

		DWORD	ReadString	(char*& pBuffer);
		DWORD	ReadString	(string& str);
//		DWORD	ReadString	(wchar_t*& pBuffer);
//		DWORD	ReadString	(wstring& str);

inline	void	WriteString	(const char* pString)		{ /*return Write( (size_t)strlen(pString) ) ?*/ Write( strlen(pString), pString); Write((BYTE)0); }
inline	void	WriteString	(string& str)				{ /*return Write( (size_t)str.size() ) ?*/ Write( str.size(), str.data() ); Write((BYTE)0); }
inline	void	WriteString	(const wchar_t* pString)	{ /*return Write( (size_t)strlen(pString) ) ?*/ Write( wcslen(pString), pString); Write((wchar_t)0); }
inline	void	WriteString	(wstring& str)				{ /*return Write( (size_t)str.size() ) ?*/ Write( str.size(), str.data() ); Write((wchar_t)0); }
template <class P>inline void  WriteVector(vector<P>& vec)	{ Write(vec.size() * sizeof(P), &vec[0]); }
template <class P>inline DWORD Read(P& var)				{ return Read(sizeof(var),&var); }
template <class P>inline DWORD Write(P var)				{ return Write(sizeof(var),&var); }
//template <class P>inline DWORD Write(P vec[])			{ return Write(sizeof(vec),vec); }
////template <class P>inline DWORD ReadVector(P& vec[])	{return Read(sizeof(vec),vec);}
};

#pragma warning(pop)

//ToDo:いい加減ファイルわけなさい
#ifdef _DALK_WINDOWS_

inline BOOL lvAddSubItem(HWND hwnd,LPTSTR name,int width,int id){
	LV_COLUMN lvcol;

	lvcol.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt		= LVCFMT_LEFT;
	lvcol.cx		= width;
	lvcol.pszText	= name;
	lvcol.iSubItem	= id;
	
	return ListView_InsertColumn(hwnd,id,&lvcol);
}

inline BOOL lvSetItem(HWND hwnd,LPTSTR name,int line,int column){
    LV_ITEM item;
	item.mask		= LVIF_TEXT;
	item.pszText	= name;
	item.iItem		= line;
	item.iSubItem	= column;

	return ListView_SetItem(hwnd, &item);
}

inline BOOL lvAddItem(HWND hwnd,LPTSTR name,int line){
    LV_ITEM item;
	item.mask		= LVIF_TEXT;
	item.pszText	= name;
	item.iItem		= line;
	item.iSubItem	= 0;

	return ListView_InsertItem(hwnd, &item);
}

inline void lvDeleteSelected(HWND hwnd){
	int res;

	while(1){
		if ( (res = ListView_GetNextItem(hwnd,-1,LVNI_ALL | LVNI_SELECTED) ) == -1 )
			return;
		ListView_DeleteItem(hwnd,res);
	}
}
#endif


#endif