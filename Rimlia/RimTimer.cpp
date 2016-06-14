#include "MultiStdafx.h"

#include "RimTimer.h"

_TIMERINFO	CTimer::m_Frequency;

CTimer::~CTimer() {
}

BOOL CTimer::Initilaize() {
	BOOL res;
#if defined(_DALK_WINDOWS_)
 #ifdef _ENABLE_PERFORMANCE_COUNTER_
	res = ::QueryPerformanceFrequency(&m_Frequency);

	if(res) {
		TCHAR ReportMes[256];
		wsprintf(ReportMes,_T("タイマデバイスの初期化　分解能＝%dms") ,m_Frequency.QuadPart);
		::OutputDebugString(ReportMes);
	}
	else
		::OutputDebugString(_T( "タイマデバイス初期化に失敗\n"));

 #else
	TIMECAPS TimeCaps;

	if ( ::timeGetDevCaps( &TimeCaps,sizeof(TIMECAPS) ) != TIMERR_NOERROR || ::timeBeginPeriod(TimeCaps.wPeriodMin) != TIMERR_NOERROR) {
		::OutputDebugString(_T( "タイマデバイス初期化に失敗\n"));
	}
	else {
		TCHAR ReportMes[256];
		m_Frequency = TimeCaps.wPeriodMin;
		wsprintf(ReportMes,_T("タイマデバイスの初期化　分解能＝%dms") ,m_Frequency);
		::OutputDebugString(ReportMes);
	}
 #endif
#elif defined(_DALK_IOS_)
	mach_timebase_info(&m_Frequency);
	res = TRUE;
#else
	res = clock_getres(CLOCK_MONOTONIC, &m_Frequency) == 0 ? TRUE : FALSE;
#endif
	return res;
}

void CTimer::Uninitialize() {
#if defined(_DALK_WINDOWS_) 
 #ifndef _ENABLE_PERFORMANCE_COUNTER_
	if(m_Frequency != 0xFFFFFFFF)
		::timeEndPeriod(m_Frequency);
#endif
#endif
}

#if 0
CTimer::CTimer() :
m_LastTime(0)
{
}

DWORD CTimer::CountTime() {
	DWORD LastTime	= GetTime();
	DWORD res		= LastTime - m_LastTime;
	m_LastTime		= LastTime;
	return res;
}
#endif
