#include "MultiStdAfx.h"

#include "RimSysInfo.h"

#include <shlwapi.h>
#include <shlobj.h>

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
uint64_t CSystemInfo::m_MemSize;
uint64_t CSystemInfo::m_MemFreeSize;
int		CSystemInfo::m_DisplayWidth;
int		CSystemInfo::m_DisplayHeight;
float	CSystemInfo::m_DisplayScale;
float	CSystemInfo::m_BatteryLevel;

HWND	CSpecialPath::m_hWnd;
_STRING	CSpecialPath::m_DeskTop;
_STRING	CSpecialPath::m_Home;
_STRING	CSpecialPath::m_SaveData;
_STRING	CSpecialPath::m_ProgramList;

int GetUnitNum(LPCTSTR drive){
	int res = -1;
	TCHAR temp[4] = _T("a:\\");
	
	if(drive[0] <= 'a' && drive[0] >= 'z')
		temp[0] = 'a';
	else if(drive[0] <= 'A' && drive[0] <= 'Z')
		temp[0] = 'A';
		
	for(int a = 0;a < drive[0] - temp[0] + 1;a ++){
		if(::GetDriveType(temp) == DRIVE_CDROM)
			res ++;
		temp[0] ++;
	}
	return res;
}

//ToDo:UINICODE対応
#if 0
BOOL CheckMedia(unsigned int number,char *name){
	char volumelabel[512] = "";
	DWORD a = 0,b = 0;

	UINT errormode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	BOOL res = ::GetVolumeInformation(DRIVELETTER[number],volumelabel,512,0,0,0,/*NULL,&a,&b,*/NULL,0);
	::SetErrorMode(errormode);

	if(name && strcmp(volumelabel,name) == 0)
		return TRUE;
	return FALSE;
}
#endif

// NTにおけるSE_SHUTDOWN_NAME特権(シャットダウン等に必要な権限)の取得
int Get_SE_SHUTDOWN_NAME_Privileges(){
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;

	if(!::OpenProcessToken(::GetCurrentProcess(),TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,&hToken))
		return -1;

	::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tp.Privileges[0].Luid);
	
	tp.PrivilegeCount=1;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	
	::AdjustTokenPrivileges(hToken,FALSE,&tp,0,NULL,0);

	return ::GetLastError() != ERROR_SUCCESS;
}

void ShutdownSystem(int type){
	//int os = GetOSVersion();

	//if((os == WIN_NT || os == WIN_2K)&&(Get_SE_SHUTDOWN_NAME_Privileges()))
		return ;
	
	switch (type){
	case 0:
		::ExitWindowsEx(EWX_LOGOFF,0);
		break;
	case 1:
		::ExitWindowsEx(EWX_POWEROFF,0);
		break;
	case 2:
		::ExitWindowsEx(EWX_REBOOT,0);
		break;
	}
	return;
}			
// 誤差があるそうです
float	GetCpuClock(BOOL IsTsc)
{
#ifndef  _WIN64
	DWORD dwCpuClock;
	DWORD dwMsCount;

	if (IsTsc == 0)
		return 0;
	__asm{
		rdtsc
		mov [dwCpuClock], eax
	}

	dwMsCount = ::timeGetTime();
	while(::timeGetTime() - dwMsCount < 500);

	__asm{
		rdtsc
		sub eax, [dwCpuClock]
		mov [dwCpuClock], eax
	}
	return (float)(dwCpuClock/500000.0);
#else
	return 0.0f;
#endif
}


