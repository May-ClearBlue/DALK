#include "MultiStdAfx.h"

#include "EliSprite.h"

#include <sstream>

//ISprite::ISprite() //:
//	m_pTexture(NULL),
//	m_BlendFactor(D3DCOLOR_ARGB(255, 255, 255, 255))
//{
//}

ISprite::ISprite() { 
	_MATRIXIDENTITY(m_TransMatrix);
	_MATRIXIDENTITY(m_RotateMatrix);
	_MATRIXIDENTITY(m_ScaleMatrix);
}

CSprite2D::CSprite2D() :
m_pTexture(NULL) { ; }

void CSprite2D::SetTexture(CTexture* pTexture) {

	ISurfaceInfo& info = pTexture->GetInfo();

	SetXYZ2D(0, 0, info.X, info.Y);
	SetUV(0, 0, info.X, info.Y, pTexture->GetWidth(), pTexture->GetHeight());
	SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

	m_VertexBuffer.Create(4, VX_PTL0);
	m_VertexBuffer.SetData(_Vertex, 4);

	_MATRIXIDENTITY(m_TransMatrix);
}

BOOL CSprite2D::Draw() {
	m_VertexBuffer.SetStream();
	_RES r;

#if defined(_DALK_WINDOWS_)
	r = CDirect3D::GetDevice()->SetTransform(D3DTS_WORLD, _MATRIXARRAYD3DPTR(m_TransMatrix));
	r = CDirect3D::GetDevice()->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
#elif defined(_DALK_IOS_)
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
	SIG_ERRORRETURNBOOL(r,"Failed@CSprite2D::Draw()");
	return TRUE;
}

BOOL CSprite2D::DrawUp() {
	_RES r;
#if defined(_DALK_WINDOWS_)
	r = CDirect3D::GetDevice()->SetTransform(D3DTS_WORLD, _MATRIXARRAYD3DPTR(m_TransMatrix));
	r = CDirect3D::GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, _Vertex, sizeof(TLVERTEX) );
#elif defined(_DALK_IOS_)
    EliVertex::SetVertexPointer(GetVertexType(),_Vertex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
	SIG_ERRORRETURNBOOL(r,"Failed@CSprite2D::DrawUp()");
	return TRUE;
}

#if defined(_DALK_WINDOWS_)
BOOL CSpriteBMFont::LoadFont(const char* name) {
	string t = name;
	t += ".fnt";

	ParseFont(t.data());

	m_pTexture = new CTexture[Pages];
	
	for(int a = 0;a < Pages;a++) {
		basic_ostringstream<TCHAR> os;
//		ostringstream os;
		os << name << "_" << a << ".PNG";
		m_pTexture[a].LoadImageFile( os.str() );
		m_pTexture[a].Create();
	}

	return TRUE;
}

#elif defined(_DALK_IOS_)
BOOL CSpriteBMFont::LoadFont(NSString* name) {
	NSString* filePath = [[NSBundle mainBundle] pathForResource:name ofType: @"fnt"];
//  NSString* filePath = [[NSBundle mainBundle] pathForResource:@"FTEST.fnt" ofType:nil];

	ParseFont([filePath UTF8String]);

	m_pTexture = new CTexture[Pages];
	
	for(int a = 0;a < Pages;a++) {
		NSString *temp = [NSString stringWithFormat:@"%@_%d.png", name, a];
		m_pTexture[a].LoadImageFile(temp);
		m_pTexture[a].Create();
	}

	return TRUE;
}
#endif

BOOL CSpriteBMFont::DrawChar(WORD code, float x, float y) {
	CharDescriptor& desc = Chars[code];

//	if(desc.Page != m_CurrentPage) {
		m_pTexture[desc.Page].SetTexture();
		m_CurrentPage = desc.Page;
//	}

	_MATRIXIDENTITY(m_TransMatrix);

	SetXYZ2D(x, y, (x + desc.XOffset + desc.Width), (y + desc.YOffset + desc.Height));
#if 0
	float rf[4]  = {
        (float)desc.x  / (float)m_pTexture[desc.Page].GetWidth(),
        (float)desc.y / (float)m_pTexture[desc.Page].GetHeight(),
        (float)(desc.x + desc.Width) / (float)m_pTexture[desc.Page].GetWidth(),
        (float)(desc.y + desc.Height) / (float)m_pTexture[desc.Page].GetHeight()
    };
#endif

    SetUV((int)desc.x, (int)desc.y, desc.x + desc.Width, desc.y + desc.Height, m_pTexture[desc.Page].GetWidth(), m_pTexture[desc.Page].GetHeight());
	SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

	_RES r;
#if defined(_DALK_WINDOWS_)
	r = CDirect3D::GetDevice()->SetTransform(D3DTS_WORLD, _MATRIXARRAYD3DPTR(m_TransMatrix));
	r = CDirect3D::GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, _Vertex, sizeof(TLVERTEX) );
#elif defined(_DALK_IOS_)
    EliVertex::SetVertexPointer(GetVertexType(),_Vertex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
	SIG_ERRORRETURNBOOL(r,"Failed@CSpriteBMFont::DrawVChar()");
	return TRUE;
}

