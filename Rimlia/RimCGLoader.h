#ifndef _SAIL_GRPH_LOADER_
#define _SAIL_GRPH_LOADER_

#include "RimHeap.h"
#include "RimFile.h"
#include "RimDataStream.h"
//#include "SailFileStream.h"

#ifndef _DALK_WINDOWS_
 #include "RimTypeDef.h"
#endif

//STRUCT
#pragma pack(push,4)

struct ISurfaceInfo {
	int32_t		X,Y;
	uint32_t	Depth;
	uint32_t	Pitch;
	void*		pData;
#ifdef _DALK_WINDOWS_
	union{
	RGBQUAD			DIBPalette[256];
	PALETTEENTRY	DXPalette[256];
	};
#else
    RGBQUAD         DIBPalette[256];
#endif
};

#pragma pack(pop)

inline void SetRGBQUAD(RGBQUAD& pal,BYTE r,BYTE g,BYTE b){
	pal.rgbBlue = b;
	pal.rgbGreen = g;
	pal.rgbRed = r;
	pal.rgbReserved = 0;
}

inline void SetBITMAPINFO(BITMAPINFO &bitmapinfo,LONG x,LONG y,WORD bpp,DWORD palettenum = 0){
	bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfo.bmiHeader.biWidth = x;
	bitmapinfo.bmiHeader.biHeight = y;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biBitCount = bpp;
	bitmapinfo.bmiHeader.biCompression = BI_RGB;
	bitmapinfo.bmiHeader.biSizeImage = (x * y * bpp) / 8;
	bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapinfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapinfo.bmiHeader.biClrUsed = palettenum;
	bitmapinfo.bmiHeader.biClrImportant = 0;
}

struct BMPINFO{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
};



//CGローダのベースクラス。
class ICGLoaderBase {
protected:
	string_t	m_ext;
public:
virtual	int		LoadData( ISurfaceInfo& rPixel, IDataStream* pData, int malloc_method = 0 )  = 0;
const string_t&	GetExtent() { return m_ext; }
};

//BMPローダ(singleton)
class CBMPLoader : public ICGLoaderBase {
private:
public:
	CBMPLoader(){ m_ext = _T("bmp"); }
		int			LoadData(ISurfaceInfo& rPixel,  IDataStream* pData, int malloc_method = 0 );
};

#endif
