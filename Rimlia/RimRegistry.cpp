#include "MultiStdafx.h"

#include "RimRegistry.h"

//  コンストラクタ
 CRegistry::CRegistry() {
    FRootKey = HKEY_CURRENT_USER;
    m_bAvaible = false;
}

//-----------------------------------------------------------------------------
//  デストラクタ
 CRegistry::~CRegistry() {
    CloseKey();
}

//-----------------------------------------------------------------------------
//  キーの作成
bool CRegistry::CreateKey(LPCTSTR KeyName)
{
    DWORD rc;
    int rc2;

    CloseKey();
	rc2 = RegCreateKeyEx(FRootKey, KeyName, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &FHKey, &rc);

    m_bAvaible = (rc2 == ERROR_SUCCESS);

    return m_bAvaible;
}

//-----------------------------------------------------------------------------
//  キーのオープン( IsCreate==trueなら開けない場合に作成します)
bool CRegistry::OpenKey(LPCTSTR KeyName,bool IsCreate) {
    bool Result;

    CloseKey();
	Result = (RegOpenKeyEx(FRootKey, KeyName, 0, KEY_ALL_ACCESS, &FHKey) == ERROR_SUCCESS);
    m_bAvaible = Result;

    if(!Result && IsCreate)
      Result = CreateKey(KeyName);

    return Result;
}


//-----------------------------------------------------------------------------
//  キーを閉じる
void CRegistry::CloseKey() {
    if(m_bAvaible)
      RegCloseKey(FHKey);
    m_bAvaible = false;
}

//-----------------------------------------------------------------------------
//  キーを削除する
bool CRegistry::DeleteKey(LPCTSTR Key) {
    return (RegDeleteKey(FHKey,Key) == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
//  boolの読み書き
bool CRegistry::Write(LPCTSTR Name, bool Flag) {
	return ( RegSetValueEx(FHKey,Name,0,REG_DWORD,(BYTE*)&Flag,sizeof(DWORD)) == ERROR_SUCCESS );
}

bool CRegistry::Read(LPCTSTR Name, bool& Def) {
    bool Result;
    DWORD Size,Mode;

	Size = sizeof(DWORD);
    Mode = REG_DWORD;

	if( RegQueryValueEx(FHKey,Name,0,&Mode,(BYTE*)&Result,&Size) != ERROR_SUCCESS )
      Result = Def;

    return Result;
}

//-----------------------------------------------------------------------------
//  Integerの読み書き
bool CRegistry::Write(LPCTSTR Name, const int Value) {
	return ( RegSetValueEx(FHKey,Name,0,REG_DWORD,(const BYTE*)&Value,sizeof(DWORD)) == ERROR_SUCCESS );
}

bool CRegistry::Read(LPCTSTR Name,int& Def) {
    DWORD Result=0;
    DWORD Size,Mode;

	Size = sizeof(DWORD);
    Mode = REG_DWORD;

	if( RegQueryValueEx(FHKey,Name,0,&Mode,(BYTE*)&Result,&Size) != ERROR_SUCCESS)
      Def = Result;
	else
		return false;
	
    return true;
}

//-----------------------------------------------------------------------------
//  Integerの読み書き
bool CRegistry::Write(LPCTSTR Name, LPCTSTR Str) {
    DWORD Size;

	Size = _tcslen(Str);
	return ( RegSetValueEx(FHKey,Name,0,REG_SZ,(BYTE*)Str, Size) == ERROR_SUCCESS );
}

bool CRegistry::Read(LPCTSTR Name, LPTSTR Def) {
    DWORD Mode,Size;
    TCHAR tmp[MAX_PATH];

    Mode = REG_SZ;
    Size = sizeof(tmp);

	if(RegQueryValueEx(FHKey, Name, 0, &Mode, (BYTE*)tmp, &Size) == ERROR_SUCCESS)
        _STRCPY(Def, MAX_PATH, tmp);
	else
		return false;

	return true;
}

//-----------------------------------------------------------------------------
//  Binaryの読み書き
bool CRegistry::Write(LPCTSTR Name,const void *Data, int Size) {
	return (RegSetValueEx(FHKey,Name,0,REG_BINARY,(const BYTE*)Data,Size) == ERROR_SUCCESS );
}

bool CRegistry::Read(LPCTSTR Name,void*& Buff,int Size, void*& Def) {
    bool Result;
    DWORD Mode;

	Mode = REG_BINARY;

	if(RegQueryValueEx(FHKey,Name,0,&Mode,(BYTE*)Buff,(DWORD*)&Size) != ERROR_SUCCESS){
        memcpy(Buff,Def,Size);
	    Result = false;
	}else
      Result = true;

    return Result;
}
