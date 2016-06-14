#ifndef _SIG_DIRECT3DDEVICE_
#define _SIG_DIRECT3DDEVICE_

#include "SigDirect3D.h"
#include <Ellison/EliMath.h>

#if defined(_DALK_USE_OPENGL_)
#include "Ellison/EliShader.h"
#endif

enum _BREND_MODE_{
	_NORMAL_BLEND_,
	_SCREEN_BLEND_,
	_MULTIPLE_BLEND_,
	_EXCLUDE_BLEND_,
	_ADD_BLEND_,
	_SUB_BLEND_,
	_CMPBRIGHT_BLEND_,
	_CMPDARK_BLEND_,
	_CONTRUST_BLEND_,
	_MULTIPLE_BLEND2_,
};

//テクスチャステージ数の上限
extern int _TEXTURESTAGE_MAX_;

//これGLとわけたほうがいいかも
#define SIG_SCREEN_Z 0.0f					//ワールド座標上のスクリーンのＺ
#define SIG_SCREEN_Z_NEAR 1.0f
#define SIG_SCREEN_Z_FAR 50.0f

//※なにげに重要レンダリングサイズ。ここから各デバイス依存のサイズに拡縮して表示。
//GLの都合にあわせて1024*576とします。
const SIZE SIG_RENDERING_SIZE = { 1024, 576 };

enum SCREEN_SIZE_ENUM {
	SCREEN_SMALL,
	SCREEN_DEFAULT,
	SCREEN_LARGE,
	SCREEN_HD,
	SCREEN_FULLHD,
	SCREEN_UNKNOWN = 100
};

const SIZE _Sig_Device_SizeType[] = {
	{ 960,	540 },	//SMALL
	{ 1024,	576 },	//DEFAULT
	{ 1136,	640 },	//LARGE
	{ 1280, 720 },	//HD
	{ 1920, 1080 }	//
};

//ポリゴン座標の原点を左上にシフトする
#define _SHIFT_XY_COORDINATE

//ポリゴンY座標を逆にする
#if 0
#define _REVERSE_Y_COORDINATE_
#endif

//テクスチャv座標を逆にする
#define _REVERSE_V_COORDINATE_

//2D座標→3Dローカル座標に変換

namespace Siglune {
//設定してるのであればprojectionwidthとかもかける必要があり
inline float Set3DPos(int x) {
	return  ((float)x / (float)SIG_RENDERING_SIZE.cx * 2.0f) - 1.0f;
}

inline float Set3DPosY(int y) {
	return ((float)y / (float)SIG_RENDERING_SIZE.cy * 2.0f) - 1.0f;
}

inline float Set3DPos(int t1, int t2){
	return ((float)t1 / (float)t2 * 2.0f) - 1.0f;
}
};// namespace Siglune

//3Dデバイスクラス。内部実装としてはGLがDXのエミュレータなのでクラス名とかはDXが基本
class CD3DDevice {
protected:
#if defined(_DALK_USE_DIRECT3D_)
	LPDIRECT3DDEVICE9		m_pDevice;
	D3DPRESENT_PARAMETERS	m_d3dparam[2];
	D3DCAPS9				m_d3dcaps;
	D3DGAMMARAMP			m_GammaRamp;
	LPDIRECT3DSURFACE9		m_pRenderSurface;
	LPDIRECT3DSURFACE9		m_CaptureBuffer;
	BOOL			m_bWindowed;
	float			m_ProjectionWidth;
	float			m_ProjectionHeight;
	_VECTOR			m_ProjectionVector;
	_MATRIX			m_ProjectionMatrix;
	_MATRIX			m_ViewMatrix;
#endif

#if defined(_DALK_USE_OPENGL_)
//	CShader			m_Shader[2];
//	GLuint			m_CurrentShader;
//	GLuint			m_RenderBuffer[2];
//	GLuint			m_FrameBuffer[2];
#endif

