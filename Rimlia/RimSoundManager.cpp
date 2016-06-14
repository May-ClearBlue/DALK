#include "MultiStdAfx.h"

#include "RimSoundManager.h"
#include "RimDataStream.h"

#ifdef _DALK_USE_OGG_
#include "RimOggDecoder.h"
#endif

ISoundDecoder* CSoundManager::CreateSoundDecoder(LPCTSTR ext) {
	string ex = ext;

	if(ex == "wav" || ex == "WAV" || ex == ".wav" || ex == ".WAV")
		return new CPCMDecoder;

#ifdef _DALK_USE_OGG_
    if(ex == "ogg" || ex == "OGG" || ex == ".ogg" || ex == ".OGG")
		return new CVorbisDecoder;
#endif
	return NULL;
}

#if defined(_DALK_IOS_)
ISoundDecoder* CSoundManager::CreateSoundDecoder(NSString* name) {
	return new CExtAudioDecoder;
}
#endif
