#include "Multistdafx.h"

#include "SailFPSManager.h"
#pragma comment(lib,"winmm.lib")

//======================================================
//�R���X�g���N�^
//======================================================
CFPSManager::CFPSManager(DWORD dwFps, bool bFrameSkip) {
	memset(this,0,sizeof(CFPSManager));
	m_bInit = 1;
	m_bFrameSkip = bFrameSkip;
	SetFPS(dwFps);
	::timeBeginPeriod(1);
}


//======================================================
//�����Ŏ��Ԑ�������܂��B
//======================================================
void CFPSManager::TimeRegular() {
	m_dwFrameCount++;

	
	if(m_bInit == 1) {
		m_dwLastMinitues = timeGetTime();
		m_bInit = 0;
		m_bDrawFlag = 1;
		return;
	}

	if(m_bFrameSkip == 1 && timeGetTime()  > (DWORD)((m_dwFrameCount + 1) * m_fFrameTime + m_dwLastMinitues)) 	{
		m_bDrawFlag = 0;
		m_dwSkipCount++;
	}
	else {
		//===========================================================
		//���̈ړ����������ƂŖ��ʂȉ��Z�����炵�܂����B
		//����			timeGetTime() - m_dwLastMinitues <= (DWORD)((m_dwFrameCount + 1) * m_fFrameTime)
		//���̈ړ�		timeGetTime()  <= (DWORD)((m_dwFrameCount + 1) * m_fFrameTime) +  m_dwLastMinitues
		//�ϐ��u������	timeGetTime()�@<= dwTime
		//===========================================================
		DWORD dwTime = (DWORD)(m_dwFrameCount * m_fFrameTime + m_dwLastMinitues);

		while(timeGetTime() <= dwTime)
			{ ::Sleep(1); }

			m_bDrawFlag = 1;
	}

	if(timeGetTime() - m_dwLastMinitues >= 1000) {
		m_dwLastMinitues = timeGetTime();
		m_dwFrameRate = m_dwFrameCount;
		m_dwFrameCount = 0;
		m_dwSkipRate = m_dwSkipCount;
		m_dwSkipCount = 0;
	}
}

//==================================================
//�ő�t���[�����[�g�𑪒肵�܂��B
//==================================================
void CFPSManager::Measure() {
	m_dwFrameCount++;
	m_bDrawFlag = 1;
	if(timeGetTime() - m_dwLastMinitues >= 1000)
	{
		m_dwLastMinitues = timeGetTime();
		m_dwFrameRate = m_dwFrameCount;
		m_dwFrameCount = 0;
		m_dwSkipRate = m_dwSkipCount;
		m_dwSkipCount = 0;
	}	
}
