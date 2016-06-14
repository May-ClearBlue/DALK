#ifndef _SAIL_VOLUME_MANAGER_
#define _SAIL_VOLUME_MANAGER_

#include "MultyStdafx.h"
#include "Rimlia\RimThread.h"
#include "Siglune\SigDirectSound.h"
#include <math.h>
#include <process.h> 

//�f�V�x���ϊ�
inline double percent2decibel(double percent){
	double res =  ( 20.0 * log10( percent / 100.0 ) ) * 100.0;
	if(res > 0.0)
		res = 0.0;
	if(res < -10000.0)
		res = -10000.0;
	return res;
}

class CVolumeManager:public CThread {
protected:
		double		m_Volume;	// 0�`100�͈̔͂Ő��`�I�Ɏw�肷��l
		double		m_Volume2;	//�t�F�[�h�p�A���ڃ��[�U�[�͂�����Ȃ����ƁB
		CDirectSoundBuffer*	m_pBuffer;
public:
	void		SetBuffer	(CDirectSoundBuffer* pBuffer){ m_pBuffer = pBuffer; }
	void		SetVolume	(double Vol)	{ m_Volume = Vol; }
	void		SetVolume2	(double Vol2)	{ m_Volume2 = Vol2; }
	void		UpdateVolume()				{ m_pBuffer->SetVolume( percent2decibel(m_Volume * m_Volume2 / 100.0) );}
	unsigned	ThreadProc()				{ return 0; }
		CVolumeManager()	{ m_Volume = 0,0f; m_Volume2 = 0.0f; m_pBuffer = NULL;}
virtual	~CVolumeManager()	{ ; }
};

#endif