/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  File	:	ipgsl.h
 *  Contents:	iPhone アプリ用GSLライブラリ
 *	Release	:	2012/10/012		Nozomi		version 1.00
 *	UpDate	:
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef PI
#define	PI 3.141592654f
#endif

/*
	Vertex Formats -----------------
*/
#define	SetXYZRGBA(p, vx, vy, vz, cr, cb, cg, ca)	p.x=vx; p.y=vy; p.z=vz; p.r=cr; p.g=cg; p.b=cb;  p.a=ca
#define	SetXYZ(a, vx, vy, vz)				a.x=vx; a.y=vy; a.z=vz
#define	SetUV(a, tu, tv)					a.u=tu; a.v=tv
#define	SetUV2(a, tu, tv)					a.u2=tu; a.v2=tv
#define	SetUV3(a, tu, tv)					a.u3=tu; a.v3=tv

typedef struct tagVERTEX
{
	float		x, y, z;
} VERTEX;

typedef struct tagLVERTEX
{
	float		x, y, z;
	float		r, g, b, a;
} LVERTEX;

typedef struct tagTLVERTEX
{
	float		x, y, z;
	float		r, g, b, a;
	float		u, v;
} TLVERTEX;

typedef struct tagTLVERTEX2
{
	float		x, y, z;
	float		r, g, b, a;
	float		u, v;
	float		u2, v2;
} TLVERTEX2;

typedef struct tagTLVERTEX3
{
	float		x, y, z;
	float		r, g, b, a;
	float		u, v;
	float		u2, v2;
	float		u3, v3;
} TLVERTEX3;


#define		SURFMAX			20

typedef struct tagSURF
{
	UINT32	texID;
	UINT32	sizeX;
	UINT32	sizeY;
	UINT32	tsizeX;
	UINT32	tsizeY;
	float	ftsizeX;
	float	ftsizeY;
} SURF;

//---------------------------------------------------------------------------------------------------------------------------------------------

extern	SURF	STable[SURFMAX];

extern	float	frontMaskY;	// 上下の16:9の画面外マスクサイズ
extern	float	viewofsYL;
extern	float	viewXL, viewYL;	// View Projection Landscape
extern	float	viewXP, viewYP;	// View Projection Portrait
extern	BOOL	SCrotate;
extern	BOOL	fLandscape;
extern	BOOL	fReverse;

extern	UINT32	SCsizeX, SCsizeY;


//---------------------------------------------------------------------------------------------------------------------------------------------





void GSLinit();

void GSLflatRect(int sx, int sy, int lx, int ly, int col, int alpha, int alphaop);
void GSLflatRectF(float fsx, float fsy, float flx, float fly, int col, int alpha, int alphaop);

void GSLsprRect(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha, int alphaop);
void GSLfontRect(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha);
void GSLfontStretch(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int ex, int ey, int col, int alpha);


void GSLresetTexture();



/*
	指定されたファイル名のpngをテクスチャーに読み込む
*/
BOOL GSLloadTexture(UINT32 sno, NSString *fname);




//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------


void GSLsetRotateMatrix(UINT32 mno, UINT32 mode, int ang);

//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------

void GetOrientation();






