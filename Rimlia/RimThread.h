#ifndef _RIM_THREAD_
#define _RIM_THREAD_

#include "MultiStdAfx.h"

#if defined (_DALK_WINDOWS_)
#include "RimSignal.h"
#include <process.h> 

//::GetCurrentThreadId()
//単一スレッドクラス
class CThread:public CSignal{
protected:
		unsigned		m_ThreadID;
		HANDLE			m_hThread;
public:
//inline	HANDLE			BeginThread			( void*(*proc)() )		{ ResetEvent(); return ( m_hThread = (HANDLE)::_beginthreadex(NULL,0,ThreadCallBack,this,0,&m_ThreadID) ); }
inline	HANDLE	BeginThread()								{ Off(); return ( m_hThread = (HANDLE)::_beginthreadex(NULL,0,ThreadCallBack,this,0,&m_ThreadID) ); }
inline	void	CloseThread()								{ SafeClose(m_hThread); }
inline	DWORD	WaitThread			(DWORD time = INFINITE)	{ if(m_hThread)return ::WaitForSingleObject(m_hThread,time);  return WAIT_TIMEOUT; }
inline	DWORD	ResumeThread()								{ return ::ResumeThread(m_hThread); }
inline	DWORD	SuspendThread()								{ return ::SuspendThread(m_hThread); }
inline	BOOL	TerminateThread		(DWORD exitcode)		{ return ::TerminateThread(m_hThread,exitcode); }
inline	BOOL	SetThreadPriority	(int pri)				{ return ::SetThreadPriority(m_hThread,pri); }
inline	int		GetExitCodeThread	(DWORD* exitcode)		{ return ::GetExitCodeThread(m_hThread,exitcode); }
static	unsigned __stdcall	ThreadCallBack(void* pData)	{ CThread* lpthread = reinterpret_cast<CThread*>(pData); return lpthread->ThreadProc(); }
//インプリメント用仮想関数
virtual	unsigned			ThreadProc() = 0;	

			CThread() :	m_hThread(NULL), m_ThreadID(0xFFFFFFFF)	{ }
virtual		~CThread()		{ CloseThread(); }
};

#elif defined(_DALK_IOS_)
#include <pthread.h>

class CThread/*:public CSignal*/{
protected:
	pthread_t	m_ThreadID;			// スレッドのID
inline	int		BeginThread			(int Priority, char* name)		{ return pthread_create( &m_ThreadID, NULL, ThreadCallBack, this); }
inline	BOOL	TerminateThread		(DWORD exitcode)				{ return pthread_join(&m_ThreadID, NULL) != 0 ? TRUE : FALSE; }
static	void	ThreadCallBack		(void* pData)				{ CThread* pThread = (CThread*)pData; pThread->ThreadProc(); }
//pthread_exit(NULL);
//インプリメント用仮想関数
virtual	unsigned			ThreadProc() = 0;	
			CThread()		{ /*m_ThreadName = "";*/ m_ThreadID = 0; }
virtual		~CThread()		{ ; }//CloseThread(); }


#endif

#endif