#include "MultiStdAfx.h"

#include "EliTexture.h"

void ConvertTextureFormat(_TEXTURE Texture,CTextureInfo& DestFormat,ISurface& SrcFormat){
	//BMPは天地逆だがTextureは正方向
	D3DLOCKED_RECT D3DRect;
	_ZEROINIT(D3DRect);

	Texture->LockRect(0,&D3DRect,NULL,D3DLOCK_NOSYSLOCK);

	BYTE* pSrc = (BYTE*)SrcFormat.GetPointer(0,0);
	BYTE* pDest= (BYTE*)D3DRect.pBits;
/*
	if(DestFormat.Depth == SrcFormat.Depth){
		for(UINT a = 0;a < SrcFormat.Y;a ++){
			::CopyMemory(pDest,pSrc,DestFormat.X * DestFormat.Depth / 8);

			pDest += D3DRect.Pitch;
			pSrc -= SrcFormat.Pitch;
		}

		return;
	}
*/
	//0,0のピクセルを透過色に
	RGBQUAD& rMaskPalette = SrcFormat.m_Info.DIBPalette[*pSrc];

	switch(DestFormat.Format){
	case D3DFMT_A1R5G5B5:
		switch(SrcFormat.m_Info.Depth){
		case 8:
			for(int a = 0;a < SrcFormat.m_Info.Y;a ++){
				for(int b = 0;b < SrcFormat.m_Info.X;b ++)
					SetPixel( ( (ARGB1555*)pDest )[b], SrcFormat.m_Info.DIBPalette[pSrc[b]],rMaskPalette );
	
				pDest += D3DRect.Pitch;
				pSrc -= SrcFormat.m_Info.Pitch;
			}
			break;

		case 24:
			//0,0のピクセルを透過色に
			RGBTRIPLE& rMask = ( (RGBTRIPLE*)pSrc )[0];

			for(int a = 0;a < SrcFormat.m_Info.Y;a ++){
				for(int b = 0;b < SrcFormat.m_Info.X;b ++)
					SetPixel( ( (ARGB1555*)pDest )[b],( (RGBTRIPLE*)pSrc )[b],rMask );
	
				pDest += D3DRect.Pitch;
				pSrc -= SrcFormat.m_Info.Pitch;
			}
			break;
		}
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8R8G8B8:
		switch(SrcFormat.m_Info.Depth){
		case 24:
			for(int a = 0;a < SrcFormat.m_Info.Y;a ++){
				for(int b = 0;b < SrcFormat.m_Info.X;b ++)
					SetPixel( ( (XRGB8888*)pDest )[b],( (RGBTRIPLE*)pSrc )[b] );
				pDest += D3DRect.Pitch;
				pSrc -= SrcFormat.m_Info.Pitch;
			}
			break;
		case 32:
			for(int a = 0;a < SrcFormat.m_Info.Y;a ++) {
				memcpy( pDest, pSrc, D3DRect.Pitch );
				pDest += D3DRect.Pitch;
				pSrc -= SrcFormat.m_Info.Pitch;
			}
		}
		break;
	}
			/*
			DWORD loopcount = SrcFormat.X - 1;
			_asm{
				mov edi,pDest
				mov esi,pSrc
				mov ecx,loopcount
CTFLoop:
				mov ebx,[esi]	//Src転送
				and	ebx,0x00FFFFFF
				mov [edi],ebx	//pBufferにストア

				add edi,4
				add esi,3

				loop CTFLoop

				dec esi

				mov ebx,[esi]	//Src転送
				shr ebx,8
				and	ebx,0x00FFFFFF
				mov [edi],ebx	//pBufferにストア
			}
			pDest += D3DRect.Pitch;
			pSrc -= SrcFormat.Pitch;
		}
	}
			*/
	Texture->UnlockRect(0);
}

CTexture::CTexture()
: m_ObjectID(NULL), m_Format(D3DFMT_UNKNOWN), m_TextureWidth(-1), m_TextureHeight(-1), m_bLocked(FALSE), m_BufferType(D3DPOOL_FORCE_DWORD) {
	;
}

