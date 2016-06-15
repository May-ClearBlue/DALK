/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  File	:	ipgsl.m
 *  Contents:	iPhone ÉAÉvÉäópGSLÉâÉCÉuÉâÉä
 *	Release	:	2012/10/012		Nozomi		version 1.00
 *	UpDate	:
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include	"General.h"
#include	"shader.h"
#include	"matrix.h"
#include	"ipgsl.h"


#define	GSL_ROT_DIV			16	// ï™äÑÉTÉCÉY
#define	GSL_ROT_DIV_MIN		64	// ï™äÑç≈í·ÉTÉCÉY


SURF	STable[SURFMAX];


float	frontMaskY;	// è„â∫ÇÃ16:9ÇÃâÊñ äOÉ}ÉXÉNÉTÉCÉY
float	viewofsYL;
float	viewXL, viewYL;	// View Projection Landscape
float	viewXP, viewYP;	// View Projection Portrait
BOOL	SCrotate;
BOOL	fLandscape;
BOOL	fReverse;

UINT32	SCsizeX, SCsizeY;

/*
	ì‡ïîââéZópçsóÒ/ÉxÉNÉgÉã
*/
static MATRIX4		m0, m1, m2, m3;
static VECTOR4		v0, v1, v2, v3;


void GSLinit()
{
	UINT32	i;
	float	lenx, leny;

	SCsizeX = [UIScreen mainScreen].bounds.size.height;
	SCsizeY = [UIScreen mainScreen].bounds.size.width;


	lenx = 2.0f;														// Xç≈ëÂ=1280Ç≈àµÇ§
	leny = ((float)SCsizeX / 1280.0f * 720.0f) / (float)SCsizeY * 2.0f;	// ÉAÉXÉyÉNÉgî‰Ç16:9Ç…ê›íË
	viewofsYL = (2.0f - leny) / 2.0f;

	frontMaskY = ((float)SCsizeY / (float)SCsizeX * 1280.0f - 720.0f) / 2.0f;

	viewXL = lenx / 1280.0f;
	viewYL = leny / 720.0f;


	SCrotate = FALSE;
	fLandscape = TRUE;


	SetupShader();

	for(i=0 ; i<SURFMAX ; i++)
	{
		STable[i].texID = 0;
	}
}


