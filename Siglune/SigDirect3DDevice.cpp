#include "Multistdafx.h"

#include "SigDirect3DDevice.h"

//static
LPDIRECT3D9			CDirect3D::m_pDirect3D = NULL;
LPDIRECT3DDEVICE9	CDirect3D::m_pDirect3DDevice;

//extern 
int _TEXTURESTAGE_MAX_ = 4;

CD3DDevice::CD3DDevice() {
	m_pDevice	= NULL;
	m_bWindowed	= TRUE;
	_ZEROINIT(m_d3dparam[0]);
	_ZEROINIT(m_d3dparam[1]);
}

BOOL CD3DDevice::Initialize ( HWND hWnd,  int viewx, int viewy, int viewwidth, int viewheight, BOOL bWindowed, DWORD ColorDepth ) {
	Release();

	LPDIRECT3D9 pDirect3D = CDirect3D::GetInstance();

	HRESULT res;

	// 現在の画面モードを取得
	D3DDISPLAYMODE d3ddisplaymode;
	res = pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddisplaymode);

	SIG_ERRORRETURNBOOL(res,_TEXT("Failed@IDirect3D::GetAdapterDisplayMode"));

	//D3DFMT_X1R5G5B5、D3DFMT_R5G6B5、D3DFMT_X8R8G8B8、および D3DFMT_A8R8G8B8 
	switch(ColorDepth){
	case 15:
		m_d3dparam[0].BackBufferFormat	= D3DFMT_X1R5G5B5;
		_DEBUGPRINTF(_TEXT("画面モード：X1R5G5B\n"));
		break;
	case 16:
		m_d3dparam[0].BackBufferFormat	= D3DFMT_R5G6B5;
		_DEBUGPRINTF(_TEXT("画面モード：R5G6B5\n"));
		break;
	case 24:
		m_d3dparam[0].BackBufferFormat	= D3DFMT_X8R8G8B8;
		_DEBUGPRINTF(_TEXT("画面モード：X8R8G8B8\n"));
		break;
	case 32:
		m_d3dparam[0].BackBufferFormat	= D3DFMT_X8R8G8B8;
		_DEBUGPRINTF(_TEXT("画面モード：X8R8G8B8\n"));
		break;
	default:
		m_d3dparam[0].BackBufferFormat	= D3DFMT_X8R8G8B8;
		_DEBUGPRINTF(_TEXT("画面モード：X8R8G8B8\n"));
		break;
	}

	m_d3dparam[1].BackBufferFormat	= d3ddisplaymode.Format;

	for(int a = 0;a < 2;a ++){
		m_d3dparam[a].BackBufferCount					= 1;
		m_d3dparam[a].BackBufferWidth					= viewwidth;
		m_d3dparam[a].BackBufferHeight					= viewheight;
		m_d3dparam[a].SwapEffect						= D3DSWAPEFFECT_DISCARD;
		m_d3dparam[a].hDeviceWindow						= hWnd;
		m_d3dparam[a].Windowed							= a ? TRUE : FALSE;
		m_d3dparam[a].EnableAutoDepthStencil			= FALSE;
		m_d3dparam[a].AutoDepthStencilFormat			= D3DFMT_D16;
		m_d3dparam[a].Flags								= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		m_d3dparam[a].FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;	
		m_d3dparam[a].PresentationInterval	= /*a ? D3DPRESENT_INTERVAL_DEFAULT :*/ D3DPRESENT_INTERVAL_IMMEDIATE;
		m_d3dparam[a].MultiSampleType					= D3DMULTISAMPLE_NONE;
		m_d3dparam[a].MultiSampleQuality		        = D3DMULTISAMPLE_NONE;
	}

	m_bWindowed = bWindowed;

	D3DPRESENT_PARAMETERS* pD3Dparam = bWindowed ? &m_d3dparam[1] : &m_d3dparam[0];

	//D3DCREATE_MULTITHREADEDはパフォーマンス低下？

