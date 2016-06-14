#ifndef _RIM_DLLLOADER_
#define _RIM_DLLLOADER_

#include <hash_map>

class CDLLLoader{
protected:
	hash_map<string_t,FARPROC> proc;
	HMODULE hModule;
public:
inline	BOOL	Load(LPCTSTR name)			{ return ( (hmodule = ::LoadLibrary(name) ) == NULL ?  TRUE : FALSE); }
inline	FARPROC	GetProcAddress(LPCSTR name)	{ return ::GetProcAddress(hModule,name); }
inline	BOOL	Release()					{ return ::FreeLibrary(hModule) ? TRUE : FALSE; }
		FARPROC	operator[](string_t method)	{ return proc[method]; ]
		DLLOBJECT()							{ hModule = NULL; }
virtual	~DLLOBJECT()						{ void(FAR WINAPI *pRelease)(void) = ( void(FAR WINAPI *)(void) )GetProcAddress("ReleaseDLL"); (*pRelease)(); Release(); }
};

#endif