#include "Multistdafx.h"

#include "SigDirect3DDevice.h"

//static
GLuint	COpenGL::g_FrontRenderBuffer;
GLuint	COpenGL::g_FrontFrameBuffer;

CD3DDevice::CD3DDevice() {
}

BOOL CD3DDevice::Initialize( HWND hWnd,  int viewx, int viewy, int viewwidth, int viewheight, BOOL bWindowed, DWORD ColorDepth ) {
	//	Release();
	SetViewPort(viewx, viewy, viewwidth, viewheight);
	return TRUE;
}

void CD3DDevice::Release(){

}

BOOL CD3DDevice::SetViewPort( int x, int y, int width, int height ) {
	m_ViewPort.X		= x;
	m_ViewPort.Y		= y;
	m_ViewPort.Width	= width;
	m_ViewPort.Height	= height;
    m_ViewPort.MinZ	= 0.0f;
	m_ViewPort.MaxZ	= 1.0f;

    _RES res;
	glViewport(x, y, width, height);
	SIG_ERRORRETURNBOOL(res,"Failed@CD3DDevice::SetViewPort");
	return TRUE;
}

/*
void CD3DDevice::BeginScene() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}

void CD3DDevice::EndScene() {
	glFlush();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
*/
BOOL CD3DDevice::SetCulling(int param) {
//裏しか描かない場合（使わないと思うけど）
//	glCullFace(GL_FRONT);
    _RES res;

	switch(param){
	case D3DCULL_NONE:
		//両面を描く		
		glDisable(GL_CULL_FACE);
		break;
	case D3DCULL_CW:
		glEnable(GL_CULL_FACE); 
		SIG_ERRORRETURNBOOL(res,"Failed@GL_CULL_FACE");
		glCullFace(GL_FRONT);
		SIG_ERRORRETURNBOOL(res,"Failed@glCullFace(GL_FRONT)");
		glFrontFace(GL_CW);
		SIG_ERRORRETURNBOOL(res,"Failed@glFrontFace(GL_CW)");
		break;
	case D3DCULL_CCW:
		//表面のみを描く
		glEnable(GL_CULL_FACE); 
		SIG_ERRORRETURNBOOL(res,"Failed@GL_CULL_FACE");
		glCullFace(GL_FRONT);
		SIG_ERRORRETURNBOOL(res,"Failed@glCullFace(GL_FRONT)");
		glFrontFace(GL_CCW);
		SIG_ERRORRETURNBOOL(res,"Failed@glFrontFace(GL_CCW)");
		break;
	}

	return TRUE;
}

void CD3DDevice::SetAlphaBlendMode( int type, BOOL bAlpha ) {
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
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
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		if(bAlpha)
			glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
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
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
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
		if(bAlpha)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		else
			glBlendFunc(GL_ONE, GL_ONE);
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
		if(bAlpha){
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO); // src = 1 固定
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO); // src = 1 固定
		}
		else {
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO); // src = 1 固定
			glBlendFunc(GL_ONE, GL_ONE);
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO); // src = 1 固定
		}
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
		break;
	case _CONTRUST_BLEND_:
	//コントラスト(x2)    : out = sRGB x dRGB + dRGB
		break;
	case _MULTIPLE_BLEND2_:
	//乗算ｘ２    : out = sRGB x dRGB + dRGB x sRGB
	//          :     = (sRGB x dRGB) x 2      ※D3DTOP_MODULATE2Xと同じ
		break;
	}
}