void GSLflatRect(int sx, int sy, int lx, int ly, int col, int alpha, int alphaop)
{
	LVERTEX		lv[4];
	float		fsx, fsy, fex, fey;
	float		r, g, b, a;

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;

	fsx = (float)sx;
	fsy = (float)sy;
	fex = (float)(sx + lx);
	fey = (float)(sy + ly);

	SetXYZRGBA(lv[0], fsx, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[1], fex, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[2], fsx, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[3], fex, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetShader(5, viewofsYL, viewXL, viewYL);
	SetLVertex((float *)lv);
	DrawPrimitiveTRISTRIP(0, 4);
}

void GSLflatRectF(float fsx, float fsy, float flx, float fly, int col, int alpha, int alphaop)
{
	LVERTEX		lv[4];
	float		r, g, b, a;

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;

	SetXYZRGBA(lv[0], fsx    , fsy    , -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[1], fsx+flx, fsy    , -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[2], fsx    , fsy+fly, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[3], fsx+flx, fsy+fly, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetShader(5, viewofsYL, viewXL, viewYL);
	SetLVertex((float *)lv);
	DrawPrimitiveTRISTRIP(0, 4);
}


void GSLsprRect(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha, int shaderno)
{
	TLVERTEX		tlv[4];
	float		fsx, fsy, fex, fey;
	float		tsx, tsy, tex, tey;
	float		r, g, b, a;

	if(alpha > 255) alpha = 255;

	tsx = (float)sx        / STable[sno].ftsizeX;
	tsy = (float)sy        / STable[sno].ftsizeY;
	tex = (float)(sx + lx) / STable[sno].ftsizeX;
	tey = (float)(sy + ly) / STable[sno].ftsizeY;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;

	fsx = (float)dx;
	fsy = (float)dy;
	fex = (float)(dx + lx);
	fey = (float)(dy + ly);

	SetXYZRGBA(tlv[0], fsx, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[0], tsx, tsy);									// u,v
	SetXYZRGBA(tlv[1], fex, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[1], tex, tsy);									// u,v
	SetXYZRGBA(tlv[2], fsx, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[2], tsx, tey);									// u,v
	SetXYZRGBA(tlv[3], fex, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[3], tex, tey);									// u,v

	SetShader(shaderno, viewofsYL, viewXL, viewYL);
	SetTexture(shaderno, 0, STable[sno].texID);
	SetTLVertex((float *)tlv);
	DrawPrimitiveTRISTRIP(0, 4);
}


/*
	ägèkï`âÊ
*/
void GSLsprStretch(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int ex, int ey, int col, int alpha, int shaderno)
{
	TLVERTEX		tlv[4];
	float		fsx, fsy, fex, fey;
	float		tsx, tsy, tex, tey;
	float		r, g, b, a;

	if(alpha > 255) alpha = 255;

	tsx = (float)sx        / STable[sno].ftsizeX;
	tsy = (float)sy        / STable[sno].ftsizeY;
	tex = (float)(sx + lx) / STable[sno].ftsizeX;
	tey = (float)(sy + ly) / STable[sno].ftsizeY;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;

	fsx = (float)dx;
	fsy = (float)dy;
	fex = (float)ex;
	fey = (float)ey;

	SetXYZRGBA(tlv[0], fsx, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[0], tsx, tsy);									// u,v
	SetXYZRGBA(tlv[1], fex, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[1], tex, tsy);									// u,v
	SetXYZRGBA(tlv[2], fsx, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[2], tsx, tey);									// u,v
	SetXYZRGBA(tlv[3], fex, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[3], tex, tey);									// u,v

	SetShader(shaderno, viewofsYL, viewXL, viewYL);
	SetTexture(shaderno, 0, STable[sno].texID);
	SetTLVertex((float *)tlv);
	DrawPrimitiveTRISTRIP(0, 4);
}





/*
	D3DÉ|ÉäÉSÉìï™äÑÉoÅ[ÉWÉáÉì	lx,lyÇÕ16ÇÃî{êîÇÃÇ›
*/
void GSLsprRectRotate(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha, int shaderno, int rot, int range)
{
	TLVERTEX		v[260];
	float			scale;
	float			cx, cy;
	float			tsx, tsy, tex, tey, tix, tiy, tdx, tdy;
	VECTOR4			v4[130][66];
	int				ix, iy;
	int				divx, divy;
	float			r, g, b, a;

	if(lx < GSL_ROT_DIV_MIN)
	{
		divx = 1;
	}
	else
	{
		divx = lx / GSL_ROT_DIV;
	}
	if(ly < GSL_ROT_DIV_MIN)
	{
		divy = 1;
	}
	else
	{
		divy = ly / GSL_ROT_DIV;
	}

	scale = (float)range / 256.0f;

	MatrixSetRotateZ(&m1, (float)rot * PI / 32768.0f);	// Zé≤âÒì]  rot x 2ÉŒ / 65536
	MatrixStretch(&m2, scale, scale, 1.0f);
	MatrixCalcMxM(&m3, &m2, &m1);					// m2 = m0 x m1

	cx = (float)(dx + lx / 2);	// ï`âÊêÊÇÃíÜêSç¿ïWÇãLâØ
	cy = (float)(dy + ly / 2);

// íÜêSÇ(0,0)Ç∆ÇµÇƒâÒì]ç¿ïWÇââéZ
	v0.x = (float)(0 - lx / 2);	//	sx
	v0.y = (float)(0 - ly / 2);	//	sy
	v1.x = (float)(lx) / (float)divx;	//	dx
	v1.y = (float)(ly) / (float)divy;	//	dy

// í∏ì_ÉfÅ[É^çÏê¨ÅïââéZ
	v2.y = v0.y;
	for(iy=0 ; iy<divy+1 ; iy++)
	{
		v2.x = v0.x;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			MatrixCalcVxM2(&v4[ix][iy], &v2, &m3);
			v2.x += v1.x;
		}
		v2.y += v1.y;
	}


// à»â∫ï`âÊèàóù

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;


	SetShader(shaderno, viewofsYL, viewXL, viewYL);
	SetTexture(shaderno, 0, STable[sno].texID);

	tsx = (float)sx / STable[sno].ftsizeX;
	tsy = (float)sy / STable[sno].ftsizeY;
	tex = (float)(sx+lx) / STable[sno].ftsizeX;
	tey = (float)(sy+ly) / STable[sno].ftsizeY;
	tdx = (tex - tsx) / (float)divx;
	tdy = (tey - tsy) / (float)divy;

	tiy = tsy;
	for(iy=0 ; iy<divy ; iy++)
	{
		tix = tsx;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			SetXYZRGBA(v[ix*2  ], cx + v4[ix][iy].x, cy + v4[ix][iy].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetXYZRGBA(v[ix*2+1], cx + v4[ix][iy+1].x, cy + v4[ix][iy+1].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(v[ix*2  ], tix, tiy);
			SetUV(v[ix*2+1], tix, tiy+tdy);
			tix += tdx;
		}
		SetTLVertex((float *)v);
		DrawPrimitiveTRISTRIP(0, divx * 2 + 2);

		tiy += tdy;
	}
}


/*
	D3DÉ|ÉäÉSÉìï™äÑÉoÅ[ÉWÉáÉì	lx,lyÇÕ16ÇÃî{êîÇÃÇ›
*/
void GSLsprRectRotateX(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha, int shaderno, int rot, int range)
{
	TLVERTEX		v[260];
	float			scale;
	float			cx, cy;
	float			tsx, tsy, tex, tey, tix, tiy, tdx, tdy;
	VECTOR4			v4[130][66];
	int				ix, iy;
	int				divx, divy;
	float			r, g, b, a;

	if(lx < GSL_ROT_DIV_MIN)
	{
		divx = 1;
	}
	else
	{
		divx = lx / GSL_ROT_DIV;
	}
	if(ly < GSL_ROT_DIV_MIN)
	{
		divy = 1;
	}
	else
	{
		divy = ly / GSL_ROT_DIV;
	}

	scale = (float)range / 256.0f;

	MatrixSetRotateX(&m1, (float)rot * PI / 32768.0f);	// Zé≤âÒì]  rot x 2ÉŒ / 65536
	MatrixStretch(&m2, scale, scale, 1.0f);
	MatrixCalcMxM(&m3, &m2, &m1);					// m2 = m0 x m1

	cx = (float)(dx + lx / 2);	// ï`âÊêÊÇÃíÜêSç¿ïWÇãLâØ
	cy = (float)(dy + ly / 2);

// íÜêSÇ(0,0)Ç∆ÇµÇƒâÒì]ç¿ïWÇââéZ
	v0.x = (float)(0 - lx / 2);	//	sx
	v0.y = (float)(0 - ly / 2);	//	sy
	v1.x = (float)(lx) / (float)divx;	//	dx
	v1.y = (float)(ly) / (float)divy;	//	dy

// í∏ì_ÉfÅ[É^çÏê¨ÅïââéZ
	v2.y = v0.y;
	for(iy=0 ; iy<divy+1 ; iy++)
	{
		v2.x = v0.x;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			MatrixCalcVxM2(&v4[ix][iy], &v2, &m3);
			v2.x += v1.x;
		}
		v2.y += v1.y;
	}


// à»â∫ï`âÊèàóù

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;


	SetShader(shaderno, viewofsYL, viewXL, viewYL);
	SetTexture(shaderno, 0, STable[sno].texID);

	tsx = (float)sx / STable[sno].ftsizeX;
	tsy = (float)sy / STable[sno].ftsizeY;
	tex = (float)(sx+lx) / STable[sno].ftsizeX;
	tey = (float)(sy+ly) / STable[sno].ftsizeY;
	tdx = (tex - tsx) / (float)divx;
	tdy = (tey - tsy) / (float)divy;

	tiy = tsy;
	for(iy=0 ; iy<divy ; iy++)
	{
		tix = tsx;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			SetXYZRGBA(v[ix*2  ], cx + v4[ix][iy].x, cy + v4[ix][iy].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetXYZRGBA(v[ix*2+1], cx + v4[ix][iy+1].x, cy + v4[ix][iy+1].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(v[ix*2  ], tix, tiy);
			SetUV(v[ix*2+1], tix, tiy+tdy);
			tix += tdx;
		}
		SetTLVertex((float *)v);
		DrawPrimitiveTRISTRIP(0, divx * 2 + 2);

		tiy += tdy;
	}
}


/*
	D3DÉ|ÉäÉSÉìï™äÑÉoÅ[ÉWÉáÉì	lx,lyÇÕ16ÇÃî{êîÇÃÇ›
*/
void GSLsprRectRotateY(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha, int shaderno, int rot, int range)
{
	TLVERTEX		v[260];
	float			scale;
	float			cx, cy;
	float			tsx, tsy, tex, tey, tix, tiy, tdx, tdy;
	VECTOR4			v4[130][66];
	int				ix, iy;
	int				divx, divy;
	float			r, g, b, a;

	if(lx < GSL_ROT_DIV_MIN)
	{
		divx = 1;
	}
	else
	{
		divx = lx / GSL_ROT_DIV;
	}
	if(ly < GSL_ROT_DIV_MIN)
	{
		divy = 1;
	}
	else
	{
		divy = ly / GSL_ROT_DIV;
	}

	scale = (float)range / 256.0f;

	MatrixSetRotateY(&m1, (float)rot * PI / 32768.0f);	// Zé≤âÒì]  rot x 2ÉŒ / 65536
	MatrixStretch(&m2, scale, scale, 1.0f);
	MatrixCalcMxM(&m3, &m2, &m1);					// m2 = m0 x m1

	cx = (float)(dx + lx / 2);	// ï`âÊêÊÇÃíÜêSç¿ïWÇãLâØ
	cy = (float)(dy + ly / 2);

// íÜêSÇ(0,0)Ç∆ÇµÇƒâÒì]ç¿ïWÇââéZ
	v0.x = (float)(0 - lx / 2);	//	sx
	v0.y = (float)(0 - ly / 2);	//	sy
	v1.x = (float)(lx) / (float)divx;	//	dx
	v1.y = (float)(ly) / (float)divy;	//	dy

// í∏ì_ÉfÅ[É^çÏê¨ÅïââéZ
	v2.y = v0.y;
	for(iy=0 ; iy<divy+1 ; iy++)
	{
		v2.x = v0.x;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			MatrixCalcVxM2(&v4[ix][iy], &v2, &m3);
			v2.x += v1.x;
		}
		v2.y += v1.y;
	}


// à»â∫ï`âÊèàóù

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;


	SetShader(shaderno, viewofsYL, viewXL, viewYL);
	SetTexture(shaderno, 0, STable[sno].texID);

	tsx = (float)sx / STable[sno].ftsizeX;
	tsy = (float)sy / STable[sno].ftsizeY;
	tex = (float)(sx+lx) / STable[sno].ftsizeX;
	tey = (float)(sy+ly) / STable[sno].ftsizeY;
	tdx = (tex - tsx) / (float)divx;
	tdy = (tey - tsy) / (float)divy;

	tiy = tsy;
	for(iy=0 ; iy<divy ; iy++)
	{
		tix = tsx;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			SetXYZRGBA(v[ix*2  ], cx + v4[ix][iy].x, cy + v4[ix][iy].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetXYZRGBA(v[ix*2+1], cx + v4[ix][iy+1].x, cy + v4[ix][iy+1].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(v[ix*2  ], tix, tiy);
			SetUV(v[ix*2+1], tix, tiy+tdy);
			tix += tdx;
		}
		SetTLVertex((float *)v);
		DrawPrimitiveTRISTRIP(0, divx * 2 + 2);

		tiy += tdy;
	}
}





/*
	D3DÉ|ÉäÉSÉìï™äÑÉoÅ[ÉWÉáÉì	lx,lyÇÕ16ÇÃî{êîÇÃÇ›
*/
void GSLsprRectRotate2(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha, int shaderno, int rot, int rangeX, int rangeY)
{
	TLVERTEX		v[260];
	float			scaleX, scaleY;
	float			cx, cy;
	float			tsx, tsy, tex, tey, tix, tiy, tdx, tdy;
	VECTOR4			v4[130][66];
	int				ix, iy;
	int				divx, divy;
	float			r, g, b, a;

	if(lx < GSL_ROT_DIV_MIN)
	{
		divx = 1;
	}
	else
	{
		divx = lx / GSL_ROT_DIV;
	}
	if(ly < GSL_ROT_DIV_MIN)
	{
		divy = 1;
	}
	else
	{
		divy = ly / GSL_ROT_DIV;
	}

	scaleX = (float)rangeX / 256.0f;
	scaleY = (float)rangeY / 256.0f;

	MatrixSetRotateZ(&m1, (float)rot * PI / 32768.0f);	// Zé≤âÒì]  rot x 2ÉŒ / 65536
	MatrixStretch(&m2, scaleX, scaleY, 1.0f);
	MatrixCalcMxM(&m3, &m2, &m1);					// m2 = m0 x m1

	cx = (float)(dx + lx / 2);	// ï`âÊêÊÇÃíÜêSç¿ïWÇãLâØ
	cy = (float)(dy + ly / 2);

// íÜêSÇ(0,0)Ç∆ÇµÇƒâÒì]ç¿ïWÇââéZ
	v0.x = (float)(0 - lx / 2);	//	sx
	v0.y = (float)(0 - ly / 2);	//	sy
	v1.x = (float)(lx) / (float)divx;	//	dx
	v1.y = (float)(ly) / (float)divy;	//	dy

// í∏ì_ÉfÅ[É^çÏê¨ÅïââéZ
	v2.y = v0.y;
	for(iy=0 ; iy<divy+1 ; iy++)
	{
		v2.x = v0.x;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			MatrixCalcVxM2(&v4[ix][iy], &v2, &m3);
			v2.x += v1.x;
		}
		v2.y += v1.y;
	}


// à»â∫ï`âÊèàóù

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;


	SetShader(shaderno, viewofsYL, viewXL, viewYL);
	SetTexture(shaderno, 0, STable[sno].texID);

	tsx = (float)sx / STable[sno].ftsizeX;
	tsy = (float)sy / STable[sno].ftsizeY;
	tex = (float)(sx+lx) / STable[sno].ftsizeX;
	tey = (float)(sy+ly) / STable[sno].ftsizeY;
	tdx = (tex - tsx) / (float)divx;
	tdy = (tey - tsy) / (float)divy;

	tiy = tsy;
	for(iy=0 ; iy<divy ; iy++)
	{
		tix = tsx;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			SetXYZRGBA(v[ix*2  ], cx + v4[ix][iy].x, cy + v4[ix][iy].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetXYZRGBA(v[ix*2+1], cx + v4[ix][iy+1].x, cy + v4[ix][iy+1].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(v[ix*2  ], tix, tiy);
			SetUV(v[ix*2+1], tix, tiy+tdy);
			tix += tdx;
		}
		SetTLVertex((float *)v);
		DrawPrimitiveTRISTRIP(0, divx * 2 + 2);

		tiy += tdy;
	}
}


/*
	3é≤âÒì]
*/
void GSLsprRectRotate3(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha, int shaderno, int rotx, int roty, int rotz, int range)
{
	TLVERTEX		v[260];
	float			scale;
	float			cx, cy;
	float			tsx, tsy, tex, tey, tix, tiy, tdx, tdy;
	VECTOR4			v4[130][66];
	int				ix, iy;
	int				divx, divy;
	float			r, g, b, a;

	if(lx < GSL_ROT_DIV_MIN)
	{
		divx = 1;
	}
	else
	{
		divx = lx / GSL_ROT_DIV;
	}
	if(ly < GSL_ROT_DIV_MIN)
	{
		divy = 1;
	}
	else
	{
		divy = ly / GSL_ROT_DIV;
	}

	scale = (float)range / 256.0f;

	MatrixRotationYawPitchRoll(&m1, (float)rotx * PI / 32768.0f, (float)roty * PI / 32768.0f, (float)rotz * PI / 32768.0f);	// Zé≤âÒì]  rot x 2ÉŒ / 65536
	MatrixStretch(&m2, scale, scale, 1.0f);
	MatrixCalcMxM(&m3, &m2, &m1);					// m2 = m0 x m1

	cx = (float)(dx + lx / 2);	// ï`âÊêÊÇÃíÜêSç¿ïWÇãLâØ
	cy = (float)(dy + ly / 2);

// íÜêSÇ(0,0)Ç∆ÇµÇƒâÒì]ç¿ïWÇââéZ
	v0.x = (float)(0 - lx / 2);	//	sx
	v0.y = (float)(0 - ly / 2);	//	sy
	v1.x = (float)(lx) / (float)divx;	//	dx
	v1.y = (float)(ly) / (float)divy;	//	dy

// í∏ì_ÉfÅ[É^çÏê¨ÅïââéZ
	v2.y = v0.y;
	for(iy=0 ; iy<divy+1 ; iy++)
	{
		v2.x = v0.x;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			MatrixCalcVxM2(&v4[ix][iy], &v2, &m3);
			v2.x += v1.x;
		}
		v2.y += v1.y;
	}


// à»â∫ï`âÊèàóù

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;


	SetShader(shaderno, viewofsYL, viewXL, viewYL);
	SetTexture(shaderno, 0, STable[sno].texID);

	tsx = (float)sx / STable[sno].ftsizeX;
	tsy = (float)sy / STable[sno].ftsizeY;
	tex = (float)(sx+lx) / STable[sno].ftsizeX;
	tey = (float)(sy+ly) / STable[sno].ftsizeY;
	tdx = (tex - tsx) / (float)divx;
	tdy = (tey - tsy) / (float)divy;

	tiy = tsy;
	for(iy=0 ; iy<divy ; iy++)
	{
		tix = tsx;
		for(ix=0 ; ix<divx+1 ; ix++)
		{
			SetXYZRGBA(v[ix*2  ], cx + v4[ix][iy].x, cy + v4[ix][iy].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetXYZRGBA(v[ix*2+1], cx + v4[ix][iy+1].x, cy + v4[ix][iy+1].y, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(v[ix*2  ], tix, tiy);
			SetUV(v[ix*2+1], tix, tiy+tdy);
			tix += tdx;
		}
		SetTLVertex((float *)v);
		DrawPrimitiveTRISTRIP(0, divx * 2 + 2);

		tiy += tdy;
	}
}


#if 0

/*
	íPèÉÉ}ÉXÉNï`âÊ
*/
void GSLsprMaskRect(int sno, int msno, int sx, int sy, int lx, int ly, int dx, int dy, int filter, int alpha, int mode, int ct)
{
	TLVERTEX		v[4];
	UINT32			col;
	float			tsx, tsy, tex, tey;
	float			tmsx, tmsy, tmex, tmey;
	UINT32			stno, mstno;
	float			calpha;
	UINT32			psno;

	if(alpha > 255) alpha = 255;
	col = filter | (alpha << 24);

	psno = mode + GSL_PS_MASKFADE;

	if(ct < 0) ct = 0;
	if(ct > 271) ct = 271;
// base alpha = (255 - base) * 16 / 255		base (0-271)
	calpha = (float)(255 - ct) * 16.0f / 255.0f;

	SetTLVertexXYZRHWC(v[0], (float)dx-0.5f,      (float)dy-0.5f,      0.0f, 1.0f, col);
	SetTLVertexXYZRHWC(v[1], (float)dx-0.5f,      (float)(dy+ly)-0.5f, 0.0f, 1.0f, col);
	SetTLVertexXYZRHWC(v[2], (float)(dx+lx)-0.5f, (float)dy-0.5f,      0.0f, 1.0f, col);
	SetTLVertexXYZRHWC(v[3], (float)(dx+lx)-0.5f, (float)(dy+ly)-0.5f, 0.0f, 1.0f, col);

	tsx = (float)sx / STable[sno].ftlx;
	tsy = (float)sy / STable[sno].ftly;
	tex = (float)(sx+lx) / STable[sno].ftlx;
	tey = (float)(sy+ly) / STable[sno].ftly;
//	tmsx = (float)msx / STable[msno].ftlx;
//	tmsy = (float)msy / STable[msno].ftly;
//	tmex = (float)(msx+mlx) / STable[msno].ftlx;
//	tmey = (float)(msy+mly) / STable[msno].ftly;
	tmsx = (float)dx / STable[msno].ftlx;
	tmsy = (float)dy / STable[msno].ftly;
	tmex = (float)(dx+lx) / STable[msno].ftlx;
	tmey = (float)(dy+ly) / STable[msno].ftly;

	SetTLVertexUV(v[0], tsx, tsy);
	SetTLVertexUV(v[1], tsx, tey);
	SetTLVertexUV(v[2], tex, tsy);
	SetTLVertexUV(v[3], tex, tey);
	SetTLVertexUV2(v[0], tmsx, tmsy);
	SetTLVertexUV2(v[1], tmsx, tmey);
	SetTLVertexUV2(v[2], tmex, tmsy);
	SetTLVertexUV2(v[3], tmex, tmey);

	stno = GSLgetTextureSampler(psno, 0);
	mstno = GSLgetTextureSampler(psno, 1);

	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, stno, STable[sno].lpD3DT9);
	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, mstno, STable[msno].lpD3DT9);
	IDirect3DDevice9_SetFVF(lpD3D9Dev, FVF_TLVERTEX);

// Shader Çê›íË
	GSLsetShader(lpD3D9Dev, psno);

	GSLsetShaderParam(lpD3D9Dev, psno, 0, calpha);	// const alphaê›íË

// ï`âÊ
	IDirect3DDevice9_DrawPrimitiveUP(lpD3D9Dev, D3DPT_TRIANGLESTRIP, 2, v, sizeof(TLVERTEX));
//	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, 1, NULL);
}

/*
	ägèkÉ}ÉXÉNï`âÊ
*/
void GSLsprMaskStretch(int sno, int msno, int sx, int sy, int lx, int ly, int dx, int dy, int ex, int ey, int filter, int alpha, int mode, int ct)
{
	TLVERTEX		v[4];
	UINT32			col;
	float			tsx, tsy, tex, tey;
	float			tmsx, tmsy, tmex, tmey;
	UINT32			stno, mstno;
	float			calpha;
	UINT32			psno;

	if(alpha > 255) alpha = 255;
	col = filter | (alpha << 24);

	psno = mode + GSL_PS_MASKFADE;

	if(ct < 0) ct = 0;
	if(ct > 271) ct = 271;
// base alpha = (255 - base) * 16 / 255		base (0-271)
	calpha = (float)(255 - ct) * 16.0f / 255.0f;

	SetTLVertexXYZRHWC(v[0], (float)dx-0.5f, (float)dy-0.5f, 0.0f, 1.0f, col);
	SetTLVertexXYZRHWC(v[1], (float)dx-0.5f, (float)ey-0.5f, 0.0f, 1.0f, col);
	SetTLVertexXYZRHWC(v[2], (float)ex-0.5f, (float)dy-0.5f, 0.0f, 1.0f, col);
	SetTLVertexXYZRHWC(v[3], (float)ex-0.5f, (float)ey-0.5f, 0.0f, 1.0f, col);

	tsx = (float)sx / STable[sno].ftlx;
	tsy = (float)sy / STable[sno].ftly;
	tex = (float)(sx+lx) / STable[sno].ftlx;
	tey = (float)(sy+ly) / STable[sno].ftly;
//	tmsx = (float)msx / STable[msno].ftlx;
//	tmsy = (float)msy / STable[msno].ftly;
//	tmex = (float)(msx+mlx) / STable[msno].ftlx;
//	tmey = (float)(msy+mly) / STable[msno].ftly;
	tmsx = (float)dx / STable[msno].ftlx;
	tmsy = (float)dy / STable[msno].ftly;
	tmex = (float)ex / STable[msno].ftlx;
	tmey = (float)ey / STable[msno].ftly;

	SetTLVertexUV(v[0], tsx, tsy);
	SetTLVertexUV(v[1], tsx, tey);
	SetTLVertexUV(v[2], tex, tsy);
	SetTLVertexUV(v[3], tex, tey);
	SetTLVertexUV2(v[0], tmsx, tmsy);
	SetTLVertexUV2(v[1], tmsx, tmey);
	SetTLVertexUV2(v[2], tmex, tmsy);
	SetTLVertexUV2(v[3], tmex, tmey);

	stno = GSLgetTextureSampler(psno, 0);
	mstno = GSLgetTextureSampler(psno, 1);

	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, stno, STable[sno].lpD3DT9);
	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, mstno, STable[msno].lpD3DT9);
	IDirect3DDevice9_SetFVF(lpD3D9Dev, FVF_TLVERTEX);

