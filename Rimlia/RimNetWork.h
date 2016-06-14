#ifndef  _RIM_NETWORK_
#define  _RIM_NETWORK_

#include "MultiStdafx.h"

#include <WinInet.h>

//レジストリ	reg://
//アーカイバ	arc://
//ファイル		files://

class CURL {
private:
	URL_COMPONENTS	m_uc;
	TCHAR	m_Scheme[256];
	TCHAR	m_HostName[256];
	TCHAR	m_UserName[256];
	TCHAR	m_Password[256];
	TCHAR	m_UrlPath[256];
	TCHAR	m_ExtraInfo[256];

	string_t	m_URL;

public:
	BOOL	SplitURL(LPCTSTR url);
	LPCTSTR	GetURLString();
};

#endif