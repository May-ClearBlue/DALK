#include "MultiStdafx.h"

#include "RimFont.h"

#include <cstdlib>

IRimFont::IRimFont(){ 
m_Point = 0;			//文字サイズ、文字間隔
m_bItalic = 0;
m_bUnderLine = FALSE;
m_bStrong = FALSE;
m_Weight = 0;
}

#ifdef _DALK_WINDOWS_

CGdiPlusFont::CGdiPlusFont() {
	m_GDIFont = NULL;
	m_GDIBrush = new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255));;
	SetRGBQUAD(m_Color,255,255,255);
}

int CGdiPlusFont::Create(LPCTSTR Name, const int Point, const int bItalic, const int bUnderLine, const int bStrong, DWORD Weight){
	if( m_GDIFont != NULL )
		SafeDelete(m_GDIFont);

#ifdef _DALK_WIDECHAR_MODE_
	m_GDIFont = new Gdiplus::Font(Name, (float)Point);
#else
	size_t len;
	wchar_t Buf[64];

	errno_t err = mbstowcs_s(&len, Buf, 64 , Name, _TRUNCATE);
	m_GDIFont = new Gdiplus::Font(Buf, (float)Point);
#endif
	_STRCPY(m_Name, Name);

	m_Point = Point;

	return 0;
}

bool CGdiPlusFont::DrawText(Gdiplus::Graphics* Graphics, char* text, int x, int y) {
	WCHAR pChar[256] = { 0 };
	size_t len;
	errno_t err = mbstowcs_s(&len, pChar, strlen(text) * sizeof(WCHAR) , text, _TRUNCATE);

	Gdiplus::Status ret;

	Gdiplus::PointF pt((float)x, (float)y);
	Gdiplus::StringFormat format;

	ret = Graphics->DrawString(pChar, -1, m_GDIFont, pt, &Gdiplus::StringFormat(), m_GDIBrush);

//	delete[] pChar;
	return true;
}

BOOL CGdiPlusFont::Release(){
	SafeDelete(m_GDIFont);
	SafeDelete(m_GDIBrush);
	return TRUE;
}

void CGdiPlusFont::SetColor(BYTE r, BYTE g, BYTE b) {
	m_GDIBrush->SetColor(Gdiplus::Color(255, r, g, b));
	SetRGBQUAD(m_Color,r,g,b);
}

#if 1
CDCFont::CDCFont() {
//	m_hDC = NULL;
//	m_hOldFont = NULL;
	m_hFont = NULL;
}

CDCFont::CDCFont(HDC hDC) {
//	m_hDC = hDC;
//	m_hOldFont = NULL;
	m_hFont = NULL;
}

int CDCFont::Create(LPCTSTR Name,const int Point,const int bItalic,const int bUnderLine,const int bStrong,DWORD Weight){
	if( m_hFont != NULL )
		Release(); /*Delete();*/

	m_hFont = ::CreateFont(Point,0,0,0,Weight,bItalic,bUnderLine,bStrong,SHIFTJIS_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY | PROOF_QUALITY,FIXED_PITCH | FF_MODERN,Name);

	if (m_hFont == NULL)
		return 1;

	m_Point		= Point;
	m_bItalic	= bItalic;
	m_bUnderLine= bUnderLine;
	m_bStrong	= bStrong;
	m_Weight	= Weight;
	_STRCPY( m_Name,Name );
#if 0
	if( m_hDC == NULL)
		return 2;

	m_hOldFont = (HFONT)::SelectObject(m_hDC,m_hFont);
	::SetBkMode(m_hDC,TRANSPARENT);
	DWORD r = ::SetMapperFlags(m_hDC,1);
#endif
	return 0;
}

BOOL CDCFont::Release(){
	if(m_hFont != NULL){
#if 0
		::SelectObject	(m_hDC,m_hOldFont);
		m_hOldFont	= NULL;
#endif
		IRimFont::Release();
		return TRUE;
	}
	return FALSE;
}
#endif



CFont::CFont():
#ifdef _DALK_WINDOWS_
m_hDC(NULL),
m_hOldFont(NULL),
#endif
m_Ascent(0),
m_Format(GGO_GRAY8_BITMAP),
m_pGlyphBuffer(NULL){
#ifdef _DALK_WINDOWS_
	MAT2 InitMatrix = { {0,1}, {0,0}, {0,0}, {0,1} };
	m_Matrix	= InitMatrix;
#endif
}

int CFont::Create(LPCTSTR Name, const int Point, const int bItalic, const int bUnderLine, const int bStrong, DWORD Weight){
#ifdef _DALK_WINDOWS_
	if(IRimFont::Create(Name, Point, bItalic, bUnderLine, bStrong, Weight) == 0 ) {
		if( (m_hDC = ::CreateCompatibleDC(NULL)/*;GetDC(sm_hWnd)*/ ) != NULL) {
			m_hOldFont = (HFONT)::SelectObject(m_hDC,m_hFont);

			TEXTMETRIC tm;
			m_Ascent	= ::GetTextMetrics(m_hDC,&tm) ? tm.tmAscent : 0;

			::SetBkMode(m_hDC,TRANSPARENT);
			::SetMapperFlags(m_hDC,1);

			m_pGlyphBuffer = new BYTE[Point * Point * 2];

			return 0;
		}
		return 1;

	}
	return 2;
#endif
}

BOOL CFont::Release(){
#ifdef _DALK_WINDOWS_
	if(m_hFont != NULL){
		::SelectObject	(m_hDC,m_hOldFont);
		::DeleteDC		(m_hDC);

		m_hOldFont	= NULL;
		IRimFont::Release();

		SafeDelete (m_pGlyphBuffer);

		return TRUE;
	}
	return FALSE;
#endif
}

