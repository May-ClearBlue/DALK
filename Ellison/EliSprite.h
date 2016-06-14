#ifndef _SAIL_SPRITE_
#define _SAIL_SPRITE_

#include "MultiStdafx.h"
#include "EliVertexBuffer.h"
#include "EliTexture.h"
#include "EliVertexSquareTLV.h"
#include <Rimlia/bmfont.h>

class ISprite {
protected:
	_MATRIX		m_ScaleMatrix;
	_MATRIX		m_RotateMatrix;
	_MATRIX		m_TransMatrix;

public:
		ISprite();
virtual	~ISprite() {};

inline	void	SetMove			( float x, float y )			{ _MATRIXTRANSLATE( m_TransMatrix, x ,y, 0.0f); }
inline	void	SetRotate		( float x, float y, float z  )	{ ; }//_MATRIXROTATE( m_RotateMatrix, x, y, z ); }
inline	void	SetZoom			( float x, float y, float z )	{ _MATRIXSCALE( m_ScaleMatrix, x ,y, z); }

//純粋仮想関数
virtual	BOOL	Draw	() = 0;
virtual	BOOL	DrawUp	() = 0;

};

class CSprite2D : public ISprite, public CVertexSquareTLV/*, public CVertexBuffer*/{
protected:
//	CVertexSquareTLV	m_Vertex;
	CVertexBuffer	m_VertexBuffer;
	CTexture*		m_pTexture;

public:
	CSprite2D();
	~CSprite2D(){ ; };

		void	SetTexture	( CTexture* pTexture );
		BOOL	Draw		();
		BOOL	DrawUp		();

		void	SetTextureRect	( float left, float top, float right, float bottom );				//pixel単位
		void	SetPolygonRect	( float left, float top, float right, float bottom );				//pixel単位
};

class CSpriteBMFont : public ISprite, public BMFont, public CVertexSquareTLV/*, public CVertexBuffer*/{
protected:
//	CVertexSquareTLV	m_Vertex;
	CVertexBuffer	m_VertexBuffer;
	CTexture*		m_pTexture;

	int				m_CurrentPage;

public:

	CSpriteBMFont() { m_CurrentPage = -1; }
	~CSpriteBMFont(){ Release(); };

#if defined(_DALK_WINDOWS_)
		BOOL	LoadFont(const char* name);
#elif defined(_DALK_IOS_)
		BOOL	LoadFont(NSString* name);
#endif

		BOOL	Release(){ delete[] m_pTexture; return TRUE; }

		void	SetTexture	( CTexture* pTexture );
		BOOL	DrawChar	( WORD code, float x, float y );
		BOOL	Draw()		{ return TRUE; };
		BOOL	DrawUp()	{ return TRUE; };

		void	SetTextureRect	( float left, float top, float right, float bottom );				//pixel単位
		void	SetPolygonRect	( float left, float top, float right, float bottom );				//pixel単位
};


#if 0
class CMesh {
protected:
	DWORD			pNumMaterials;
	LPD3DXMESH		pMesh;
	LPD3DXBUFFER	pD3DBuffer;
public:
	void			Load(char* name) {
		HRESULT		hr;
		hr = D3DXLoadMeshFromX( "Objects\\TEST.x", D3DXMESH_VB_MANAGED, oApp.GetDirect3D().GetDirect3DDevice(), NULL, &pD3DBuffer, NULL, &pNumMaterials, &pMesh);
		if (FAILED(hr)) return FALSE;

		D3DXMATERIAL *pmtrls = (D3DXMATERIAL *)pD3DBuffer->GetBufferPointer();

		D3DMATERIAL9* pMeshMaterials = new D3DMATERIAL9[pNumMaterials];
	
		LPDIRECT3DTEXTURE9* pMeshTextures	 = new LPDIRECT3DTEXTURE9[pNumMaterials];

		for (DWORD i=0; i<pNumMaterials; i++) {
			pMeshMaterials[i] = pmtrls[i].MatD3D;
			pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;

			hr = D3DXCreateTextureFromFile(
				oApp.GetDirect3D().GetDirect3DDevice(),
				pmtrls[i].pTextureFilename,
				&pMeshTextures[i]);
			if (FAILED(hr)) pMeshTextures[i] = NULL;
		}

		pD3DBuffer->Release();
	}

	void	Draw()
	{		oApp.GetDirect3D().Clear(0,0,0);
			oApp.GetDirect3D().BeginScene();
			oApp.GetDirect3D().SetZBuffer(TRUE);
			oApp.GetDirect3D().SetZBufWrite(TRUE);
			oApp.GetDirect3D().SetLighting(TRUE);
			oApp.GetDirect3D().GetDirect3DDevice()->SetRenderState( D3DRS_AMBIENT, 0xffffffff);

			for (DWORD i=0; i<pNumMaterials; i++) {
				oApp.GetDirect3D().GetDirect3DDevice()->SetMaterial(&pMeshMaterials[i]);
				oApp.GetDirect3D().GetDirect3DDevice()->SetTexture(0, pMeshTextures[i]);

				pMesh->DrawSubset(i);
			}
			oApp.GetDirect3D().EndScene();
			oApp.GetDirect3D().Present();
	}
};
#endif

#endif