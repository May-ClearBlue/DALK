#include "MultiStdAfx.h"

#include "RimLZMACompressor.h"

void Compress2( std::vector<unsigned char> &outBuf, const std::vector<unsigned char> &inBuf) {
	size_t propsSize = LZMA_PROPS_SIZE;
	size_t destLen = inBuf.size() + inBuf.size() / 3 + 128;
	outBuf.resize(propsSize + destLen);

	CLzmaEncProps props;
	LzmaEncProps_Init(&props);
	props.dictSize = 1 << 16; // 64 KB
	props.writeEndMark = 1; // 0 or 1

	int res = LzmaEncode(	&outBuf[LZMA_PROPS_SIZE], &destLen, &inBuf[0], inBuf.size(),
							&props, &outBuf[0], &propsSize, props.writeEndMark, &g_ProgressCallback, &SzAllocForLzma, &SzAllocForLzma );

	assert(res == SZ_OK && propsSize == LZMA_PROPS_SIZE);
  
	outBuf.resize(propsSize + destLen);
}

#define _LZMA_UNCOMPRESS_BUFFER_SIZE_RATIO_ 10
static void Uncompress1( std::vector<unsigned char> &outBuf, const std::vector<unsigned char> &inBuf, int outsize = -1) {
	outBuf.resize(outsize != -1 ? outsize : inBuf.size() * _LZMA_UNCOMPRESS_BUFFER_SIZE_RATIO_);

	size_t dstLen = outBuf.size();
	size_t srcLen = inBuf.size() - LZMA_PROPS_SIZE;

	SRes res = LzmaUncompress( &outBuf[0], &dstLen, &inBuf[LZMA_PROPS_SIZE], &srcLen, &inBuf[0], LZMA_PROPS_SIZE);

	assert(res == SZ_OK);

	outBuf.resize(dstLen); // If uncompressed data can be smaller
}