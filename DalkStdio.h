#ifndef _MULTI_STDIO_
#define _MULTI_STDIO_

#include "DalkConfig.h"
#include "DalkStringMacro.h"

//Saya:メモ。
//良く使われてる(googleさん調べ)プリプロセッサのインチキテクニック。
//シェーダとか外部で動的にコンパイルする時があるテキストファイルを切り分けたい時に
//
// int g[] = {
// #include "data.csv"
// };
//
//とか書くときの文字列版。
//
// static const char* TextFile = _STRINGIFY(
// #include "String.txt"
// );
//
//と書くだけ。一長一短なやり方だとは思うので使い方は計画的に。

#define _STRINGIFY(A)  #A

#if defined(_DALK_WINDOWS_)
//#define _WIN32_WINNT_WIN7
#include <windows.h>
#include <windowsx.h>
#include <comsvcs.h>
#include <gdiplus.h>
#include <crtdbg.h>
#include <shellapi.h>
#include <iomanip>
#include <cstdarg>
#include <stdint.h>
#include <tchar.h>

#include <mmreg.h>
#include <mmsystem.h>

#define nil NULL			//iOS互換用
#define _FUNC_NAME __FUNCTION__
#define _SLEEP(x)			::Sleep(x)
#define _GETSYSTEMTIME()	::timeGetTime()
#define _MEMCPY(x,y,z)		::CopyMemory(x, y, z)
#define _MEMSET(x,y,z)		memset(x,y,z)

#define	_ZEROINIT(x)		::ZeroMemory(&x,sizeof(x))
#define _ZEROINITPTR(x,y)	::ZeroMemory(x, y);

#ifdef _DALK_DEBUG_BUILD_
#define _DEBUGPRINTF(fmt, ... ) \
      { \
        TCHAR c[256]; \
		wsprintf( c, fmt, ##__VA_ARGS__ ); \
		_tcscat_s(c, TEXT("\n"));\
        ::OutputDebugString( c ); \
      }
#else
#define _DEBUGPRINTF(...) { ; }
#endif

#else

//コンソールはmallocを生で使わない
#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */

//SYSTEMTIME Win32ベタ移植
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;


#if defined(_DALK_IOS_)
 #import <UIKit/UIKit.h>
 #define FAILED(r) (r != 0)
 #define _FUNC_NAME __PRETTY_FUNCTION__
 #define _SLEEP(x) usleep(x*1000)
// #define _SLEEP(x) [NSThread sleepForTimeInterval:x / 1000.f]
  #define _GETSYSTEMTIME()

inline void GetLocalTime(SYSTEMTIME* pSystemTime) {
	if(pSystemTime != NULL) {
		NSDate*		date		= [NSDate date];
		NSCalendar*	calendar	= [NSCalendar currentCalendar];
		NSDateComponents* comps;

		// 年月日をとりだす
		comps = [calendar components:(NSCalendarUnitYear | NSCalendarUnitMonth | NSCalendarUnitDay) fromDate:date];
	/*
		NSInteger year  = [comps year];
		NSInteger month = [comps month];
		NSInteger day   = [comps day];
	*/
		pSystemTime->wYear  = [comps year];
		pSystemTime->wMonth = [comps month];
		pSystemTime->wDay   = [comps day];
		//NSLog(@"year: %d month: %d, day: %d", year, month, day);
		//=> year: 2010 month: 5, day: 22

		// 時分秒をとりだす
		comps = [calendar components:(NSCalendarUnitHour | NSCalendarUnitMinute | NSCalendarUnitSecond) fromDate:date];
	/*
		NSInteger hour = [comps hour];
		NSInteger minute = [comps minute];
		NSInteger second = [comps second];
	*/
		pSystemTime->wHour   = [comps hour];
		pSystemTime->wMinute = [comps minute];
		pSystemTime->wSecond = [comps second];
		pSystemTime->wMilliseconds = 0;
		//NSLog(@"hour: %d minute: %d second: %d", hour, minute, second);
		//=> hour: 18 minute: 24 second: 31

		// 週や曜日などをとりだす
		comps = [calendar components:(NSCalendarUnitWeekOfMonth| NSCalendarUnitWeekday | NSCalendarUnitWeekdayOrdinal) fromDate:date];
	/*
		NSInteger week = [comps week]; // 今年に入って何週目か
		NSInteger weekday = [comps weekday]; // 曜日
		NSInteger weekdayOrdinal = [comps weekdayOrdinal]; // 今月の第何曜日か
	*/
		pSystemTime->wDayOfWeek = [comps weekday]; // 曜日

		//NSLog(@"week: %d weekday: %d weekday ordinal: %d", week, weekday, weekdayOrdinal);
		//=> week: 21 weekday: 7(日曜日) weekday ordinal: 4(第4日曜日)
	}
}
#endif //_DALK_IOS_

//#define _MALLOC(x) malloc(x)
//#define _FREE(x) free(x);
#define _MEMCPY(x,y,z) memcpy(x,y,z)
#define _MEMSET(x,y,z) memset(x,y,z)

#define	_ZEROINIT(x)		memset(&x, 0, sizeof(x))
#define _ZEROINITPTR(x,y)	memset(x,0,y);

//DEBUGPRINTF
#if defined(_DALK_IOS_)
 #ifdef DEBUG
//    NSString* _temp_ = [NSString stringWithCString: fmt encoding:NSUTF8StringEncoding];
  #define _DEBUGPRINTF(fmt, ...) NSLog((@"%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
 #else
  #define _DEBUGPRINTF(...)
 #endif
#elif defined(_DALK_ANDROID_)
 #ifdef DEBUG
  #define _DEBUGPRINTF(...) __android_log_print(ANDROID_LOG_DEBUG, __VA_ARGS__)
 #else 
  #define _DEBUGPRINTF(...)
 #endif
#endif

#endif

#ifdef _DALK_DEBUG_BUILD_
#define DALK_ERRORRETURN(res,mes)\
	{\
		_DEBUGPRINTF(mes);\
		return res;\
	}
#define DALK_ERRORRETURNINT(res,mes)\
	if(res != 0){\
		_DEBUGPRINTF(mes);\
		return res;\
	}
#define DALK_ERRORRETURNBOOL(res,mes)\
	if(res != TRUE){\
		_DEBUGPRINTF(mes);\
		return 	FALSE;\
	}
#else
#define DALK_ERRORRETURN(res,mes) 
#define DALK_ERRORRETURNINT(res,mes) 
#define DALK_ERRORRETURNBOOL(res,mes) 
#endif

#if defined(_DALK_USE_UNICODE_)

#if 0
//inline FILE* _wfopen(TCHAR* wname, TCHAR* wmode) {
inline FILE* _wfopen(LPCTSTR wname, LPCTSTR wmode) {
	string_t wsname = wname;
	string_t wsmode = wmode;

	string name = DalkString::EncodefromString_t(wsname);
	string mode = DalkString::EncodefromString_t(wsmode);

	return fopen( name.data(), mode.data() );
}
#endif
#endif

//#define MAKELONG(a, b) ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
//各種エンディアン変換
template<class T> T swapEndian16(T& value){
#ifdef _DALK_DEBUG_BUILD_
	if(sizeof(value) != 2)
		_DEBUGPRINTF(_TEXT("WARNING:swapEndian32"));
#endif

    return value = ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
}

template<class T> T swapEndian32(T& value){
#ifdef _DALK_DEBUG_BUILD_
	if(sizeof(value) != 4)
		_DEBUGPRINTF(_TEXT("WARNING:swapEndian32"));
#endif
	return value = (value >> 24) | (value << 24) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000);
}

