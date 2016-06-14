#ifndef _RIM_COMPRESSOR_BASE_
#define _RIM_COMPRESSOR_BASE_

#include <MultiStdAfx.h>

class IDataCompressorBase {
public:
virtual size_t	Compress	(void* pSrc, size_t SrcSize, void*& Dest, size_t DestSize) = 0;
virtual size_t	Decompress	(void* pSrc, size_t SrcSize, void*& Dest, size_t DestSize) = 0;
};

#endif