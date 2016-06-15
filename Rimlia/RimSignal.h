#ifndef _SAIL_EVENTOBJECT_
#define _SAIL_EVENTOBJECT_

#include "MultiStdAfx.h"

//単独イベントの制御
class CSignal {
protected:
		HANDLE	m_hEvent;
public:
		CSignal()	{ m_hEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL); }
virtual	~CSignal()	{ ::CloseHandle(m_hEvent); }

inline	HANDLE	GetHandle()			{ return m_hEvent; }
inline	BOOL	On()				{ return ::SetEvent(m_hEvent); }
inline	BOOL	Off()				{ return ::ResetEvent(m_hEvent); }
inline	DWORD	Wait(DWORD time)	{ return ::WaitForSingleObject(m_hEvent,time); }
};


//複数イベントの制御、単独と違ってHANDLEは外部で作成されてるもの
class CMultiSignal {
protected:
		vector<HANDLE>		m_EventList;
public:
		CMultiSignal()	{ ; }
virtual	~CMultiSignal()	{ ; }

inline	void	Add			(HANDLE hEvent)						{ m_EventList.push_back(hEvent); }
inline	HANDLE	GetHandle	(size_t num)						{ return m_EventList[num]; }
inline	BOOL	On			(size_t num)						{ return ::SetEvent(m_EventList[num]); }
inline	BOOL	Off			(size_t num)						{ return ::ResetEvent(m_EventList[num]); }
inline	BOOL	On			(size_t from,size_t to)				{ for(size_t a = from;a < to;a ++) ::ResetEvent(m_EventList[a]); }
inline	BOOL	Off			(size_t from,size_t to)				{ for(size_t a = from;a < to;a ++) ::SetEvent(m_EventList[a]); }
inline	BOOL	AllOn()											{ for(size_t a = 0; a < m_EventList.size(); a++ ) On(a); }
inline	BOOL	AllOff()										{ for(size_t a = 0; a < m_EventList.size(); a++ ) Off(a); }
inline	DWORD	Wait		(size_t num,DWORD time)				{ return ::WaitForSingleObject(m_EventList[num],time); }
inline	DWORD	Wait		(size_t from,size_t to,DWORD time)	{ return ::WaitForMultipleObjects(to - from + 1,&(m_EventList[from]),FALSE,time); }
};

#endif