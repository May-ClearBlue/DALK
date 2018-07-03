#ifndef _DALK_STRINGMACRO_
#define _DALK_STRINGMACRO_

#include "DalkTypeDef.h"

/*
BOMは

UTF-8						EF BB BF
UTF-16	ビッグエンディアン	FE FF
		リトルエンディアン	FF FE
UTF-32	ビッグエンディアン	00 00 FE FF
		リトルエンディアン	FF FE 00 00

無い時はビッグエンディアン
*/

//wchat_tはWinが2byte（UTF16）、Linux（Apple含む）が4byte（UTF32）
/*
const char*     string1 = u8"UTF-8 文字列";
const char16_t* string2 = u"UTF-16 文字列";
const char32_t* string3 = U"UTF-32 文字列";
このように、文字列リテラルの先頭に文字コードを意味する記号を付けます。
記号	データ型	用途
u8	char	UTF-8 を表す文字列リテラル
u	char16_t	UTF-16 を表す文字列リテラル
U	char32_t	UTF-32 を表す文字列リテラル
*/

//#include <unicode/uchar.h>
#include <string.h>

#if defined(_DALK_WIDECHAR_MODE_)
  #define _FSTREAM wfstream
  #define _STRCPY wcscpy
  #define _STRNCPY wcsncpy
  #define _STRLEN wcslen
  #define _STRCMP wcscmp
  #define _FOPEN(fp, NAME,flag)  _wfopen(NAME,flag)
  #define _SPLITPATH _wsplitpath_s
  #define _COUT	std::wcout
#else
  #define _FSTREAM fstream
  #define _COUT	std::cout
 #ifdef _DALK_WINDOWS_
  #define _STRCPY strcpy_s
  #define _STRNCPY strncpy_s
  #define _STRLEN strnlen
 #define _STRCMP  strcmp
  #define _FOPEN(fp, NAME,flag) fopen_s(&fp, NAME, flag)
  #define _SPLITPATH _splitpath_s
 #else
  #define _STRCPY strcpy
  #define _STRNCPY strncpy
  #define _STRLEN strlen
  #define _STRCMP strcmp
  #define _FOPEN(fp, NAME, flag) fp=fopen(NAME, flag)
  #define _SPLITPATH _splitpath
 #endif
#endif

#if defined(_DALK_WINDOWS_)
#define _GETSTRING(a) a.data()
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> >			string_t;
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >	string_u16;
typedef std::basic_string<char32_t, std::char_traits<char32_t>, std::allocator<char32_t> >string_u32;
typedef string_t _STRING;
#define _T16(x)    L ## x
#define _TEXT16(x) L ## x
//#define _T32(x)    U ## x
//#define _TEXT32(x) U ## x

#elif defined(_DALK_IOS_)
#include "DalkString_iOS.h"
#define _GETSTRING(a) a.GetNSString()
typedef std::basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> >			string_t;
typedef std::basic_string<char16_t, char_traits<char16_t>, allocator<char16_t> >string_u16;
typedef std::basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >	string_u32;
#ifndef WCHAR
#define WCHAR    wchar_t
#endif
#define _T(x)     x
#define _TEXT(x) @x 
#define _T16(x)    u ## x
#define _TEXT16(x) u ## x
#define _T32(x)    U ## x
#define _TEXT32(x) U ## x
#endif
 
//ToDo:ICUに統一したいが、iPhone（XCodeでのユニバーサルビルド）についての資料がほとんどないので保留。
#if 0
#include <unicode/unistr.h>
#pragma comment(lib,"icudt.lib")
#pragma comment(lib,"icuucd.lib")
#endif

#if(DALK_LOCAL_LANGUEAGE == _DALK_USE_JAPANESE_)
#include <DalkLocalizer_JP.h>
#endif
 
