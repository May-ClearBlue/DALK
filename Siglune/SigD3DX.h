#ifndef _SIG_D3DX_
#define _SIG_D3DX_

//D3DXのDefineをOpenGLで使うために抜粋してきたもの。そもそもD3DX依存をしない方向なのであまりメンテはしません。
#ifdef _DALK_USE_OPENGL_
#include <OpenGLES/ES2/glext.h>
#endif

#ifndef CONST
 #define CONST const
#endif

#ifndef FLOAT
 #define FLOAT float
#endif

#define D3DX_PI    ((FLOAT)  3.141592654f)

typedef struct D3DXFLOAT16
{
#ifdef __cplusplus
public:
    D3DXFLOAT16() {};
    D3DXFLOAT16( FLOAT );
    D3DXFLOAT16( CONST D3DXFLOAT16& );

    // casting
    operator FLOAT ();

    // binary operators
    BOOL operator == ( CONST D3DXFLOAT16& ) const;
    BOOL operator != ( CONST D3DXFLOAT16& ) const;

protected:
#endif //__cplusplus
    WORD value;
} D3DXFLOAT16, *LPD3DXFLOAT16;

//D3D9TYPES

//-------------------------------------------------------------------

// Flexible vertex format bits
//
#define D3DFVF_RESERVED0        0x001
#define D3DFVF_POSITION_MASK    0x400E
#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZRHW           0x004
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c
#define D3DFVF_XYZB5            0x00e
#define D3DFVF_XYZW             0x4002

#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080

#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300
#define D3DFVF_TEX4             0x400
#define D3DFVF_TEX5             0x500
#define D3DFVF_TEX6             0x600
#define D3DFVF_TEX7             0x700
#define D3DFVF_TEX8             0x800

#define D3DFVF_LASTBETA_UBYTE4   0x1000
#define D3DFVF_LASTBETA_D3DCOLOR 0x8000

#define D3DFVF_RESERVED2         0x6000  // 2 reserved bits

//OpenGL用拡張
#define D3DFVF_DIFFUSE1          0x040
#define D3DFVF_DIFFUSE2          0x080

// D3DCOLOR is equivalent to D3DFMT_A8R8G8B8
#ifndef D3DCOLOR_DEFINED
typedef DWORD D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