// ニューバージョン、いらない情報満載
void GetCpuInfo(CPUINFO *cpuinfo)
{
#ifndef _WIN64
	TCHAR	szVenderID[13];
	TCHAR	szCpuName[49];
	float	CpuClock;
	DWORD	CPUIDMax, CPUIDExtMax;
	DWORD	TypeID, FamilyID, ModelID, SteppingID;
	DWORD	hasCPUID, hasFPU, hasTSC, hasMSR, hasCMOV, hasMMX, has3DNow;
	DWORD	hasSSE_INT, hasSSE_FP, has3DNowExt;
	
	lstrcpy(cpuinfo->szVenderID, _T(""));
	lstrcpy(cpuinfo->szCpuName, _T(""));
	memset(cpuinfo, sizeof(CPUINFO), 0);

	__asm{
		//Check CPUID
		pushfd
		pop eax
		mov ebx, eax
		xor eax, 1<<21
		push eax
		popfd
		pushfd
		pop eax
		cmp eax, ebx
		je exitasm
		mov [hasCPUID], 1

		//CPUID 0
		xor eax, eax
		cpuid
		mov [CPUIDMax], eax
		mov DWORD PTR [szVenderID+0], ebx
		mov DWORD PTR [szVenderID+4], edx
		mov DWORD PTR [szVenderID+8], ecx
		mov BYTE PTR [szVenderID+12], 0

		//CPUID 1
		cmp eax, 1
		jb exitasm
		mov eax, 1
		cpuid

		mov esi, eax
		and esi, 0xf
		mov [SteppingID], esi

		shr eax, 4
		mov esi, eax
		and esi, 0xf
		mov [ModelID], esi

		shr eax, 4
		mov esi, eax
		and esi, 0xf
		mov [FamilyID], esi

		shr eax, 4
		mov esi, eax
		and esi, 0x3
		mov [TypeID], esi

		xor eax, eax
		test edx, 1<<0
		setnz al
		mov [hasFPU], eax

		test edx, 1<<4
		setnz al
		mov [hasTSC], eax

		test edx, 1<<5
		setnz al
		mov [hasMSR], eax

		test edx, 1<<15
		setnz al
		mov [hasCMOV], eax

		test edx, 1<<23
		setnz al
		mov [hasMMX], eax

		test edx, 1<<25
		setnz al
		mov [hasSSE_INT], eax
		mov [hasSSE_FP], eax

		//CPUID 0x80000000
		mov eax, 0x80000000
		cpuid
		mov [CPUIDExtMax], eax

		//CPUID 0x80000001
		cmp eax, 0x80000001
		jb exitasm
		mov eax, 0x80000001
		cpuid

		xor eax, eax
		test edx, 1<<22
		setnz al
		mov [hasSSE_INT], eax

		test edx, 1<<30
		setnz al
		mov [has3DNowExt], eax

		test edx, 1<<31
		setnz al
		mov [has3DNow], eax

		//CPUID 0x80000002-4
		cmp [CPUIDExtMax], 0x80000004
		jb exitasm

		mov eax, 0x80000002
		cpuid
		mov DWORD PTR [szCpuName+ 0], eax
		mov DWORD PTR [szCpuName+ 4], ebx
		mov DWORD PTR [szCpuName+ 8], ecx
		mov DWORD PTR [szCpuName+12], edx

		mov eax, 0x80000003
		cpuid
		mov DWORD PTR [szCpuName+16], eax
		mov DWORD PTR [szCpuName+20], ebx
		mov DWORD PTR [szCpuName+24], ecx
		mov DWORD PTR [szCpuName+28], edx

		mov eax, 0x80000004
		cpuid
		mov DWORD PTR [szCpuName+32], eax
		mov DWORD PTR [szCpuName+36], ebx
		mov DWORD PTR [szCpuName+40], ecx
		mov DWORD PTR [szCpuName+44], edx
	exitasm:
	}
	// クロック計る
	CpuClock = GetCpuClock(hasTSC);

	// 構造体にコピー
	lstrcpy(cpuinfo->szVenderID, szVenderID);
	lstrcpy(cpuinfo->szCpuName, szCpuName);
	cpuinfo->CPUIDMax = CPUIDMax;
	cpuinfo->CPUIDExtMax = CPUIDExtMax;
	cpuinfo->TypeID = TypeID;
	cpuinfo->FamilyID = FamilyID;
	cpuinfo->ModelID = ModelID;
	cpuinfo->SteppingID = SteppingID;
	cpuinfo->hasCPUID = hasCPUID;
	cpuinfo->hasFPU = hasFPU;
	cpuinfo->hasMSR = hasMSR;
	cpuinfo->hasTSC = hasTSC;
	cpuinfo->hasCMOV = hasCMOV;
	cpuinfo->hasMMX = hasMMX;
	cpuinfo->has3DNow = has3DNow;
	cpuinfo->hasSSE_INT = hasSSE_INT;
	cpuinfo->hasSSE_FP = hasSSE_FP;
	cpuinfo->has3DNowExt = has3DNowExt;
	cpuinfo->CpuClock = CpuClock;
#endif
}