namespace DalkString {
#if 0
inline string_t stringTostring_t(const std::string& value) {
#ifdef 	_DALK_SHIFTJIS_NATIVE_
	return value;
#else

#if(DALK_LOCAL_LANGUEAGE == _DALK_USE_JAPANESE_)
	icu::UnicodeString src(value.c_str(), "ibm-943_P15A-2003"/*"shift_jis"*/);
#endif
	int length = 0;

	if(sizeof(wchar_t) == 2)
		length = src.extract(0, src.length(), NULL, "utf16");
	else if(sizeof(wchar_t) == 2)
		length = src.extract(0, src.length(), NULL, "utf32");

	std::vector<wchar_t> result(length + 1);
    src.extract(0, src.length(), &result[0], 0);

    return string_t(result.begin(), result.end() - 1);
#endif
}
#endif


//ToDo:正規のコードかどうかチェック
inline DWORD UTF16toUTF32(const WORD C) {
	const WORD HI_SURROGATE_START = 0xD800;
	WORD X = C;
	DWORD U = ((DWORD)C >> 16) & ((1 << 5) - 1);
	WORD W = (WORD)U - 1;
	WORD HiSurrogate = HI_SURROGATE_START | (W << 6) | X >> 10;

	const WORD LO_SURROGATE_START = 0xDC00;
	WORD LoSurrogate = (WORD) (LO_SURROGATE_START | X & ((1 << 10) - 1));

	DWORD X2 = (HiSurrogate & ((1 << 6) -1)) << 10 | LoSurrogate & ((1 << 10) -1);
	DWORD W2 = (HiSurrogate >> 6) & ((1 << 5) - 1);
	DWORD U2 = W2 + 1;
	DWORD RES = U2 << 16 | X2;

	return RES;
}

//http://www.ibm.com/developerworks/jp/java/library/j-u-encode.html
//ToDo:正規のコードかどうかチェック

inline WORD UTF32toUTF16(const DWORD Dest) {
	if (Dest <= 65535)
		return (WORD)Dest;
	else if (Dest <= 1114111) {
		WORD t[2] = { (WORD)( ((Dest - 65536) / 1024) + 55296 ),(WORD)((Dest % 1024) + 56320) };
		return t[0] | t[1];
	}
	else
	    return 0xFFFF;
}

#if 0
//http://yanchde.gozaru.jp/utf8_to_utf16/utf8_to_utf16_2.html
//UTF8は１文字が可変サイズなのでポインタを受け取る（参照で自動的に進める。引数をシンプルにしたいので）

inline WORD UTF8ToUTF16(char*& src, size_t src_size/* = 6*/) {
//    const int nMaxReadSize = 6;
    int       iCh1;
    int       sizeBytes;
    wchar_t   wcWork1, wcWork2, wcWork3;
    
	WORD	result;

	if (src == NULL)
        return 0xFFFF;
//    if (src_size < 1)
//        return FALSE;
    
    char* chBuffer = src;

        /* data size の調べる */
        iCh1 = ((int)(*chBuffer)) & 0x00ff;
        iCh1 = ~iCh1;    /* ビット反転 */
        if (iCh1 & 0x0080) {
            /* 0aaabbbb */
            sizeBytes = 1;
        }
        else if (iCh1 & 0x0040) {
            /* error(ここに出現してはいけないコード) */
            return 0xFFFF;
        }
        else if (iCh1 & 0x0020) {
            /* 110aaabb 10bbcccc */
            sizeBytes = 2;
        }
        else if (iCh1 & 0x0010) {
            /* 1110aaaa 10bbbbcc 10ccdddd */
            sizeBytes = 3;
        }
        else if (iCh1 & 0x0008) {
            /* 未対応のマッピング(UTF-16に存在しないコード) */
            sizeBytes = 4;
        }
        else if (iCh1 & 0x0004) {
            /* 未対応のマッピング(UTF-16に存在しないコード) */
            sizeBytes = 5;
        }
        else if (iCh1 & 0x0002) {
            /* 未対応のマッピング(UTF-16に存在しないコード) */
            sizeBytes = 6;
        }
        else {
            /* error(ここに出現してはいけないコード) */
            return 0xFFFF;
        }
      
        /* sizeBytes毎に処理を分岐 */
        switch (sizeBytes) {
        case 1:
            /*
             * ビット列
             * (0aaabbbb)UTF-8 ... (00000000 0aaabbbb)UTF-16
             */
            result = ((wchar_t)(chBuffer[0])) & (wchar_t)0x00ff;     /* 00000000 0aaabbbb */
            return result;
            break;
        case 2:
            /*
             * ビット列
             * (110aaabb 10bbcccc)UTF-8 ... (00000aaa bbbbcccc)UTF-16
             */
            wcWork1 = ((wchar_t)(chBuffer[0])) & (wchar_t)0x00ff;   /* 00000000 110aaabb */
            wcWork2 = ((wchar_t)(chBuffer[1])) & (wchar_t)0x00ff;   /* 00000000 10bbcccc */
            wcWork1 <<= 6;                                          /* 00110aaa bb?????? */
            wcWork1 &= 0x07c0;                                      /* 00000aaa bb000000 */
            wcWork2 &= 0x003f;                                      /* 00000000 00bbcccc */
            result = wcWork1 | wcWork2;                              /* 00000aaa bbbbcccc */
            return result;
            break;
        case 3:
            /*
             * ビット列
             * (1110aaaa 10bbbbcc 10ccdddd)UTF-8 ... (aaaabbbb ccccdddd)UTF-16
             */
            wcWork1 = ((wchar_t)(chBuffer[0])) & (wchar_t)0x00ff;   /* 00000000 1110aaaa */
            wcWork2 = ((wchar_t)(chBuffer[1])) & (wchar_t)0x00ff;   /* 00000000 10bbbbcc */
            wcWork3 = ((wchar_t)(chBuffer[2])) & (wchar_t)0x00ff;   /* 00000000 10ccdddd */
            wcWork1 <<= 12;                                         /* aaaa???? ???????? */
            wcWork1 &= 0xf000;                                      /* aaaa0000 00000000 */
            wcWork2 <<= 6;                                          /* 0010bbbb cc?????? */
            wcWork2 &= 0x0fc0;                                      /* 0000bbbb cc000000 */
            wcWork3 &= 0x003f;                                      /* 00000000 00ccdddd */
            result = wcWork1 | wcWork2 | wcWork3;                    /* aaaabbbb ccccdddd */
            return result;
            break;
        case 4:
        case 5:
        case 6:
        default:
            /* ダミーデータ(uff1f)を出力 */
			result = 0xff1f;
            break;
        }

	if(sizeBytes >= 6 && sizeBytes <= 1)
		src += sizeBytes;

	return result;
}
#endif

#ifdef _DALK_IOS_

inline NSString* EncodefromChar(const char* str) {
    @autoreleasepool {
		return [[NSString alloc] initWithCString:str encoding:NSUTF8StringEncoding];
    }
}

inline NSString* EncodefromWideChar(WCHAR* str) {
    @autoreleasepool {
		return [[NSString alloc] initWithBytes:str length:char_traits<WCHAR>::length(str)*sizeof(WCHAR) encoding:NSUTF16LittleEndianStringEncoding];
	}
}

inline void NSStringToTCHAR(WCHAR* dest, NSString* src) {
	@autoreleasepool {
		WCHAR* temp = (WCHAR*)[src cStringUsingEncoding:NSUTF16LittleEndianStringEncoding];
		memcpy(dest, temp,char_traits<WCHAR>::length(temp)*sizeof(WCHAR));
	}
}

#if 0
    inline void NSStringToString(string_t& dest, NSString* src) {
    @autoreleasepool {
#ifdef _DALK_USE_UNICODE_
		dest = (char16_t*)[src cStringUsingEncoding:NSUTF16LittleEndianStringEncoding];
#else
		dest = [src UTF8String];
#endif

	}
}
#endif
    
    
#endif

//itoa string仕様
void  itoa		(int num, std::string& str,int radix);
char* ToUpper	(char *str);

char* GetChar	(char* src, char* dest);
bool  IncChar	(char*& str);
bool  FindChar	(char* dest, char* src, char* buf = NULL);
int   FindString(std::string& s, char* src);

//一文字を１６進数に変換
int ToHex(char code);

}; //namespace DalkString

#endif
