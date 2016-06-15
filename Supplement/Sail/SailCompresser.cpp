#include "Multistdafx.h"

#include "SailCompresser.h"

CLZO::CLZO() {
	if (lzo_init() == LZO_E_OK)
		m_pWork = (lzo_voidp) xmalloc(LZO1X_1_MEM_COMPRESS);
	else
		m_LastError = -1;
}

CLZO::~CLZO() {
	lzo_free(m_pWork);
}

long_t CLZO::Compress(size_t src_size, const void* pSrc, size_t& dest_size, void*& pDest, bool bMalloc) {
	if(bMalloc)
		_VALLOC(pDest, dest_size);

	m_LastError = lzo1x_1_compress((const unsigned char*)pSrc, src_size, (unsigned char*)pDest, (lzo_uint*)&dest_size, m_pWork);

	if (m_LastError != LZO_E_OK)
        return -1;
    /* check for an incompressible block */
    if (dest_size >= src_size)
		return 0;

	return dest_size;
}

long_t CLZO::Decompress(size_t src_size, const void* pSrc, size_t& dest_size,void*& pDest, bool bMalloc) {
	long_t res = dest_size;

	if(bMalloc)
		_VALLOC(pDest, dest_size);

	m_LastError = lzo1x_decompress((const unsigned char*)pSrc, src_size, (unsigned char*)pDest, (lzo_uint*)&dest_size, m_pWork);

	if (m_LastError != LZO_E_OK ||  res != dest_size)
        return -1;

	return dest_size;
}
