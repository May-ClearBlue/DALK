#include "MultiStdafx.h"

#include "RimNetWork.h"

BOOL CURL::SplitURL(LPCTSTR url) {
	m_uc.dwStructSize	= sizeof(m_uc);
	m_uc.lpszScheme		= m_Scheme;
	m_uc.lpszHostName	= m_HostName;
	m_uc.lpszUserName	= m_UserName;
	m_uc.lpszPassword	= m_Password;
	m_uc.lpszUrlPath	= m_UrlPath;
	m_uc.lpszExtraInfo	= m_ExtraInfo;

	m_uc.dwSchemeLength		= 256;
	m_uc.dwHostNameLength	= 256;
	m_uc.dwUserNameLength	= 256;
	m_uc.dwPasswordLength	= 256;
	m_uc.dwUrlPathLength	= 256;
	m_uc.dwExtraInfoLength	= 256;

	return InternetCrackUrl( url , 0, 0, &m_uc);
/*
	printf("scheme: '%s'\n", uc.lpszScheme);
	printf("host name: '%s'\n", uc.lpszHostName);
	printf("port: %d\n", uc.nPort);
	printf("user name: '%s'\n", uc.lpszUserName);
	printf("password: '%s'\n", uc.lpszPassword);
	printf("url path: '%s'\n", uc.lpszUrlPath);
	printf("extra info: '%s'\n", uc.lpszExtraInfo);
	
	printf("scheme type: ");
	switch(uc.nScheme) {
		case INTERNET_SCHEME_PARTIAL: printf("partial.\n"); break;
		case INTERNET_SCHEME_UNKNOWN: printf("unknown.\n"); break;
		case INTERNET_SCHEME_DEFAULT: printf("default.\n"); break;
		case INTERNET_SCHEME_FTP: printf("FTP.\n"); break;
		case INTERNET_SCHEME_GOPHER: printf("GOPHER.\n"); break;
		case INTERNET_SCHEME_HTTP: printf("HTTP.\n"); break;
		case INTERNET_SCHEME_HTTPS: printf("HTTPS.\n"); break;
		case INTERNET_SCHEME_FILE: printf("FILE.\n"); break;
		case INTERNET_SCHEME_NEWS: printf("NEWS.\n"); break;
		case INTERNET_SCHEME_MAILTO: printf("MAILTO.\n"); break;
		default: printf("%d\n", uc.nScheme);
	}
*/
}

LPCTSTR CURL::GetURLString() {
	m_URL = m_Scheme;
	m_URL += m_HostName;
	m_URL += m_UrlPath;
	return m_URL.data();
}