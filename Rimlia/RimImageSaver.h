#ifndef _RIM_IMAGESAVER_
#define _RIM_IMAGESAVER_

#include "RimImageLoader.h"

int SaveBMP(ISurfaceInfo& rInfo,int flag, LPCTSTR Name,BOOL isReversed = FALSE);


#ifdef _DALK_USE_DDS_
int SaveDDS(ISurfaceInfo& rInfo,int flag, LPCTSTR Name,BOOL isReversed = FALSE);
#endif

#ifdef _DALK_USE_PNG_
int SavePNG(ISurfaceInfo& rInfo,int flag, LPCTSTR Name,BOOL isReversed = FALSE);

void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass);
#endif

#endif