	D3DVIEWPORT9	m_ViewPort;

public:
		CD3DDevice();
		~CD3DDevice()		{ Release(); }
//GL-DX共通
		BOOL	Initialize	( HWND hWnd, int viewx, int viewy, int viewwidth, int viewheight, BOOL bWindowed = TRUE,DWORD ColorDepth = 32 );
		void	Release();
		BOOL	SetViewPort	( int x, int y, int width,int height );	//GL
//DX専用
#if defined(_DALK_USE_DIRECT3D_)
inline	LPDIRECT3DDEVICE9&	GetInstance()						{ return m_pDevice; }
inline	BOOL	SetDialogBoxMode	( BOOL bUse )				{ return SUCCEEDED( m_pDevice->SetDialogBoxMode( bUse ) ); }
inline	BOOL	GetViewPort			( D3DVIEWPORT9* ViewPort )	{ return SUCCEEDED(m_pDevice->GetViewport ( ViewPort ) ); }

//変換行列はシェーダクラスに移行予定（OpenGLにあわせる＆固定頂点機能を使わないように）
inline	float	GetProjectionWidth()	{ return m_ProjectionWidth; }	//ワールド座標のスクリーン端の位置を返す
inline	float	GetProjectionHeight()	{ return m_ProjectionHeight; }	//ワールド座標のスクリーン端の位置を返す
inline	float	PixelXtoProjection	(float x)				{ return ( x / m_ViewPort.Width ) * m_ProjectionWidth * 2.0f - m_ProjectionWidth; }
inline	float	PixelYtoProjection	(float y)				{ return ( y / m_ViewPort.Height ) * m_ProjectionHeight * -2.0f + m_ProjectionHeight; }
		BOOL	SetProjectionMatrix	( float fovy, float aspect, float znear, float zfar );
		BOOL	SetProjectionVector	( float x, float y, float z, bool isFrust = false);
		BOOL	SetViewMatrix		( _VECTOR& Eye, _VECTOR& At, _VECTOR& Up );
inline	BOOL	SetProjectionMatrix(_MATRIX& _matrix)		{ return SUCCEEDED( m_pDevice->SetTransform( D3DTS_PROJECTION, _MATRIXARRAYD3DPTR(_matrix))); }
inline	BOOL	SetProjectionMatrix()						{ return SUCCEEDED( m_pDevice->SetTransform( D3DTS_PROJECTION, _MATRIXARRAYD3DPTR(m_ProjectionMatrix))); }
inline	BOOL	SetViewMatrix		(_MATRIX& _matrix)		{ return SUCCEEDED( m_pDevice->SetTransform( D3DTS_VIEW, _MATRIXARRAYD3DPTR(_matrix))); }
inline	BOOL	SetViewMatrix()								{ return SUCCEEDED( m_pDevice->SetTransform( D3DTS_VIEW, _MATRIXARRAYD3DPTR(m_ViewMatrix))); }

		BOOL	SetGamma			( WORD Value );
#endif

#if defined(_DALK_USE_DIRECT3D_)
//--------------------レンダリングステート系------------
inline	HRESULT	SetRenderState	( D3DRENDERSTATETYPE state, DWORD value )	{ return m_pDevice->SetRenderState( state, value ); }
inline	HRESULT	SetCulling		( int param )	{ return m_pDevice->SetRenderState( D3DRS_CULLMODE, param ); }
inline	HRESULT	SetLighting		( BOOL bUse )	{ return m_pDevice->SetRenderState( D3DRS_LIGHTING, bUse ); }
inline	HRESULT	SetZBuffer		( BOOL bUse )	{ return m_pDevice->SetRenderState( D3DRS_ZENABLE, bUse ); }
inline	HRESULT SetZBufWrite	( BOOL bUse )	{ return m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, bUse ); }
inline	HRESULT	SetAlphaBlend	( BOOL bUse )	{ return m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, bUse ); }
inline	void	SetAlphaBlendFunc( D3DBLEND src, D3DBLEND dest)	{ m_pDevice->SetRenderState(D3DRS_SRCBLEND, src); m_pDevice->SetRenderState(D3DRS_DESTBLEND, dest); }
inline	HRESULT	SetAlphaTest	( BOOL bUse )	{ return m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, bUse ); }
inline	HRESULT	SetAlphaRef		( BYTE param )	{ return m_pDevice->SetRenderState( D3DRS_ALPHAREF, param ); }
inline	HRESULT	SetAlphaTestFunc( D3DCMPFUNC param )		{ return m_pDevice->SetRenderState( D3DRS_ALPHAFUNC, param ); }
inline	void	SetAlphaTest(D3DCMPFUNC func, float param)	{ SetAlphaRef((BYTE)(param * 256.0f)); SetAlphaTestFunc(func); }
//inline	HRESULT	ColorFill( IDirect3DSurface9 *pSurface, CONST RECT *pRect, D3DCOLOR color );
//--------------------シーン描画系---------------------
inline	HRESULT	ResetTexture (int Stage=0)		{ return m_pDevice->SetTexture(Stage, NULL); }
inline	HRESULT	Clear( UINT r, UINT g, UINT b )	{ return m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, D3DCOLOR_XRGB( r, g, b ), 1.0f, 0 ); }
inline	HRESULT	BeginScene()					{ return m_pDevice->BeginScene(); }
inline	HRESULT	EndScene()						{ return m_pDevice->EndScene(); }
inline	HRESULT	ResetDevice()					{ return m_pDevice->Reset( &m_d3dparam[m_bWindowed] ); }
inline	HRESULT	SetFVF(DWORD type)				{ return m_pDevice->SetFVF(FVF_PARAM[type]); }

		BOOL	Present ( RECT* pSourceRect = NULL , RECT* pDestRect = NULL );
		void	SetNormalAlphaBlendMode();
		void	SetAlphaBlendMode( int type, BOOL bAlhpa = FALSE);
