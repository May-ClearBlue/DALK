#ifndef _DDSLOADER_
#define _DDSLOADER_

#include "RimCGLoader.h"

#include "Squish-1.11/squish.h"

struct DDSHEADER {
public:
	DWORD	dwMagic;	// == 0x20534444  ' SDD'
	DWORD	dwSize;		// == 124
	DWORD	dwFlags;	// ヘッダ内の有効な情報 DDSD_* の組み合わせ
	DWORD	dwHeight;	// 画像の高さ x size
	DWORD	dwWidth;	// 画像の幅   y size
	DWORD	dwPitchOrLinearSize;	// 横1 line の byte 数 (pitch)
					// または 1面分の byte 数 (linearsize)
	DWORD	dwDepth;	// 画像の奥行き z size (Volume Texture 用)
	DWORD	dwMipMapCount;	// 含まれている mipmap レベル数
	DWORD	dwReserved1[11];
	DWORD	dwPfSize;	// == 32
	DWORD	dwPfFlags;	// pixel フォーマットを表す DDPF_* の組み合わせ
	DWORD	dwFourCC;	// フォーマットが FourCC であらわされる場合のみ
	DWORD	dwRGBBitCount;	// 1 pixel の bit 数
	DWORD	dwRBitMask;	// RGB format 時の mask
	DWORD	dwGBitMask;	// RGB format 時の mask
	DWORD	dwBBitMask;	// RGB format 時の mask
	DWORD	dwRGBAlphaBitMask;	// RGB format 時の mask
	DWORD	dwCaps;		// mipmap 等のフラグ指定用
	DWORD	dwCaps2;	// cube/volume texture 等のフラグ指定用
	DWORD	dwReservedCaps[2];
	DWORD	dwReserved2;
};

class CDDSLoader:public ICGLoaderBase {
protected:
public:
	CDDSLoader() { m_ext = _T("dds"); }
		int		LoadData	(ISurfaceInfo& rPixel, IDataStream* pData, int malloc_method = 0);
};

#pragma pack(push,1)

typedef struct {
	DWORD	Version;
	DWORD	Size;
	DWORD	NumTexture;
}	GtfFileHeader;

typedef struct {
    uint8_t		format;
    uint8_t		mipmap;
    uint8_t		dimension;
    uint8_t		cubemap;
    DWORD	remap;
    uint16_t	width;
    uint16_t	height;
    uint16_t	depth;
    uint8_t		location;
    uint8_t		_padding;
    DWORD	pitch;
    DWORD	offset;
}	GcmTexture;

typedef struct {
	DWORD	Id;
	DWORD	OffsetToTex;
	DWORD	TextureSize;
	GcmTexture	tex;
}	GtfTextureAttribute;

#pragma pack(pop)

enum	{
	GTF_TEXTURE_COMPRESSED_DXT1			= (0x86),
	GTF_TEXTURE_COMPRESSED_DXT23		= (0x87),
	GTF_TEXTURE_COMPRESSED_DXT45		= (0x88),
	GTF_TEXTURE_SZ	= (0x00),
	GTF_TEXTURE_LN	= (0x20),
	GTF_TEXTURE_NR	= (0x00),
	GTF_TEXTURE_UN	= (0x40),
	GTF_TEXTURE_COMPRESSED_DXT1_LN		= GTF_TEXTURE_COMPRESSED_DXT1  | GTF_TEXTURE_LN,
	GTF_TEXTURE_COMPRESSED_DXT23_LN		= GTF_TEXTURE_COMPRESSED_DXT23 | GTF_TEXTURE_LN,
	GTF_TEXTURE_COMPRESSED_DXT45_LN		= GTF_TEXTURE_COMPRESSED_DXT45 | GTF_TEXTURE_LN,
	GTF_TEXTURE_COMPRESSED_DXT1_SZ		= GTF_TEXTURE_COMPRESSED_DXT1  | GTF_TEXTURE_SZ,
	GTF_TEXTURE_COMPRESSED_DXT23_SZ		= GTF_TEXTURE_COMPRESSED_DXT23 | GTF_TEXTURE_SZ,
	GTF_TEXTURE_COMPRESSED_DXT45_SZ		= GTF_TEXTURE_COMPRESSED_DXT45 | GTF_TEXTURE_SZ,
	GTF_TEXTURE_COMPRESSED_DXT1_UN		= GTF_TEXTURE_COMPRESSED_DXT1  | GTF_TEXTURE_UN,
	GTF_TEXTURE_COMPRESSED_DXT23_UN		= GTF_TEXTURE_COMPRESSED_DXT23 | GTF_TEXTURE_UN,
	GTF_TEXTURE_COMPRESSED_DXT45_UN		= GTF_TEXTURE_COMPRESSED_DXT45 | GTF_TEXTURE_UN,
};

class CGTFLoader:public ICGLoaderBase {
protected:
public:
	CGTFLoader() { m_ext = _T("gtf"); }

	int LoadData	( ISurfaceInfo& rCGInfo, IDataStream* pData , int malloc_method = 0 );
};


#endif