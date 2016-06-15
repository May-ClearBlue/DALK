#ifndef _SIG_3DTYPEDEF_
#define _SIG_3DTYPEDEF_

#include "DalkTypedef.h"
#if defined _DALK_WINDOWS_
#include "Siglune/SigDirect3DDevice.h"
#else
#include "Siglune/SigDirect3DInit.h"
#endif
//頂点種類の列挙子。
enum VERTEX_TYPE {
	VX_P,		// struct VERTEX		頂点座標のみ
	VX_PL,		// struct LVERTEX		頂点+Diffuse
	VX_PTL0,	// struct TLVERTEX		頂点+色+UV*1
	VX_PTL1,	// struct TLVERTEX2		頂点+色+UV*2
	VX_PTL2,	// struct TLVERTEX3		頂点+色+UV*3
	VX_NUM,		// 種類のカウント
	VX_INVALID = 100	//無効
};

//Attribute

enum ATTRIB_TYPE {
	SH_ATR_POSITION,
	SH_ATR_DIFFUSE,
	SH_ATR_UV0,
	SH_ATR_UV1,
	SH_ATR_UV2,
	SH_ATR_SPECULAR,
	SH_ATR_DIFFUSE1,
	SH_ATR_DIFFUSE2,
	SH_ATR_TERMINATE
};

struct ShaderAttributeInfo {
	char	name[32];
	int		size_count;
//	int		size_total;
//	int		size_offset;
};

//ATTRIB_TYPEと連動
const ShaderAttributeInfo ShaderAttributeRef[SH_ATR_TERMINATE] = {
	{"position"	, 3 },//, 3, 0 },
	{"diffuse"	, 4 },//, 7, sizeof(float)*3 },
	{"uv0"		, 2 },//, 9, sizeof(float)*7 },
	{"uv1"		, 2 },//, 11,sizeof(float)*9 },		
	{"uv2"		, 2 },//, 13,sizeof(float)*11 },
	{"specular"	, 4 },//, 17,sizeof(float)*11 },
	{"diffuse1"	, 4 },//, 17,sizeof(float)*11 },
	{"diffuse2"	, 4 },//, 17,sizeof(float)*11 },
};

//VERTEX_TYPEと連動
static ATTRIB_TYPE _FIXED_VERTEX_FORMAT_[][6] = {
	{ SH_ATR_POSITION, SH_ATR_TERMINATE },
	{ SH_ATR_POSITION, SH_ATR_DIFFUSE, SH_ATR_TERMINATE  },
	{ SH_ATR_POSITION, SH_ATR_DIFFUSE, SH_ATR_UV0, SH_ATR_TERMINATE },
	{ SH_ATR_POSITION, SH_ATR_DIFFUSE, SH_ATR_UV0, SH_ATR_UV1, SH_ATR_TERMINATE },
	{ SH_ATR_POSITION, SH_ATR_DIFFUSE, SH_ATR_UV0, SH_ATR_UV1, SH_ATR_UV2, SH_ATR_TERMINATE},
};

#ifdef _DALK_WINDOWS_

