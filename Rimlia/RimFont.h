#ifndef _RIM_FONT_
#define _RIM_FONT_

#include "MultiStdafx.h"
//#include "Sail\SailImageManager.h"
//#include <gdiplus.h>
#include "RimCGLoader.h"
//class ISurface;
//class CDIBSection;

//static const char* FONTNAME[4] = {"ＭＳ Ｐゴシック","ＭＳ Ｐ明朝","ＭＳ ゴシック","ＭＳ 明朝","メイリオ"};

#define _MAT2_NORMAL_ 0
#define _MAT2_MIRROR_ 1
#define _MAT2_BOTTOMUP_ 2

#ifndef _DALK_WINDOWS_
enum { FW_REGULAR };
enum { GGO_BITMAP, GGO_GRAY2_BITMAP, GGO_GRAY4_BITMAP , GGO_GRAY8_BITMAP };
#endif

inline UINT EncodeCharacter(char* str){
	return *str >= 0 ? *str : ( (*(UINT *)str) & 0x000000ff ) << 8 | ( ( (*(UINT *)str) & 0x0000ff00 ) >> 8) ;
}

class IRimFont {
public:
			IRimFont();
	virtual	~IRimFont() { this->Release(); };
protected:
//property
		int		m_Point;
		int		m_bItalic,m_bUnderLine,m_bStrong;
		DWORD	m_Weight;
		RGBQUAD	m_Color;
		TCHAR	m_Name[128];

public:

//管理系
virtual	int		Create		( LPCTSTR Name, const int Point, const int bItalic = 0, const int bUnderline = 0, const int bStrong = 0, DWORD Weight = FW_REGULAR){ return 0; };// = 0;
virtual	BOOL	Release() { return 0; };
virtual	void	SetColor	( BYTE r, BYTE g, BYTE b ) = 0;
inline	int		GetFontSize()	{ return m_Point; }

};

#ifdef _DALK_WINDOWS_

class CGdiPlusFont : public IRimFont {
protected:
	Gdiplus::Font*			m_GDIFont;
	Gdiplus::SolidBrush*	m_GDIBrush;
public:
		CGdiPlusFont();
//		~CGPFont()			{ Release(); };
		int		Create		( LPCTSTR Name, const int Point, const int bItalic = 0, const int bUnderline = 0, const int bStrong = 0, DWORD Weight = FW_REGULAR);
		bool	DrawText	( Gdiplus::Graphics* Graphics, char* wText, int x, int y );
//inline	bool	DrawText	( WCHAR* wText, RECT& Rect ) { return DrawText( wText, Gdiplus::RectF(Rect.left, Rect.top, Rect.right, Rect.bottom) ); }
	BOOL	Release();
	void	SetColor	( BYTE r, BYTE g, BYTE b );
};

//旧GDI墓場☆
#if 1
//HDCを外部参照するタイプ。
class CDCFont : public IRimFont {
public:
	CDCFont();
	CDCFont(HDC hDC);
//	~CDCFont();
protected:
	HFONT	m_hFont;
/*
	HFONT	m_hOldFont;
	HDC		m_hDC;
*/
public:
	int		Create		( LPCTSTR Name, const int Point, const int bItalic = 0, const int bUnderline = 0, const int bStrong = 0, DWORD Weight = FW_REGULAR);
	BOOL	Release();
	inline	HFONT	GetHandle(){ return m_hFont; }
/*
inline	BOOL	SetCharacterExtra		(int val)					{ return ::SetTextCharacterExtra(m_hDC,val) == 0x80000000 ? FALSE : TRUE; }
		void	SetColor				(BYTE r, BYTE g, BYTE b)	{ ::SetTextColor( m_hDC,RGB(r,g,b) ); }
inline	BOOL	TextOut					(char* text,int x,int y)	{ ::BeginPath(m_hDC); ::TextOut( m_hDC,x,y,text,strlen(text) ); ::EndPath(m_hDC); ::StrokeAndFillPath(m_hDC); return TRUE; }
*/
};

class CFont : public IRimFont/*, public ISurface*/ {
protected:
#ifdef _DALK_WINDOWS_
		HDC		m_hDC;
		HFONT	m_hFont;
		HFONT	m_hOldFont;
		GLYPHMETRICS	m_GlyphMetrics;
		MAT2			m_Matrix;
#endif
//Temp
		LONG			m_Ascent;
		UINT			m_Format;
		DWORD			m_Pitch;
		BYTE*			m_pGlyphBuffer;

		ISurfaceInfo	m_Info;

public:
		CFont();
//		~CFont();

//フォントのビットマップデータを内部バッファに格納。Shift-JISコードは事前にUINT形式にエンコードされている（前後の数値を入れ替える）こと。
virtual	DWORD	GetFontData	(UINT Code);
//virtual	DWORD	GetFontData	(UINT Code, BYTE* pBuffer, UINT pitch);
		
//管理系
	int		Create		(LPCTSTR Name, const int Point, const int bItalic = 0, const int bUnderline = 0, const int bStrong = 0, DWORD Weight = FW_REGULAR);
	BOOL	Release();
#ifdef _DALK_WINDOWS_
//	int		Blt			( CDIBSection* dib, int PosX, int PosY );
//	int		Blt			( ISurfaceInfo& pData, int PosX, int PosY );
#endif
//	int		Blt			( ISurface* pCG, int PosX, int PosY );

inline	ISurfaceInfo&	GetSurface() { return m_Info; }
	
	void	SetFormat	( unsigned int Depth );
	void	SetColor	( BYTE r, BYTE g, BYTE b )	{ m_Color.rgbBlue = b; m_Color.rgbRed = r; m_Color.rgbGreen = g; }
//		void	SetTextMatrix( MAT2 matrix );
};

//fontcacheのデータ構造：以下Width、Height、セル内x座標、セル内y座標、データの順で繰り返す。WidthとHeightは参照の際にDWORD境界に丸める必要あり
//外字：ASCII 0x01-0x20
/*
左右反転
mat.eM11 = DoubleToFixed(-1);
mat.eM12 = DoubleToFixed( 0);
mat.eM21 = DoubleToFixed( 0);
mat.eM22 = DoubleToFixed( 1);
*/

#endif

#endif

#endif