int CTexture::GetFormat( int Depth ) {
//基本的に8bitαと32bitRGBA以外はサポートしない
// D3DFMT_P8
// D3DFMT_X1R5G5B5:
// D3DFMT_A1R5G5B5:
// D3DFMT_R5G6B5:
// D3DFMT_R8G8B8:
// D3DFMT_X8R8G8B8:

	switch( Depth ) {
	case 8:
#ifdef _DALK_WINDOWS_
		return D3DFMT_A8;
#else
		return D3DFMT_L8;
#endif
	case 24:
		return D3DFMT_R8G8B8;
	case 32:
		return D3DFMT_A8R8G8B8;
	}
	return -1;
}

BOOL CTexture::Create(UINT width,UINT height, int Format, D3DPOOL type, BOOL isRender) {
#if 1
	UINT tw = 1, th=1;

	while(width > tw)
		{ tw *= 2; }
	while(height > th)
		{ th *= 2; }
#else
	UINT tw = width, th=height;
#endif

	_RES r;

	if(isRender)
		type = D3DPOOL_DEFAULT;

	r = CDirect3D::GetDevice()->CreateTexture( tw,th, 1, isRender ? D3DUSAGE_RENDERTARGET : 0,  (D3DFORMAT)Format, type/*isRender ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED*/, &m_ObjectID, NULL);

	SIG_ERRORRETURNBOOL(r,TEXT("Failed@CTexture::Create"));

	m_TextureWidth	= tw;
	m_TextureHeight	= th;
	m_Format		= Format;
	m_BufferType	= type;
//	m_IsRenderTarget = isRender;

	if(isRender) {
		m_Info.X		= width;
		m_Info.Y		= height;
		m_Info.Depth	= 32;
	}
//	m_TextureInfo = TextureInfo;

	return TRUE;
}

int CTexture::Create() {
	if(m_ObjectID) {
		_DEBUGPRINTF(TEXT("Texture overwrite@CTexture::Create"));
		Release();
	}

	if(Create( m_Info.X, m_Info.Y, GetFormat(m_Info.Depth) ) == FALSE)
		return 1;
	
	Lock();

	BYTE* pDest = (BYTE*)m_D3DRect.pBits;

	for(int a = 0;a < m_Info.Y; a++, pDest += m_D3DRect.Pitch)
		memcpy(pDest, GetPointer(a), m_Info.X * m_Info.Depth / 8);

#if 0
	m_Info.Pitch = m_D3DRect.Pitch;
	m_Info.pData = m_D3DRect.pBits;
#endif
	Unlock();

	//Src解放
	_VFREE(m_Info.pData);

	return 0;
}

void CTexture::Release() {
	SafeRelease(m_ObjectID);
} 

void CTexture::GetRenderTargetData() {
#if 0
	LPDIRECT3DSURFACE9 pSrc = NULL;
	LPDIRECT3DSURFACE9 pDest = NULL;

	HRESULT res;

	res = CDirect3D::GetDevice()->GetRenderTarget(0, &pSrc);
	SIG_ERRORCHECK(res,_TEXT("Failed@GetRenderTargetSurface"));

	D3DSURFACE_DESC srcDesc;
	D3DSURFACE_DESC destDesc;

	if(pSrc) {
		pSrc->GetDesc(&srcDesc);
	}

	if( m_Format != srcDesc.Format || GetWidth() != srcDesc.Width || GetHeight() != srcDesc.Height ) {
		Release();
		Create(srcDesc.Width ,srcDesc.Height, srcDesc.Format, D3DPOOL_SYSTEMMEM, FALSE);
	}

	res = GetSurface(pDest,0);
	SIG_ERRORCHECK(res,_TEXT("Failed@GetSurface"));

	if(pDest) {
		pDest->GetDesc(&destDesc);
	}

	res = CDirect3D::GetDevice()->StretchRect(pSrc, NULL, pDest, NULL, D3DTEXF_LINEAR);
//	res = CDirect3D::GetDevice()->GetRenderTargetData(pSrc, pDest);
	SIG_ERRORCHECK(res,_TEXT("Failed@GetRenderTargetData"));

	SafeRelease(pDest);
	SafeRelease(pSrc);
#endif
}

