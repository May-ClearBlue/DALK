#ifndef	_SAIL_SYSINFO_
#define	_SAIL_SYSINFO_

#include "DalkStdio.h"

#if defined(_DALK_WINDOWS_)
#include "RimSysInfo_Win.h"
#elif defined(_DALK_IOS_)
/*
NSString* str = "AAA";
char* cp = [str UTF8String];

なのでchar*でデータを渡す時は

(char *) [str UTF8String]

などとする。


char*->NSString*への変換

char* cp = "AAA";
NSString* str = [NSString stringWithCString: cp encoding:NSUTF8StringEncoding];
*/


/*
//OUTPUT:
iPhone 3    480x320 460x320
iPhone 4    960x640
iPhone 5   1136x640
iPad        1024x768 1024x768
iPad3       2048x1536
*/

/*
//INPUT:
960 * 540
1024 * 576
1136 * 640
1280 * 720
*/
#endif

enum OSTYPE {
	ARCH_WINDOWS,
	ARCH_IOS,
	ARCH_ANDROID,
	ARCH_PS3,
	ARCH_UNKOWN
};

class CSystemInfo {
protected:
static	_STRING	m_ModelType;			// 機種
static	OSTYPE	m_OSType;				// OS
static	float	m_OSVersion;
static	DWORD	m_OSMajorVersion;		// メジャーバージョン番号
static  DWORD	m_OSMinorVersion;		// マイナーバージョン番号
static	DWORD	m_BuildMajorVersion;    // メジャービルド番号＠Winでは使わない
static  DWORD	m_BuildMinorVersion;    // マイナービルド番号＠Winでは使わない
static	DWORD	m_BuildNumber;			// ビルド＠Winではこの値のみ更新される
static	_STRING	m_AppName;
static	DWORD	m_AppMajorVersion;
static	DWORD	m_AppMinorVersion;

static	uint64_t	m_MemSize;
static	uint64_t	m_MemFreeSize;

static	int		m_DisplayWidth;
static	int		m_DisplayHeight;

//UITouchにはScaleはかからない
static	float	m_DisplayScale;

static	float	m_BatteryLevel;
	
//Fix:最初に一度だけ呼ばれる（基本に変わらないプロパティ）
static	void	SetOSInfo();
static	void	SetAppInfo();
static	void	SetMemorySize();
static	void	SetDisplaySize();

public:

static	inline	LPCTSTR			GetModelType()	{ return m_ModelType.data(); }
static	inline	const OSTYPE	GetOSType()		{ return m_OSType; }
static	inline	const float		GetOSVersion()	{ return m_OSVersion; }	//未実装
static	inline	void			GetOSVersion(DWORD& maj, DWORD& _min )				{ maj = m_OSMajorVersion; _min = m_OSMinorVersion; }
static	inline	void			GetBuildInfo(DWORD& maj, DWORD& _min, DWORD& num)	{ maj = m_BuildMajorVersion; _min = m_BuildMinorVersion; num = m_BuildNumber; }
static	inline	LPCTSTR			GetAppName()	{ return m_AppName.data(); }
static	inline	void			GetAppVersion(DWORD& maj, DWORD& _min )				{ maj = m_AppMajorVersion; _min = m_AppMinorVersion; }
static	inline	const int		GetDisplayWidth()		{ return m_DisplayWidth; }		//inlineがかかりやすいようにこの２つは分ける
static	inline	const int		GetDisplayHeight()		{ return m_DisplayHeight; }
static	inline	const uint64_t	GetTotalMemorySize()	{ return m_MemSize; } 
	//リアルタイムで変わるステート
static	uint64_t	GetFreeMemorySize();
static	float		GetBatteryLevel();

static	void	Init() {
		SetOSInfo();
		SetAppInfo();
		SetMemorySize();
		SetDisplaySize();
		GetFreeMemorySize();
		GetBatteryLevel();
	}
};

class CSpecialPath {
protected:
static	HWND m_hWnd;

static	_STRING	m_DeskTop;
static	_STRING	m_Home;
static	_STRING	m_SaveData;
static	_STRING	m_ProgramList;
static	BOOL	SetDesktopPath();
static	BOOL	SetHomePath();
static	BOOL	SetSaveDataPath();
static	BOOL	SetProgramListPath();

public:
static	inline LPCTSTR	GetDesktopPath()	{ return m_DeskTop.data(); }
static	inline LPCTSTR	GetHomePath()		{ return m_Home.data(); }
static	inline LPCTSTR	GetSaveDataPath()	{ return m_SaveData.data(); }
static	inline LPCTSTR	GetProgramListPath(){ return m_ProgramList.data(); }

static	void	Init() {
		SetDesktopPath();
		SetHomePath();
		SetSaveDataPath();
		SetProgramListPath();
	}
};

#endif
