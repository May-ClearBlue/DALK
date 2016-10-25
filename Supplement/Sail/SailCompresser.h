#ifndef _SAIL_LZO_
#define _SAIL_LZO_

#include "lzo/lzoconf.h"
#include "lzo/lzo1x.h"

/* portability layer */
#define WANT_LZO_MALLOC 1
#define WANT_XMALLOC 1
#include "portab.h"

#include "Rimlia\RimHeap.h"

class ICompress {
//圧縮インターフェース
//返り値はdestのサイズ：0以下はエラー
public:
virtual	long_t Compress	(size_t src_size, const void* pSrc, size_t& dest_size,void*& dest_buf, bool bMalloc = false) = 0;
virtual	long_t Decompress	(size_t src_size, const void* pSrc, size_t& dest_size,void*& dest_buf, bool bMalloc = false) = 0;
};

class CLZO {
protected:
	void*	m_pWork;
	int		m_LastError;
public:
	CLZO();
	~CLZO();

	long_t	Compress	(size_t src_size, const void* pSrc, size_t& dest_size,void*& dest_buf, bool bMalloc = false);
	long_t	Decompress	(size_t src_size, const void* pSrc, size_t& dest_size,void*& dest_buf, bool bMalloc = false);
};

#endif