#ifndef _ELI_SHADER_
#define _ELI_SHADER_

#include <MultiStdafx.h>

//#if (TARGET_IPHONE || TARGET_OS_IPHONE)
//#ifdef _DALK_IOS_ || _DALK_ANDROID_

#include "EliVertexTypeDef.h"
#include "EliMath.h"

#if defined(_DALK_USE_DIRECT3D_)
#include <d3dx9.h>			//ToDo:Remove
#include "Shader/HLSL/Default_Vert.h"
#define _SIG_VERTEXSHADER_PROFILE_	"vs_3_0"	//バーテックスシェーダのプロファイル：現状9.0c
#define _SIG_PIXELSHADER_PROFILE_	"ps_3_0"	//ピクセルシェーダのプロファイル：現状9.0c
#define _SIG_HLSL_ENTRYPOINT_		"main"		//HLSLコンパイル時のエントリーポイント		
#elif defined(_DALK_USE_OPENGL_)
#include "Shader/GLSL/Default.vert"
#include "Shader/GLSL/Default.frag"
#include "Shader/GLSL/DefaultA8.flag"
#include "Shader/GLSL/UV2.vert"
#include "Shader/GLSL/UV2.flag"
#include "Shader/GLSL/NoTexture.vert"
#include "Shader/GLSL/NoTexture.flag"
#endif

//Uniform
enum SHADER_UNIFORM_TYPE {
	SH_UNI_TYPE_INT,
	SH_UNI_TYPE_FLOAT,
	SH_UNI_TYPE_MATRIX
};

enum SHADER_UNIFORM_INDEX {
	SH_UNI_DEFY,	// 16:9との画面サイズとの誤差
	SH_UNI_OFSX,	// OffsetX
	SH_UNI_OFSY,	// OffsetY
	SH_UNI_TEX0,	// Sampler
	SH_UNI_TEX1,	// Sampler
	SH_UNI_TEX2,	// Sampler
	SH_UNI_WORLD,
	SH_UNI_VIEW,
	SH_UNI_PROJECTION,
	SH_UNI_TERMINATE
};

enum SHADE_UNIFORM_ENUM {
	SH_UNI_ENUM_PL,
	SH_UNI_ENUM_PTL0,
	SH_UNI_ENUM_PTL1,
	SH_UNI_ENUM_PTL2,
	SH_UNI_ENUM_TERMINATE
};


struct ShaderUniformInfo {
	char name[32];		//
	SHADER_UNIFORM_INDEX index;
	SHADER_UNIFORM_TYPE	type;
	int	register_number;		//HLSLのレジスタ番号：int, float4で違う
};

const ShaderUniformInfo ShaderUniformRef[SH_UNI_TERMINATE] = {
	{"diffY"			, SH_UNI_DEFY		, SH_UNI_TYPE_INT, 0 },
	{"ofssetX"			, SH_UNI_OFSX		, SH_UNI_TYPE_INT, 1 },
	{"ofssetY"			, SH_UNI_OFSY		, SH_UNI_TYPE_INT, 2 },
	{"texture0"			, SH_UNI_TEX0		, SH_UNI_TYPE_INT, 3 },
	{"texture1"			, SH_UNI_TEX1		, SH_UNI_TYPE_INT, 4 },		
	{"texture2"			, SH_UNI_TEX2		, SH_UNI_TYPE_INT, 5 },		
	{"world_matrix"		, SH_UNI_WORLD		, SH_UNI_TYPE_MATRIX, 0 },
	{"view_matrix"		, SH_UNI_VIEW		, SH_UNI_TYPE_MATRIX, 4 },
	{"projection_matrix", SH_UNI_PROJECTION	, SH_UNI_TYPE_MATRIX, 8 },
};

struct ShaderUniformVar {
	int	diffY;
	int offsetX;
	int offsetY;
	int texture0;
	int texture1;
	int texture2;
	_MATRIX world;
	_MATRIX view;
	_MATRIX projection;
};

//SHADE_UNIFORM_ENUMと同期
static SHADER_UNIFORM_INDEX ShaderUniformIndexList[][SH_UNI_TERMINATE+1] = {
	{ SH_UNI_DEFY, SH_UNI_OFSX, SH_UNI_OFSY, SH_UNI_WORLD, SH_UNI_VIEW,  SH_UNI_PROJECTION, SH_UNI_TERMINATE },
	{ SH_UNI_DEFY, SH_UNI_OFSX, SH_UNI_OFSY, SH_UNI_TEX0,  SH_UNI_WORLD, SH_UNI_VIEW,  SH_UNI_PROJECTION, SH_UNI_TERMINATE },
	{ SH_UNI_DEFY, SH_UNI_OFSX, SH_UNI_OFSY, SH_UNI_TEX0,  SH_UNI_TEX1,  SH_UNI_WORLD, SH_UNI_VIEW,       SH_UNI_PROJECTION, SH_UNI_TERMINATE },
	{ SH_UNI_DEFY, SH_UNI_OFSX, SH_UNI_OFSY, SH_UNI_TEX0,  SH_UNI_TEX1,  SH_UNI_TEX2,  SH_UNI_WORLD,      SH_UNI_VIEW,       SH_UNI_PROJECTION, SH_UNI_TERMINATE },
};