#if 0
BOOL CTexture::LoadAlphaChannel(ISurface& oData){
//BMPは天地逆だがTextureは正方向
	D3DLOCKED_RECT D3DRect;
	_ZEROINIT(D3DRect);

#if 0
	m_pTexture->LockRect(0,&D3DRect,NULL,D3DLOCK_NOSYSLOCK);
	BYTE* pSrc = (BYTE*)oData.GetPointer(0,0);
	BYTE* pDest= (BYTE*)D3DRect.pBits;
#else
	Lock();
	BYTE* pSrc = (BYTE*)oData.GetPointer(0,0);
	BYTE* pDest= (BYTE*)m_Info.pData;
#endif
	switch(m_TextureInfo.Format){
	case D3DFMT_A8R8G8B8:
		for(UINT a = 0;a < oData.m_Info.Y;a ++){
			for(UINT b = 0;b < oData.m_Info.X;b ++)
				( (XRGB8888*)pDest )[b].Alpha = pSrc[b];
#if 0
			pDest += D3DRect.Pitch;
#else
			pDest += m_Info.Pitch;
#endif
			pSrc -= oData.m_Info.Pitch;
		}
		break;
	}
#if 0
	m_pTexture->UnlockRect(0);
#else
	Unlock();
#endif
	return TRUE;
}

#endif

int CTexture::Lock(int left,int top,int right,int bottom){
	if( m_bLocked )
		return -1;

	RECT Rect;
	SetRect(&Rect,left,top,right,bottom);

	_ZEROINIT(m_D3DRect);

	if( FAILED( m_ObjectID->LockRect(0, &m_D3DRect, &Rect, D3DLOCK_NOSYSLOCK) ) )
		return 1;

	m_bLocked = TRUE;
	return 0;
}

int CTexture::Lock(){
	if(m_bLocked)
		return -1;

	_ZEROINIT(m_D3DRect);
	_ZEROINIT(m_D3DDesc);

	HRESULT hRes = 0;
	m_ObjectID->GetLevelDesc(0, &m_D3DDesc);

	if( FAILED( m_ObjectID->LockRect(0, &m_D3DRect, NULL, D3DLOCK_NOSYSLOCK) ) )
		return 1;

	m_bLocked = TRUE;
	return 0;
/*
		if(m_bLocked == FALSE) {
		if( m_bRenderTarget && SUCCEEDED( g_pD3DDevice.GetInstance()->CreateOffscreenPlainSurface( D3DDesc.Width, D3DDesc.Height, D3DDesc.Format, D3DPOOL_SYSTEMMEM, &m_TextureInfo.pTempSurface, NULL ) ) ){
			hRes = m_pTexture->GetSurfaceLevel( 0, &m_TextureInfo.pLockSurface );
			hRes = g_pD3DDevice.GetInstance()->GetRenderTarget( 0, &m_TextureInfo.pOldRenderSurface );
			hRes = g_pD3DDevice.GetInstance()->SetRenderTarget( 0, m_TextureInfo.pLockSurface );
			hRes = g_pD3DDevice.GetInstance()->GetRenderTargetData( m_TextureInfo.pLockSurface, m_TextureInfo.pTempSurface );
			hRes = m_TextureInfo.pTempSurface->LockRect( &D3DRect, NULL, 0 );
		}
*/
}

int CTexture::Unlock(){
	if( m_bLocked == FALSE )
		return -1;

	HRESULT hRes = 0;

	if ( FAILED(m_ObjectID->UnlockRect(0)) )
		return 1;

	m_bLocked = FALSE;
	return 0;
/*
	if (m_bRenderTarget ) {
			hRes = m_TextureInfo.pTempSurface->UnlockRect();
			hRes = g_pD3DDevice.GetInstance()->SetRenderTarget( 0, m_TextureInfo.pOldRenderSurface );
			hRes = g_pD3DDevice.GetInstance()->UpdateSurface( m_TextureInfo.pTempSurface, NULL, m_TextureInfo.pLockSurface, NULL);
			SafeRelease(m_TextureInfo.pTempSurface);
			SafeRelease(m_TextureInfo.pLockSurface);
			SafeRelease(m_TextureInfo.pOldRenderSurface);
		}
	}
*/
}

void CTexture::Blt( CTexture* pDest ) {
	LPDIRECT3DSURFACE9 pSrcSurface = NULL;
	LPDIRECT3DSURFACE9 pDestSurface = NULL;
	HRESULT res;

	GetSurface(pSrcSurface);
	pDest->GetSurface(pDestSurface);
	res = CDirect3D::GetDevice()->StretchRect( pSrcSurface, NULL, pDestSurface, NULL, D3DTEXF_NONE );
	SIG_ERRORCHECK(res,"Failed@CTexture::Blt");
	SafeRelease( pSrcSurface );
	SafeRelease( pDestSurface );
}

