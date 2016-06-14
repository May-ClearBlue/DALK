#include "MultiStdAfx.h"

#include "RimCGLoader.h"

//ToDo:isReversed
int CBMPLoader::LoadData( ISurfaceInfo& rPixel,IDataStream* pData, int malloc_method ) {
	BITMAPFILEHEADER bitmapfileheader = { 0 };
	BITMAPINFOHEADER bitmapinfo = { 0 };

	pData->Read(bitmapfileheader);

	if(bitmapfileheader.bfType == 0x4D42){
		pData->Read(sizeof(BITMAPINFOHEADER),&bitmapinfo,false);

		rPixel.Depth	= bitmapinfo.biBitCount;
		rPixel.X		= bitmapinfo.biWidth;
		rPixel.Y		= bitmapinfo.biHeight;
		rPixel.Pitch	= bitmapinfo.biWidth * bitmapinfo.biBitCount / 8;

		if(bitmapinfo.biClrUsed && bitmapinfo.biBitCount <= 8)
			pData->Read(bitmapinfo.biClrUsed * sizeof(RGBQUAD),rPixel.DIBPalette,false);
		else{
			unsigned palettenum;

			switch(bitmapinfo.biBitCount){
			case 1:
				palettenum = 2;
				break;
			case 2:
				palettenum = 4;
				break;
			case 4:
				palettenum = 16;
				break;
			case 8:
				palettenum = 256;
				break;
			default:
				palettenum = 0;
			}

			if(palettenum)
				pData->Read(palettenum * sizeof(RGBQUAD),rPixel.DIBPalette,false);
		}

		pData->Seek( (LONG)bitmapfileheader.bfOffBits,FILE_BEGIN );

		if( rPixel.pData == NULL )
			_VALLOC(rPixel.pData, bitmapinfo.biWidth * bitmapinfo.biHeight * bitmapinfo.biBitCount / 8 );

//		memcpy(rPixel.pData, pData, bitmapinfo.biWidth * bitmapinfo.biHeight * bitmapinfo.biBitCount / 8);
		pData->Read( bitmapinfo.biWidth * bitmapinfo.biHeight * bitmapinfo.biBitCount / 8, rPixel.pData,false);

		return 0;
	}

//	pData->Seek( 0,FILE_BEGIN );
	return 1;
}
