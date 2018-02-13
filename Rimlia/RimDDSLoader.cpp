#include "MultiStdAfx.h"

#if false
#include "RimDDSLoader.h"

#ifdef _DEBUG
#pragma comment(lib, "squish.lib")
#else
#pragma comment(lib, "squish.lib")
#endif

//ToDo:isReversed
int CDDSLoader::LoadData(ISurfaceInfo& rPixel, IDataStream* pData, int malloc_method) {

	DDSHEADER ddsh;

	pData->Read(128, &ddsh, false);

	int format = 0;

	switch((DWORD)ddsh.dwFourCC) {
	case '1TXD':
		format = squish::kDxt1;
		break;
	case '3TXD':
		format = squish::kDxt3;
		break;
	case '5TXD':
		format = squish::kDxt5;
		break;
	default:
		return 1;
	}

	rPixel.Depth = ddsh.dwRGBBitCount == 0 ? 32 : ddsh.dwRGBBitCount;
	rPixel.X = ddsh.dwWidth;
	rPixel.Y = ddsh.dwHeight;
	rPixel.Pitch = ddsh.dwWidth * rPixel.Depth / 8; //ddsh.dwPitchOrLinearSize;

	_VALLOC(rPixel.pData, rPixel.Y * rPixel.X * rPixel.Depth / 8 );

	size_t SrcSize = pData->GetSize() - 128;

	void* pSrc;
	_VALLOC(pSrc, SrcSize);

	pData->Read(SrcSize, pSrc, false);

	squish::DecompressImage( (squish::u8*)rPixel.pData, rPixel.X, rPixel.Y, pSrc, format );

	RGBQUAD* pPix = (RGBQUAD*)rPixel.pData;

	for(int a = 0;a < rPixel.X * rPixel.Y; a++) {
		RGBQUAD t;
		
		t.rgbBlue = pPix[a].rgbRed;
		t.rgbGreen = pPix[a].rgbGreen;
		t.rgbRed = pPix[a].rgbBlue;
		t.rgbReserved = pPix[a].rgbReserved;
		pPix[a] = t;
	}	

	_VFREE(pSrc);

	return 0;
}

int CGTFLoader::LoadData( ISurfaceInfo& rCGInfo, IDataStream* pData, int malloc_method ) {
	GtfFileHeader gtfhed;
	GtfTextureAttribute	gtfatt;

	pData->Read(gtfhed);
	pData->Read(gtfatt);

	swapEndian32(gtfhed.NumTexture);
	swapEndian32(gtfhed.Size);
	swapEndian32(gtfhed.Version);
	swapEndian32(gtfatt.Id);
	swapEndian32(gtfatt.OffsetToTex);
	swapEndian32(gtfatt.TextureSize);

	swapEndian32(gtfatt.tex.pitch);
	swapEndian32(gtfatt.tex.offset);
	swapEndian32(gtfatt.tex.remap);
	swapEndian16(gtfatt.tex.width);
	swapEndian16(gtfatt.tex.height);
	swapEndian16(gtfatt.tex.depth);

	int flags = 0;
	switch(gtfatt.tex.format)
	{
	case GTF_TEXTURE_COMPRESSED_DXT1_LN:
	case GTF_TEXTURE_COMPRESSED_DXT1_SZ:
	case GTF_TEXTURE_COMPRESSED_DXT1_UN:
		flags |= squish::kDxt1;
		break;
	case GTF_TEXTURE_COMPRESSED_DXT23_LN:
	case GTF_TEXTURE_COMPRESSED_DXT23_SZ:
	case GTF_TEXTURE_COMPRESSED_DXT23_UN:
		flags |= squish::kDxt3;
		break;
	case GTF_TEXTURE_COMPRESSED_DXT45_LN:
	case GTF_TEXTURE_COMPRESSED_DXT45_SZ:
	case GTF_TEXTURE_COMPRESSED_DXT45_UN:
		flags |= squish::kDxt5;
		break;
	}

	rCGInfo.X = gtfatt.tex.width;
	rCGInfo.Y = gtfatt.tex.height;
	rCGInfo.Depth = 32;
	rCGInfo.Pitch = gtfatt.tex.width * rCGInfo.Depth / 8; //ddsh.dwPitchOrLinearSize;
	_VALLOC(rCGInfo.pData, rCGInfo.Y * rCGInfo.X * rCGInfo.Depth / 8 );

	size_t SrcSize = pData->GetSize() - 128;

	void* pSrc;
	_VALLOC(pSrc, SrcSize);

	pData->Read(SrcSize, pSrc, false);

	squish::DecompressImage( (squish::u8*)rCGInfo.pData, rCGInfo.X, rCGInfo.Y, pSrc, flags );

	RGBQUAD* pPix = (RGBQUAD*)rCGInfo.pData;

	for(int a = 0;a < rCGInfo.X * rCGInfo.Y; a++) {
		RGBQUAD t;
		
		t.rgbBlue = pPix[a].rgbRed;
		t.rgbGreen = pPix[a].rgbGreen;
		t.rgbRed = pPix[a].rgbBlue;
		t.rgbReserved = pPix[a].rgbReserved;
		pPix[a] = t;
	}	

	_VFREE(pSrc);

	return 0;
}
#endif