//VERTEX_TYPEと連動
const D3DVERTEXELEMENT9 ShaderAttributeInfoHLSL[SH_ATR_TERMINATE] =  {
	//Position
	{ 0, ShaderAttributeRef[SH_ATR_POSITION].size_count, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	//Diffuse color
	{ 0, ShaderAttributeRef[SH_ATR_DIFFUSE].size_count,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	//Base tex coords
	{ 0, ShaderAttributeRef[SH_ATR_UV0].size_count,     D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	//Base tex coords
	{ 0, ShaderAttributeRef[SH_ATR_UV1].size_count,     D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	//Base tex coords
	{ 0, ShaderAttributeRef[SH_ATR_UV2].size_count,     D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
//	D3DDECL_END()
};
#endif

//--------アライメント4byte強制ここから--------

#ifdef _DALK_WINDOWS_
 #pragma pack(push,4)
#else
 #pragma pack(4)
#endif

//RECTのfloat版
struct FRECT {
	float	left,top,right,bottom;
};

//
//頂点座標のみ
typedef struct tagVERTEX
{
	float		x, y, z;
} VERTEX;

//頂点＋Diffuse
typedef struct tagLVERTEX
{
	float		x, y, z;
#if defined(_DALK_WINDOWS_)
	DWORD	color;
#else
	float		r, g, b, a;
#endif
} LVERTEX;

//頂点＋色＋UV＊１
typedef struct tagTLVERTEX
{
	float		x, y, z;

#if defined(_DALK_WINDOWS_)
	DWORD	color;
#else
	float		r, g, b, a;
#endif

	float		u, v;
} TLVERTEX;

//頂点＋色＋UV＊２
typedef struct tagTLVERTEX2
{
	float		x, y, z;

#if defined(_DALK_WINDOWS_)
	DWORD	color;
#else
	float		r, g, b, a;
#endif

	float		u, v;
	float		u2, v2;
} TLVERTEX2;

//頂点＋色＋UV＊３
typedef struct tagTLVERTEX3
{
	float		x, y, z;

#if defined(_DALK_WINDOWS_)
	float		r, g, b, a;
#else
	DWORD	color;
#endif

	float		u, v;
	float		u2, v2;
	float		u3, v3;
} TLVERTEX3;

#ifdef _DALK_WINDOWS_
 #pragma pack(pop)
#else
 #pragma pack()
#endif
//--------アライメント4byte強制ここまで--------

//VERTEX_TYPEと連動
//サイズ
static const DWORD VERTEX_SIZE[5] =  {
	sizeof(VERTEX),
	sizeof(LVERTEX),
	sizeof(TLVERTEX),
	sizeof(TLVERTEX2),
	sizeof(TLVERTEX3)
};

//クラス外の単純な関数はnamespaceで覆う
namespace EliVertex {

inline void SetFRECT( FRECT& rect, float left, float top, float right, float bottom) {
	rect.left = left;
	rect.top = top;
	rect.right = right;
	rect.bottom = bottom;
}

inline void SetVertexXYZ(TLVERTEX& v, float x, float y, float z) {
	v.x = x;
	v.y = y;
	v.z = z;
}

template<typename T>inline void SetVertexColor(T& v, float r,float g, float b, float a ) {
#if defined(_DALK_WINDOWS_)
	v.color =  D3DCOLOR_COLORVALUE(r,g,b,a);
#elif defined(_DALK_IOS_)
	v.r = r;
	v.g = g;
	v.b = b;
	v.a = a;
#endif
}

template<typename T>inline void SetVertexUV(T& vx, float u, float v) {
	vx.u = u;
	vx.v = v;
}

inline void SetVertex(TLVERTEX& vx, float x, float y, float z, float r,float g, float b, float a, float u, float v) {
	vx.x = x;
	vx.y = y;
	vx.z = z;
#if defined(_DALK_WINDOWS_)
	vx.color =  D3DCOLOR_COLORVALUE(r,g,b,a);
#elif defined(_DALK_IOS_)
	vx.r = r;
	vx.g = g;
	vx.b = b;
	vx.a = a;
#endif
	vx.u = u;
	vx.v = v;
}

inline void SetVertex(TLVERTEX2& vx, float x, float y, float z, float r,float g, float b, float a, float u, float v, float u2, float v2) {
	vx.x = x;
	vx.y = y;
	vx.z = z;
#if defined(_DALK_WINDOWS_)
	vx.color =  D3DCOLOR_COLORVALUE(r,g,b,a);
#elif defined(_DALK_IOS_)
	vx.r = r;
	vx.g = g;
	vx.b = b;
	vx.a = a;
#endif
	vx.u = u;
	vx.v = v;
	vx.u2 = u2;
	vx.v2 = v2;
}

#if defined(_DALK_IOS_)
//頂点バッファの時はpDataはNULL
inline void SetVertexPointer(VERTEX_TYPE type, void* pPointer) {
	GLenum res;
	BYTE* pData = (BYTE*)pPointer;

	if(pData != NULL) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
        SIG_ERRORCHECK(res,"Failed@glBindBuffer");
	}
	
	GLuint totalsize = 0;
	GLuint index = 0;

	for(ATTRIB_TYPE* a = _FIXED_VERTEX_FORMAT_[type];*a != SH_ATR_TERMINATE ;a++) {
		glEnableVertexAttribArray(index);
		SIG_ERRORCHECK(res,"Failed@glEnableVertexAttribArray");
		// index, num, type, normalized, datalen, bufp
		glVertexAttribPointer(index, ShaderAttributeRef[*a].size_count, GL_FLOAT, GL_FALSE, VERTEX_SIZE[type], (void*)(pData+totalsize));
		SIG_ERRORCHECK(res,"Failed@glVertexAttribPointer");
        
        totalsize += ShaderAttributeRef[*a].size_count * sizeof(float);
		index++;
	}
}
#endif

}; //namespace EliVertex


//2Dポリゴンのテンプレ－ト
template<typename T, VERTEX_TYPE U> class CVertexSquare {
public:
	CVertexSquare() : m_VertexType(U) { ; };
protected:
	const VERTEX_TYPE	m_VertexType;
	T	_Vertex[4];
public:
inline	const VERTEX_TYPE	GetVertexType()	{ return m_VertexType; }
inline	T*					GetArray()		{ return _Vertex; }

//(左上－右上－左下－右下）//右回り：D3D D3DCULL_CCW：Ｚ字)
//(左上－左下－右上－右下）//左回り：GL D3DCULL_CW)：逆Ｎ字
inline void SetXYZ(float left, float top, float right, float bottom, float z = 0.0f) {
#if defined(_DALK_WINDOWS_)
enum { _LU, _RU, _LD, _RD };
#elif defined(_DALK_IOS_)
enum { _LU, _LD, _RU, _RD };
#endif
		_Vertex[_LU].x = left;	_Vertex[_LU].y = top;		_Vertex[_LU].z = z;
		_Vertex[_RU].x = right;	_Vertex[_RU].y = top;		_Vertex[_RU].z = z;
		_Vertex[_LD].x = left;	_Vertex[_LD].y = bottom;	_Vertex[_LD].z = z;
		_Vertex[_RD].x = right;	_Vertex[_RD].y = bottom;	_Vertex[_RD].z = z;     
}

//ディフューズ指定(DWORD版、GL、DX共通）
inline	void SetDiffuse(D3DCOLOR color) {
		for(int c = 0; c < 4; c++ ){
#if defined(_DALK_WINDOWS_)
			_Vertex[c].color = color;
#elif defined(_DALK_IOS_)
            _COLORVALUE val;
            val.color = color;
			_Vertex[c].r = (float)val.rgba[0] / 255.0f;
			_Vertex[c].g = (float)val.rgba[1] / 255.0f;
			_Vertex[c].b = (float)val.rgba[2] / 255.0f;
			_Vertex[c].a = (float)val.rgba[3] / 255.0f;
#endif
		}
}

inline	void SetDiffuse(float r,float g, float b, float a) {
		for(int c = 0; c < 4; c++ ){
#if defined(_DALK_WINDOWS_)
			_Vertex[c].color =  D3DCOLOR_COLORVALUE(r,g,b,a);
#elif defined(_DALK_IOS_)
			_Vertex[c].r = r; _Vertex[c].g = g; _Vertex[c].b = b; _Vertex[c].a = a;
#endif
		}
}

inline void SetXYZ( RECT& rect )		{ SetXYZ( rect.left, rect.top, rect.right, rect.bottom ); }
inline void SetXYZ( FRECT& rect )		{ SetXYZ( rect.left, rect.top, rect.right, rect.bottom ); }
inline void SetXYZ2D( int left, int top, int right, int bottom, int z = 0 ) { SetXYZ( (float)left, (float)top, (float)right, (float)bottom, (float)z); 	}
//virtual	void SetUV		(float left, float top, float right, float bottom) { ; }
//virtual	void SetUV2		(float left, float top, float right, float bottom) { ; }
//virtual	void SetUV3		(float left, float top, float right, float bottom) { ; }
virtual	void DrawPrimitiveUp() {
	_RES r;
#if defined(_DALK_WINDOWS_)
	r = CDirect3D::GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, _Vertex, sizeof(T) );
#elif defined(_DALK_IOS_)
	EliVertex::SetVertexPointer(GetVertexType(), _Vertex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
    SIG_ERRORCHECK(r,TEXT("Failed@CVertexSquare::DrawPrimitiveUp()"));
}

};

typedef CVertexSquare<LVERTEX,  VX_PL> CVertexSquareLV;

#endif