// Shader Çê›íË
	GSLsetShader(lpD3D9Dev, psno);

	GSLsetShaderParam(lpD3D9Dev, psno, 0, calpha);	// const alphaê›íË

// ï`âÊ
	IDirect3DDevice9_DrawPrimitiveUP(lpD3D9Dev, D3DPT_TRIANGLESTRIP, 2, v, sizeof(TLVERTEX));
//	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, 1, NULL);

}


void GSLsetMaskTexture(int sno, int msno, int mode, int ct)
{
	UINT32			psno;
	UINT32			stno, mstno;
	float			calpha;

	psno = mode + GSL_PS_MASKFADE;

	if(ct < 0) ct = 0;
	if(ct > 271) ct = 271;
// base alpha = (255 - base) * 16 / 255		base (0-271)
	calpha = (float)(255 - ct) * 16.0f / 255.0f;

	stno = GSLgetTextureSampler(psno, 0);
	mstno = GSLgetTextureSampler(psno, 1);

	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, stno, STable[sno].lpD3DT9);
	hr = IDirect3DDevice9_SetTexture(lpD3D9Dev, mstno, STable[msno].lpD3DT9);
	IDirect3DDevice9_SetFVF(lpD3D9Dev, FVF_TLVERTEX);

// Shader Çê›íË
	GSLsetShader(lpD3D9Dev, psno);

	GSLsetShaderParam(lpD3D9Dev, psno, 0, calpha);	// const alphaê›íË
}