// maps unsigned 8 bits/channel to D3DCOLOR
#define D3DCOLOR_ARGB(a,r,g,b) \
((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)

#define D3DCOLOR_XYUV(y,u,v)   D3DCOLOR_ARGB(0xff,y,u,v)
#define D3DCOLOR_AYUV(a,y,u,v) D3DCOLOR_ARGB(a,y,u,v)

// maps floating point channels (0.f to 1.f range) to D3DCOLOR
#define D3DCOLOR_COLORVALUE(r,g,b,a) \
D3DCOLOR_RGBA((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))


typedef enum _D3DFORMAT
{
	D3DFMT_UNKNOWN		= 0,
	D3DFMT_R8G8B8		= GL_RGB,
	D3DFMT_X8R8G8B8		= GL_RGBA,
	D3DFMT_A8R8G8B8		= GL_RGBA,
	D3DFMT_A8B8G8R8		= GL_BGRA,
	D3DFMT_A8			= GL_ALPHA,
	D3DFMT_FORCE_DWORD	= 0x7fffffff
} D3DFORMAT;

union _COLORVALUE {
	D3DCOLOR	color;
	BYTE		rgba[4];
};

// Primitives supported by draw-primitive API
enum D3DPRIMITIVETYPE {
	D3DPT_POINTLIST             = GL_POINTS,
	D3DPT_LINELIST              = GL_LINES,
	D3DPT_LINESTRIP             = GL_LINE_STRIP,
	D3DPT_TRIANGLELIST          = GL_TRIANGLES,
	D3DPT_TRIANGLESTRIP         = GL_TRIANGLE_STRIP,
	D3DPT_TRIANGLEFAN           = GL_TRIANGLE_FAN,
	D3DPT_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
};

typedef enum _D3DCULL {
    D3DCULL_NONE                = 1,
    D3DCULL_CW                  = GL_CW,
    D3DCULL_CCW                 = GL_CCW,
    D3DCULL_FORCE_DWORD         = 0x7fffffff, /* force 32-bit size enum */
} D3DCULL;

typedef enum _D3DCMPFUNC {
   D3DCMP_NEVER                = GL_NEVER,
   D3DCMP_LESS                 = GL_LESS,
   D3DCMP_EQUAL                = GL_EQUAL,
   D3DCMP_LESSEQUAL            = GL_LEQUAL,
   D3DCMP_GREATER              = GL_GREATER,
   D3DCMP_NOTEQUAL             = GL_NOTEQUAL,
   D3DCMP_GREATEREQUAL         = GL_GEQUAL,
   D3DCMP_ALWAYS               = GL_ALWAYS,
   D3DCMP_FORCE_DWORD          = 0x7fffffff
} D3DCMPFUNC;

typedef enum _D3DBLEND {
    D3DBLEND_ZERO               = GL_ZERO,
    D3DBLEND_ONE                = GL_ONE,
    D3DBLEND_SRCCOLOR           = GL_SRC_COLOR,
    D3DBLEND_INVSRCCOLOR        = GL_ONE_MINUS_SRC_COLOR,
    D3DBLEND_SRCALPHA           = GL_SRC_ALPHA,
    D3DBLEND_INVSRCALPHA        = GL_ONE_MINUS_SRC_ALPHA,
    D3DBLEND_DESTALPHA          = GL_DST_ALPHA,
    D3DBLEND_INVDESTALPHA       = GL_ONE_MINUS_DST_ALPHA,
    D3DBLEND_DESTCOLOR          = GL_DST_COLOR,
    D3DBLEND_INVDESTCOLOR       = GL_ONE_MINUS_DST_COLOR,
    D3DBLEND_SRCALPHASAT        = GL_SRC_ALPHA_SATURATE,
//	D3DBLEND_BOTHSRCALPHA       = 12,
//  D3DBLEND_BOTHINVSRCALPHA    = 13,
//  D3DBLEND_BLENDFACTOR        = 14, /* Only supported if D3DPBLENDCAPS_BLENDFACTOR is on */
//  D3DBLEND_INVBLENDFACTOR     = 15, /* Only supported if D3DPBLENDCAPS_BLENDFACTOR is on */
/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)
    D3DBLEND_SRCCOLOR2          = 16,
    D3DBLEND_INVSRCCOLOR2       = 17,
#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */
    D3DBLEND_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
} D3DBLEND;


#ifndef D3DVECTOR_DEFINED
typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif

#ifndef D3DCOLORVALUE_DEFINED
typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;
#define D3DCOLORVALUE_DEFINED
#endif

#ifndef D3DRECT_DEFINED
typedef struct _D3DRECT {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
} D3DRECT;
#define D3DRECT_DEFINED
#endif

#ifndef D3DMATRIX_DEFINED
typedef struct _D3DMATRIX {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
    };
} D3DMATRIX;
#define D3DMATRIX_DEFINED
#endif

typedef struct _D3DVIEWPORT9 {
    DWORD       X;
    DWORD       Y;            /* Viewport Top left */
    DWORD       Width;
    DWORD       Height;       /* Viewport Dimensions */
    float       MinZ;         /* Min/max of clip Volume */
    float       MaxZ;
} D3DVIEWPORT9;

#endif

typedef enum D3DPOOL
{
    D3DPOOL_DEFAULT = 0,
    D3DPOOL_MANAGED = 1,
    D3DPOOL_SYSTEMMEM = 2,
    D3DPOOL_SCRATCH = 3,
    D3DPOOL_FORCE_DWORD = 0x7fffffff,
} D3DPOOL, *LPD3DPOOL;