void CSystemInfo::SetOSInfo() {
	OSVERSIONINFO OSInfo;
	
	OSInfo.dwOSVersionInfoSize = sizeof(OSInfo);

	m_OSType = ARCH_WINDOWS;

	m_ModelType = _T("Windows");

	if( ::GetVersionEx(&OSInfo) == FALSE )
		return;
	
//95=4.0 98=4.10 Me=4.90
	if(OSInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS){
		if(OSInfo.dwMajorVersion == 4){
			if(OSInfo.dwMinorVersion == 0)
				m_ModelType = _T("Windows95");
			else if(OSInfo.dwMinorVersion == 1)
				m_ModelType = _T("Windows98");
			else
				m_ModelType = _T("WindowsMe");
		}
	}
//NT3系は無視 2000=5.0 XP=5.1 Server2003=5.2
//2003R2とXP64bitも5.2なのはとりあえずスルーで。OSVERSIONINFOEXを使う必要があり
	else if(OSInfo.dwPlatformId == VER_PLATFORM_WIN32_NT){
		if(OSInfo.dwMajorVersion == 4)
			m_ModelType = _T("WindowsNT");
		else if(OSInfo.dwMajorVersion == 5) {
			if(OSInfo.dwMinorVersion == 0)
				m_ModelType = _T("Windows2000");
			else if(OSInfo.dwMinorVersion == 1)
				m_ModelType = _T("WindowsXP");
			else if(OSInfo.dwMinorVersion == 2)
				m_ModelType = _T("WindowsServer2003");
		}
//Vista=6.0 7=6.1 8=6.2
//↓サーバ系はとりあえずスルーで。
//Server2008=6.0 Server2008R2=6.1 Server2012=6.2
		else if(OSInfo.dwMajorVersion == 6) {
			if(OSInfo.dwMinorVersion == 0)
				m_ModelType = _T("WindowsVista");
			else if(OSInfo.dwMinorVersion == 1)
				m_ModelType = _T("Windows7");
			else if(OSInfo.dwMinorVersion == 2)
				m_ModelType = _T("Windows8");
		}
	}

	m_OSMajorVersion = OSInfo.dwMajorVersion;
	m_OSMinorVersion = OSInfo.dwMinorVersion;
	m_BuildMajorVersion = 1;
	m_BuildMinorVersion = 1;
	m_BuildNumber = OSInfo.dwBuildNumber;
//	sprintf( m_OSName, "%ls %ls %d.%d.%d ", OSName, m_OSInfo.szCSDVersion, m_OSInfo.dwMajorVersion,	m_OSInfo.dwMinorVersion, m_OSInfo.dwBuildNumber&0xfff );
}

void CSystemInfo::SetAppInfo() {
//ToDo
	m_AppName = _T("");
	m_AppMajorVersion = 1;
	m_AppMinorVersion = 0;
}

//9x系は失敗しますあきらめてください
void CSystemInfo::SetMemorySize(){
	MEMORYSTATUSEX memorystatus = { 0 };
	memorystatus.dwLength = sizeof(memorystatus);
	if(::GlobalMemoryStatusEx(&memorystatus)) {
		m_MemSize = memorystatus.ullTotalPhys;// >> 10;
		m_MemFreeSize = memorystatus.ullAvailPhys;// >> 10;
	}
	else {
		m_MemSize = 0;
		m_MemFreeSize = 0;
	}
}

void CSystemInfo::SetDisplaySize() {
//スクリーンのサイズを取得
	m_DisplayScale = 1.0f;	//使わないはず

	m_DisplayWidth = ::GetSystemMetrics(SM_CXSCREEN);
	m_DisplayHeight = ::GetSystemMetrics(SM_CYSCREEN);
}

uint64_t CSystemInfo::GetFreeMemorySize(){
	MEMORYSTATUSEX memorystatus = { 0 }; 
	memorystatus.dwLength = sizeof(memorystatus);
	if(::GlobalMemoryStatusEx(&memorystatus) == FALSE)
		return 0;

	m_MemFreeSize = memorystatus.ullAvailPhys;// >> 10;
	return m_MemFreeSize;
}

float CSystemInfo::GetBatteryLevel() {
	m_BatteryLevel = 1.0f;
	return 1.0f;
}
	
BOOL CSpecialPath::SetDesktopPath() {
	TCHAR buf[MAX_PATH];
	if(SHGetSpecialFolderPath(m_hWnd, buf, CSIDL_DESKTOPDIRECTORY, FALSE) == FALSE)
		return FALSE;

	m_DeskTop = buf;

	return TRUE;
}

BOOL CSpecialPath::SetHomePath() {
//	void	GetDesktopRect() { ::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_Desktop, 0); }
//	::GetCurrentDirectory(256,m_CurrentPath);

	TCHAR buf[256];
	::GetModuleFileName(NULL, buf, 256);

	m_Home = buf;

	return TRUE;
}

BOOL CSpecialPath::SetSaveDataPath() {
//自由設定
	m_SaveData = _T("");
	return TRUE;
}

BOOL CSpecialPath::SetProgramListPath(){
	TCHAR buf[MAX_PATH];

	if(SHGetSpecialFolderPath(m_hWnd, buf, CSIDL_PROGRAMS,FALSE) == FALSE)
		return FALSE;

	m_ProgramList = buf;
	return TRUE;
}
