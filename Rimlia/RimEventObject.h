#ifndef _SAIL_EVENTOBJECT_
#define _SAIL_EVENTOBJECT_

#include "WinStdio.h"
#include <vector>

//単独イベントの制御
class CEvent {
protected:
		HANDLE	m_hEvent;
public:
		CEvent()	{ m_hEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL); }
virtual	~CEvent()	{ ::CloseHandle(m_hEvent); }

inline	HANDLE	GetHandle()			{ return m_hEvent; }
inline	BOOL	Set()				{ return ::SetEvent(m_hEvent); }
inline	BOOL	Reset()				{ return ::ResetEvent(m_hEvent); }
inline	DWORD	Wait(DWORD time)	{ return ::WaitForSingleObject(m_hEvent,time); }
inline	BOOL	IsSignal()			{ return WaitEvent(0); }
};

//複数イベントの制御
class CMultipleEvent {
protected:
		vector<HANDLE>		m_HandleList;
public:
inline	void	Add()												{ hevent.push_back( ::CreateEvent(NULL,TRUE,FALSE,NULL) ); }
inline	BOOL	Delete()											{ ::CloseHandle( hevent.back() ); }
inline	void	Push			(HANDLE hEvent)						{ hevent.push_back( hEvent ); }
inline	void	Pop()												{ hevent.pop_back(); }
		void	Init			(size_t num)						{ for(unsigned int a = 0;a < num;a ++) Add(); }
		void	Release()											{ while( hevent.size() > 0)	{ ::CloseHandle( hevent.back() ); hevent.pop_back(); } }
inline	HANDLE	GetHandle		(size_t num)						{ return hevent[num]; }
inline	BOOL	Set				(size_t num)						{ return ::SetEvent(hevent[num]); }
inline	BOOL	Reset			(size_t num)						{ return ::ResetEvent(hevent[num]); }
inline	BOOL	ResetMultiple	(size_t from,size_t to)				{ for(size_t a = from;a < to;a ++) ::ResetEvent(hevent[a]); }
inline	BOOL	SetMultiple		(size_t from,size_t to)				{ for(size_t a = from;a < to;a ++) ::SetEvent(hevent[a]); }
inline	DWORD	WaitEvent		(size_t num,DWORD time)				{ return ::WaitForSingleObject(hevent[num],time); }
inline	DWORD	WaitMultiple	(size_t from,size_t to,DWORD time)	{ return ::WaitForMultipleObjects(to - from + 1,&(hevent[from]),FALSE,time); }
		CMultipleEvent()	{ ; }
virtual	~CMultipleEvent()	{ Release(); }
};

#endif