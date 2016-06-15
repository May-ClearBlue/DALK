#ifndef _RIM_CRITICAL_SECTION_
#define _RIM_CRITICAL_SECTION_

#include "../MultiStdafx.h"
//Windowsで言うところのクリティカルセクション＝ミューテックス
#if defined(_DALK_WINDOWS_)
typedef CRITICAL_SECTION _MUTEX;
#elif defined(_DALK_PS3_)
#include <cell/sync/mutex.h>
typedef CellSyncMutex  _MUTEX;
#elif defined(_DALK_IOS_)
#include <pthread.h>
typedef pthread_mutex_t _MUTEX;
#endif

class CCriticalSection {
protected:
	_MUTEX	m_CriticalSection;
	DWORD	m_ThreadID;			//	LockしているThreadId(0:非Lock)
	int		m_LockCount;		//	Lockされている回数
public:
		CCriticalSection();
virtual	~CCriticalSection();
		void	Enter();
		void	Leave();
		BOOL	Try();
		bool	IsLockedByThisThread() const;
};

#endif