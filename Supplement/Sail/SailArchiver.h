#ifndef _SAIL_ARCHIVER_
#define _SAIL_ARCHIVER_


#include "MultiStdAfx.h"

#include "Rimlia/RimFile.h"
#include "Rimlia/RimDataStream.h"
#include "SailCompresser.h"

#include <hash_map>

//ファイル構造
// ヘッダ(8byte)
// ファイルデータ...
// ファイル数(size_t)
// インフォの展開前サイズ(int)
// インフォの展開後サイズ(int)
	//圧縮済みインフォデータ
	// |-インフォのインデックス（int*ファイル数）
	// |-ファイル名のインデックス（char[]*ファイル数）
// データ末尾のオフセット(int)

#define MARCID "MARC"			//ID							(4byte)

enum {
	COMPRESS_NONE,
	COMPRESS_LZO,
};

struct SArcInfo{
	uint64_t	Offset;
	uint64_t	Size;
	FILETIME	FileTime;
	int			ExtraInfo;
	int			CompressType;
};

struct SArcInfoData{
	SArcInfo	Info;
	string_t	Name;
	string_t	Path;
};

class CArcMaker {
public:
//	CArcMaker(){;}
//	~CArcMaker(){;}
protected:
	vector<SArcInfoData>	m_InfoData;
public:
	int		AddFile	(LPCTSTR Path, LPCTSTR Name = NULL);
	int		AddDir	(LPCTSTR Path);
	int		Make	(LPCTSTR Name);
};

//仕様変更：アーカイバそのものを読み込む時は必ずCloseしてハンドルをNULLにしてから
class CArchiver : public IDataStream {
protected:
	CFile		m_File;
//int			currentnumber;			//現在操作中のナンバー
	int	m_FileNumber;		//総ファイル数					(4byte)

	void*		m_pList;
	SArcInfo*	m_pInfo;
	LPCTSTR		m_pFileList;
	DWORD		dirnum;
	LPCTSTR		dirnamelist;
	LPCTSTR*		dirnameindex;

	hash_map<string_t,SArcInfo*>				nametable;
	hash_map<string_t,SArcInfo*>::iterator	hasher;
public:
	CArchiver();
	~CArchiver(){ Close(); }

	int		Open(LPCTSTR name);
	DWORD	Seek(LONG offset,DWORD type);
//ToDo:ループ処理
	DWORD 	Read(DWORD byte,void* pBuffer,bool bLoop)	{ return m_File.Read(byte,pBuffer); }
	DWORD	Write(DWORD byte,void* pBuffer,bool bLoop)	{ return m_File.Write(byte,pBuffer); }
	size_t	GetOffset()			{ return m_File.GetOffset() - (*hasher).second->Offset; }
	size_t	GetSize()			{ return (*hasher).second->Size; }

	STREAM_TYPE	GetStreamType()	{ return STREAM_PACKFILE; }

	int		Init(LPCTSTR name);
	BOOL	Close();

inline	FILETIME*		GetFileTime()								{ return &(*hasher).second->FileTime;/*GetFileTime(currentnumber);*/ }
inline	int				GetExtraInfo	(LPCTSTR filename);

inline	int				GetAllNumber()								{ return m_FileNumber; }
inline	DWORD			GetAllDirNumber()							{ return dirnum; }
inline	LPCTSTR*		GetDirIndex()								{ return dirnameindex; }
inline	LPCTSTR			GetDirName		(const unsigned int number)	{ return dirnameindex ? dirnameindex[number] : NULL; }
};
/*
class ACHVCOPY:public THREAD{
protected:
	ACHV*	src;
	string_t	lpExistingFileName;
	string_t	lpNewFileName;
	BOOL	bFailIfExists;
	HWND	hfilesize;
	HWND	htotalsize;
	HWND	dialogwnd;
	HWND	dialogparent;
public:
	void	Init(ACHV* achv,HWND hfile,HWND htotal,HWND dlg,HWND dlgparent);
	BOOL	Copy(LPCTSTR srcfile,LPCTSTR destfile,BOOL overwrite);
	void	ThreadProc();
};

//ACHVからコピー
BOOL CopyFile3(ACHV& src,LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists,HWND hfilesize = NULL,HWND htotalsize = NULL,HWND dialogwnd = NULL,HWND dialogparent = NULL);
*/

#endif