void GSLdrawMaskprimTRISTRIP(int msno, TLVERTEX *tlv, UINT32 pnum)
{
	UINT32	i;

	for(i=0 ; i<pnum+2 ; i++)
	{
		tlv[i].u2 = tlv[i].x / STable[msno].ftlx;
		tlv[i].v2 = tlv[i].y / STable[msno].ftly;
	}

// ï`âÊ
	IDirect3DDevice9_DrawPrimitiveUP(lpD3D9Dev, D3DPT_TRIANGLESTRIP, pnum, tlv, sizeof(TLVERTEX));
}

#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------



void GSLfontRect(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int col, int alpha)
{
	TLVERTEX		tlv[4];
	float		fsx, fsy, flx, fly;
	float		tsx, tsy, tex, tey;
	float		r, g, b, a;

	tsx = (float)sx / STable[sno].ftsizeX;
	tsy = (float)sy / STable[sno].ftsizeY;
	tex = (float)(sx+lx) / STable[sno].ftsizeX;
	tey = (float)(sy+ly) / STable[sno].ftsizeY;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;

	fsx = (float)dx;
	fsy = (float)dy;
	flx = (float)lx;
	fly = (float)ly;

	SetXYZRGBA(tlv[0], fsx    , fsy    , -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[0], tsx, tsy);									// u,v
	SetXYZRGBA(tlv[1], fsx+flx, fsy    , -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[1], tex, tsy);									// u,v
	SetXYZRGBA(tlv[2], fsx    , fsy+fly, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[2], tsx, tey);									// u,v
	SetXYZRGBA(tlv[3], fsx+flx, fsy+fly, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[3], tex, tey);									// u,v

	SetShader(4, viewofsYL, viewXL, viewYL);
	SetTexture(4, 0, STable[sno].texID);
	SetTLVertex((float *)tlv);
	DrawPrimitiveTRISTRIP(0, 4);
}

