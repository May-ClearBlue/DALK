#ifndef _SIG_D3DINIT_
#define _SIG_D3DINIT_

#include "DalkStdio.h"

//Saya:
//XPも廃れる流れなので、そのうち11.0(Windows7&XBoxOne)動作版も追加する予定です。
//Vista？　いえ、知らない子ですね……

#if defined(_DALK_USE_DIRECT3D_)

 #include <d3d9.h>
 #pragma comment(lib,"d3d9.lib")

 #if defined(_DEBUG)
 #pragma comment(lib,"d3dx9d.lib")
 #else
 #pragma comment(lib,"d3dx9.lib")
 #endif

 #pragma comment(lib, "dxguid.lib")

 #if 0
  #define _SIG_DIRECT3D_VERSION_		0x0A00				//DirectXのバージョン
  #define _SIG_SHADERMODEL_VERSION_	D3DVS_VERSION(5,0)	//シェーダモデルのバージョン

  #include <D3D11.h>
  #pragma comment(lib,"d3d11.lib")

  #if defined(_DEBUG)
   #pragma comment(lib,"d3dx11d.lib")
  #else
   #pragma comment(lib,"d3dx11.lib")
  #endif

 #pragma comment(lib, "dxguid.lib")
  #pragma comment(lib, "dxgi.lib")
  #pragma comment(lib, "DxErr.lib")
  #pragma comment(lib, "d3dcompiler.lib")
 #endif

#elif defined(_DALK_USE_OPENGL_)
 #ifdef _DALK_WINDOWS_
   #define GL_GLEXT_PROTOTYPES
   #include <glcorearb.h>
 #endif
 #include "SigD3DX.h"
#endif


//9.0c＆シェーダモデル3.0が基本（XBox360/美少女ゲーム用）
#define _SIG_DIRECT3D_VERSION_		0x0900					//DirectXのバージョン
#define _SIG_SHADERMODEL_VERSION_	D3DVS_VERSION(3,0)		//シェーダモデルのバージョン

//頂点トランスフォーム済み（基本的に使用しない）
#define FVF_2DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//固定頂点用パラメータ。OpenGL側のTLVとと一致させてます
static DWORD FVF_PARAM[5] = {
	D3DFVF_XYZ,
	D3DFVF_XYZ | D3DFVF_DIFFUSE,
	D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
	D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2,
	D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX3,
};

//DirectX-OpenGL返値、define等共通化
#if defined(_DALK_USE_DIRECT3D_)
typedef HRESULT _RES;
#ifdef GLenum
	typedef HRESULT	GLenum;
#endif

	typedef LPDIRECT3DVERTEXBUFFER9 _VERTEXBUFFER;
	typedef LPDIRECT3DTEXTURE9		_TEXTURE; 
	typedef	LPDIRECT3DSURFACE9		_RENDERBUFFER;
#elif defined(_DALK_USE_OPENGL_)
	typedef GLenum HRESULT;
	typedef GLenum _RES;
	typedef GLuint	_VERTEXBUFFER;
	typedef GLuint	_TEXTURE;
	typedef	GLuint	_RENDERBUFFER;

	#define SUCCEEDED(hr) ( (hr = glGetError()) == GL_NO_ERROR )
	#define FAILED(hr) ( (hr = glGetError()) != GL_NO_ERROR )
#endif

//エラーチェック用マクロ。RELEASEは何もしない
#if defined(_DALK_USE_DIRECT3D_) && (_DEBUG)
#define SIG_ERRORCHECK(hr,mes)\
  if(FAILED(hr))\
	_DEBUGPRINTF(mes);
#define SIG_ERRORRETURN(hr,mes)\
	if(FAILED(hr)){\
		_DEBUGPRINTF(mes);\
		return hr;\
	}
#define SIG_ERRORRETURNBOOL(hr,mes)\
	if(FAILED(hr)){\
		_DEBUGPRINTF(mes);\
		return 	FALSE;\
	}

#elif defined(_DALK_USE_OPENGL_) && (DEBUG)
#define SIG_ERRORCHECK(hr,mes)\
  if((hr = glGetError()) != GL_NO_ERROR)\
	_DEBUGPRINTF(mes);
#define SIG_ERRORRETURN(hr,mes)\
	if((hr = glGetError()) != GL_NO_ERROR){\
		_DEBUGPRINTF(mes);\
		return hr;\
	}
#define SIG_ERRORRETURNBOOL(hr,mes)\
	if((hr = glGetError()) != GL_NO_ERROR){\
		_DEBUGPRINTF(mes);\
		return 	FALSE;\
	}
#else
#define SIG_ERRORCHECK(hr,mes) 
#define SIG_ERRORRETURN(hr,mes) 
#define SIG_ERRORRETURNBOOL(hr,mes) 
#endif 

#endif
