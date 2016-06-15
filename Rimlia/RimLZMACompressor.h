#ifndef _RIM_LZMA_COMPRESSOR_
#define _RIM_LZMA_COMPRESSOR_

#include "RimHeap.h"
#include "lzma1602\C\LzmaLib.h"
#include <lzma1602\CPP\7zip\Compress\LzmaDecoder.h>
#include <lzma1602\CPP\7zip\Compress\LzmaEncoder.h>

SRes OnProgress(void *p, UInt64 inSize, UInt64 outSize)
{
  // Update progress bar.
  return SZ_OK;
}

static ICompressProgress g_ProgressCallback = { &OnProgress };

static void*	AllocForLzma(void*p, size_t size)	{ void* r; _VALLOC(r, size); return r; }
static void		FreeForLzma	(void*p, void* address) { _VFREE(address); }
static ISzAlloc SzAllocForLzma = { &AllocForLzma, &FreeForLzma };

typedef struct
{
  ISeqInStream SeqInStream;
  const std::vector<unsigned char> *Buf;
  unsigned BufPos;
} VectorInStream;

SRes VectorInStream_Read(void *p, void *buf, size_t *size)
{
  VectorInStream *ctx = (VectorInStream*)p;
  *size = min(*size, ctx->Buf->size() - ctx->BufPos);
  if (*size)
    memcpy(buf, &(*ctx->Buf)[ctx->BufPos], *size);
  ctx->BufPos += *size;
  return SZ_OK;
}

typedef struct
{
  ISeqOutStream SeqOutStream;
  std::vector<unsigned char> *Buf;
} VectorOutStream;

size_t VectorOutStream_Write(void *p, const void *buf, size_t size)
{
  VectorOutStream *ctx = (VectorOutStream*)p;
  if (size)
  {
    unsigned oldSize = ctx->Buf->size();
    ctx->Buf->resize(oldSize + size);
    memcpy(&(*ctx->Buf)[oldSize], buf, size);
  }
  return size;
}

void Compress2( std::vector<unsigned char> &outBuf, const std::vector<unsigned char> &inBuf);
void Uncompress1( std::vector<unsigned char> &outBuf, const std::vector<unsigned char> &inBuf);

#endif