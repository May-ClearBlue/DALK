#include "MultiStdafx.h"


#include "RimCriticalSection.h"

CCriticalSection::CCriticalSection(){
#if defined(_DALK_WINDOWS_)
	::InitializeCriticalSection(&m_CriticalSection);
#endif
	m_ThreadID	= (DWORD)-1;
	m_LockCount	= 0;
}

CCriticalSection::~CCriticalSection(){
#if defined(_DALK_WINDOWS_)
    ::DeleteCriticalSection(&m_CriticalSection);
#endif
}

void CCriticalSection::Enter(){
#if defined(_DALK_WINDOWS_)
    ::EnterCriticalSection(&m_CriticalSection);
	m_ThreadID = ::GetCurrentThreadId();
#endif
	m_LockCount ++;
}

void CCriticalSection::Leave(){
	if (--m_LockCount == 0) 
		m_ThreadID = (DWORD) -1;
	//↑Leaveした直後に他スレッドがEnterする可能性がある
#if defined(_DALK_WINDOWS_)
    ::LeaveCriticalSection(&m_CriticalSection);
#endif
}

BOOL CCriticalSection::Try() {
#if defined(_DALK_WINDOWS_)
    return ::TryEnterCriticalSection(&m_CriticalSection);
#endif
}

bool CCriticalSection::IsLockedByThisThread() const{
#if defined(_DALK_WINDOWS_)
    return m_ThreadID == ::GetCurrentThreadId();
#endif
}