#if 0
/*
	D3DXMATRIX WorldMatrix;
	D3DXMatrixIdentity( &WorldMatrix );
	D3DXMATRIX DestMatrix;
	D3DXMatrixIdentity( &DestMatrix );
/*
		//D3DXMatrixRotationYawPitchRoll(&matWorld,回転.y,回転.x,回転.z);
		WorldMatrix._11 = 1.0f;//vec3Scale.x * matWorld._11;
		WorldMatrix._12 = 1.0f;//vec3Scale.x * matWorld._12; 
		WorldMatrix._13 = 1.0f;//vec3Scale.x * matWorld._13; 
		        
		WorldMatrix._21 = 1.0f;//vec3Scale.y * matWorld._21; 
		WorldMatrix._22 = 1.0f;//vec3Scale.y * matWorld._22;
		WorldMatrix._23 = 1.0f;//vec3Scale.y * matWorld._23;

		WorldMatrix._31 = 1.0f;//vec3Scale.z * matWorld._31; 
		WorldMatrix._32 = 1.0f;//vec3Scale.z * matWorld._32;
		WorldMatrix._33 = 1.0f;//vec3Scale.z * matWorld._33;
*/
		//===========================================================
		//位置座標をSet
		//===========================================================
		//WorldMatrix._41 = 0;//m_PosX;
		//WorldMatrix._42 = 0;//m_PosY;  

	WorldMatrix = WorldMatrix * m_ScaleMatrix * m_RotateMatrix * m_TransMatrix;
	//HRESULT hres = CTexture::ms_pDevice->GetTransform( D3DTS_WORLD, &WorldMatrix );
	HRESULT hres = g_pD3DDevice.GetInstance()->SetTransform( D3DTS_WORLD, &WorldMatrix );
#endif

/*
void CSprite::SetWorldPos( float x, float y ) {
	x = x * CTexture::ms_pDevice->GetProjectionWidth() * 2.0f / 800.0f - CTexture::ms_pDevice->GetProjectionWidth();
	y = -1.0f * ( y * CTexture::ms_pDevice->GetProjectionHeight() * 2.0f / 600.0f - CTexture::ms_pDevice->GetProjectionHeight() );
	float z = 0.0f;
	SetTrans(x,y,z);
}
*/
/*
void CSprite2D::SetDiffuse( D3DCOLOR Color ) {
	 m_Diffuse = Color;

	 for(int a = 0;a < 4;a ++){
		 m_Vertex[a].color = Color;
	}
}

void CSprite3D::SetDiffuse( D3DCOLOR Color ) {
	 m_Diffuse = Color;

	 for(int a = 0;a < 4;a ++){
		 m_Vertex[a].color = Color;
	}
}
*/
/*
void CSprite::SetScaleAnimation(float x,float y,float z){
	D3DXVECTOR3 a(x,y,z);
	m_vScale[1] = a;
	//m_bEnableAnimation = end == 0xFFFFFFFF ? SPRITE_ANIMATION_SETVECTOR : SPRITE_ANIMATION_SETTIME ;
	//m_BeginTime = begin;
	//m_BeginTime = end;

}

void CSprite::SetTransAnimation(float x,float y,float z){
	D3DXVECTOR3 a(x,y,z);
	m_vTrans[1] = a;
	//m_bEnableAnimation = end == 0xFFFFFFFF ? SPRITE_ANIMATION_SETVECTOR : SPRITE_ANIMATION_SETTIME ;
	//m_BeginTime = begin;
	//m_BeginTime = end;
}
void CSprite::SetRotateAnimation(float x,float y,float z){
	D3DXVECTOR3 a(x,y,z);
	m_vRotate[1] = a;
	//m_bEnableAnimation = end == 0xFFFFFFFF ? SPRITE_ANIMATION_SETVECTOR : SPRITE_ANIMATION_SETTIME ;
	//m_BeginTime = begin;
	//m_BeginTime = end;
}

void CSprite::AddAnimation( DWORD time ){
	m_vRotate[0] += m_vRotate[1];
	m_vScale[0] += m_vScale[1];
	m_vTrans[0] += m_vTrans[1];
	//m_bEnableAnimation = end == 0xFFFFFFFF ? SPRITE_ANIMATION_SETVECTOR : SPRITE_ANIMATION_SETTIME ;
	//m_BeginTime = begin;
	//m_BeginTime = end;
}

void CSprite::AddAlphaAnimation( DWORD time ) {
	if( UpdateTimeCounter(time) ) {
		//int t1 = time - m_BeginTime;
		//int t2 = m_EndTime - m_BeginTime;
		m_Alpha =  ( ( m_AlphaState[0] - m_AlphaState[1] ) * GetTimeProgressRate()/ ) + m_AlphaState[1];
		SetDiffuse( D3DCOLOR_RGBA(0xFF,0xFF,0xFF,m_Alpha) );

		//αテスト
		if( m_bAlphaTest ) {
			CDirect3D* pDirect3D = CTexture::ms_pDevice;
			pDirect3D->SetAlphaRef( m_Alpha );
			pDirect3D->SetAlphaFunc( D3DCMP_GREATER );
			pDirect3D->SetAlphaTest( TRUE ); 
			//pDirect3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			//pDirect3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}

	}
	else {
		m_Alpha = m_AlphaState[0];
		SetDiffuse( D3DCOLOR_RGBA(0xFF,0xFF,0xFF,m_Alpha) );
		m_bEnableAnimation = 0;
	}
}

void CSprite::SetDiffuseAnimation( BYTE Alpha, DWORD time, bool bAlphaTest ) {
	m_AlphaState[0] = Alpha;
	m_AlphaState[1] = m_Vertex[0].color >> 24;

	m_bAlphaTest = bAlphaTest;
	m_bEnableAnimation = time == 0xFFFFFFFF ? SPRITE_ANIMATION_SETVECTOR : SPRITE_ANIMATION_SETTIME ;
	SetTimeCounter( time );
	//m_BeginTime = begin;
	//m_EndTime = end;
}
*/
/*
void CSprite::ZoomVertex(float Value){
	for(int a = 0;a < 4;a ++){
		m_Vertex[a].x *= Value;
		m_Vertex[a].y *= Value;
	}
}
*/