inline float swapEndianFloat(float& valf){
	int val;
	memcpy(&val,&valf,sizeof(val));	// Not BitShift float
	val = (val<<24) | ((val<<8) & 0x00ff0000) | ((val>>8) & 0x0000ff00) | ((val>>24) & 0x000000ff) ;
	memcpy(&valf,&val,sizeof(valf));
	return valf;
}

//変数のbitが立ってる数を数える
template <class P>inline int bitcount(P var){
    int c = 0;

    while (var) {
        ++c;
        var &= (var - 1);
    }

    return c;
}

#if defined(_DALK_WINDOWS_)
//汎用クローズ関数……実質DirctX等COMのクローズ用
#define SafeRelease(P)\
	if(P) {\
		(P)->Release();\
		P = NULL;\
	}

//汎用ハンドルクローズ関数
inline BOOL SafeClose(HANDLE& handle){
	if(handle != NULL && handle != INVALID_HANDLE_VALUE){
		if( ::CloseHandle(handle) )
			handle = NULL;
		else
			return FALSE;
	}
	return TRUE;
}
#endif

//newで確保したメモリ解放関数
#define SafeDelete(ptr) if ( ptr != NULL ) { delete ptr; ptr = NULL; }

//new[]で確保したメモリ解放関数
#define SafeDeletes(ptr) if ( ptr != NULL ) { delete[] ptr; ptr = NULL; }

template <class P>inline void* MemSet(void* pBuf,P data){
	P* pData = (P*)pBuf;
	*pData = data;
	return ++pData;
}

inline void MemStreamCopy(void* pDest, void*& pSrc, int size) {
	memcpy(pDest,pSrc,size);
	pSrc = ((unsigned char*)pSrc) + size;
}

template <class P>inline void MemStreamCopyVar(P data, void*& pBuf) {
	MemStreamCopy(&data, pBuf, sizeof(data));
}

inline void MemeSetOffset(void*& pDest, int size) {
	pDest = ((unsigned char*)pDest) + size;
}

#endif
