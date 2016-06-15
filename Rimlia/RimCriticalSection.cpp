#include "MultiStdafx.h"


#include "RimCriticalSection.h"

CCriticalSection::CCriticalSection(){
#if defined(_DALK_WINDOWS_)
	::InitializeCriticalSection(&m_CriticalSection);
#elif defined(_DALK_PS3_)
	cellSyncMutexInitialize(&m_CriticalSection);
#endif
	/*
	CELL_OK
	 正常終了
 
	CELL_SYNC_ERROR_ALIGN
	mutexが4バイトアラインされていない
 
	CELL_SYNC_ERROR_INVAL
	tagの値が不正(32以上)
 
	CELL_SYNC_ERROR_NULL_POINTER
	 mutexがNULLポインタ
*/ 
	m_ThreadID	= (DWORD)-1;
	m_LockCount	= 0;
}

CCriticalSection::~CCriticalSection(){
#if defined(_DALK_WINDOWS_)
    ::DeleteCriticalSection(&m_CriticalSection);
#elif defined(_DALK_PS3_)
    //mutex解放がない？
//	cellSyncMutexFinalize(&m_CriticalSection);
#endif
}

void CCriticalSection::Enter(){
#if defined(_DALK_WINDOWS_)
    ::EnterCriticalSection(&m_CriticalSection);
	m_ThreadID = ::GetCurrentThreadId();
#elif defined(_DALK_PS3_)
	cellSyncMutexLock(&m_CriticalSection);
#endif
	m_LockCount ++;
}

void CCriticalSection::Leave(){
	if (--m_LockCount == 0) 
		m_ThreadID = (DWORD) -1;
	//↑Leaveした直後に他スレッドがEnterする可能性がある
#if defined(_DALK_WINDOWS_)
    ::LeaveCriticalSection(&m_CriticalSection);
#elif defined(_DALK_PS3_)
    cellSyncMutexUnlock(&m_CriticalSection);
#endif
}

BOOL CCriticalSection::Try() {
#if defined(_DALK_WINDOWS_)
    return ::TryEnterCriticalSection(&m_CriticalSection);
#elif defined(_DALK_PS3_)
    return cellSyncMutexTryLock(&m_CriticalSection);
#endif
}

bool CCriticalSection::IsLockedByThisThread() const{
#if defined(_DALK_WINDOWS_)
    return m_ThreadID == ::GetCurrentThreadId();
#elif defined(_DALK_PS3_)
    return false;
#endif
}
