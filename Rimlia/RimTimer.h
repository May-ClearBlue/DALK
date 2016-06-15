#ifndef _RIM_TIMER_
#define _RIM_TIMER_

#include "MultiStdafx.h"

#define _ENABLE_PERFORMANCE_COUNTER_

#if defined(_DALK_WINDOWS_)
 #include <mmsystem.h>
 #pragma comment(lib, "WinMM.lib")
#elif defined(_DALK_IOS_)
 #include <mach/mach_time.h>
#else
 #include <sys/sys_time.h>
#endif

#if defined(_DALK_WINDOWS_)
 #ifndef _ENABLE_PERFORMANCE_COUNTER_
  #define GetFrequencyCount(a) (a = ::timeGetTime())
  typedef DWORD _TIMERCOUNT;
  typedef DWORD _TIMERINFO;
 #else
 #define GetFrequencyCount(a) ::QueryPerformanceCounter(&a)
  typedef LARGE_INTEGER _TIMERCOUNT;
  typedef LARGE_INTEGER _TIMERINFO;
 #endif

#elif defined(_DALK_IOS_)
 #define GetFrequencyCount(a)	(a = mach_absolute_time())
 typedef uint64_t					_TIMERCOUNT;
 typedef mach_timebase_info_data_t	_TIMERINFO;
#else
 #define GetFrequencyCount(a)	clock_gettime(CLOCK_MONOTONIC, &a)
 typedef timespec _TIMERCOUNT;
 typedef timespec _TIMERINFO;

#endif
 
class CTimer{
public:
//	CTimer();
	~CTimer();

protected:
static	_TIMERINFO	m_Frequency;

public:
static	BOOL	Initilaize();
static	void	Uninitialize();
static	inline	void	GetCountTimer(_TIMERCOUNT& count)					{ GetFrequencyCount(count); }
#if defined(_DALK_WINDOWS_)
static	inline	float	GetSecondsPerTick()									{ return 1.0f / (float)m_Frequency.QuadPart; }
static	inline	__int64	GetSecond	(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin.QuadPart - end.QuadPart) / m_Frequency.QuadPart; }
static	inline	__int64	GetMilliSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin.QuadPart - end.QuadPart) * 1000 / m_Frequency.QuadPart; }
static	inline	__int64	GetMicroSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin.QuadPart - end.QuadPart) * 1000000/ m_Frequency.QuadPart; }
static	inline	__int64	GetNanoSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin.QuadPart - end.QuadPart) * 1000000000/ m_Frequency.QuadPart; }
#elif defined(_DALK_IOS_)
static	inline	float	GetSecondsPerTick()									{ return (float)m_Frequency.numer / (float)m_Frequency.denom / 1000000000.0f; }
static	inline	__int64	GetSecond	(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) * m_Frequency.numer / m_Frequency.denom / 1000000000; }
static	inline	__int64	GetMilliSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) * m_Frequency.numer / m_Frequency.denom / 1000000; }
static	inline	__int64	GetMicroSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) * m_Frequency.numer / m_Frequency.denom / 1000; }
static	inline	__int64	GetNanoSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) * m_Frequency.numer / m_Frequency.denom; }
#else
static	inline	float	GetSecondsPerTick()									{ return 1.0f / m_Frequency.tv_nsec; }
static	inline	__int64	GetSecond	(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) / m_Frequency.tv_nsec;  }
static	inline	__int64	GetMilliSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) * 1000 / m_Frequency.tv_nsec; }
static	inline	__int64	GetMicroSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) * 1000000 / m_Frequency.tv_nsec; }
static	inline	__int64	GetNanoSecond(_TIMERCOUNT begin, _TIMERCOUNT end)	{ return (begin - end) * 1000000000 / m_Frequency.tv_nsec; }
#endif
};

#if 0
		_TIME	m_LastTime;
inline	_TIME	GetTime()		{ return _GETTIME_(); }
inline	void	ResetCounter()	{ m_LastTime = GetTime(); }
inline	_TIME	ElapsedTime()	{ return GetTime() - m_LastTime; }
		_TIME	CountTime();

class CTimeCounter {
protected:
		DWORD	m_BeginTime;
		DWORD	m_CurrentTime;
		DWORD	m_EndTime;
		float	m_TimeProgressRate;
		bool	bEnableTimeCounter;
#if defined(_DALK_WINDOWS_)
inline	DWORD	GetCurrentTime() { return ::timeGetTime(); }
#else
inline	system_time_t	GetCurrentTime()	{ return sys_time_get_system_time(); }
#endif

public:
	CTimeCounter() {
		m_BeginTime = 0;
		m_CurrentTime= 0;
		m_EndTime = 0;
		m_TimeProgressRate = 0.0f;
		bEnableTimeCounter = false;;
	}
inline	bool	IsEndTimeCounter()	{ return bEnableTimeCounter; }
inline	void	SetTimeCounter( DWORD time ) {
		m_TimeProgressRate = 0.0f;
		bEnableTimeCounter = true;
		m_BeginTime = GetCurrentTime();
		m_EndTime = m_BeginTime + time;
		m_CurrentTime = m_BeginTime;
	}
inline	bool	UpdateTimeCounter( DWORD time = 0xFFFFFFFF ) {
		if ( bEnableTimeCounter ) { 
			if( time == 0xFFFFFFFF)
				{ m_CurrentTime = GetCurrentTime(); }
			else
				{ m_CurrentTime = time; }

			m_TimeProgressRate = (float)(m_CurrentTime - m_BeginTime) / (float)(m_EndTime - m_BeginTime);

			if( m_CurrentTime > m_EndTime ) {
				m_TimeProgressRate = 1.0f;
				bEnableTimeCounter = false;
			}
		}
		return bEnableTimeCounter;
	}
inline	float	GetTimeProgressRate() { return m_TimeProgressRate; }
};
#endif

#endif