void GSLfontStretch(int sno, int sx, int sy, int lx, int ly, int dx, int dy, int ex, int ey, int col, int alpha)
{
	TLVERTEX		tlv[4];
	float		fsx, fsy, fex, fey;
	float		tsx, tsy, tex, tey;
	float		r, g, b, a;

	tsx = (float)sx / STable[sno].ftsizeX;
	tsy = (float)sy / STable[sno].ftsizeY;
	tex = (float)(sx+lx) / STable[sno].ftsizeX;
	tey = (float)(sy+ly) / STable[sno].ftsizeY;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;

	fsx = (float)dx;
	fsy = (float)dy;
	fex = (float)ex;
	fey = (float)ey;

	SetXYZRGBA(tlv[0], fsx, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[0], tsx, tsy);							// u,v
	SetXYZRGBA(tlv[1], fex, fsy, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[1], tex, tsy);							// u,v
	SetXYZRGBA(tlv[2], fsx, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[2], tsx, tey);							// u,v
	SetXYZRGBA(tlv[3], fex, fey, -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
	SetUV(tlv[3], tex, tey);							// u,v
	SetShader(4, viewofsYL, viewXL, viewYL);
	SetTexture(4, 0, STable[sno].texID);
	SetTLVertex((float *)tlv);
	DrawPrimitiveTRISTRIP(0, 4);
}


void GSLresetTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}



