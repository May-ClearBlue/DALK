#include <MultiStdAfx.h>

#include "RimRLECompressor.h"

/*
TGA の RLE
if ( CBYTE & 0x80 )
{
  // 最上位ビットが1のときリピート。繰り返す回数は、CBYTE & 0x7F + 1
} else {
  // 最上位ビットが0のときコピー。コピーする回数は、CBYTE+1
}
Phtoshop, TIFF, IFF(ILBM), DICOM の RLE（PackBits）
if ( CBYTE == 0x80 )
{
  // 何もしない（制御バイトが0x80のとき）
} else if ( CBYTE & 0x80 ) {
  // 最上位ビットが1のときリピート。繰り返す回数は、257 - CBYTE
} else {
  // 最上位ビットが0のときコピー。コピーする回数は、CBYTE+1
}
SGI の RLE
int Count = CBYTE & 0x7F;
if ( Count == 0 )
{
  // 何もしない（制御バイトが0x80のとき）
} else if ( CBYTE & 0x80 ) {
  // 最上位ビットが1のときコピー。コピーする回数は、Count
} else {
  // 最上位ビットが0のときリピート。繰り返す回数は、Count
}
Paint Shop Pro の RLE
int Count = CBYTE & 0x7F;
if ( Count == 0 )
{
  // 何もしない（制御バイトが0x80のとき）
} else if ( CBYTE & 0x80 ) {
  // 最上位ビットが1のときリピート。繰り返す回数は、Count
} else {
  // 最上位ビットが0のときコピー。コピーする回数は、Count
}
*/

//size_t RLECompressor::Compress(void* pSrc, size_t SrcSize, void*& Dest, size_t DestSize);

size_t RLECompressor::Decompress(void* pSrc, size_t SrcSize, void*& pDest, size_t DestSize) {
	size_t count_src = 0;
	size_t count_dest = 0;

	BYTE* pSrcData = (BYTE*)pSrc;
	BYTE* pDestData = (BYTE*)pDest;

	while(count_src < SrcSize && count_dest < DestSize) {
		if( *pSrcData == 0x80 )				// 何もしない（制御バイトが0x80のとき）
			pSrcData++;
		else if( *pSrcData & 0x80 ) {		// 最上位ビットが1のときリピート。繰り返す回数は、257 - CBYTE
			size_t bytes = 257 - *(pSrcData++);
#ifdef _DALK_DEBUG_BUILD_
			if(count_dest + bytes > DestSize) {
				_DEBUGPRINTF(_TEXT("バッファサイズオーバー@RLECompressor::Decompress"));
				return 0xFFFFFFFF;
			}
#endif
			_MEMSET(pDestData, *(pSrcData++), bytes);
			count_src	+= 2;
			pSrcData	+= 2;
			count_dest	+= bytes;
			pDestData	+= bytes;		
		}
		// 最上位ビットが0のときコピー。コピーする回数は、CBYTE+1
		else {
			size_t bytes = 1 + *(pSrcData++);
			_MEMCPY(pDestData, pSrcData, bytes);

#ifdef _DALK_DEBUG_BUILD_
			if(count_dest + bytes > DestSize) {
				_DEBUGPRINTF(_TEXT("バッファサイズオーバー@RLECompressor::Decompress"));
				return 0xFFFFFFFF;
			}
#endif
			count_src	+= bytes + 1;
			pSrcData	+= bytes + 1;
			count_dest	+= bytes;
			pDestData	+= bytes;		
		}
	}
	return count_dest;
}

#if 0
template <typename V>
int		ListSpr(
			void*		pSurface,											// サーフェスアドレス
			uint32_t		primtype,											// プリミティブタイプ
			uint32_t		vertform,											// 頂点フォーマット
			V*				vertlist,											// 頂点テーブル
			uint32_t		vertnum,											// 頂点数
			DWORD			col[4],												// カラー
			float			alpha,												// アルファー値
			uint32_t		attr,												// アットリビュート
			void*	fsp,													// シェーダー番号（0:スタンダード）
			uint32_t		bid,												// ブレンダー番号（0:スタンダード）
			uint16_t		pri													// Ｚソートプライオリティー
			)
/*------------------------------------------------------------------------------*/
{
	return sizeof(V);
};

static void* TEMP = NULL;

void GSLdrawprimTRI(int sno, int* tlv, UINT32 pnum, int alphaop)
{
	ListSpr(TEMP, 0, 1, (RECT*)tlv, pnum*3, NULL, 1.0f, 0, NULL);
}
#endif
