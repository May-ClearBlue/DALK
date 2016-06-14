//  レジストリアクセスクラス
#ifndef _RIM_REGISTRY_
#define _RIM_REGISTRY_

#include "MultiStdafx.h"

class CRegistry {
private:
	HKEY FRootKey;
	HKEY FHKey;
	bool m_bAvaible;

public:
	 CRegistry();
	 virtual ~CRegistry();

	bool  OpenKey	(LPCTSTR KeyName, bool IsCreate);
	bool  CreateKey	(LPCTSTR KeyName);
	void  CloseKey	(void);
    bool  DeleteKey	(LPCTSTR Key);

	bool Write	(LPCTSTR Name, const bool Flag);
	bool Write	(LPCTSTR Name, const int Value);
	bool Write	(LPCTSTR Name, LPCTSTR Str);
	bool Write	(LPCTSTR Name, const void* Data, int Size);
	bool Read	(LPCTSTR Name, bool& Def);
	bool Read	(LPCTSTR Name, int& Def);
	bool Read	(LPCTSTR Name, LPTSTR Def);
	bool Read	(LPCTSTR Name, void*& Buff, int size, void*& Def);
};

#endif