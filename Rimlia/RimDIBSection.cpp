#include "MultiStdafx.h"

#include "RimDIBSection.h"

#include "RimImageLoader.h"
#include "RimImageSaver.h"

#if defined(_DALK_WINDOWS_)

ULONG_PTR CGDIPlus::m_Token = NULL;
Gdiplus::GdiplusStartupInput CGDIPlus::m_StartupInput;

bool CGDIPlus::Init() {
    if (Gdiplus::GdiplusStartup(&m_Token, &m_StartupInput, NULL) != Gdiplus::Ok)
        return false;
    return true;
}

void CGDIPlus::ShutDown() {
	if(m_Token != NULL)
		Gdiplus::GdiplusShutdown(m_Token);
}
void ISurface::ReversePixel() {
	void* buf;

	_VALLOC(buf, m_Info.Pitch * m_Info.Y);

	_MEMCPY(buf, m_Info.pData, m_Info.Pitch * m_Info.Y);

	BYTE	*pSrc = (BYTE*)buf, *pDest = GetPointer(0);

	for(int a = 0;a < m_Info.Y; a++) {
		BYTE* pDest = GetPointer(a);
		_MEMCPY(pDest, pSrc, m_Info.Pitch );
		pSrc += m_Info.Pitch;
	}

	_VFREE(buf);
}
#endif

void ISurface::Init( UINT	X, UINT Y, UINT Depth, void* pData, int pitch) {
	m_Info.X = X;
	m_Info.Y = Y;
	m_Info.Depth = Depth;
	m_Info.pData = pData;

	if(pitch <= 0)
		m_Info.Pitch = X * Depth / 8;
}

int ISurface::LoadImageFile(LPCTSTR path) {
#ifdef _DALK_DEBUG_BUILD_
	int r = CImageLoader::LoadImage(path, m_Info);
	if(r == 0)
		m_ResourceName = path;
	return r;
#else
	return CImageLoader::LoadImage(path, m_Info);
#endif	
}

void ISurface::SaveImageFile(LPCTSTR name,BOOL overwrite){
	CPathString ps(name);

	_STRING ext = ps.GetExtentString();
//#if defined(_DALK_WINDOWS_)
//#elif defined(_DALK_IOS_)
//    NSString* temp = ps.GetExtentString();
//	string_t ext;
//    DalkString::NSStringToString(ext, temp);
//#endif
	if(ext == _T("bmp") || ext == _T("BMP") )
		SaveBMP(m_Info, 0 , name);
#ifdef _DALK_USE_DDS_
	else if(ext == _T("dds") || ext == _T("DDS")) 
		SaveDDS(m_Info, 0 , name);
#endif
#ifdef _DALK_USE_PNG_
	else if(ext == _T("png") || ext == _T("PNG") )
		SavePNG(m_Info, 0 , name);
#endif
	else
		SaveBMP(m_Info, 0 , name);
#if 0
	CFile file;
	file.Create(name,TRUE);

	BITMAPFILEHEADER bmfh;
	ZeroMemory(&bmfh,sizeof(bmfh));

	BMPINFO bmpi;
	SetBITMAPINFO((BITMAPINFO&)bmpi,m_Info.X,m_Info.Y,m_Info.Depth,0);

	SetRGBQUAD(bmpi.bmiColors[0],0,0,1);
	SetRGBQUAD(bmpi.bmiColors[1],255,255,255);

	file.Write(sizeof(bmfh),&bmfh);
	file.Write(sizeof(bmpi.bmiHeader),&(bmpi.bmiHeader));

	DWORD Palette = 1;
	switch(m_Info.Depth){
	case 1:
		Palette = 2;
		break;
	case 4:
		Palette = 16;
		break;
	case 8:
		Palette = 256;
	}

	file.Write(Palette * 4,&bmpi.bmiColors);

	bmfh.bfOffBits = file.GetFileSize();

	file.Write(m_Info.X * m_Info.Y * m_Info.Depth / 8 ,m_Info.pData);

	bmfh.bfSize = file.GetFileSize();
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfType = 0x4D42;

	file.SeekBegin(0);
	file.Write(sizeof(bmfh),&bmfh);

	//file.Close();
#endif

}

#if defined(_DALK_WINDOWS_)
int CDIBSection::Create(const int sX,const int sY,const int sbpp){
	BMPINFO BmpInfo;

	SetBITMAPINFO((BITMAPINFO &)BmpInfo,sX,sY,sbpp);

	switch(sbpp){
	case 1:
		SetRGBQUAD(BmpInfo.bmiColors[0],0,0,0);
		SetRGBQUAD(BmpInfo.bmiColors[1],255,255,255);
		break;
	case 4:
	case 8:
		for(int a = 0;a < 255;a ++)
			SetRGBQUAD(BmpInfo.bmiColors[a],a,a,a);
		break;
	}

	void* p;

	if( (m_HBitmap = ::CreateDIBSection(NULL,(BITMAPINFO *)&BmpInfo,DIB_RGB_COLORS,&p,NULL,0)) == NULL )
		return 1;

	else if( (m_HDC = ::CreateCompatibleDC(NULL)) == FALSE )
		return 2;

//GDI+のnewを使うと何故かエラーになるので通常のnewで。deleteも対応させる。
	m_HDefaultBitmap = SelectBitmap(m_HDC,m_HBitmap);
	::SetStretchBltMode(m_HDC,HALFTONE);

	m_GDIGraphics = new Gdiplus::Graphics(m_HDC,0);

	Init(sX, sY, sbpp, p);

	return 0;
}

