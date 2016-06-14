#ifndef _RIM_DIBSECTION_
#define _RIM_DIBSECTION_

#include "RimFile.h"

#ifdef _DALK_WINDOWS_

#include <gdiplus.h>
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "gdiplus.lib")

#include "RimFont.h"
class CGPFont;
#endif

#include "RimCGLoader.h"
#include "RimImageLoader.h"

//ピクセルデータ操作用。３Ｄテクスチャでも使う
class ISurface {
public:
	ISurfaceInfo	m_Info;
	ISurface() { _ZEROINIT(m_Info); }
	virtual ~ISurface() { ; }

#ifdef _DALK_DEBUG_BUILD_
		_STRING	m_ResourceName;
		_STRING	GetResourceName()				{ return m_ResourceName; }
#endif

#if defined(_DALK_WINDOWS_)
inline	unsigned char*	GetPointer(UINT x,UINT y)	{ return (unsigned char*)m_Info.pData + ( (m_Info.Y - 1 - y) * m_Info.X + x) * m_Info.Depth / 8 ; }
inline 	unsigned char*	GetPointer(UINT y)			{ return (unsigned char*)m_Info.pData + ( (m_Info.Y - 1 - y) * m_Info.X)     * m_Info.Depth / 8 ; }
inline	void			SetPalette(UINT num, BYTE r, BYTE g, BYTE b) { SetRGBQUAD( m_Info.DIBPalette[num], r, g, b ); }
inline	RGBQUAD&		GetPalette(UINT num)		{ ; }
//上下反転
		void			ReversePixel();
#else
inline	unsigned char*	GetPointer(UINT x,UINT y)	{ return (unsigned char*)m_Info.pData + (m_Info.Y * m_Info.X + x) * m_Info.Depth / 8 ; }
inline 	unsigned char*	GetPointer(UINT y)			{ return (unsigned char*)m_Info.pData +  m_Info.Y * m_Info.X      * m_Info.Depth / 8 ; }
#endif
inline	ISurfaceInfo&	GetInfo()			{ return m_Info; }
inline	int		LoadImage(string_t& str)		{ return LoadImage(str.data()); }
inline	int		LoadImage(LPCTSTR path)	{ return LoadImageFile(path); }
inline	int		LoadImageFile(string_t& str)	{ return LoadImageFile(str.data()); }
		int		LoadImageFile(LPCTSTR path);
#if defined(_DALK_IOS_)
inline	int		LoadImageFile(NSString* path){ return CImageLoader::LoadImage(path, m_Info); }
#endif
		void	Init( UINT	X, UINT Y, UINT Depth, void* pData, int pitch = -1 );
virtual	void	SaveImageFile(LPCTSTR name, BOOL overwrite = TRUE);

//virtual	void	Blt(ISurfaceInfo* pImage) = 0;
};

//BMPファイル操作用。
#ifdef _DALK_WINDOWS_

//DC系クラスは整理＆仕様策定中
class CGDIPlus {
protected:
static	Gdiplus::GdiplusStartupInput m_StartupInput;
static	ULONG_PTR m_Token;

public:
static	bool Init();
static	void ShutDown();
};

class CMemoryDC {
protected:
	HDC	m_hDC;
	HFONT	hOrgFont;
	HBITMAP	hOrgBitmap;
	HPEN	hOrgPen;

public:
	CMemoryDC() { m_hDC = ::CreateCompatibleDC(NULL); }
};

#if 0
class CBitmapInfo {
public:
	CBitmapInfo() { Init(640,480,32,0); }
protected:
	BMPINFO	m_Info;
public:
inline	void		Init		(LONG x, LONG y, WORD bpp, DWORD palettenum = 0 )	{ SetBITMAPINFO( (BITMAPINFO&)m_Info, x, y, bpp, palettenum ); }
inline	void		SetPalette	(UINT num, BYTE r, BYTE g, BYTE b)	{ ; }
inline	BMPINFO&	GetObject()										{ return m_Info; }
};
#endif


