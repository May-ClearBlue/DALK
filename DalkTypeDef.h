#ifndef _SAIL_MULTITYPEDEF_
#define _SAIL_MULTITYPEDEF_

//longはWin64が32bitでそれ以外が64bit。なので直接使わない
//long_tを定義

//inlcude
#if defined(_DALK_WINDOWS_)
 #include <stdint.h>
 #include <windows.h>

 #ifdef _DALK_USE_UNICODE_
  #include <tchar.h>
 #endif
 // #include <boost/cstdint.hpp>
#elif defined(_DALK_IOS_)
 #import <Foundation/Foundation.h>
#endif

#if defined(_DALK_WINDOWS_)
 typedef LONGLONG	long_t;		//32bit<->64bit対応用
#else
#include <ctype.h>
typedef long		long_t;
typedef long		LONG;
typedef void*		HWND;
typedef int64_t		__int64;
typedef uint32_t	LPARAM;
typedef uint32_t	WPARAM;
typedef uint32_t	LRESULT;
typedef const char*	LPCSTR;
typedef char*		LPSTR;
typedef uint8_t		BYTE;
typedef uint32_t	UINT;
typedef uint16_t	WORD;
typedef uint32_t	DWORD;
typedef uint32_t	COLORREF;
typedef void*		LPVOID;
typedef char*		LPSTR;
typedef char		CHAR;
typedef size_t		SIZE_T;
//typedef wchar_t		TCHAR;
typedef uint16_t    WCHAR;
typedef char		TCHAR;
typedef char*       LPTSTR;
typedef const char* LPCTSTR;

#define MAX_PATH 256

//Win側（long=32bit）にあわせる
typedef struct tagRECT { 
   int left;
   int top;
   int right;
   int bottom;
} RECT;

#ifndef _DALK_WINDOWS_
inline	void	SetRect(RECT* pRect, int left, int top, int right, int bottom)  {
	if(pRect) {
		pRect->left  = left;
		pRect->top  = top;
		pRect->right = right;
		pRect->bottom = bottom;
	}
}
#endif

typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE;

#ifdef _DALK_IOS_
 #define TRUE 1
 #define FALSE 0
#else
 typedef bool BOOL;
 #define TRUE true
 #define FALSE false
#endif

#define LOWORD(l)           ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))
//Win Orgin.
//#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
//#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#endif

#endif
