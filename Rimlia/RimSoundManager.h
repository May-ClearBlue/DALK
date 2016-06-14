#ifndef _RIM_SOUNDMANAGER_
#define _RIM_SOUNDMANAGER_

#include "RimSoundDecoder.h"

#if defined(_DALK_WINDOWS_)
 #define _DALK_USE_ACM_
#elif defined(_DALK_IOS_)
 #define _DALK_USE_EXTAUDIO_
 #include "RimExtAudioDecoder.h"
#endif

class CSoundManager {
public:
static	ISoundDecoder* CreateSoundDecoder(LPCTSTR ext);
static	ISoundDecoder* CreateSoundDecoder(const string_t& name) { return CreateSoundDecoder(name.data()); }
#if defined(_DALK_IOS_)
static	ISoundDecoder* CreateSoundDecoder(NSString* name);
#endif
};

#endif