/*
	éwíËÇ≥ÇÍÇΩÉtÉ@ÉCÉãñºÇÃpngÇÉeÉNÉXÉ`ÉÉÅ[Ç…ì«Ç›çûÇﬁ
*/
BOOL GSLloadTexture(UINT32 sno, NSString *fname)
{
	GLuint texid;
	size_t	tsizeX, tsizeY;
	size_t	sx, sy;

	if(STable[sno].texID)
	{
		glDeleteTextures(1, STable[sno].texID);
		STable[sno].texID = 0;
	}

	CGImageRef img = [UIImage imageNamed:fname].CGImage;
	if(!img)
	{
		NSLog(fname);
		NSLog(@"loadTexture:load error");
		return 0;
	}

	size_t sizeX = CGImageGetWidth(img);
	size_t sizeY = CGImageGetHeight(img);

	for(tsizeX=256 ; tsizeX<sizeX ; tsizeX*=2) ;
	for(tsizeY=256 ; tsizeY<sizeY ; tsizeY*=2) ;

	sx = tsizeX - sizeX;
	sy = tsizeY - sizeY;

	CGColorSpaceRef colspc = CGColorSpaceCreateDeviceRGB();

	GLubyte *data = (GLubyte *)malloc(tsizeX * tsizeY * 4);
	CGContextRef imgContext = CGBitmapContextCreate(data, tsizeX, tsizeY, 8, tsizeX * 4, colspc, kCGImageAlphaPremultipliedLast);
	CGContextDrawImage(imgContext, CGRectMake(sx, sy, (CGFloat)sizeX, (CGFloat)sizeY), img);
	CGContextRelease(imgContext);

	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tsizeX, tsizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	free(data);

	if(texid == 0)
	{
		NSLog(@"ì«Ç›çûÇ›ÉGÉâÅ[");
		return FALSE;
	}
	else
	{
		STable[sno].texID = texid;
		STable[sno].sizeX = sizeX;
		STable[sno].sizeY = sizeY;
		STable[sno].tsizeX = tsizeX;
		STable[sno].tsizeY = tsizeY;
		STable[sno].ftsizeX = (float)tsizeX;
		STable[sno].ftsizeY = (float)tsizeY;

		return TRUE;
	}
}