void CTexture::ColorFill ( D3DCOLOR Color, RECT* pRect ) {
	LPDIRECT3DSURFACE9 pSurface = NULL;
	HRESULT res;
	GetSurface(pSurface);
	res = CDirect3D::GetDevice()->ColorFill( pSurface, pRect, Color );
	SIG_ERRORCHECK(res,"Failed@CTexture::ColorFill");
	SafeRelease(pSurface);
}

//	D3DXSaveSurfaceToFile( Name, D3DXIFF_BMP, pSurface, NULL, &rect );

CRenderingBuffer::CRenderingBuffer()
{ ; }

BOOL CRenderingBuffer::SetRenderTarget(CTexture& tex, int Index ) {
	CRenderingTargetInfo	OldInfo = { 0 };
	HRESULT res;

	if(m_InfoList.size() <= 0) {
		res = CDirect3D::GetDevice()->GetRenderTarget(Index, &OldInfo.m_RenderBuffer);
		SIG_ERRORRETURNBOOL(res,"Failed@CTexture::GetRenderTarget");
		res = CDirect3D::GetDevice()->GetViewport(&OldInfo.m_ViewPort);
		SIG_ERRORRETURNBOOL(res,"Failed@CTexture::GetViewport");
	}
	else {
		OldInfo.m_RenderBuffer = m_Info.m_RenderBuffer;
		OldInfo.m_ViewPort = m_Info.m_ViewPort;
	}

	CRenderingTargetInfo	NewInfo = { 0 };

	res = tex.GetSurface(NewInfo.m_RenderBuffer);
	SIG_ERRORRETURNBOOL(res,"Failed@CTexture::GetSurface");

	ISurfaceInfo& iInfo = tex.GetInfo();

	NewInfo.m_ViewPort.X		= 0;
	NewInfo.m_ViewPort.Y		= tex.GetHeight() - iInfo.Y;
	NewInfo.m_ViewPort.Width	= iInfo.X;
	NewInfo.m_ViewPort.Height	= iInfo.Y;
	NewInfo.m_ViewPort.MinZ	= 0.0f;
	NewInfo.m_ViewPort.MaxZ	= 1.0f;

	res = CDirect3D::GetDevice()->SetRenderTarget(Index, NewInfo.m_RenderBuffer);
	SIG_ERRORRETURNBOOL(res,"Failed@CTexture::SetRenderTarget");

	res = CDirect3D::GetDevice()->SetViewport(&NewInfo.m_ViewPort);
	SIG_ERRORRETURNBOOL(res,"Failed@CTexture::SetViewPort");

	m_Info = NewInfo;

	m_InfoList.push_back(OldInfo);

	return TRUE;
}

BOOL CRenderingBuffer::ResetRenderTarget( int Index ) {
	if(m_InfoList.size() <= 0)
		return FALSE;

	CRenderingTargetInfo& Info = m_InfoList.back();
	
	_RES res = CDirect3D::GetDevice()->SetRenderTarget(Index, Info.m_RenderBuffer);
	SIG_ERRORRETURNBOOL(res,"Failed@CTexture::SetRenderTarget");

	res = CDirect3D::GetDevice()->SetViewport(&Info.m_ViewPort);
	SIG_ERRORRETURNBOOL(res,"Failed@CTexture::SetViewPort");

	SafeRelease(m_Info.m_RenderBuffer);
	m_Info.m_RenderBuffer = Info.m_RenderBuffer;
	m_Info.m_ViewPort = Info.m_ViewPort;
	m_InfoList.pop_back();

	return TRUE;
}

void CTexture::SaveImageFile(LPCTSTR name, BOOL overwrite) {
	Lock();

	m_Info.pData = m_D3DRect.pBits;
	m_Info.Pitch = m_D3DRect.Pitch;

//Yを反転
	m_Info.Y *= -1;
	ISurface::SaveImageFile(name,overwrite);
	m_Info.Y *= -1;

	Unlock();

	m_Info.pData = NULL;
}
