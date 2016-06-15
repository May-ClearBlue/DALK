#ifndef _RIM_ACMDECODER_

#include "RimSoundDecoder.h"

#include <msacm.h> 
#pragma comment (lib, "msacm32.lib")

class CACMDecoder : public CPCMDecoder {
protected:
		HACMSTREAM		m_hACMStream;
		CQueBuffer		m_SrcBuffer;
public:
	CACMDecoder()	{ m_hACMStream = NULL; }
	~CACMDecoder()	{ Close(); }
		void	SetDestFormat(DWORD SamplingRate = 44100, WORD BitRate = 16, WORD Channels = 1  );
		size_t	Decode		( void* pDestBuffer, DWORD DestSize, DWORD SrcSize );
		int		Close();
};

#endif