//基本DIB操作クラス
//セーブが必要なデータ
/*
		BOOL	visible;
		string_t	filename,alphafilename;
public:
		int		posx,posy;

//管理系
inline	size_t	GetDataSize()								{ return X * Y * bpp / 8; }
inline	string_t&	GetName()									{ return filename; }
inline	void*	GetAlphaPointer()							{ return alphachannel; }
inline	void*	GetAlphaPointer	(const int sX,const int sY)	{ return alphachannel ? (unsigned char*)alphachannel + ( (Y - 1 - sY) * X + sX ) : NULL; }
inline	void	GetPos			(int& posx_,int& posy_)		{ posx_ = posx; posy_ = posy; }
virtual	void	SetPos			(int posx_,int posy_)		{ posx = posx_; posy = posy_; }
inline	BOOL	IsVisible()									{ return visible; }
inline	void	SetVisible		(BOOL stat)					{ visible = stat; } 
inline	void	Snap			(DIB* dib,int pos)			{ posy = dib->X - X; posx = pos - (X / 2); }//posを画像の中心x座標としてスナップ（立ちキャラ用関数）

virtual	HDC		GetDC()										{ return NULL; }

virtual	int		Create		(const int sX,const int sY,const int sbpp);
virtual	BOOL	Release()									{ return ( HeapFree(pixeldata) && HeapFree(alphachannel) ) ? TRUE : FALSE; }

//blt系
virtual	void	AlphaBlt	(DIB* dib,int dest_x = -1,int dest_y = -1);
virtual void	AlphaBlt	(DIB* dib,int dest_x,int dest_y,int width,int height,int src_x,int src_y);
virtual void	Blt			(DIB* dib);
virtual	void	Blt			(DIB* dib,int x,int y,int width,int height,int sx,int sy);
virtual	void	Blt			(HDC hdc,int x,int y,int width,int height,int sx,int sy,DWORD rop = SRCCOPY);
virtual	void	StretchBlt	(DIB* dib,int x,int y,int width,int height,int sx,int sy,int swidth,int sheight)	{ StretchBlt(dib->GetDC(),x,y,width,height,sx,sy,swidth,sheight); }
virtual	void	StretchBlt	(HDC hdc,int x,int y,int width,int height,int sx,int sy,int swidth,int sheight,DWORD rop = SRCCOPY);

virtual void	Copy		(DIB* dib,int dest_x,int dest_y);
virtual	void	DrawRect	(DIB* sib,int drawleft,int drawtop,int drawwidth,int drawheight);

virtual	void	GrayScale();//フィルタリングするように変更
virtual	void	Sepia();	//フィルタリングするように変更（データは自体は変更しないで、Blt結果をフィルタリング

//ローダー系
static	void	InitLoader()	{ loadermanager.Init(); }
virtual	BOOL	Load				(BINFILE* binfile);
virtual	BOOL	Load				(LPCTSTR name);
		BOOL	LoadAlphachannel	(BINFILE* binfile,unsigned bpp = 64);
		BOOL	LoadAlphachannel	(char* name,unsigned bpp = 64);
		void	SaveFile			(LPCTSTR name);
		void	SaveAlphachannelFile(LPCTSTR name);

//		DIB(DIB& dib)	{ Create(dib.X,dib.Y,dib.bpp); memcpy(pixeldata,dib.GetPointer(),dib.X * dib.Y * dib.bpp / 8); }
		DIB()			{ X = 0; Y = 0; bpp = 0; posx = 0; posy = 0; pixeldata = NULL; alphachannel = NULL; selector = 0x000000FF;  }
virtual ~DIB()			{ Release(); }

};
*/

//WinAPI依存のDIB操作クラス。
class CDIBSection : public ISurface {
protected:
		Gdiplus::Graphics*	m_GDIGraphics;

//		CGPFont				m_GDIFont;

		BLENDFUNCTION		m_BrendFunction;
//		void*		m_AlphaData;	//アルファチャンネル

		HBITMAP	m_HDefaultBitmap;	//CreateCompatibleDCのデフォルトBMP
		HBITMAP	m_HBitmap;			//作成するBMP
		HDC		m_HDC;				//作成するメモリDC
public:
		CDIBSection()	;
		~CDIBSection()	{ Release(); }

//		void	Blt(ISurfaceInfo* pImage)	{ ; };

inline	void	Blt			(CDIBSection* dib,int x = 0,int y = 0, DWORD rop = SRCCOPY)														{ ::AlphaBlend(dib->GetDC(), x, y, m_Info.X, m_Info.Y,m_HDC, 0, 0, m_Info.X ,m_Info.Y ,m_BrendFunction); }
inline	void	Blt			(CDIBSection* dib,int x,int y,int width,int height,int sx,int sy,DWORD rop = SRCCOPY)							{ ::BitBlt(dib->GetDC(),x,y,width,height,m_HDC,sx,sy,rop); }
inline	void	StretchBlt	(CDIBSection* dib,int x,int y,int width,int height,int sx,int sy,int swidth,int sheight,DWORD rop = SRCCOPY)	{ ::StretchBlt(dib->GetDC(),x,y,width,height,m_HDC,sx,sy,swidth,sheight,rop); }
inline	void	Blt			(CDIBSection* dib,RECT* rect,DWORD rop = SRCCOPY)																{ ::BitBlt(dib->GetDC(),rect->left,rect->top,rect->right - rect->left + 1,rect->bottom - rect->top + 1,m_HDC,rect->left,rect->top,rop); }
inline	void	FillRect	( int x,int y,int width,int height, DWORD rop = WHITENESS )						{ ::BitBlt(m_HDC,x,y,width,height,m_HDC,x,y,rop); }
inline	void	Fill		( bool isBlack = true )															{ ::BitBlt(m_HDC,0,0,m_Info.X,m_Info.Y,m_HDC,0,0, isBlack ? BLACKNESS : WHITENESS); }

		BOOL	SetFont		(const char *Name, const int Point, const int bItalic = 0, const int bUnderline = 0, const int bStrong = 0, DWORD Weight = FW_REGULAR);
		BOOL	TextOut		(HFONT hFont, char* text, int x, int y);
		BOOL	DrawText	(CGPFont& pFont, char* text, int x, int y);

		BOOL	DrawString	(char* text, int x, int y);

		void	SetPixel	(int x,int y);
		int		Create		(const int sX,const int sY,const int sbpp);//空のBMPを作成
		int		Create		( ISurfaceInfo& iInfo );
		BOOL	Release();

inline	HDC					GetDC()	{ return m_HDC; }
inline	Gdiplus::Graphics*	GetGDIGraphics() { return m_GDIGraphics; }
inline	HBITMAP	GetHBITMAP() { return m_HBitmap; }
};

#endif

#endif