//コンシューマかPCか
//_DALK_MACHINETYPE_CONSOLE_
//_DALK_MACHINETYPE_PC_
//_DALK_MACHINETYPE_MOBILE_

//コンソールのターゲット機
//_DALK_TARGET_PS4_
//_DALK_TARGET_VITA_

//_DALK_WINDOWS_
//_DALK_IOS_
//_DALK_ANDROID_

//_DALK_USE_DIRECT3D_
//_DALK_USE_OPENGL_

//_DALK_USE_DIRECTSOUND_  //用意はするけど使わないはず
//_DALK_USE_XAUDIO_
//_DALK_USE_OPENAL_

//_DALK_USE_SQUIRREL_

//_DALK_USE_EASTL

//_DALK_64BIT_NATIVE_
//_DALK_BIGENDIAN_
//_DALK_LITTLEENDIAN_

//_WIN32
//_WIN64
//__APPLE__

#ifndef _DALK_CONFIG_
#define _DALK_CONFIG_

#pragma message("--------------DALK Config--------------")

//ビルドタイプ　DEBUGかRELEASE
#if defined(_DEBUG) || defined(DEBUG)
 #define _DALK_DEBUG_BUILD_
#elif defined(_RELEASE) || defined(RELEASE) || defined(NDEBUG)
 #define _DALK_RELEASE_BUILD_
#else
 #define _DALK_UNKNOWN_BUILD_
#endif

//プラットフォーム
#if defined(_WIN32) && !defined(_DALK_WINDOWS_)
 #define _DALK_WINDOWS_
#elif defined(__APPLE__) && !defined(_DALK_IOS_)
 #define _DALK_IOS_
#elif defined(__ANDROID__) && !defined(_DALK_ANDROID_)
 #define _DALK_ANDROID_
#else
#endif

#if defined(_DALK_WINDOWS_)
#pragma message("Platform:Windows")
#elif defined(_DALK_IOS_)
 #pragma message("Platform:iOS")
#elif defined(_DALK_ANDROID_)
#pragma message("Platform:Android")
#else
#pragma message("Platform:Unknown")
#endif

//64bit対応
#if defined (_WIN64) || defined(__LP64__)
#define _DALK_64BIT_NATIVE_
#pragma message("Arch:64bit")
#else
#pragma message("Arch:32bit")
#endif

//エンディアン
#if defined (_DALK_WINDOWS_) || defined(_DALK_IOS_)
#define _DALK_LITTLEENDIAN_
#pragma message("Order:LittleEndian")
#else
#define _DALK_BIGENDIAN_
#pragma message("Order:BigEndian")
#endif

//使用言語（データ＝スクリプト側）
enum {
	_DALK_CHARCODE_UTF8_,
	_DALK_CHARCODE_UTF16_,
	_DALK_CHARCODE_UTF32_,
	_DALK_CHARCODE_ENGLISH_,
	_DALK_CHARCODE_JAPANESE_,
	_DALK_CHARCODE_KOREAN_,
	_DALK_CHARCODE_CHINESE_,
	_DALK_CHARCODE_OTHER_,
	_DALK_CHARCODE_UNKNOWN_ = 255
};

//システムのデフォルト言語(=TCHARの扱い)
#if defined (_DALK_WINDOWS_)
  #define _DALK_WCHAR_SIZE_  2

 #if defined(_UNICODE) || defined(UNICODE)
  #define _DALK_SYSTEM_LANGUEAGE_	_DALK_CHARCODE_UTF16_
  #define _DALK_TCHAR_SIZE_  2
  #define _DALK_WIDECHAR_MODE_
  #pragma message("DefaultEncode:UTF16")
 #else
  #define _DALK_TCHAR_SIZE_  1
  #define _DALK_SYSTEM_LANGUEAGE_	_DALK_CHARCODE_JAPANESE_
  #pragma message("DefaultEncode:ShitfJIS")
 #endif
#elif defined(_DALK_IOS_)
  #define _DALK_TCHAR_SIZE_  1
  #define _DALK_WCHAR_SIZE_  4
  #define _DALK_SYSTEM_LANGUEAGE_	_DALK_CHARCODE_UTF8_
  #pragma message("DefaultEncode:UTF32")
#endif

