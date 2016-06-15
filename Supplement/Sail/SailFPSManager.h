#ifndef __TIMECONTROL_H__
#define __TIMECONTROL_H__

#include <windows.h>
#include <mmsystem.h>

//=======================================================
//�t���[�����[�g�𐧌䂷��N���X�ł��B
//======================================================
class CFPSManager {
public:
	CFPSManager(DWORD dwFps = 60, bool bFrameSkip = true);
	~CFPSManager() { ::timeEndPeriod(1); }

private:
	DWORD	m_dwLastFlipped;
	DWORD	m_dwLastMinitues;
	DWORD	m_dwFrameRate;
	DWORD	m_dwSkipRate;
	float	m_fFrameTime;
	bool	m_bDrawFlag;
	bool	m_bFrameSkip;
	bool	m_bInit;

	//=======================================================
	//���݂̂P�b�P�ʂ̃��[�g�̕\��
	//======================================================
	DWORD	m_dwFrameCount;
	DWORD	m_dwSkipCount;
public:
	void	Measure();
	void	TimeRegular();
	void	OnFrameSkip( bool bFrameSkip )	{ m_bFrameSkip = bFrameSkip; }
	void	SetFPS( DWORD fps )	{ m_fFrameTime = 1000.0f / fps; }
	DWORD	GetSkipRate()		{ return m_dwSkipRate; }
	DWORD	GetFrameRate()		{ return m_dwFrameRate; }
	bool	GetDrawFlag()		{ return m_bDrawFlag; }
};




#endif