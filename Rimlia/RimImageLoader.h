#ifndef _SAIL_IMAGELOADER_
#define _SAIL_IMAGELOADER_

#include "RimCGLoader.h"

#if defined(_DALK_WINDOWS_)
#define _DALK_USE_PNG_
//#define _DALK_USE_DDS_
//#define _DALK_USE_TLG_
#undef _DALK_USE_DDS_
#endif

#ifdef _DALK_USE_PNG_
#pragma message("--------------ImageLoader--------------")
#pragma message("Enable:PNG")
#pragma message("---------------------------------------")
#include "RimPNGLoader.h"
#endif

#ifdef _DALK_USE_DDS_
#pragma message("--------------ImageLoader--------------")
#pragma message("Enable:DDS")
#pragma message("---------------------------------------")
#include "RimDDSLoader.h"
#endif

#ifdef _DALK_USE_TLG_
#pragma message("--------------ImageLoader--------------")
#pragma message("Enable:TLG")
#pragma message("---------------------------------------")
#pragma message("AddOn:libTLG")
#include "RimTLGLoader.h"
#endif

class CImageLoader {
protected:
static	CBMPLoader m_BMPLoader;

#ifdef _DALK_USE_PNG_
static 	CPNGLoader m_PNGLoader;
#endif

#ifdef _DALK_USE_DDS_
static 	CDDSLoader m_DDSLoader;
#endif

#ifdef _DALK_USE_TLG_
static 	CTLGLoader m_TLGLoader;
#endif


//	map<string_t, ICGLoaderBase>	m_LoaderManager;

static	ICGLoaderBase* GetLoader(LPCTSTR ext);

public:
//	CImageLoader();
static	int	LoadImage( LPCTSTR name, ISurfaceInfo& rPixel, void* pStream = NULL);
static	int LoadImage( const string_t& name, ISurfaceInfo& rPixel, void* pStream = NULL) { return LoadImage(name.data(), rPixel, pStream); }
#if defined(_DALK_IOS_)
static	int	LoadImage( NSString* name, ISurfaceInfo& rPixel, void* pStream = NULL);
#endif

};

#endif