DWORD CFont::GetFontData(UINT Code){
#ifdef _DALK_WINDOWS_
	::ZeroMemory(&m_GlyphMetrics, sizeof(m_GlyphMetrics));

	DWORD Size = ::GetGlyphOutline( m_hDC, Code, m_Format, &m_GlyphMetrics, 0, NULL, &m_Matrix);
	::GetGlyphOutline( m_hDC, Code, m_Format, &m_GlyphMetrics, Size, m_pGlyphBuffer, &m_Matrix);

	m_Pitch = /*m_GlyphMetrics.gmBlackBoxX = */(Size / m_GlyphMetrics.gmBlackBoxY) & ~0x03;
/*
	fontbmpinfo->width	= gm.gmBlackBoxX;
	fontbmpinfo->height	= gm.gmBlackBoxY;
	fontbmpinfo->posx	= gm.gmptGlyphOrigin.x;
	fontbmpinfo->posy	= ascent - gm.gmptGlyphOrigin.y;
*/
//あらかじめ8bitにインデックスを伸張
	for(unsigned int a = 0;a < m_GlyphMetrics.gmBlackBoxY * m_GlyphMetrics.gmBlackBoxY;a ++) {
		if(m_pGlyphBuffer[a] <= 64)
			m_pGlyphBuffer[a] <<= 2;
		else
			m_pGlyphBuffer[a] = 255;
		
	}

	m_Info.Depth = 8;
	m_Info.pData = m_pGlyphBuffer;
	m_Info.Pitch = m_Pitch;
	m_Info.X = m_GlyphMetrics.gmBlackBoxX;
	m_Info.Y = m_GlyphMetrics.gmBlackBoxY;

	return Size;
#endif
}

#ifdef _DALK_WINDOWS_

#if 0
int CFont::Blt(ISurfaceInfo& dib,int PosX,int PosY){
	//PosX += m_GlyphMetrics.gmptGlyphOrigin.x;
	PosY += (m_Ascent - m_GlyphMetrics.gmptGlyphOrigin.y);

	BMPINFO	bitmapinfo;
	SetBITMAPINFO((BITMAPINFO&)bitmapinfo,m_GlyphMetrics.gmBlackBoxX,m_GlyphMetrics.gmBlackBoxY,8);

	switch(m_Format){
		case GGO_BITMAP:
			bitmapinfo.bmiColors[0].rgbBlue = 0;
			bitmapinfo.bmiColors[0].rgbRed = 0;
			bitmapinfo.bmiColors[0].rgbGreen= 0;
			bitmapinfo.bmiColors[0].rgbReserved = 0;
			bitmapinfo.bmiColors[1].rgbBlue = 255;
			bitmapinfo.bmiColors[1].rgbRed = 255;
			bitmapinfo.bmiColors[1].rgbGreen= 255;
			bitmapinfo.bmiColors[1].rgbReserved = 255;
		break;
	}
/*
	bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfo.bmiHeader.biWidth = m_GlyphMetrics.gmBlackBoxX;
	bitmapinfo.bmiHeader.biHeight = m_GlyphMetrics.gmBlackBoxY;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biBitCount = m_Format == (GGO_BITMAP ? 1 : 8);
	bitmapinfo.bmiHeader.biCompression = BI_RGB;
	bitmapinfo.bmiHeader.biSizeImage = (m_GlyphMetrics.gmBlackBoxX * m_GlyphMetrics.gmBlackBoxY * bitmapinfo.bmiHeader.biBitCount) / 8;
	bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapinfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapinfo.bmiHeader.biClrUsed = 0;
	bitmapinfo.bmiHeader.biClrImportant = 0;
*/

	BYTE* pData = m_pGlyphBuffer;
//	COLORREF cres;

	for(unsigned int a = 0;a < m_GlyphMetrics.gmBlackBoxY;a ++){
		for(unsigned int b = 0;b < m_GlyphMetrics.gmBlackBoxX;b ++){
				long shift = 7 - (b & 7);
				long no = b >> 3;
				if( ((pData[no] >> shift) & 0x01) )
					dib->SetPixel(PosX + b,PosY + a);
		}
		pData += m_Pitch;//m_GlyphMetrics.gmBlackBoxX;
	}

	//int res =  ::SetDIBitsToDevice(dc,PosX,PosY,m_GlyphMetrics.gmBlackBoxX,m_GlyphMetrics.gmBlackBoxY,0,0,0,m_GlyphMetrics.gmBlackBoxY,m_pGlyphBuffer,(BITMAPINFO*)&bitmapinfo,DIB_RGB_COLORS);
	return 1;
}
#endif

#if 0
int CFont::Blt( ISurfaceInfo pInfo, int PosX, int PosY ) {
	/*
	BYTE a = 255;
	BYTE r = 255;
	BYTE g = 255;
	BYTE b = 255;
	DWORD c2  = 0xFFFFFFFF;

	_asm {
		__m64w alpha = { a, a, a, 0 };
		__m64w c1m   = { b, g, r, 0 };
		__movq(__mm0, alpha);
		__movq(__mm1, c1m); 
		__movd(__mm2, c2); 
		__punpcklbw(__mm2, __mm3); 
		__paddw(__mm2, __mm1);
		__psrlw(__mm2, 8); 
		__movd(c2, __mm2); 
	}
	*/
	return 1;
}
#endif

void CFont::SetFormat(unsigned int Depth){
	switch(Depth){
	case 2:
		m_Format = GGO_GRAY2_BITMAP;
		break;
	case 4:
		m_Format = GGO_GRAY4_BITMAP;
		break;
	case 8:
		m_Format = GGO_GRAY8_BITMAP;
		break;
	default:
		m_Format = GGO_BITMAP;
		break;
	}
}

#endif
#endif