//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------
static MATRIX4		m0, m1, m2, m3;
static VECTOR4		v0, v1, v2, v3, v4;
MATRIX4		mat[4];


void GSLsetRotateMatrix(UINT32 mno, UINT32 mode, int ang)
{
	switch(mode)
	{
	case 0:
		MatrixSetRotateX(&mat[mno], (float)ang * PI / 32768.0f);	// Xé≤âÒì]  rot x 2ÉŒ / 65536
		break;
	case 1:
		MatrixSetRotateY(&mat[mno], (float)ang * PI / 32768.0f);	// Yé≤âÒì]  rot x 2ÉŒ / 65536
		break;
	case 2:
		MatrixSetRotateZ(&mat[mno], (float)ang * PI / 32768.0f);	// Zé≤âÒì]  rot x 2ÉŒ / 65536
		break;
	}
}

/*
	angh â°ï˚å¸âÒì]äp
	angv ècï˚å¸âÒì]äp
	angc ÉJÉÅÉââÒì]äp
*/
void GSLsetViewMatrixAng(UINT32 mno, int len, int angh, int angv, int angc)
{
	float	 cx, cy, cz;
	float	 ux, uy, uz;


	cx = 0.0f;
	cy = 0.0f;
	cz = (float)len;

	MatrixSetRotateX(&m0, (float)angc * PI / 32768.0f);	// Xé≤âÒì]  rot x 2ÉŒ / 65536
	v3.x = 0.0f;
	v3.y = 100.0f;
	v3.z = 0.0f;
	MatrixCalcVxM3(&v4, &v3, &m0);
	ux = v4.x;
	uy = v4.y;
	uz = v4.z;


	MatrixSetRotateX(&m0, (float)angv * PI / 32768.0f);	// Xé≤âÒì]  rot x 2ÉŒ / 65536
	v3.x = cx;
	v3.y = cy;
	v3.z = cz;
	MatrixCalcVxM3(&v4, &v3, &m0);
	cx = v4.x;
	cy = v4.y;
	cz = v4.z;

	ux += cx;
	uy += cy;
	uz += cz;
	v3.x = ux;
	v3.y = uy;
	v3.z = uz;
	MatrixCalcVxM3(&v4, &v3, &m0);
	ux = v4.x;
	uy = v4.y;
	uz = v4.z;



	MatrixSetRotateY(&m0, (float)angh * PI / 32768.0f);	// Yé≤âÒì]  rot x 2ÉŒ / 65536
	v3.x = cx;
	v3.y = cy;
	v3.z = cz;
	MatrixCalcVxM2(&v4, &v3, &m0);
	cx = v4.x;
	cy = v4.y;
	cz = v4.z;

	v3.x = ux;
	v3.y = uy;
	v3.z = uz;
	MatrixCalcVxM2(&v4, &v3, &m0);
	ux = v4.x;
	uy = v4.y;
	uz = v4.z;

	v3.x = (float)cx;
	v3.y = (float)cy;
	v3.z = (float)cz;
	v3.x = (float)0.0f;
	v3.y = (float)0.0f;
	v3.z = (float)0.0f;
	v3.x = (float)ux;
	v3.y = (float)uy;
	v3.z = (float)uz;

//	D3DXMatrixLookAtLH(&mat[mno], &v30, &v31, &v32);
}