#else
//--------------------レンダリングステート系------------		void	SetCulling			( int param );
		BOOL    SetCulling          ( int param );
//inline	void	SetLighting			( BOOL bUse )					{ bUse ? glEnable(GL_LIGHTING)	: glDisable(GL_LIGHTING); }
inline	void	SetZBuffer			( BOOL bUse )					{ bUse ? glEnable(GL_DEPTH_TEST): glDisable(GL_DEPTH_TEST); }
//inline	HRESULT SetZBufWrite	( BOOL bUse )	{ return m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, bUse ); }
inline	void	SetAlphaBlend		( BOOL bUse )					{ bUse ? glEnable(GL_BLEND)		: glDisable(GL_BLEND); }
inline	void	SetAlphaBlendFunc	(D3DBLEND src, D3DBLEND dest)	{ glBlendFunc(src, dest); }
//αテストはES2.0で削除
//inline	void	SetAlphaTest		( BOOL bUse )				{ bUse ? glEnable(GL_ALPHA_TEST): glDisable(GL_ALPHA_TEST); }
//inline	void	SetAlphaRef			( BYTE param )				{ return m_pDevice->SetRenderState( D3DRS_ALPHAREF, param ); }
//inline	void	SetAlphaFunc		( D3DCMPFUNC param )		{ return m_pDevice->SetRenderState( D3DRS_ALPHAFUNC, param ); }
//inline	void	SetAlphaTest(D3DCMPFUNC func, float param)		{ glAlphaFunc(func, param); }
//--------------------シーン描画系---------------------
inline	void	ResetTexture ()					{ glBindTexture(GL_TEXTURE_2D, 0); }
inline	void	Clear( UINT r, UINT g, UINT b )	{ glClearColor(float(r) / 256.0f, float(g) / 256.0f, float(b) / 256.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT); }
inline	HRESULT	BeginScene()					{ CShader::ChangeShader(_SHADER_DEFAULT_); glActiveTexture(GL_TEXTURE0); }
//inline	void	EndScene()						{`return m_pDevice->EndScene(); }
//inline	HRESULT	ResetDevice()					{ return m_pDevice->Reset( &m_d3dparam[m_bWindowed] ); }
//inline	void	SetFVF(VERTEX_TYPE type)		{ m_Shader[m_CurrentShader].SetTLVertex(NULL); }

//		BOOL	Present ( RECT* pSourceRect = NULL , RECT* pDestRect = NULL );
//		void	SetNormalAlphaBlendMode();
		void	SetAlphaBlendMode( int type, BOOL bAlhpa = FALSE);


/*GL_COLOR_BUFFER_BIT	カラー バッファ
GL_DEPTH_BUFFER_BIT	デプス バッファ
GL_ACCUM_BUFFER_BIT	アキュムレーション バッファ
GL_STENCIL_BUFFER_BIT	ステンシル バッファ
*/
#endif