/*	if(	SUCCEEDED( m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, ms_hAppWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_MULTITHREADED ,pD3Dparam, &m_pDevice ) ) )
		CLogReporter::Report("ピュアデバイスを作成\n");
	else*/
	if( SUCCEEDED( pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,	pD3Dparam, &m_pDevice ) ) )
		_DEBUGPRINTF(_TEXT("HALデバイスを作成\n"))
	else if( SUCCEEDED( pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, pD3Dparam, &m_pDevice ) ) )
		_DEBUGPRINTF(_TEXT("HELデバイスを作成\n"))
	else if( SUCCEEDED( pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, pD3Dparam, &m_pDevice ) ) )
		_DEBUGPRINTF(_TEXT("REFデバイスを作成\n"))
	else
		return FALSE;

	res = m_pDevice->GetDeviceCaps( &m_d3dcaps );

	SIG_ERRORRETURNBOOL(res,_TEXT("Failed@IDirect3D::GetDeviceCaps"));

	if(m_d3dcaps.VertexShaderVersion < _SIG_SHADERMODEL_VERSION_)
		_DEBUGPRINTF(_TEXT("VertexShaderがシステム想定より下位のバージョンです"));
	if(m_d3dcaps.PixelShaderVersion < _SIG_SHADERMODEL_VERSION_)
		_DEBUGPRINTF(_TEXT("Pixelがシステム想定より下位のバージョンです"));

	SetViewPort(viewx, viewy, viewwidth, viewheight);

	_TEXTURESTAGE_MAX_ = m_d3dcaps.MaxTextureBlendStages;

	_DEBUGPRINTF(_TEXT("TextureStagesMax=%d"),_TEXTURESTAGE_MAX_);

	for(DWORD a = 0;a < _TEXTURESTAGE_MAX_ ;a ++){
		//DX9ではSetSamplerStateに移行したメソッド
#if 1
		m_pDevice->SetSamplerState( a, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
		m_pDevice->SetSamplerState( a, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
		m_pDevice->SetSamplerState( a, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
		//補間を無しに。これをしないとたいていのカードでBG表示の縁にゴミがでる。
#if 0
		m_pDevice->SetSamplerState( a, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pDevice->SetSamplerState( a, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pDevice->SetSamplerState( a, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR/*D3DTEXF_POINT*/ );
#else
		m_pDevice->SetSamplerState( a, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m_pDevice->SetSamplerState( a, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		m_pDevice->SetSamplerState( a, D3DSAMP_MIPFILTER, D3DTEXF_POINT/*D3DTEXF_POINT*/ );
#endif
		m_pDevice->SetSamplerState( a, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF ); 
#endif
		//DX8&9共通
		m_pDevice->SetTextureStageState( a, D3DTSS_COLOROP, D3DTOP_MODULATE );//(a==0)?(D3DTOP_MODULATE):(D3DTOP_DISABLE) );
		m_pDevice->SetTextureStageState( a, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( a, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pDevice->SetTextureStageState( a, D3DTSS_ALPHAOP, D3DTOP_MODULATE );//(a==0)?(D3DTOP_MODULATE):(D3DTOP_DISABLE) );
		m_pDevice->SetTextureStageState( a, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( a, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	}

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

//regacy
	SetDialogBoxMode( TRUE );

	CDirect3D::SetDevice(m_pDevice);

	return TRUE;
}

void CD3DDevice::Release(){
	SafeRelease(m_pDevice);

	_DEBUGPRINTF(_TEXT("Direct3DDeviceを解放\n"));
}

BOOL CD3DDevice::SetViewPort( int x, int y, int width, int height ) {
	// ビューポート初期値
	D3DVIEWPORT9 ViewPort = { 0 };

	ViewPort.X		= x;
	ViewPort.Y		= y;
	ViewPort.Width	= width;
	ViewPort.Height	= height;
    ViewPort.MinZ	= 0.0f;
	ViewPort.MaxZ	= 1.0f;

	//ビューポート設定
	HRESULT res = m_pDevice->SetViewport(&ViewPort);

	SIG_ERRORRETURNBOOL(res,_TEXT("Failed@CD3DDevice::SetViewPort"));

	m_ViewPort = ViewPort;

	return TRUE;
}

BOOL CD3DDevice::SetProjectionMatrix( float fovy, float aspect, float znear, float zfar ) {
	_MATRIXPERSPECTIVE(m_ProjectionMatrix, fovy, aspect, znear, zfar );
//	D3DXMatrixPerspectiveFovLH( _MATRIXARRAYD3DXPTR(m_ProjectionMatrix), fovy, aspect, znear, zfar );
	return SUCCEEDED( m_pDevice->SetTransform( D3DTS_PROJECTION,  _MATRIXARRAYD3DPTR(m_ProjectionMatrix)) );
}

BOOL CD3DDevice::SetViewMatrix( _VECTOR& Eye, _VECTOR& At, _VECTOR& Up ) {
	_MATRIXLOOKAT(m_ViewMatrix, Eye, At, Up);
//	D3DXMatrixLookAtLH(_MATRIXARRAYD3DXPTR(m_ViewMatrix), _VECTOR3D3DXPTR(Eye), _VECTOR3D3DXPTR(At), _VECTOR3D3DXPTR(Up));
	return SUCCEEDED(m_pDevice->SetTransform( D3DTS_VIEW, _MATRIXARRAYD3DPTR(m_ViewMatrix)) );
}

BOOL CD3DDevice::SetGamma(WORD Value){
//	0--------------65535
//	黒-------------白
	if( m_pDevice == NULL )
		return FALSE;

	D3DGAMMARAMP Gamma;

	for(int i=0;i < 256;i++){
		Gamma.red[i]	= Value;
		Gamma.green[i]	= Value;
		Gamma.blue[i]	= Value;
	}

	m_pDevice->SetGammaRamp(D3DSGR_CALIBRATE, 0, &Gamma);

	return TRUE;
}

BOOL CD3DDevice::Present( RECT* pSourceRect, RECT* pDestRect ) {
	HRESULT res = m_pDevice->Present( pSourceRect, pDestRect, NULL, NULL );

	SIG_ERRORCHECK(res,_TEXT("Failed@CD3DDevice::Present"));

	switch ( res ) {
	//---------------------------- これ出たらもうダメポ
	case D3DERR_DRIVERINTERNALERROR:
		return FALSE;
	//---------------------------- デバイス消失状態
	case D3DERR_DEVICELOST:
		// １秒まっとけ
		_SLEEP( 1000 );
		// デバイス状態チェック
		switch ( m_pDevice->TestCooperativeLevel() ) {
		//---------------- デバイス消失
		case D3DERR_DEVICELOST:
			_SLEEP( 100 );
			break;
		//---------------- デバイスリセットＯＫ
		case D3DERR_DEVICENOTRESET:
			return FAILED( ResetDevice() );
		}
		return FALSE;
	case S_OK:
		return TRUE;
	}
	return FALSE;
}

void CD3DDevice::SetNormalAlphaBlendMode() {
	//DX8&9共通
	for( UINT a = 0; a < _TEXTURESTAGE_MAX_; a ++ ) {	
		m_pDevice->SetTextureStageState( a, D3DTSS_COLOROP, D3DTOP_MODULATE );//(a==0)?(D3DTOP_MODULATE):(D3DTOP_DISABLE) );
		m_pDevice->SetTextureStageState( a, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( a, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pDevice->SetTextureStageState( a, D3DTSS_ALPHAOP, D3DTOP_MODULATE );//(a==0)?(D3DTOP_MODULATE):(D3DTOP_DISABLE) );
		m_pDevice->SetTextureStageState( a, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState( a, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	}
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CD3DDevice::SetAlphaBlendMode( int type, BOOL bAlpha ) {
	UINT AlphaOp = bAlpha ? D3DTOP_MODULATE : D3DTOP_SELECTARG1;

	switch( type ) {
	case _NORMAL_BLEND_:
	//通常          : sRGB = sRGB x cRGB
	//              : sA   = sA x cA
	//              : out  = sRGB x sA + dRGB x (1-sA)
	//TexStage0:RGB : [difuuse] x [texture]
	//TexStage0:A   : [difuuse] x [texture]
	//TexStage1:RGB : disable
	//TexStage1:A   : disable
	//BlendStage    : sRGB x [sA] + dRGB x [^sA]
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
		break;
	case _SCREEN_BLEND_:
	//スクリーン: sRGB = sRGB x cRGB
	//			: sA   = sA x cA
	//			: sRGB = sRGB x sA + 0 x (1-sA)
	//			: out  = dRGB + sRGB - dRGB x sRGB
	//			:      = dRGB x 1    + (1-dRGB) x sRGB 
	//TexStage0:RGB : [diffuse] x [Texture]
	//TexStage0:A   : [diffuse] x [Texture]
	//TexStage1:RGB : [current]x(CurrentA) x [0]x(1-CurrentA)
	//TexStage1:A   : disable
	//BlendStage    : sRGB x [^dRGB] + dRGB x [one]

#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,BlackTexture);  //Black
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
		break;
	case _MULTIPLE_BLEND_:
	//乗算		: sRGB = sRGB x cRGB
	//			: sA   = sA x cA
	//			: sRGB = sRGB x sA + 1 x (1-sA)
	//			: out  = dRGB x sRGB + 0 x 0
	//TexStage0:RGB : [diffuse] x [Texture]
	//TexStage0:A   : [diffuse] x [Texture]
	//TexStage1:RGB : [current]x(CurrentA) x [1]x(1-CurrentA)
	//TexStage1:A   : disable
	//BlendStage    : sRGB x [zero] + dRGB x [sRGB]

#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,WhiteTexture);  //White
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
		break;
	case _EXCLUDE_BLEND_:
	//除外かな？: sRGB = sRGB x cRGB
	//			: sA   = sA x cA
	//			: out  = (1-dRGB) x sRGB + dRGB x sRGB  （反転後とのアルファブレンド）
	//TexStage0:RGB : [diffuse] x [Texture]
	//TexStage0:A   : [diffuse] x [Texture]
	//TexStage1:RGB : [current]x(CurrentA) x [0]x(1-CurrentA)
	//TexStage1:A   : disable
	//BlendStage    : sRGB x [dRGB^] + dRGB x [sRGB^]
#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,BlackTexture);  //Black
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
		break;
	case _ADD_BLEND_:
	//加算        : sRGB = sRGB x cRGB
	//			: sA   = sA x cA
	//			: out  = sRGB x sA + dRGB x 1
	//TexStage0:RGB : [difuuse] x [texture]
	//TexStage0:A   : [difuuse] x [texture]
	//TexStage1:RGB : disable
	//TexStage1:A   : disable
	//BlendStage    : sRGB x [sA] + dRGB x [one]
#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,nil);
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
		break;
	case _SUB_BLEND_:
	//通常          : sRGB = sRGB x cRGB
	//			: sA   = sA x cA
	//			: out  = sRGB x sA + dRGB x 1
	//TexStage0:RGB : [difuuse] x [texture]
	//TexStage0:A   : [difuuse] x [texture]
	//TexStage1:RGB : disable
	//TexStage1:A   : disable
	//BlendStage    : dRGB x [one] - sRGB x [sA]   ※位置逆転
#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,nil);
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_REVSUBTRACT);
		break;
	case _CMPBRIGHT_BLEND_:
	//比較明        : sRGB = sRGB x cRGB
	//			: sA   = sA x cA
	//			: sRGB = sRGB x sA + 0 x (1-sA)
	//			: out  = dRGB x 1 (high) sRGB x 1
	//TexStage0:RGB : [diffuse] x [Texture]
	//TexStage0:A   : [diffuse] x [Texture]
	//TexStage1:RGB : [current]x(CurrentA) x [0]x(1-CurrentA)
	//TexStage1:A   : disable
	//BlendStage    : sRGB x [dRGB] (high) dRGB x [one]
#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,BlackTexture);  //Black
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_MAX);
		break;
	case _CMPDARK_BLEND_:
	//比較暗        : sRGB = sRGB x cRGB
	//			: sA   = sA x cA
	//			: sRGB = sRGB x sA + 1 x (1-sA)
	//			: out  = dRGB x 1 (low) sRGB x 1
	//TexStage0:RGB : [diffuse] x [Texture]
	//TexStage0:A   : [diffuse] x [Texture]
	//TexStage1:RGB : [current]x(CurrentA) x [1]x(1-CurrentA)
	//TexStage1:A   : disable
	//BlendStage    : sRGB x [dRGB] (low) dRGB x [one]
#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,WhiteTexture);  //White
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _CONTRUST_BLEND_:
	//コントラスト(x2)    : out = sRGB x dRGB + dRGB
#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,BlackTexture);  //Black
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
		break;
	case _MULTIPLE_BLEND2_:
	//乗算ｘ２    : out = sRGB x dRGB + dRGB x sRGB
	//          :     = (sRGB x dRGB) x 2      ※D3DTOP_MODULATE2Xと同じ
#if 0
		SetTexture(0,SrcTexture);
		SetTexture(1,GrayTexture);  //$808080
#endif
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);  //D3DTOP_SELECTARG1にするとAlpha無効になります

		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR);
		SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
		break;
	}
}

/*
//リセットするデバイスを作成したスレッドとは別のスレッドで呼び出すと、TestCooperative Level の呼び出しは失敗する。
//D3DUSAGE_RENDERTARGET フラグで作成されたオブジェクトはリソース管理がサポートされていない。
unsigned CDrawManager::ThreadProc(){
	ResetEvent();

	while(WaitEvent(0) == WAIT_TIMEOUT){
		if(1){//drawflag
			m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0 );
			m_pDevice->BeginScene();
			//描画
			m_pDevice->EndScene();

			switch( m_pDevice->Present( NULL, NULL, NULL, NULL ) ){
			case D3DERR_DRIVERINTERNALERROR:
				CLogReporter::Report("Direct3D内部エラー\n");
				SetEvent();
				break;
			case D3DERR_DEVICELOST:
				switch ( m_pDevice->TestCooperativeLevel() ){
				case D3DERR_DEVICELOST:
					::Sleep( 10 );
					break;
				case D3DERR_DEVICENOTRESET:
					m_pDevice->Reset(NULL); //&PresentParam )
					break;
				}
				break;
			}
		}
	}
	return 0;
}
*/