#if defined(_DALK_USE_DIRECT3D_)
inline BOOL  SetVertexStreamUp(void* pV, VERTEX_TYPE type, DWORD num, LPDIRECT3DVERTEXDECLARATION9& pDeclarateion) {
	D3DVERTEXELEMENT9 decl_end = D3DDECL_END();

	std::vector<D3DVERTEXELEMENT9> el;

	for(int a = 0;a < type+1;a++)
		el.push_back(ShaderAttributeInfoHLSL[a]);

	el.push_back( decl_end );

	LPDIRECT3DVERTEXDECLARATION9 g_pVertexDeclaration;
	_RES r = CDirect3D::GetDevice()->CreateVertexDeclaration(el.data() , &g_pVertexDeclaration);
	SIG_ERRORRETURNBOOL(r,"Failed@Direct3DDevice::CreateVertexDeclaration@SetVertexStreamUp");

	if(pDeclarateion) {
		r = pDeclarateion->Release();
		SIG_ERRORRETURNBOOL(r,"Failed@LPDIRECT3DVERTEXDECLARATION9::Release@SetVertexStreamUp");
	}

	r = CDirect3D::GetDevice()->SetVertexDeclaration( g_pVertexDeclaration );
	SIG_ERRORRETURNBOOL(r,"Failed@Direct3DDevice::SetVertexDeclaration@SetVertexStreamUp");

	return TRUE;
}
#endif

#ifdef _DALK_DEBUG_BUILD_
#define CHECKUNIFORMINDEX(a) 
#if 0
 #if defined(_DALK_IOS_)
  #define CHECKUNIFORMINDEX(a)\
	if(m_UniformIndex[a] == -1)\
		NSLog(@"No Use Index:%d",a);\
	else
 #else
  #define CHECKUNIFORMINDEX(a)\
	if(m_UniformIndex[a] == -1)\
		_DEBUGPRINTF("No Use Index");\
	else
 #endif
#endif
#else

#define CHECKUNIFORMINDEX(a) 
#endif

//////

#define _SHADER_NUM_MAX_ 10
enum _SHADERTYPE_ {
	_SHADER_DEFAULT_,
	_SHADER_A8_,
	_SHADER_UV2,
	_SHADER_NOTEXTURE_
};

struct _ShaderSource {
	const char* _vert;
	const char* _flag;
	SHADE_UNIFORM_ENUM  _type;
};

#if defined(_DALK_USE_OPENGL_)
const _ShaderSource ShaderSourceList[5] = {
	{ DefaultVertexShader, DefaultFragmentShader,       SH_UNI_ENUM_PTL0 },
	{ DefaultVertexShader, DefaultA8FragmentShader,     SH_UNI_ENUM_PTL0 },
	{ UV2VertexShader,     UV2FragmentShader,           SH_UNI_ENUM_PTL1 },
	{ NoTextureVertexShader,  NoTextureFragmentShader, 	SH_UNI_ENUM_PL },
	{ NULL, NULL ,SH_UNI_ENUM_TERMINATE }
};
#endif


class CShader {
public:
	CShader();
	~CShader() { Release(); }

protected:
static	CShader		g_ShaderInstance[_SHADER_NUM_MAX_];
static	CShader*	g_ShaderList[_SHADER_NUM_MAX_];
static	CShader*	g_pCurrentShader;

	SHADE_UNIFORM_ENUM	m_UniformType;
	ShaderUniformVar	m_UniformVar;
	/*GLint*/int32_t	m_UniformIndex[SH_UNI_TERMINATE];

#if defined(_DALK_USE_DIRECT3D_)
		LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;
		LPDIRECT3DVERTEXSHADER9			m_pVertexShader;
		LPDIRECT3DPIXELSHADER9			m_pPixelShader;
#elif defined(_DALK_USE_OPENGL_)
		GLuint	m_ProgramID;

		BOOL	GetCompileError(GLuint id);
		BOOL	GetLinkError();
#endif
		void	SetUniformVar(_MATRIX& matrix);
		void	SetUniformVar(int& i);
		void	SetUniformVar(float& f);
		void	SetUniformVar(_VECTOR2& f);
		void	SetUniformVar(_VECTOR3& f);
		void	SetUniformVar(_VECTOR4& f);

public:
inline	void	RegistShaderList(_SHADERTYPE_ num)	{ g_ShaderList[num] = this; }
static	void	ChangeShader	(_SHADERTYPE_ num)	{
#ifdef _DALK_DEBUG_BUILD_
		if(g_ShaderList[num] == NULL)
			return;
#endif
		if( g_pCurrentShader != g_ShaderList[num]) {
			_RES res;
			g_ShaderList[num]->SetShader();
			SIG_ERRORCHECK(res,_TEXT("FAILED::SetShader"));
			g_pCurrentShader = g_ShaderList[num];

			g_pCurrentShader->SetUniform();
		}
	}
		void	Release();

