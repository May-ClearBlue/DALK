#ifndef _SAIL_ARCHIVER_
#define _SAIL_ARCHIVER_


#include "MultiStdAfx.h"

#include "Rimlia/RimFile.h"
#include "Rimlia/RimDataStream.h"
#include "SailCompresser.h"

#include <hash_map>

//�t�@�C���\��
// �w�b�_(8byte)
// �t�@�C���f�[�^...
// �t�@�C����(size_t)
// �C���t�H�̓W�J�O�T�C�Y(int)
// �C���t�H�̓W�J��T�C�Y(int)
	//���k�ς݃C���t�H�f�[�^
	// |-�C���t�H�̃C���f�b�N�X�iint*�t�@�C�����j
	// |-�t�@�C�����̃C���f�b�N�X�ichar[]*�t�@�C�����j
// �f�[�^�����̃I�t�Z�b�g(int)

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

//�d�l�ύX�F�A�[�J�C�o���̂��̂�ǂݍ��ގ��͕K��Close���ăn���h����NULL�ɂ��Ă���
class CArchiver : public IDataStream {
protected:
	CFile		m_File;
//int			currentnumber;			//���ݑ��쒆�̃i���o�[
	int	m_FileNumber;		//���t�@�C����					(4byte)

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
//ToDo:���[�v����
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

//ACHV����R�s�[
BOOL CopyFile3(ACHV& src,LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists,HWND hfilesize = NULL,HWND htotalsize = NULL,HWND dialogwnd = NULL,HWND dialogparent = NULL);
*/

#endif