int CDIBSection::Create( ISurfaceInfo& iInfo ) {
	int r = Create(iInfo.X,iInfo.Y,iInfo.Depth);
	if( r != 0 )
		return r;

	BYTE* pSrc = (BYTE*)iInfo.pData;

	for(int a = 0;a < iInfo.Y; pSrc += iInfo.Pitch, a++)
		memcpy( GetPointer(a), pSrc , iInfo.Pitch);

	return 0;
}

BOOL CDIBSection::Release(){
	if( m_GDIGraphics != NULL ) {
		delete m_GDIGraphics;
		m_GDIGraphics = NULL;
	//m_GDIGraphics->ReleaseHDC();
	}
	if( m_HDefaultBitmap != NULL ) {
		SelectBitmap(m_HDC, m_HDefaultBitmap);
		
		m_HDefaultBitmap = NULL;

		if( ::DeleteObject(m_HBitmap) ){
		//_ASSERT(bres);
			m_HBitmap = NULL;

			if( ::DeleteDC(m_HDC) ){
				//_ASSERT(bres);
	
				m_HDC = NULL;
				return TRUE;
			}
		}
	}
	return FALSE;
}

CDIBSection::CDIBSection() {
	m_HDefaultBitmap = NULL;
	m_HBitmap = NULL;
	m_HDC = NULL;
	m_GDIGraphics = NULL;
//	m_GDIFont = NULL;
	m_BrendFunction.BlendOp = AC_SRC_OVER;
	m_BrendFunction.BlendFlags = 0;
	m_BrendFunction.AlphaFormat = AC_SRC_ALPHA;
	m_BrendFunction.SourceConstantAlpha = 255;
}

BOOL CDIBSection::SetFont( const char *Name, const int Point, const int bItalic, const int bUnderline, const int bStrong, DWORD Weight) {
//	m_GDIFont.Create(Name,Point,bItalic,bUnderline,bStrong,Weight);
	return TRUE;
}

		
BOOL CDIBSection::TextOut( HFONT hFont, char* text, int x, int y) {
#if 0
	HFONT hTemp = (HFONT)::SelectObject(m_HDC, hFont);
	if(hTemp == NULL)
		return FALSE;
#endif
// パスを終了
// パスから一部の領域を抽出
// 次のコードでも SelectClipPath() と同じ結果が得られる
// SelectClipRgn(hdc, PathToRegion(hdc));
// 得られた領域をグレイで塗りつぶす

	RECT r = { 0,0,1280,720};
	
//	::SetTextColor( m_HDC, 0xFFFFFFFF );
//	::BeginPath(m_HDC);
//	BOOL res = ::TextOut( m_HDC, x, y, text, strlen(text) );
//	::EndPath(m_HDC);
//	::SelectClipPath(m_HDC, RGN_AND);
	::FillRect(m_HDC, &r, (HBRUSH)GetStockObject(GRAY_BRUSH));

#if 0
	::SelectObject(m_HDC,hTemp);
#endif
	return TRUE;
}

BOOL CDIBSection::DrawText(CGPFont& pFont, char* text, int x, int y) {
#if 0
	WCHAR* pChar = new WCHAR(strlen(text) + 1);
	Gdiplus::RectF Rect(x, y, strlen(text) * pFont.GetFontSize(), pFont.GetFontSize());
	Gdiplus::StringFormat format;
	m_GDIGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
	m_GDIGraphics->DrawString(pChar, -1, &nt, Rect, &format, &brush);

	delete[] pChar;
#endif
	WCHAR pChar[256] = { 0 };
	size_t len;
	errno_t err = mbstowcs_s(&len, pChar, strlen(text) * sizeof(WCHAR) , text, _TRUNCATE);

	Gdiplus::Status ret;

	Gdiplus::PointF pt((float)x, (float)y);
	Gdiplus::StringFormat format;

    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 255));

//	ret = m_GDIGraphics->DrawString(pChar, -1, m_GDIFont, pt, &Gdiplus::StringFormat(), &brush);

	return TRUE;
}

BOOL CDIBSection::DrawString(char* text, int x, int y) {
	return TRUE;
}

void CDIBSection::SetPixel(int x,int y){
	BYTE* pPixel = (BYTE*)m_Info.pData;
	*(pPixel +( (m_Info.Y - 1 - y) *  m_Info.X / 8) + x / 8) |= (0x80 >> x % 8);
}

#endif
