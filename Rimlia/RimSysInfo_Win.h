#ifndef _RIM_SYSTEMINFO_WIN_
#define _RIM_SYSTEMINFO_WIN_

#include "DalkStdio.h"

struct CPUINFO {
	TCHAR	szVenderID[13];		// ベンダー名
	TCHAR	szCpuName[49];		// ＣＰＵ名
	DWORD	CPUIDMax;			// CPUID標準機能のサポート最大値
	DWORD	CPUIDExtMax;		// CPUID拡張機能のサポート最大値
	DWORD	TypeID;				// CPUTYPE
	DWORD	FamilyID;			// CPUFAMILY
	DWORD	ModelID;			// CPUMODEL
	DWORD	SteppingID;			// CPUSTEPPING
	BOOL	hasCPUID;			// CPUIDサポートの有無
	BOOL	hasFPU;				// 浮動小数点演算器の有無
	BOOL	hasTSC;				// RDTSC命令の有無
	BOOL	hasMSR;				// RDMSR,WRMSR命令の有無
	BOOL	hasCMOV;			// CMOV命令の有無
	BOOL	hasMMX;				// MMX命令の有無
	BOOL	has3DNow;			// 3DNow!命令の有無
	BOOL	hasSSE_INT;			// SSE integer(=MMX拡張)命令の有無
	BOOL	hasSSE_FP;			// SSE floating-point命令の有無
	BOOL	has3DNowExt;		// 3DNow!拡張命令の有無
	float	CpuClock;			// クロック数
};

//余談	FCMOV命令の有無の確認		hasFPU & hasCMOV

int		GetUnitNum(LPCTSTR drive);
//BOOL	CheckMedia(unsigned int number,char *name = NULL);
int		Get_SE_SHUTDOWN_NAME_Privileges();
void	ShutdownSystem(int type = 0);
float	GetCpuClock(BOOL IsTsc);
void	GetCpuInfo(CPUINFO *cpuinfo);

#endif
