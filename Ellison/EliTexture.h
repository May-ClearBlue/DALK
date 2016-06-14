#ifndef _SAIL_TEXTURE_
#define _SAIL_TEXTURE_

#include <Siglune/SigDirect3D.h>
#include <Rimlia/RimDataStream.h>
#include <Rimlia/RimDIBSection.h>

#if defined(_DALK_WINDOWS_)
//static void (*fpPixelConvert[4][4])(void*&,void*&);

/*
__declspec(naked) int hoge( int src1, int src2 ){
    asm {
        mov     eax, dword ptr [esp + 4]  ; eax = src1
        add     eax, dword ptr [esp + 8]  ; eax += src2
        ret                               ; return
    }
}
*/
#pragma pack(push,1)
/*
	命名規則
		１０進数の数字と同じく、左側に上位ビットを書く。

		XRGB8888
		とあれば、上位からX,R,G,Bときて、
		それぞれが8,8,8,8ビットであることを意味します。
*/
struct ARGB1555{
	unsigned short Blue	:5;
	unsigned short Green:5;
	unsigned short Red	:5;
	unsigned short Alpha:1;
};

struct XRGB1555{
		unsigned short Blue	:5;
		unsigned short Green:5;
		unsigned short Red	:5;
		unsigned short Alpha:1;
};

struct RGB565{
	unsigned short Blue	:5;
	unsigned short Green:6;
	unsigned short Red	:5;
};

struct ARGB4444{
	unsigned short Blue	:4;
	unsigned short Green:4;
	unsigned short Red	:4;
	unsigned short Alpha:4;
};

struct ARGB8888{
	unsigned short Blue	:8;
	unsigned short Green:8;
	unsigned short Red	:8;
	unsigned short Alpha:8;
};

struct XRGB8888{
	unsigned short Blue	:8;
	unsigned short Green:8;
	unsigned short Red	:8;
 	unsigned short Alpha:8;
};

#pragma pack(pop)

inline void SetPixel(ARGB1555& rDest,RGBTRIPLE& rSrc,RGBTRIPLE& rMask){
	rDest.Red	= rSrc.rgbtRed	/ 8;
	rDest.Green	= rSrc.rgbtGreen/ 8;
	rDest.Blue	= rSrc.rgbtBlue	/ 8;

	rDest.Alpha = (rSrc.rgbtRed == rMask.rgbtRed && rSrc.rgbtGreen == rMask.rgbtGreen && rSrc.rgbtBlue == rMask.rgbtBlue) ? 0 : 1;
}

inline void SetPixel(ARGB1555& rDest,RGBQUAD& rSrc,RGBQUAD& rMask){
/*
	rDest.Red	= 31;
	rDest.Green	= 31;
	rDest.Blue	= 31;

	rDest.Alpha = 1;
/*/

	rDest.Red	= rSrc.rgbRed	/ 8;
	rDest.Green	= rSrc.rgbGreen	/ 8;
	rDest.Blue	= rSrc.rgbBlue	/ 8;

	rDest.Alpha = (rSrc.rgbRed == rMask.rgbRed && rSrc.rgbGreen == rMask.rgbGreen && rSrc.rgbBlue == rMask.rgbBlue) ? 0 : 1;

}

inline void SetPixel(ARGB8888& pDest,RGBTRIPLE& pSrc){
	pDest.Red	= pSrc.rgbtRed;
	pDest.Green	= pSrc.rgbtGreen;
	pDest.Blue	= pSrc.rgbtBlue;
	pDest.Alpha = 255;
}

inline void SetPixel(XRGB8888& pDest,RGBTRIPLE& pSrc){
	pDest.Red	= pSrc.rgbtRed;
	pDest.Green	= pSrc.rgbtGreen;
	pDest.Blue	= pSrc.rgbtBlue;
	pDest.Alpha = 255;
}

struct CTextureInfo {
	D3DFORMAT			Format;
	LPDIRECT3DSURFACE9	pLockSurface;
	LPDIRECT3DSURFACE9	pTempSurface;
	LPDIRECT3DSURFACE9	pOldRenderSurface;

};

void ConvertTextureFormat(_TEXTURE Texture, CTextureInfo& DestFormat, ISurfaceInfo& SrcFormat);

#endif

class CTexture : public ISurface {
public:
		CTexture();
		CTexture( UINT width, UINT height, int Format )	{ Create(width, height, Format); }
virtual	~CTexture()										{ Release(); }

protected:

//環境固有
#if defined(_DALK_USE_DIRECT3D_)
	BOOL				m_bLocked;
	D3DSURFACE_DESC		m_D3DDesc;
	D3DLOCKED_RECT		m_D3DRect;
#elif defined(_DALK_USE_OPENGL_)
	GLint	m_TextureStageHistory;			
#endif

//共通
	_TEXTURE	m_ObjectID;
	int			GetFormat(int Depth);
	int			m_Format;
	int			m_TextureWidth;
	int			m_TextureHeight;
	D3DPOOL		m_BufferType;				//GLでは意味はない
public:
inline	_TEXTURE&	GetTexture()		{ return m_ObjectID; }
inline	D3DPOOL		GetBufferType()		{ return m_BufferType; }
inline	int			GetWidth()			{ return m_TextureWidth; }
inline	int			GetHeight()			{ return m_TextureHeight; }

virtual	BOOL		Create	(UINT width, UINT height, int Format, D3DPOOL type = D3DPOOL_MANAGED, BOOL isRender = FALSE);
		int			Create();
		void		Release();
inline	void		ColorFill	(DWORD r, DWORD g, DWORD b, DWORD a, RECT* pRect = NULL) { ColorFill(D3DCOLOR_COLORVALUE(r,g,b,a), pRect); }
		void		Blt			(CTexture* pDest);
		void		GetRenderTargetData();
		void		ColorFill	(D3DCOLOR Color, RECT* pRect = NULL);
#if defined(_DALK_WINDOWS_)
inline	_RES		SetTexture	(int Stage = 0)	{ return CDirect3D::GetDevice()->SetTexture( Stage, m_ObjectID ); }
		int			Lock		(int left, int top, int right, int bottom);
		int			Lock();
		int			Unlock();
		_RES		GetSurface	(LPDIRECT3DSURFACE9& pSurface, int Level = 0) { return m_ObjectID->GetSurfaceLevel(Level, &pSurface); }
		void		SaveImageFile(LPCTSTR name, BOOL overwrite = TRUE);
#elif defined(_DALK_IOS_)
		void		PushTexture	(int Stage = 7);
		void		PopTexture();
		void		SetTexture	(int Stage = 0);
		_RES		GetSurface	(GLint& Surface, int Level = 0);
#endif
};

struct CRenderingTargetInfo {
	_RENDERBUFFER		m_RenderBuffer;
	D3DVIEWPORT9		m_ViewPort;
};

class CRenderingBuffer {
public:
	CRenderingBuffer();
protected:
	CRenderingTargetInfo			m_Info;
	vector<CRenderingTargetInfo>	m_InfoList;
public:
	BOOL	SetRenderTarget		(CTexture& tex, int Index = 0);
	BOOL	ResetRenderTarget	(int Index = 0);
};

#endif