#ifndef _DALK_SYSTEM_LANGUEAGE_
#define _DALK_SYSTEM_LANGUEAGE_	_DALK_USE_UTF16_
#endif

#pragma message("---------------------------------------")


//ここからプラットフォーム固有

//------------Windows----------------
#if defined(_DALK_WINDOWS_)

#ifndef STRICT
 #define STRICT
#endif

//16bitOSは基本的に（応用的にも）ライブラリサポート外
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS	0x0410	//Win98以降
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0600	//WinVista以降
#endif

#ifndef _WIN32_IE
#define _WIN32_IE		0x0900	//IE9以降
#endif

#ifndef WINVER
#define WINVER			0x0600
#endif

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#ifdef _DALK_RELEASE_BUILD_
#undef _ITERATOR_DEBUG_LEVEL
#define _ITERATOR_DEBUG_LEVEL 0
#endif

#pragma warning(disable:4180)
#pragma warning(disable:4995)
#pragma warning(disable:4996)
#endif

//------------XCodeConfig----------------

#if defined(_DALK_IOS_)
#include "TargetConditionals.h"
#if defined(TARGET_IPHONE)
#elif defined(TARGET_IPHONE_SIMULATOR)
#else
// MacOS
#endif
#endif

//-------------EASTL/デフォルトSTL切り替え---------------

using namespace std;

#ifdef _DALK_USE_EASTL_
//#define EASTL_MINMAX_ENABLED = true
#ifdef _WIN32
#define EA_PLATFORM_WIN32
#else
#define EA_PLATFORM_PS3_PPU
#endif

#include <EASTL/set.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <stack>
#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include <EASTL/algorithm.h>
#include <EASTL/sort.h>
using namespace eastl;
#define stack std::stack
#define bad_alloc std::bad_alloc
#define length_error std::length_error
#define bad_exception std::bad_exception
#define invalid_argument std::invalid_argument
#define domain_error std::domain_error
#define logic_error std::logic_error
#define range_error std::range_error
#define overflow_error std::overflow_error
#define underflow_error std::underflow_error
#define runtime_error std::runtime_error
#define out_of_range std::out_of_range
#else
#pragma warning(push, 3)
#pragma warning(disable:4702) // 制御がわたらないコード警告
#include <set>
#include <list>
#include <vector>
#include <string>
#include <stack>
#include <map>
#ifdef _WIN32
 #include <hash_map>
#endif
#include <algorithm>
#pragma warning(pop)
#endif

//乱数
#include <random>

//---------boost--------------

#define BOOST_DISABLE_ASSERTS

//---------Optional----------- 
//#define _DALK_USE_SQUIRREL_
#define _DALK_USE_OGG_
#define _DALK_USE_WMA_
#define _DALK_USE_TGL_
#define _DALK_USE_PNG_
#define _DALK_USE_DDS_
#define _DALK_USE_LZMA_

//---------Squirrel----------

#if defined(_DALK_USE_SQUIRREL_)

#pragma warning(disable:4138)

#include <stdarg.h>
#include <sqrat.h>

class DALK_Object{
protected:
//static void Bind(HSQUIRRELVM vm);
public:
};

//! 文字列出力関数
static void printfunc(HSQUIRRELVM vm, const SQChar* format, ...) {
    va_list args;
    va_start(args, format);
#ifdef SQUNICODE
    vwprintf(format, args);
#else
    vprintf(format, args);
#endif
    va_end(args);
}

#define SQUIRREL_BEGIN_BIND(_BIND_CLASS_, _BINDNAME_)\
	static void _Bind_##_BIND_CLASS_##(HSQUIRRELVM vm){\
	using namespace Sqrat;\
	const char* bindname = _BINDNAME_; \
	Class<_BIND_CLASS_> ##_BIND_CLASS_##_Binder(vm, bindname);\
	_BIND_CLASS_##_Binder

#define SQUIRREL_END_BIND(_BIND_CLASS_)\
	;\
	RootTable(vm).Bind(_SC(bindname), _BIND_CLASS_##_Binder);\
}\

#define SQUIRREL_MEMBERFUNC_BIND(_DEF, _FUNC)\
	.Func(_SC(_FUNC), &_DEF)\

#define SQUIRREL_MEMBER_BIND( _DEF, _VAR)\
	.Var(_SC(_VAR), &_DEF)\

#endif

#endif