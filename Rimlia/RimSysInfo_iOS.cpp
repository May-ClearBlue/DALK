#include "MultiStdAfx.h"
#include "RimSysInfo.h"

#import	<mach/mach.h>
#import	<mach/host_info.h>
#import	<mach/mach_init.h>
#import	<mach/mach_host.h>

_STRING	CSystemInfo::m_ModelType;			// 機種
OSTYPE	CSystemInfo::m_OSType;				// OS
float	CSystemInfo::m_OSVersion;
DWORD	CSystemInfo::m_OSMajorVersion;		// メジャーバージョン番号
 DWORD	CSystemInfo::m_OSMinorVersion;		// マイナーバージョン番号
DWORD	CSystemInfo::m_BuildMajorVersion;    // メジャービルド番号＠Winでは使わない
DWORD	CSystemInfo::m_BuildMinorVersion;    // マイナービルド番号＠Winでは使わない
DWORD	CSystemInfo::m_BuildNumber;			// ビルド＠Winではこの値のみ更新される
_STRING	CSystemInfo::m_AppName;
DWORD	CSystemInfo::m_AppMajorVersion;
DWORD	CSystemInfo::m_AppMinorVersion;
uint64_t	CSystemInfo::m_MemSize;
uint64_t	CSystemInfo::m_MemFreeSize;
int		CSystemInfo::m_DisplayWidth;
int		CSystemInfo::m_DisplayHeight;
float	CSystemInfo::m_DisplayScale;
float	CSystemInfo::m_BatteryLevel;

HWND CSpecialPath::m_hWnd;
_STRING CSpecialPath::m_DeskTop;
_STRING	CSpecialPath::m_Home;
_STRING	CSpecialPath::m_SaveData;
_STRING	CSpecialPath::m_ProgramList;

void CSystemInfo::SetOSInfo() {
	m_OSType = ARCH_IOS;

	m_ModelType = _T("iOS");

	m_ModelType = [ [ UIDevice currentDevice] model];

/*
	if([modelname isEqualToString:@"iPhone"])
	else if([modelname isEqualToString:@"iPod touch"])
	else if([modelname isEqualToString:@"iPad"])
	else if([modelname isEqualToString:@"iPhone Simulator"])
*/
	NSArray *versionArray = [ [ [ UIDevice currentDevice]systemVersion]componentsSeparatedByString:@"."];
	m_OSMajorVersion  = [ [ versionArray objectAtIndex:0]intValue];
	m_OSMinorVersion = [ [ versionArray objectAtIndex:1]intValue];

	//NSLog(@"VERSION %d.%d",majorVersion,minorVersion);
}

void CSystemInfo::SetAppInfo() {
	// アプリ情報
	NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
	// ビルドバージョン情報
	NSString *build = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];

	NSArray *AppArray	= [version componentsSeparatedByString:@"."];
	NSArray *BuildArray	= [build componentsSeparatedByString:@"."];

	if([AppArray count] >= 1)
		m_AppMajorVersion  = [ [ AppArray objectAtIndex:0]intValue];
	if([AppArray count] >= 2)
		m_AppMinorVersion = [ [ AppArray objectAtIndex:1]intValue];
	if([BuildArray count] >= 1)
		m_BuildMajorVersion  = [ [ BuildArray objectAtIndex:0]intValue];
	if([BuildArray count] >= 2)
		m_BuildMinorVersion = [ [ BuildArray objectAtIndex:1]intValue];
	if([BuildArray count] >= 3)
		m_BuildNumber = [ [ BuildArray objectAtIndex:2]intValue];

	m_AppName = _T("");
}

void CSystemInfo::SetMemorySize(){
	struct host_basic_info a_host;
    mach_msg_type_number_t a_count = HOST_BASIC_INFO_COUNT;
    host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&a_host, &a_count);
	m_MemSize = a_host.memory_size;
}

void CSystemInfo::SetDisplaySize() {
//転地必要かも
	CGSize result = [[UIScreen mainScreen] bounds].size;
	CGFloat scale = [UIScreen mainScreen].scale;

	m_DisplayScale = scale;

	m_DisplayWidth = result.width * scale;  //幅
	m_DisplayHeight = result.height * scale; //高さ	
}

uint64_t CSystemInfo::GetFreeMemorySize(){
    mach_port_t host_port;
    mach_msg_type_number_t host_size;
    vm_size_t pagesize;

    host_port = mach_host_self();
    host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    host_page_size(host_port, &pagesize);
    vm_statistics_data_t vm_stat;

    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
        return 0;

//    natural_t mem_free = vm_stat.free_count * pagesize;
	m_MemFreeSize = vm_stat.free_count * pagesize;

    return (unsigned int)m_MemFreeSize;
}

float CSystemInfo::GetBatteryLevel() {
	[UIDevice currentDevice].batteryMonitoringEnabled = YES;
	float batteryLevel = [UIDevice currentDevice].batteryLevel;
	return batteryLevel;
}

BOOL CSpecialPath::SetDesktopPath() {
//ToDo
	m_DeskTop = _T("");

	return TRUE;
}

BOOL CSpecialPath::SetHomePath() {
	m_Home = NSHomeDirectory();
	return TRUE;
}

BOOL CSpecialPath::SetSaveDataPath() {
//iOSは書き込める場所が基本的にここだけ
//シミュレータの場所は	/Users/(ユーザ名)/Library/Application Support/iPhone Simulator/(シミュレータのバージョン)/Applications/(アプリケーションごとのGUID)/Documents

	NSArray* paths;
#if 0
    NSString* path;
#endif
//	paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);

	if ([paths count] < 1)
		return FALSE;

	m_SaveData = [paths objectAtIndex:0];

	return TRUE;
}

BOOL CSpecialPath::SetProgramListPath(){
//ないはず、もしくはApplicationへ
	m_ProgramList = _T("");
	return TRUE;
}

//NSString* a_tmp_dir = NSTemporaryDirectory();