BOOL Set2DCustom() {
//※変換行列はOpenGL側はシェーダ制御(Uniform)
	// ポリゴンの片面を描画する
//	SetCulling(D3DCULL_NONE);
#if defined(_DALK_USE_DIRECT3D_)
	SetCulling(D3DCULL_CCW);
#elif defined(_DALK_USE_OPENGL_)
	SetCulling(D3DCULL_CW);
#endif

//Common
	//Zバッファを使わない
	SetZBuffer( FALSE );
	//αブレンドを使う
	SetAlphaBlend( TRUE );
	//αブレンド設定
	SetAlphaBlendFunc(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);

#if defined(_DALK_USE_DIRECT3D_)
	SetZBufWrite(FALSE);
	//ライティングを使わない
	SetLighting( FALSE );
	//αテストを使わない
	SetAlphaTest( FALSE );

	//プロジェクション行列
	_MATRIX proj;

//Eigen Only
	proj <<  2.0f / (float)m_ViewPort.Width, 0.0f, 0.0f, 0.0f,

#ifdef _REVERSE_Y_COORDINATE_
		0.0f, 2.0f/(float)m_ViewPort.Height , 0.0f, 0.0f,
#else
		0.0f, -2.0f/(float)m_ViewPort.Height , 0.0f, 0.0f,
#endif
		0.0f, 0.0f, 1.0f, 0.0f,

#ifdef _SHIFT_XY_COORDINATE
 #ifdef _REVERSE_Y_COORDINATE_
		-1.0f, -1.0f, 0.0f, 1.0f
 #else
		-1.0f, 1.0f, 0.0f, 1.0f
 #endif
#else
		0.0f, 0.0f, 0.0f, 1.0f
#endif
	;

	//ビュー行列
	_MATRIX view;
	_MATRIXIDENTITY(view);
//	D3DXMatrixIdentity(&view);

	m_pDevice->SetTransform( D3DTS_PROJECTION, _MATRIXARRAYD3DPTR(proj) );
	m_pDevice->SetTransform( D3DTS_VIEW, _MATRIXARRAYD3DPTR(view) );

	// 頂点ＦＶＦを設定
//	SetFVF(VX_PTL);
	SetFVF(2);

	m_ViewMatrix = view;
	m_ProjectionMatrix = proj;

#elif defined(_DALK_USE_OPENGL_)
	//	glm::transpose(proj);
	_MATRIX view(1.0f);
#endif
	return TRUE;
}

BOOL Set3DCustom() {
	// ポリゴンの片面を描画する
	SetCulling( D3DCULL_CW );
	//Zバッファを使わない
	SetZBuffer( FALSE );
	//αブレンドを使う
	SetAlphaBlend( TRUE );
	//αブレンド設定
	SetAlphaBlendFunc(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
#if defined(_DALK_USE_DIRECT3D_)
	SetZBufWrite(FALSE);
	//ライティングを使わない
	SetLighting( FALSE );
	//αテストを使わない
	SetAlphaTest( FALSE );

	// 頂点ＦＶＦを設定
//	SetFVF(VX_PTL);
	SetFVF(2);

	float fovy = 45.0f * _PI / 180.0f;
	//------------------------------------------------------
	// プロジェクション行列
	//------------------------------------------------------
	SetProjectionMatrix( fovy, (float)m_ViewPort.Width / (float)m_ViewPort.Height , SIG_SCREEN_Z_NEAR , SIG_SCREEN_Z_FAR );
	//------------------------------------------------------
	// ビュー行列を設定
	//------------------------------------------------------
	_VECTOR vEye( 0, 0, -8.0f );
	_VECTOR vAt( 0, 0, 0 );
	_VECTOR vUp( 0, 1.0f, 0 );

	SetViewMatrix( vEye, vAt, vUp );

	//z=0のときのスクリーンに映るワールド座標の位置を算出
//Eigen Only
	m_ProjectionHeight = abs(vEye.z() - SIG_SCREEN_Z /*vAt.z*/) * tan( fovy / 2.0f );
//	m_ProjectionHeight = abs(vEye.z - SIG_SCREEN_Z /*vAt.z*/) * tan(fovy / 2.0f);
	m_ProjectionWidth  = m_ProjectionHeight * (float)m_ViewPort.Width / (float)m_ViewPort.Height;
/*
	//D3DXMATRIX InverseWorld = WorldMatrix;
	D3DXMATRIX InverseProject = m_ProjectionMatrix;
	D3DXMATRIX InverseView = m_ViewMatrix;

	D3DXMATRIX s;
	D3DXMatrixIdentity(&s);
	s._33 = 0.0f;
	//D3DXMatrixInverse( &InverseWorld, NULL, &InverseWorld );
	D3DXMatrixInverse( &InverseProject, NULL, &InverseProject );
	D3DXMatrixInverse( &InverseView, NULL, &InverseView );

	s *= InverseProject;
	s *= InverseView;
*/
#endif
	return TRUE;
}

}; //C3DDevice

#endif