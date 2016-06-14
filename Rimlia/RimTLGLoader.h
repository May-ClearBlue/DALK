//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TLG5/6 decoder
//---------------------------------------------------------------------------

#ifndef _LoadTLGH_
#define _LoadTLGH_

#include "RimImageLoader.h"
#include "../Supplement/libTLG/tvpgl.h"

//---------------------------------------------------------------------------
// Graphic Loading Handler Type
//---------------------------------------------------------------------------
typedef void (*tTVPGraphicSizeCallback)
	(void *callbackdata, tjs_uint w, tjs_uint h);
/*
	callback type to inform the image's size.
	call this once before TVPGraphicScanLineCallback.
*/

typedef void * (*tTVPGraphicScanLineCallback)
	(void *callbackdata, tjs_int y);
/*
	callback type to ask the scanline buffer for the decoded image, per a line.
	returning null can stop the processing.

	passing of y=-1 notifies the scan line image had been written to the buffer that
	was given by previous calling of TVPGraphicScanLineCallback. in this time,
	this callback function must return NULL.
*/

#define TVP_GL_IA32_FUNC_DECL(rettype, funcname, arg)  rettype __cdecl funcname arg
#define TVP_GL_IA32_FUNC_EXTERN_DECL(rettype, funcname, arg)  extern rettype __cdecl funcname arg
#define TVP_GL_IA32_FUNC_PTR_DECL(rettype, funcname, arg) rettype __cdecl (*funcname) arg
#define TVP_GL_IA32_FUNC_PTR_EXTERN_DECL(rettype, funcname, arg) extern rettype __cdecl (*funcname) arg

#define TVP_TLG6_H_BLOCK_SIZE 8
#define TVP_TLG6_W_BLOCK_SIZE 8

class CTLGLoader:public ICGLoaderBase{
public:
		int			LoadData(ISurfaceInfo& rPixel,  IDataStream* pData, int malloc_method = 0 );
};

#endif

