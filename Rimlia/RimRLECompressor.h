#ifndef _RIM_RLECOMPRESSOR_
#define _RIM_RLECOMPRESSOR_

#include "RimCompressor.h"

class RLECompressor : public IDataCompressorBase {
size_t	Compress	(void* pSrc, size_t SrcSize, void*& Dest, size_t DestSize) { return 0; }
size_t	Decompress	(void* pSrc, size_t SrcSize, void*& Dest, size_t DestSize);
};


template <typename V>
int			ListSpr(
					void*		pSurface,													// サーフェスアドレス
					uint32_t		primtype,													// プリミティブタイプ
					uint32_t		vertform,													// 頂点フォーマット
					V*				vertlist,													// 頂点テーブル
					uint32_t		vertnum,													// 頂点数
					DWORD			col[4]		= NULL,											// カラー
					float			alpha		= 1.0f,											// アルファー値
					uint32_t		attr		= 0,											// アットリビュート
					void*	fsp			= NULL,											// シェーダー番号（NULL:スタンダード）
					uint32_t		bid			= 0,											// ブレンダー番号（0:スタンダード）
					uint16_t		pri			= 0												// Ｚソートプライオリティー
					);

#endif