		void	SetUniformIndex();
		void	SetUniform();

inline	void	SetProjectionMatrix	( _MATRIX* pMatrix )									{ m_UniformVar.projection = *pMatrix; }
		void	SetProjectionMatrix();
inline	void	SetViewMatrix		( _MATRIX* pMatrix )									{ m_UniformVar.view = *pMatrix; }
		void	SetViewMatrix();
inline	void	SetWorldMatrix		( _MATRIX* pMatrix )									{ m_UniformVar.world = *pMatrix; }
		void	SetWorldMatrix();
#if defined(_DALK_USE_DIRECT3D_)
inline	void	SetProjectionMatrix(float fovy, float aspect, float znear, float zfar)	{ D3DXMatrixPerspectiveFovLH(_MATRIXARRAYD3DXPTR(m_UniformVar.projection), fovy, aspect, znear, zfar); }
inline	void	SetProjectionOrtho	( float left, float right, float bottom, float top, float znear, float zfar) { D3DXMatrixOrthoLH(_MATRIXARRAYD3DXPTR(m_UniformVar.projection), right - left + 1, bottom - top + 1, znear, zfar); }
inline	void	SetProjectionFrustum( float left, float right, float bottom, float top, float zNear, float zFar) { /*→存在しなかったD3DXMatrixFrustum(m_UniformVar.projection);*/ }
inline	void	SetViewMatrix(_VECTOR& Eye, _VECTOR& At, _VECTOR& Up)						{ D3DXMatrixLookAtLH(_MATRIXARRAYD3DXPTR(m_UniformVar.view), _VECTOR3D3DXPTR(Eye), _VECTOR3D3DXPTR(At), _VECTOR3D3DXPTR(Up)); }
#elif defined(_DALK_IOS_)
inline	void	SetShader(){ glUseProgram(m_ProgramID); /*SetUniform();*/ }
inline	void	SetProjectionMatrix	( float fovy, float aspect, float znear, float zfar )	{ m_UniformVar.projection = glm::perspective(fovy, aspect, znear, zfar); }
inline	void	SetProjectionOrtho	( float left, float right, float bottom, float top, float zNear, float zFar) { m_UniformVar.projection = glm::ortho( left, right, bottom, top, zNear, zFar); }
inline	void	SetProjectionFrustum( float left, float right, float bottom, float top, float zNear, float zFar) { m_UniformVar.projection = glm::frustum( left, right, bottom, top, zNear, zFar); }
inline	void	SetViewMatrix		( _VECTOR& Eye, _VECTOR& At, _VECTOR& Up )				{ m_UniformVar.view = glm::lookAt(Eye, At, Up); }
#endif
        BOOL	CreateShaderFromString	(const char* Ver, const char* Fl, SHADE_UNIFORM_ENUM format);
        BOOL	CreateShaderFromFile	(const char* Ver, const char* Fl, SHADE_UNIFORM_ENUM format);

#ifdef	_DALK_WINDOWS_
		BOOL	CreateShaderFromBinary	(const void* pData){
			_RES hr;
			hr = CDirect3D::GetDevice()->CreateVertexShader( (const DWORD *)pData, &m_pVertexShader);
			SIG_ERRORRETURNBOOL(hr, "Failed:CreateVertexShader");
			hr = CDirect3D::GetDevice()->CreatePixelShader( (const DWORD *)pData, &m_pPixelShader);
			SIG_ERRORRETURNBOOL(hr, "Failed:CreatePixelShader");
			return TRUE;
		}

		BOOL	SetShader() {
		_RES hr;
			hr = CDirect3D::GetDevice()->SetVertexDeclaration(m_pVertexDeclaration);
			SIG_ERRORRETURNBOOL(hr, "Failed:CreatePixelShader");
			hr = CDirect3D::GetDevice()->SetVertexShader	( m_pVertexShader );
			SIG_ERRORRETURNBOOL(hr, "Failed:CreatePixelShader");
			hr = CDirect3D::GetDevice()->SetPixelShader	( m_pPixelShader );
			SIG_ERRORRETURNBOOL(hr, "Failed:CreatePixelShader");
		}
inline	void SetVertexDeclaration() { CDirect3D::GetDevice()->SetVertexDeclaration( m_pVertexDeclaration ); }
#endif
#ifdef _DALK_IOS_
		BOOL	CreateShaderFromFile	(NSString* Ver, NSString* Fl, SHADE_UNIFORM_ENUM format);
#endif  
};

class CShaderManager {
private:
static CShader	g_Shader[10];
public:
static 	void InitAllShader(DWORD width, DWORD height);
};

#endif