void GSLsetViewMatrix(UINT32 mno, int cx, int cy, int cz, int ax, int ay, int az, int ux, int uy, int uz)
{
//	v30.x = (float)cx;
//	v30.y = (float)cy;
//	v30.z = (float)cz;
//	v31.x = (float)ax;
//	v31.y = (float)ay;
//	v31.z = (float)az;
//	v32.x = (float)ux;
//	v32.y = (float)uy;
//	v32.z = (float)uz;
//
//	D3DXMatrixLookAtLH(&mat[mno], &v30, &v31, &v32);
}


void GSLsetTransMatrixF(UINT32 mno, float x, float y, float z)
{
//	D3DXMatrixTranslation(&mat[mno], x, y, z);
	mat[mno]._11 = 1.0f;
	mat[mno]._12 = 0.0f;
	mat[mno]._13 = 0.0f;
	mat[mno]._14 = 0.0f;
	mat[mno]._21 = 0.0f;
	mat[mno]._22 = 1.0f;
	mat[mno]._23 = 0.0f;
	mat[mno]._24 = 0.0f;
	mat[mno]._31 = 0.0f;
	mat[mno]._32 = 0.0f;
	mat[mno]._33 = 1.0f;
	mat[mno]._34 = 0.0f;
	mat[mno]._41 = x;
	mat[mno]._42 = y;
	mat[mno]._43 = z;
	mat[mno]._44 = 1.0f;
}

void GSLsetTransMatrix(UINT32 mno, int x, int y, int z)
{
	GSLsetTransMatrixF(mno, (float)x, (float)y, (float)z);
}

void GSLsetScaleMatrixF(UINT32 mno, float x, float y, float z)
{
	MatrixStretch(&mat[mno], x, y, z);
}

void GSLsetRotateAngle(UINT32 mno, int angx, int angy, int angz)
{
	MatrixRotationYawPitchRoll(&m1, (float)angy * PI / 32768.0f, (float)angx * PI / 32768.0f, (float)angz * PI / 32768.0f);
}

void GSLcalcMatrixMultiply(UINT32 mno0, UINT32 mno1, UINT32 mno2)
{
	MatrixCalcMxM(&mat[mno0], &mat[mno1], &mat[mno2]);
}


void GSLcalcRotateMatrix(UINT32 mno, int *x, int *y)
{
	v3.x = (float)*x;
	v3.y = (float)*y;
	MatrixCalcVxM2(&v4, &v3, &mat[mno]);
	*x = (int)v4.x;
	*y = (int)v4.y;
}

void GSLcalcRotateMatrixF(UINT32 mno, float *x, float *y)
{
	v3.x = *x;
	v3.y = *y;
	MatrixCalcVxM2(&v4, &v3, &mat[mno]);
	*x = v4.x;
	*y = v4.y;
}

void GSLcalcRotateMatrixF3(UINT32 mno, float *x, float *y, float *z)
{
	v3.x = *x;
	v3.y = *y;
	v3.z = *z;
	MatrixCalcVxM3(&v4, &v3, &mat[mno]);
	*x = v4.x;
	*y = v4.y;
	*z = v4.z;
}



//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------

void GetOrientation()
{
	UINT32	dir;

	dir = [UIDevice currentDevice].orientation;

	switch(dir)
	{
	case UIDeviceOrientationUnknown:				//Å@"ïsñæ"
	case UIDeviceOrientationFaceDown:				//Å@"âÊñ Ç™â∫"
		// ï˚å¸ÇåàÇﬂÇ»Ç¢
		break;
	case UIDeviceOrientationLandscapeLeft:			//Å@"â°è„ïîç∂"
		fReverse = TRUE;
		SCrotate = FALSE;
		fLandscape = TRUE;
		break;
	case UIDeviceOrientationLandscapeRight:			//Å@"â°è„ïîâE"
		fReverse = FALSE;
		SCrotate = FALSE;
		fLandscape = TRUE;
		break;
	case UIDeviceOrientationPortrait:				//Å@"èc"
	case UIDeviceOrientationPortraitUpsideDown:		//Å@"ècÇ≈è„â∫ãtÇ≥"
		SCrotate = TRUE;
		fLandscape = FALSE;
		break;
	}
}

