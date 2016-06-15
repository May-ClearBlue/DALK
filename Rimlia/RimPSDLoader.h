#include "RimCGLoader.h"

enum _PSD_COLORMODE_PARAM_ {
	PSD_MONOTONE,			//０：白黒
	PSD_GRAY,				//１：グレースケール
	PSD_INDEX,				//２：インデックスカラー
	PSD_RGB,				//３：RGBカラー
	PSD_CMYK,				//４：CMYKカラー
	PSD_MULTICHANNEL = 7,	//７：マルチチャンネルカラー
	PSD_DOUBLETONE,			//８：ダブルトーン（デュオトーン）
	PSD_LAB					//９：Labカラー
};

/*
PSDFILEHEADER
PSDCOLORMODE
PSDIMAGERES
PSDLAYER
PSDIMAGEDATA
*/

#if 0
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
#endif

struct PSDFILEHEADER {
	BYTE		id[4];			//	0000 - 0003
	WORD		ver;			//	0004 - 0005
	BYTE		reserve[6];		//	0006 - 000b
	WORD		channel;		//	000c - 000d
	DWORD		ly;				//	000e - 0011
	DWORD		lx;				//	0012 - 0015
	WORD		depth;			//	0016 - 0017
	WORD		mode;			//	0018 - 0019
};

struct PSDCOLORMODE {
	DWORD		len;
	BYTE		data[1];
};

struct PSDIMAGERES {
	DWORD		len;
	BYTE		data[1];
};

struct PSDLAYER {
	DWORD		len;
	BYTE		data[1];		// PSDLAYERINFO	/ PSDGLOBALLAYERMASK
};

struct PSDLAYERINFO {
	DWORD		len;
	BYTE		data[1];		// PSDLAYERSTRC / PSDCHANNELIMAGEDATA
};

struct PSDLAYERSTRC {
	SINT16		cnt;
	BYTE		data[1];		// PSDLAYERREC / PSDLAYERREC2
};

struct PSDLAYERREC {
	SINT32		sy;
	SINT32		sx;
	SINT32		ey;
	SINT32		ex;
	WORD		channel;
	BYTE		data[1];		// PSDCHANNELLEN
};

struct PSDLAYERREC2 {
	BYTE		bid[4];
	BYTE		bkey[4];
	BYTE		op;
	BYTE		clip;
	BYTE		flag;
	BYTE		filter;
	DWORD		exsize;
	BYTE		data[1];		// PSDLAYERMASK / PSDLAYERBLENDRANGE / layer name / ...
};

struct PSDCHANNELLEN {
	WORD		id;
	DWORD		len;
};

struct PSDLAYERMASK {
	DWORD		size;		// size of struct data
	SINT32		sy;
	SINT32		sx;
	SINT32		ey;
	SINT32		ex;
	BYTE		defcolor;
	BYTE		flag;
	WORD		padding;
	BYTE		realflag;
	BYTE		maskback;
	SINT32		sy2;
	SINT32		sx2;
	SINT32		ey2;
	SINT32		ex2;
} PSDLAYERMASK;

struct PSDLAYERBLENDRANGE {
	DWORD		len;
} PSDLAYERBLENDRANGE;

struct PSDCHANNELIMAGEDATA {
	WORD		comp;
	BYTE		data[1];
};

struct PSDGLOBALLAYERMASK {
	DWORD		len;
	WORD		overlaycol;
	BYTE		color[8];
	WORD		op;
	BYTE		kind;
	BYTE		data[1];
};

struct PSDIMAGEDATA
{
	WORD		comp;
	BYTE		data[1];
};


struct PSDFILETABLE {
	PSDFILEHEADER			*pfh;
	PSDCOLORMODE			*pcm;
	PSDIMAGERES				*pim;
	PSDLAYER				*ply;
	PSDLAYERINFO			*pli;
	PSDLAYERSTRC			*pls;
	PSDLAYERREC				*plr[400];
	PSDLAYERREC2			*plr2[400];
	PSDCHANNELLEN			*pcl[400][20];
	PSDLAYERMASK			*plm[400];
	PSDLAYERBLENDRANGE		*plbr[400];
	PSDCHANNELIMAGEDATA		*pcim;
	PSDGLOBALLAYERMASK		*pglm;
	PSDIMAGEDATA			*pid;

	DWORD					lnum;
	DWORD					sizeX;
	DWORD					sizeY;

	BYTE					*ldatp;
	DWORD					lsizeX[400];
	DWORD					lsizeY[400];
	DWORD					ldata[400][4];
	DWORD					lcsize[400][4];
	DWORD					lnlen[400];
	BYTE					lname[400][256];
	DWORD					ltype[400];
	DWORD					lfname[400];
	RECT					lrc[400];		// レイヤーサイズ
};

class CPSDLoader : public ICGLoaderBase {
protected:

public:
		CPSDLoader() { m_ext = _T("psd"); }
		int		LoadData	(ISurfaceInfo& rPixel, IDataStream* pData, int malloc_method = 0);
};
