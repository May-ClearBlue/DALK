//
//  ViewController.m
//  test
//
//  Created by 中野 利哉 on 2012/08/31.
//  Copyright (c) 2012年 中野 利哉. All rights reserved.
//

#import "ViewController.h"


#import <AVFoundation/AVFoundation.h>
#import <QuartzCore/QuartzCore.h>


#include	"General.h"
#include	"shader.h"
#include	"ipgsl.h"


UINT8	PADlog[40][256];
UINT32	PADlogNum;


UINT32	PADstackID[100];			// hash
float	PADstackData[100][4];		// pos
UINT32	PADstackNum;




float	accDeviceX, accDeviceY, accDeviceZ;
float	rotDevice;

GLuint	movtex;

GLKMatrix4 VPmat;


float	csizex, csizey;



static const NSString* itemStatusContext;
AVURLAsset *asset;
AVPlayerItem *playerItem;
AVPlayer *player;
AVMutableComposition *comp;
AVMutableCompositionTrack *track;
AVAssetImageGenerator *imageGen;
CGContextRef imgContext;
GLubyte *buffer;
float	MoviePos;

BOOL	ShaderUse[100];
GLuint	ShaderID[100];

NSTimeInterval	oldTime = 0;
NSTimeInterval	nowTime;
NSTimeInterval	execTime;
NSTimeInterval	drawTime;




#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Uniform index.
enum
{
    UNIFORM_MODELVIEWPROJECTION_MATRIX,
    UNIFORM_NORMAL_MATRIX,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum
{
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    NUM_ATTRIBUTES
};



#define		CHAPPRIMMAX		(128*128*6)		// (dx + 1) * (dy + 1)
/*
// header
	CHAcellMax[bno2] = CHAcellMax[bno];
	CHAcellIDMax[bno2] = CHAcellIDMax[bno];
	copyMem(CHAcellID[bno2], CHAcellID[bno], CHAcellIDMax[bno] * 12);	// 8 * 4 * 20000
	copyMem(CHAcell[bno2], CHAcell[bno], CHAcellMax[bno] * 16);	// 8 * 4 * 20000
*/
UINT32	CHAcellMax[16];
UINT32	CHAcellIDMax[16];
float	*CHAcell[16];	// 8 * 4 * 20000
UINT32	*CHAcellID[16];	// 8 * 3 * 200
UINT32	CHAdispWorkNum;
float	*CHAdispWork;
UINT8	*CHAdispAlpha;

TLVERTEX	*ctlv;
UINT32		chaPrimMax = 0;
UINT32		chaPrimSet = 0;
UINT32		chaPrimMax2 = 0;
UINT32		chaPrimSet2 = 0;


void CharaInit()
{
	UINT32	i;

	for(i=0 ; i<16 ; i++)
	{
		CHAcellMax[i] = 0;
		CHAcellIDMax[i] = 0;
		CHAcell[i] = NULL;
		CHAcellID[i] = NULL;
	}

	CHAdispWork = allocMem(sizeof(float) * 10 * 14000);
	CHAdispAlpha = allocMem(14000);
	ctlv = (TLVERTEX *)allocMem(sizeof(TLVERTEX) * CHAPPRIMMAX);
	chaPrimMax = 0;
	chaPrimSet = 0;
	chaPrimMax2 = 0;
	chaPrimSet2 = 0;
}

void CharaEnd()
{
	UINT32	i;

	for(i=0 ; i<16 ; i++)
	{
		freeMem(CHAcellID[i]);
		freeMem(CHAcell[i]);
	}

	freeMem(CHAdispWork);
	freeMem(CHAdispAlpha);
	freeMem(ctlv);
}


void CharaLayerLoad(UINT32 bno, NSString *fname)
{
	NSString *path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	// メモリを確保し、指定パス名のファイルを読み込む
	NSData *data = [[NSData alloc] initWithContentsOfFile:path];
	// バッファのポインタ取得
	unsigned char const* pData = [data bytes];
	// データサイズ取得
	int dataSize = [data length];

	freeMem(CHAcellID[bno]);
	freeMem(CHAcell[bno]);

	CHAcellIDMax[bno] = *(UINT32 *)pData;	pData += 4;
	CHAcellMax[bno] = *(UINT32 *)pData;		pData += 4;
	CHAcellID[bno] = allocMem(CHAcellIDMax[bno] * sizeof(UINT32) * 3);
	CHAcell[bno] = allocMem(CHAcellMax[bno] * sizeof(float) * 4);

	copyMem(CHAcellID[bno], pData, CHAcellIDMax[bno] * sizeof(UINT32) * 3);	// 8 * 4 * 20000
	pData += CHAcellIDMax[bno] * sizeof(UINT32) * 3;
	copyMem(CHAcell[bno], pData, CHAcellMax[bno] * sizeof(float) * 4);	// 8 * 4 * 20000

	// リリース
	[data release];
}



void CharaLayerSetPrim(UINT32 bno, UINT32 scel, UINT32 ncel, UINT8 alpha)
{
//	UINT32	bno;
	UINT32	i, j, cpos, dpos, rcs, wcs, rcadr, wcadr, rcadr2, wcadr2;
//	float	sx, sy, tx, ty, tlx, tly;

//	bno = CHAcellBuf[sno - SURF_CHA1];
//	tlx = 30.0f / STable[sno].ftlx;
//	tly = 30.0f / STable[sno].ftly;

	cpos = scel * 4;
	dpos = CHAdispWorkNum * 10;

	for(i=0 ; i<ncel ; i++)
	{
		CHAdispWork[dpos  ] = CHAcell[bno][cpos  ];
		CHAdispWork[dpos+1] = CHAcell[bno][cpos+1];
		CHAdispWork[dpos+2] = CHAcell[bno][cpos  ];
		CHAdispWork[dpos+3] = CHAcell[bno][cpos+1]+30.0f;
		CHAdispWork[dpos+4] = CHAcell[bno][cpos  ]+30.0f;
		CHAdispWork[dpos+5] = CHAcell[bno][cpos+1];
		CHAdispWork[dpos+6] = CHAcell[bno][cpos  ]+30.0f;
		CHAdispWork[dpos+7] = CHAcell[bno][cpos+1]+30.0f;
		CHAdispWork[dpos+8] = CHAcell[bno][cpos+2];
		CHAdispWork[dpos+9] = CHAcell[bno][cpos+3];
		CHAdispAlpha[CHAdispWorkNum] = alpha;
		CHAdispWorkNum ++;
		cpos += 4;
		dpos += 10;
	}
}


void CharaLayerPrim(UINT32 bno, UINT32 pose, UINT32 face, UINT32 eye, UINT32 mouth, UINT32 ex)
{
	UINT32	i, id;
//	UINT32	bno, f;
	UINT32	f;
	UINT32	posefl[32], poseno;
	UINT32	faceid, eyeid, mouthid, exid, eyemouthid;

	faceid = 0x20000000 + face;
	eyeid = 0x30000000 + (face << 8) + eye;
	mouthid = 0x40000000 + (face << 8) + mouth;
	exid = 0x50000000 + ex;
	eyemouthid = 0x60000000 + (face << 8) + mouth + eye * 3;

	f = 1;

	for(i=0 ; i<32 ; i++)
	{
		posefl[i] = FALSE;
		if(pose & f)
		{
			posefl[i] = TRUE;
		}
		f <<= 1;
	}

	CHAdispWorkNum = 0;

	id = 0;
	for(i=0 ; i<CHAcellIDMax[bno] ; i++)
	{
		if(CHAcellID[bno][id] == faceid  ||  CHAcellID[bno][id] == eyeid  ||  CHAcellID[bno][id] == 0x00000001
			||  CHAcellID[bno][id] == eyemouthid  ||  CHAcellID[bno][id] == mouthid)
		{
			CharaLayerSetPrim(bno, CHAcellID[bno][id+1], CHAcellID[bno][id+2], 0);
		}
//		else if(CHAcellID[bno][id] == mouthid)
//		{
//			CharaLayerSetPrim(sno, CHAcellID[bno][id+1], CHAcellID[bno][id+2], 0);
//		}
		else if((CHAcellID[bno][id] & 0xf0000000) == 0x10000000)	// pose
		{
			poseno = CHAcellID[bno][id] & 0xff;
			if(posefl[poseno-1])
			{
				CharaLayerSetPrim(bno, CHAcellID[bno][id+1], CHAcellID[bno][id+2], 0);
			}
		}
		else if(ex != 0  &&  CHAcellID[bno][id] == exid)
		{
// 	0x0ss000nn				ss:セット番号         1-
			if(((ex & 0x0ff00000) >> 20)  >= 10)
			{
				CharaLayerSetPrim(bno, CHAcellID[bno][id+1], CHAcellID[bno][id+2], 1);
			}
			else
			{
				CharaLayerSetPrim(bno, CHAcellID[bno][id+1], CHAcellID[bno][id+2], 0);
			}
		}
		id += 3;
	}
}

void CHAstretch(UINT32 sizex, UINT32 sizey)
{
	UINT32	i;
	float	fsizex, fsizey;

// 拡大処理
	if(sizex != 0  &&  sizex != 1000)
	{
		fsizex = (float)sizex / 1000.0f;
		for(i=0 ; i<CHAdispWorkNum*10 ; i+=10)
		{
			CHAdispWork[i] *= fsizex;
			CHAdispWork[i+2] *= fsizex;
			CHAdispWork[i+4] *= fsizex;
			CHAdispWork[i+6] *= fsizex;
		}
	}
	if(sizey != 0  &&  sizey != 1000)
	{
		fsizey = (float)sizey / 1000.0f;
		for(i=0 ; i<CHAdispWorkNum*10 ; i+=10)
		{
			CHAdispWork[i+1] *= fsizey;
			CHAdispWork[i+3] *= fsizey;
			CHAdispWork[i+5] *= fsizey;
			CHAdispWork[i+7] *= fsizey;
		}
	}
}


void CHAsetPos2(UINT32 sno, SINT32 posx, SINT32 posy, UINT32 alpha, UINT32 col)
{
	UINT32	filter, dpos;
	UINT32	i, j;
	float	sizex, sizey, dz, px, py;
	float	tlx, tly, r, g, b, a;

	if(alpha > 255) alpha = 255;

	r = (float)((col & 0xff0000) >> 16) / 255.0f;
	g = (float)((col & 0xff00) >> 8) / 255.0f;
	b = (float)(col & 0xff) / 255.0f;
	a = (float)alpha / 255.0f;


//	bno = CHAcellBuf[chafade.surf - SURF_CHA1];
	tlx = 30.0f / STable[sno].ftsizeX;
	tly = 30.0f / STable[sno].ftsizeY;

	px = (float)posx;
	py = (float)posy;

// 表示位置へ平行移動
	for(i=0 ; i<CHAdispWorkNum*10 ; i+=10)
	{
		CHAdispWork[i] += px;
		CHAdispWork[i+1] += py;
		CHAdispWork[i+2] += px;
		CHAdispWork[i+3] += py;
		CHAdispWork[i+4] += px;
		CHAdispWork[i+5] += py;
		CHAdispWork[i+6] += px;
		CHAdispWork[i+7] += py;
	}
// プリミティブ登録
	j = 0;
	for(i=0 ; i<CHAdispWorkNum*10 ; i+=10, j++)
	{
		CHAdispAlpha[j] &= 0x7f;
		if(CHAdispWork[i] >= 0.0f  &&  CHAdispWork[i] <= 1280.0f  &&  CHAdispWork[i+1] >= 0.0f  &&  CHAdispWork[i+1] <= 720.0f)
		{
			CHAdispAlpha[j] |= 0x80;
			continue;
		}
		if(CHAdispWork[i+2] >= 0.0f  &&  CHAdispWork[i+2] <= 1280.0f  &&  CHAdispWork[i+3] >= 0.0f  &&  CHAdispWork[i+3] <= 720.0f)
		{
			CHAdispAlpha[j] |= 0x80;
			continue;
		}
		if(CHAdispWork[i+4] >= 0.0f  &&  CHAdispWork[i+4] <= 1280.0f  &&  CHAdispWork[i+5] >= 0.0f  &&  CHAdispWork[i+5] <= 720.0f)
		{
			CHAdispAlpha[j] |= 0x80;
			continue;
		}
		if(CHAdispWork[i+6] >= 0.0f  &&  CHAdispWork[i+6] <= 1280.0f  &&  CHAdispWork[i+7] >= 0.0f  &&  CHAdispWork[i+7] <= 720.0f)
		{
			CHAdispAlpha[j] |= 0x80;
			continue;
		}
	}
	chaPrimMax = 0;
	chaPrimSet = 0;
	j = 0;
	for(i=0 ; i<CHAdispWorkNum*10 ; i+=10, j++)
	{
		if(CHAdispAlpha[j] == 0x80)
		{
			SetXYZRGBA(ctlv[chaPrimMax], CHAdispWork[i], CHAdispWork[i+1], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax], CHAdispWork[i+8], CHAdispWork[i+9]);
			SetXYZRGBA(ctlv[chaPrimMax+1], CHAdispWork[i+2], CHAdispWork[i+3], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax+1], CHAdispWork[i+8], CHAdispWork[i+9]+tly);
			SetXYZRGBA(ctlv[chaPrimMax+2], CHAdispWork[i+4], CHAdispWork[i+5], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax+2], CHAdispWork[i+8]+tlx, CHAdispWork[i+9]);
			chaPrimMax += 3;
			chaPrimSet ++;

			SetXYZRGBA(ctlv[chaPrimMax], CHAdispWork[i+4], CHAdispWork[i+5], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax], CHAdispWork[i+8]+tlx, CHAdispWork[i+9]);
			SetXYZRGBA(ctlv[chaPrimMax+1], CHAdispWork[i+2], CHAdispWork[i+3], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax+1], CHAdispWork[i+8], CHAdispWork[i+9]+tly);
			SetXYZRGBA(ctlv[chaPrimMax+2], CHAdispWork[i+6], CHAdispWork[i+7], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax+2], CHAdispWork[i+8]+tlx, CHAdispWork[i+9]+tly);
			chaPrimMax += 3;
			chaPrimSet ++;
		}
	}
	chaPrimMax2 = chaPrimMax;
	chaPrimSet2 = 0;
	j = 0;
	for(i=0 ; i<CHAdispWorkNum*10 ; i+=10, j++)
	{
		if(CHAdispAlpha[j] == 0x81)
		{
			SetXYZRGBA(ctlv[chaPrimMax2], CHAdispWork[i], CHAdispWork[i+1], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax2], CHAdispWork[i+8], CHAdispWork[i+9]);
			SetXYZRGBA(ctlv[chaPrimMax2+1], CHAdispWork[i+2], CHAdispWork[i+3], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax2+1], CHAdispWork[i+8], CHAdispWork[i+9]+tly);
			SetXYZRGBA(ctlv[chaPrimMax2+2], CHAdispWork[i+4], CHAdispWork[i+5], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax2+2], CHAdispWork[i+8]+tlx, CHAdispWork[i+9]);
			chaPrimMax2 += 3;
			chaPrimSet2 ++;

			SetXYZRGBA(ctlv[chaPrimMax2], CHAdispWork[i+4], CHAdispWork[i+5], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax2], CHAdispWork[i+8]+tlx, CHAdispWork[i+9]);
			SetXYZRGBA(ctlv[chaPrimMax2+1], CHAdispWork[i+2], CHAdispWork[i+3], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax2+1], CHAdispWork[i+8], CHAdispWork[i+9]+tly);
			SetXYZRGBA(ctlv[chaPrimMax2+2], CHAdispWork[i+6], CHAdispWork[i+7], -0.5f,  r, g, b, a);	// x,y,z, r,g,b,a
			SetUV(ctlv[chaPrimMax2+2], CHAdispWork[i+8]+tlx, CHAdispWork[i+9]+tly);
			chaPrimMax2 += 3;
			chaPrimSet2 ++;
		}
	}
// 表示位置復元
	for(i=0 ; i<CHAdispWorkNum*10 ; i+=10)
	{
		CHAdispWork[i] -= px;
		CHAdispWork[i+1] -= py;
		CHAdispWork[i+2] -= px;
		CHAdispWork[i+3] -= py;
		CHAdispWork[i+4] -= px;
		CHAdispWork[i+5] -= py;
		CHAdispWork[i+6] -= px;
		CHAdispWork[i+7] -= py;
	}
}


void drawChara(SINT32 posx, SINT32 posy, UINT32 pose, UINT32 face, UINT32 eye, UINT32 mouth, UINT32 sizex, UINT32 sizey)
{
	CharaLayerPrim(0, pose, face, eye, mouth, 0);	// UINT32 bno, UINT32 pose, UINT32 face, UINT32 eye, UINT32 mouth, UINT32 ex

	CHAstretch(sizex, sizey);

	CHAsetPos2(12, posx, posy, 255, 0xffffff);	// (UINT32 sno, SINT32 posx, SINT32 posy, UINT32 alpha, UINT32 col)

	if(chaPrimMax2)
	{
		SetShader(1, viewofsYL, viewXL, viewYL);
		SetTexture(1, 0, STable[12].texID);
		SetTLVertex(ctlv);
	}
	if(chaPrimMax)
	{
		DrawPrimitiveTRI(0, chaPrimMax);
	}
	if(chaPrimMax2 > chaPrimMax)
	{
		DrawPrimitiveTRI(chaPrimMax, chaPrimMax);
	}


}





UINT8 *DrawFontTbl = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-*/:;.,!?=()[]<>_&%$#'`~ﾟ^@ ";

void DrawText(SINT32 tx, SINT32 ty, UINT8 *text, UINT32 size, UINT32 col, UINT32 alpha)
{
	TLVERTEX	tlv[4];
	UINT32		i, x, y, lx, ly;

	lx = size / 2;
	ly = size;

	while(*text)
	{
		for(i=0 ; DrawFontTbl[i]!=0 ; i++)
		{
			if(DrawFontTbl[i] == *text) break;
		}
		if(DrawFontTbl[i] != 0)
		{
			y = i / 16;
			x = i - y * 16;

			GSLfontStretch(10, x*16, y*32, 16, 32, tx, ty, tx+lx, ty+ly, col, alpha);
	
			tx += lx;
		}
	
		text ++;
	}
}







void DrawTest()
{
	TLVERTEX	tlv[4];
//	LVERTEX		lv[4];
//	VERTEX		v[4];

	SetXYZRGBA(tlv[0], -1.0f, 1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[0], 0.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[1], 1.0f, 1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[1], 1.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[2], -1.0f, -1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[2], 0.0f, 1.0f);										// u,v
	SetXYZRGBA(tlv[3], 1.0f, -1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[3], 1.0f, 1.0f);										// u,v

	SetShader(1, viewofsYL, 1.0f, 1.0f);
	SetTexture(1, 0, STable[1].texID);
	SetTLVertex(tlv);
	DrawPrimitiveTRISTRIP(0, 4);


	SetXYZRGBA(tlv[0], -1.0f, 0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[0], 0.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[1], 1.0f, 0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[1], 480.0f / 512.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[2], -1.0f, -0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[2], 0.0f, 320.0f / 512.0f);										// u,v
	SetXYZRGBA(tlv[3], 1.0f, -0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[3], 480.0f / 512.0f, 320.0f / 512.0f);										// u,v
/*
	SetXYZRGBA(tlv[0], -1.0f, 0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[0], 0.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[1], 1.0f, 0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[1], 1.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[2], -1.0f, -0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[2], 0.0f, 0.8f);										// u,v
	SetXYZRGBA(tlv[3], 1.0f, -0.8f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[3], 1.0f, 0.8f);										// u,v
*/

	SetShader(1, viewofsYL, 1.0f, 1.0f);
	SetTexture(1, 0, movtex);
	SetTLVertex(tlv);
	DrawPrimitiveTRISTRIP(0, 4);


	SetXYZRGBA(tlv[0], -1.0f, 1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[0], 0.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[1], 1.0f, 1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[1], 1.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[2], -1.0f, -1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[2], 0.0f, 1.0f);										// u,v
	SetXYZRGBA(tlv[3], 1.0f, -1.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[3], 1.0f, 1.0f);										// u,v

	SetShader(1, viewofsYL, 1.0f, 1.0f);
	SetTexture(1, 0, STable[2].texID);
	SetTLVertex(tlv);
	DrawPrimitiveTRISTRIP(0, 4);


/*
	SetXYZRGBA(lv[0], -0.5f, 0.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[1], 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[2], -0.5f, 0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetXYZRGBA(lv[3], 0.5f, 0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a

	SetShader(0, viewofsYL, 1.0f, 1.0f);
	SetLVertex(lv);
	DrawPrimitiveTRISTRIP(0, 4);


	SetXYZ(v[0], -0.1f, 0.0f, -0.5f);	// x,y,z, r,g,b,a
	SetXYZ(v[1], 0.1f, -0.5f, -0.5f);	// x,y,z, r,g,b,a
	SetXYZ(v[2], -0.1f, 0.5f, -0.5f);	// x,y,z, r,g,b,a
	SetXYZ(v[3], 0.5f, 0.5f, -0.5f);	// x,y,z, r,g,b,a

	SetShader(30, viewofsYL, 1.0f, 1.0f);
	SetVertex(v);
	DrawPrimitiveTRISTRIP(0, 4);
*/
}

UINT32 DFcol[10] = 
{
	0xccffff,
	0x88ffaa,
	0x44ff88,
	0x22ff00,
	0x66ff00,
	0xaaff00,
	0xffff00,
	0xffcc00,
	0xff6600,
	0xff0000
};

void DrawFrameRate()
{
	LVERTEX		lv[4];
	SINT32		ly, ix, iy, i, lly;
	SINT32		sx, sy, col;
	UINT8		buf[1024];

/*
	sy = 0;
	for(iy=0 ; iy<9 ; iy++)
	{
		sx = 0;
		for(ix=0 ; ix<16 ; ix++)
		{
			col = iy * 16 * 65536 + ix * 16 * 256 + 64;
			GSLflatRect(sx, sy, 80, 80, col, 255, 0);
			sx += 80;
		}
		sy += 80;
	}
*/





	ly = (SINT32)(drawTime*1000.0f);
//	ly = (SINT32)(execTime*1000.0f);

	sx = 0;
	for(i=0 ; i<10 ; i++)
	{
		if(ly > 0)
		{
			lly = ly * 720 / 16;
			if(lly > 720) lly = 720;

			GSLflatRect(sx, 0, 16, 720, 0, 64, 0);
			GSLflatRect(sx, 0, 16, lly, DFcol[i], 255, 0);

			sx += 16;
			ly -= 16;
		}
	}


	sprintf(buf, "Exec Time:%f10.6", execTime);
	DrawText(1, 1, buf, 16, 0, 255);
	DrawText(0, 0, buf, 16, 0xffffff, 255);
	sprintf(buf, "Movie Time:%f10.6", MoviePos);
	DrawText(1, 16+1, buf, 16, 0, 255);
	DrawText(0, 16, buf, 16, 0xffffff, 255);

	sprintf(buf, "FPS :%f10.6", 60.0f/(execTime/0.016f));
	DrawText(1001, 1, buf, 16, 0, 255);
	DrawText(1000, 0, buf, 16, 0xffffff, 255);


	if(fReverse)
	{
		sprintf(buf, "Landscape Right :%f10.6, %f10.6, %f10.6:%f10.6", accDeviceX, accDeviceY, accDeviceZ, rotDevice);
	}
	else
	{
		sprintf(buf, "Landscape Left  :%f10.6, %f10.6, %f10.6:%f10.6", accDeviceX, accDeviceY, accDeviceZ, rotDevice);
	}
	DrawText(1, 32+1, buf, 16, 0, 255);
	DrawText(0, 32, buf, 16, 0xffffff, 255);

	sprintf(buf, "X:%d  Y:%d", SCsizeX, SCsizeY);
	DrawText(1, 48+1, buf, 16, 0, 255);
	DrawText(0, 48, buf, 16, 0xffffff, 255);


	sy = 32;
	for(i=0 ; i<PADlogNum ; i++)
	{
		DrawText(701, sy+1, PADlog[i], 16, 0, 255);
		DrawText(700, sy, PADlog[i], 16, 0xffffff, 255);
		sy += 16;
	}




//	GSLsprRect(10, 0, 0, 256, 256, 0, 0, 0xffffff, 255, 1);
//	GSLfontRect(10, 0, 0, 256, 256, 0, 0, 0xffffff, 255);

/*
	TLVERTEX	tlv[4];

	SetXYZRGBA(tlv[0], 0.0f, 0.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[0], 0.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[1], 256.0f, 0.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[1], 1.0f, 0.0f);										// u,v
	SetXYZRGBA(tlv[2], 0.0f, 256.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[2], 0.0f, 1.0f);										// u,v
	SetXYZRGBA(tlv[3], 256.0f, 256.0f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f);	// x,y,z, r,g,b,a
	SetUV(tlv[3], 1.0f, 1.0f);										// u,v

	SetShader(4, viewofsYL, viewXL, viewYL);
	SetTexture(4, 0, STable[10].texID);
	SetTLVertex(tlv);
	DrawPrimitiveTRISTRIP(0, 4);
*/

}








GLfloat gCubeVertexData[216] = 
{
    // Data layout for each line below is:
    // positionX, positionY, positionZ,     normalX, normalY, normalZ,
    0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,          1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
    
    0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,
    
    -0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,
    
    -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,
    
    0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
    
    0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f
};

@interface ViewController () {
    GLuint _program;
    
    GLKMatrix4 _modelViewProjectionMatrix;
    GLKMatrix3 _normalMatrix;
    float _rotation;
    
    GLuint _vertexArray;
    GLuint _vertexBuffer;
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation ViewController

/*
	最初にくるっぽい
*/
- (void)viewDidLoad
{

// フルスクリーンにする
//	[UIApplication sharedApplication].statusBarHidden	= YES;
//	fullscreenView.bounds	= [UIScreen mainScreen].bounds;

// マルチタッチイベントを検出する
	self.view.multipleTouchEnabled = YES;

// 傾きセンサ登録
    UIAccelerometer *accel = [UIAccelerometer sharedAccelerometer];
    accel.delegate = self;
    accel.updateInterval = 1.0f/60.0f;       

    [super viewDidLoad];
    
// OpenGL ES 2.00 で初期化
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
// 関連付けの定型文
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;

// レンダリングの深度バッファを指定
// GLKViewDrawableDepthFormat24 : 深度バッファを使用する
// GLKViewDrawableDepthFormatNone : 深度バッファを使用しない
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

    [self PADinit];
	PADlogNum = 0;

// setupGLを呼ぶ
    [self setupGL];

//	[self movieSetup];



//	SetViewProjection();

	GetOrientation();
}

- (void)viewDidUnload
{    
    [super viewDidUnload];
    
    [self tearDownGL];
    
// コンテキストが自分が設定したものであれば、NULL(nil)を設定する
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
	self.context = nil;

	CharaEnd();
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

/*
	傾き取得用
*/
- (void)accelerometer:(UIAccelerometer *)acel 
        didAccelerate:(UIAcceleration *)acceleration {
    
	accDeviceX = acceleration.x;
	accDeviceY = acceleration.y;
	accDeviceZ = acceleration.z;

	if(fabs(accDeviceX) > fabs(accDeviceZ))
	{
		rotDevice = atan2(accDeviceY, -accDeviceX);

	}
	else
	{
		rotDevice = atan2(accDeviceY, -accDeviceZ);
	}
}

/*
	端末を回転させたりとかすると来る
	FALSE(NO)を返すと、回転を無視する（画面固定）
*/
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
	{
	// iPhone
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    }
	else
	{
	// iPad
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
//        return YES;
    }
}
/*------------------------------------------------------------------------------------------
if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
は、端末がiPhoneかどうかを識別している

識別詳細
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)		// この表記は古い方法らしい
	{
		//iphone = UIUserInterfaceIdiomPhone
	}
	else
	{
		//iPad = UIUserInterfaceIdiomPad
	}



UIDeviceOrientation はデバイスの向きを判別フラグだが、以下の７種類がある
	
typedef enum {
    UIDeviceOrientationUnknown,    //向き不明
    UIDeviceOrientationPortrait,            // HomeButton 下
    UIDeviceOrientationPortraitUpsideDown,  //HomeButton 上
    UIDeviceOrientationLandscapeLeft,       // HomeButton 右（左向き）
    UIDeviceOrientationLandscapeRight,      //HomeButton 左（右向き）
    UIDeviceOrientationFaceUp,              // 表
    UIDeviceOrientationFaceDown             // 裏
} UIDeviceOrientation;

UIInterfaceOrientation は以下の４種類

typedef enum {
    UIInterfaceOrientationPortrait           = UIDeviceOrientationPortrait,
    UIInterfaceOrientationPortraitUpsideDown = UIDeviceOrientationPortraitUpsideDown,
    UIInterfaceOrientationLandscapeLeft      = UIDeviceOrientationLandscapeRight,
    UIInterfaceOrientationLandscapeRight     = UIDeviceOrientationLandscapeLeft
} UIInterfaceOrientation;

------------------------------------------------------------------------------------------*/
GLuint createCaptureBuffer()
{


}



-(void)movieMain
{
	size_t	tsizex, tsizey;
	size_t	sx, sy;

	CMTime time = [player currentTime];
	CMTime actualTime;
	NSError* error = nil;

	MoviePos = (float)(time.value / time.timescale);

	CGImageRef cgImage = [imageGen copyCGImageAtTime:time actualTime:&actualTime error:&error];
	if (cgImage != nil) {
		size_t sizeX = CGImageGetWidth(cgImage);
		size_t sizeY = CGImageGetHeight(cgImage);
//		size_t size = MIN(width, height);
		if (!imgContext) {
			CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Big;
			switch (CGImageGetAlphaInfo(cgImage)) {
				case kCGImageAlphaPremultipliedLast:
				case kCGImageAlphaPremultipliedFirst:
				case kCGImageAlphaLast:
				case kCGImageAlphaFirst:
					bitmapInfo |= kCGImageAlphaPremultipliedLast;
					break;
				default:
					bitmapInfo |= kCGImageAlphaNoneSkipLast;
					break;
			}
			CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
			imgContext = CGBitmapContextCreate(buffer, 512, 512, 8, 512*4, colorSpace, bitmapInfo);
//			imgContext = CGBitmapContextCreate(buffer, 1024, 1024, 8, 1024*4, colorSpace, bitmapInfo);
			CGColorSpaceRelease(colorSpace);
//			if (width != 256 || height != 256) {
//				CGContextScaleCTM(cgContext, 256.0f/size, 256.0f/size);
//			}
		}
//		CGRect rect = CGRectMake((width-size)*0.5f, (height-size)*0.5f, size, size);

		for(tsizex=256 ; tsizex<sizeX ; tsizex*=2) ;
		for(tsizey=256 ; tsizey<sizeY ; tsizey*=2) ;

		sx = tsizex - sizeX;
		sy = tsizey - sizeY;
		glBindTexture(GL_TEXTURE_2D, movtex);
		CGContextDrawImage(imgContext, CGRectMake(sx, sy, (CGFloat)sizeX, (CGFloat)sizeY), cgImage);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tsizex, tsizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		//glGenerateMipmap(GL_TEXTURE_2D);
		CGImageRelease(cgImage);
	} else {
		NSLog(@"error copyCGImageAtTime");
		NSLog(@"%@", [error localizedDescription]);
	}
}


- (void)movieSetup
{
//	NSURL* assetURL = [[NSBundle mainBundle] URLForResource:@"airi" withExtension:@"png"];
	NSURL* assetURL = [[NSBundle mainBundle] URLForResource:@"ar" withExtension:@"mp4"];
	NSDictionary* assetOptions = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES]
															 forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
	asset = [[AVURLAsset alloc] initWithURL:assetURL options:assetOptions];
	[asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:@"tracks"] completionHandler:^{
		NSError* error = nil;
		AVKeyValueStatus status = [asset statusOfValueForKey:@"tracks" error:&error];
		if (status == AVKeyValueStatusLoaded) {
			playerItem = [[AVPlayerItem alloc] initWithAsset:asset];
			[playerItem addObserver:self forKeyPath:@"status" options:0 context:&itemStatusContext];
			[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playerItemDidReachEnd:)
														 name:AVPlayerItemDidPlayToEndTimeNotification
													   object:playerItem];
			player = [[AVPlayer alloc] initWithPlayerItem:playerItem];
		} else {
			NSLog(@"%@", [error localizedDescription]);
		}
	}];

	AVAssetTrack* srcTrack = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
	comp = [AVMutableComposition composition];
	track = [comp addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
	NSError* error = nil;
	BOOL ok = [track insertTimeRange:srcTrack.timeRange ofTrack:srcTrack atTime:kCMTimeZero error:&error];
	if (!ok) {
		NSLog(@"%@", [error localizedDescription]);
	}
	imageGen = [[AVAssetImageGenerator alloc] initWithAsset:comp];

	buffer = malloc(512 * 512 * 4);
//	buffer = malloc(1024 * 1024 * 4);
	if (buffer == nil) {
		NSLog(@"out of memory");
	}

	glGenTextures(1, &movtex);
	glBindTexture(GL_TEXTURE_2D, movtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glEnable(GL_TEXTURE_2D);
}

- (void)observeValueForKeyPath:(NSString*)keyPath ofObject:(id)object 
						change:(NSDictionary*)change context:(void*)ctx
{
	if (ctx == &itemStatusContext) {
		if (playerItem.status == AVPlayerItemStatusReadyToPlay && player.rate == 0) {
			for (AVPlayerItemTrack* itemTrack in playerItem.tracks) {
				if (![itemTrack.assetTrack hasMediaCharacteristic:AVMediaCharacteristicAudible]) {
					itemTrack.enabled = NO;
				}
			}
			[player play];
		}
	} else {
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:ctx];
	}
}

- (void)playerItemDidReachEnd:(NSNotification*)notification
{
	[player seekToTime:kCMTimeZero];
	//[player pause];
}





//	-----------------------------------------------------------------------------------------------------
//	-----------------------------------------------------------------------------------------------------
//	-----------------------------------------------------------------------------------------------------
//	-----------------------------------------------------------------------------------------------------





- (void) PADinit
{
/*
// シングルタップ
	UITapGestureRecognizer* tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTapGesture:)];
	[self.view addGestureRecognizer:tapGesture];
	[tapGesture release];
// ダブルタップ
	UITapGestureRecognizer* doubleTapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapGesture:)];
	doubleTapGesture.numberOfTapsRequired = 2;
	[self.view addGestureRecognizer:doubleTapGesture];
	[doubleTapGesture release];

//	ピンチ
	UIPinchGestureRecognizer* pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
	[self.view addGestureRecognizer:pinchGesture];
	[pinchGesture release];

//	パン（ドラッグ）

	UIPanGestureRecognizer* panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
	// ジェスチャーを認識する指の最大数
	panGesture.maximumNumberOfTouches = 3;
	// ジェスチャーを認識する指の最小数
	panGesture.minimumNumberOfTouches = 1;
	[self.view addGestureRecognizer:panGesture];
	[panGesture release];

// 左へスワイプ
	UISwipeGestureRecognizer* swipeLeftGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeLeftGesture:)];
	swipeLeftGesture.direction = UISwipeGestureRecognizerDirectionLeft;
	[self.view addGestureRecognizer:swipeLeftGesture];
	[swipeLeftGesture release];
// 右へスワイプ
	UISwipeGestureRecognizer* swipeRightGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeRightGesture:)];
	swipeRightGesture.direction = UISwipeGestureRecognizerDirectionRight;
	[self.view addGestureRecognizer:swipeRightGesture];
	[swipeRightGesture release];

//	ローテイト
	UIRotationGestureRecognizer* rotationGesture = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotationGesture:)];
	[self.view addGestureRecognizer:rotationGesture];
	[rotationGesture release];

//	ロングプレス
	UILongPressGestureRecognizer* longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
	[self.view addGestureRecognizer:longPressGesture];
	[longPressGesture release];
*/
}

void PADaddLog(UINT8 *txt)
{
	UINT32	i;

	if(PADlogNum == 40) PADlogNum --;
	for(i=PADlogNum ; i>0 ; i--)
	{
		strcpy(PADlog[i], PADlog[i - 1]);
	}

	strcpy(PADlog[0], txt);
	PADlogNum ++;
}


//-----------------------------------------------------------------------------------------------------
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UINT8	buf[256];
	UINT32	i, ct, hash;
	CGPoint touchPoint;

	//タッチされた場所
	UITouch *touch = [touches anyObject];
//	ct = [touches count];
	i = 0;
//	for(i=0 ; i<ct ; i++)
	for(touch in [touches allObjects])
	{
		touchPoint = [touch locationInView:self.view];
		hash = [touch hash];
		sprintf(buf, "touchesBegan %d:x=%f, y=%f", hash, touchPoint.x, touchPoint.y);
		PADaddLog(buf);
		i ++;
	}

/*	i = 0;
    for (touch in [event allTouches])
//	for(i=0 ; i<ct ; i++)
	{
		touchPoint = [touch locationInView:self.view];
		sprintf(buf, "touchesBegan %d:x=%f, y=%f", i+1, touchPoint.x, touchPoint.y);
		PADaddLog(buf);
		i ++;
	}
*/
}

//-----------------------------------------------------------------------------------------------------
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UINT8	buf[256];
	UINT32	i, ct, hash;
	CGPoint touchPoint;

	//タッチされた場所
	UITouch *touch = [touches anyObject];
//	ct = [touches count];
	i = 0;
//	for(i=0 ; i<ct ; i++)
	for(touch in [touches allObjects])
	{
		touchPoint = [touch locationInView:self.view];
		hash = [touch hash];
		sprintf(buf, "touchesEnded %d:x=%f, y=%f", hash, touchPoint.x, touchPoint.y);
		PADaddLog(buf);
		i ++;
	}

/*
	i = 0;
    for (touch in [event allTouches])
//	for(i=0 ; i<ct ; i++)
	{
		touchPoint = [touch locationInView:self.view];
		sprintf(buf, "touchesEnded %d:x=%f, y=%f", i+1, touchPoint.x, touchPoint.y);
		PADaddLog(buf);
		i ++;
	}
*/
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	UINT8	buf[256];
	UINT32	i, ct, hash;
	CGPoint cur_pos;
	CGPoint last_pos;

	UITouch *touch = [touches anyObject];
//	ct = [[event allTouches] count];

	i = 0;
    for (touch in [event allTouches])
//	for(i=0 ; i<ct ; i++)
	{
		CGPoint cur_pos = [touch locationInView:self.view];
		CGPoint last_pos = [touch previousLocationInView:self.view];
		hash = [touch hash];

		sprintf(buf, "touchesMoved %d:x=%f, y=%f : x=%f, y=%f", hash, cur_pos.x, cur_pos.y, last_pos.x, last_pos.y);
		PADaddLog(buf);
		i ++;
	}
}

/*
//-----------------------------------------------------------------------------------------------------
//	タップ
- (void) handleTapGesture:(UITapGestureRecognizer*)sender
{
	PADaddLog("Tap");
//	NSLog(@"tap");
}

- (void) handleDoubleTapGesture:(UITapGestureRecognizer*)sender
{
	PADaddLog("Double Tap");
//	NSLog(@"double tap");  
}


//----------------------------------------------------------------------------------------------------
//	ピンチ
- (void) handlePinchGesture:(UIPinchGestureRecognizer*) sender
{
	UINT8	buf[256];

	UIPinchGestureRecognizer* pinch = (UIPinchGestureRecognizer*)sender;
	sprintf(buf, "Pinch scale=%f, velocity=%f", pinch.scale, pinch.velocity);
	PADaddLog(buf);
//	NSLog(@"pinch scale=%f, velocity=%f", pinch.scale, pinch.velocity);
}


//-----------------------------------------------------------------------------------------------------
//	パン（ドラッグ）
- (void) handlePanGesture:(UIPanGestureRecognizer*) sender
{
	UINT8	buf[256];

	UIPanGestureRecognizer* pan = (UIPanGestureRecognizer*) sender;
	CGPoint location = [pan translationInView:self.view];
	sprintf(buf, "Pan x=%f, y=%f", location.x, location.y);
	PADaddLog(buf);
//	NSLog(@"pan x=%f, y=%f", location.x, location.y);
}


//-----------------------------------------------------------------------------------------------------
//	スワイプ
- (void) handleSwipeLeftGesture:(UISwipeGestureRecognizer *)sender
{
	PADaddLog("Swipe left");
//	NSLog(@"swipe left");
}
- (void) handleSwipeRightGesture:(UISwipeGestureRecognizer *)sender
{
	PADaddLog("Swipe right");
//	NSLog(@"swipe right");
}


//-----------------------------------------------------------------------------------------------------
//	ローテイト
- (void) handleRotationGesture:(UIRotationGestureRecognizer*) sender
{
	UINT8	buf[256];

	UIRotationGestureRecognizer* rotation = (UIRotationGestureRecognizer*) sender;
	sprintf(buf, "Rotation rad=%f, velocity=%f", rotation.rotation, rotation.velocity);
	PADaddLog(buf);
//	NSLog(@"rotation rad=%f, velocity=%f", rotation.rotation, rotation.velocity);
}


//-----------------------------------------------------------------------------------------------------
//	ロングプレス
- (void) handleLongPressGesture:(UILongPressGestureRecognizer*) sender
{
	PADaddLog("Long press");
//	NSLog(@"long press");
}  
*/

//	-----------------------------------------------------------------------------------------------------
//	-----------------------------------------------------------------------------------------------------
//	-----------------------------------------------------------------------------------------------------
//	-----------------------------------------------------------------------------------------------------

















/*
	viewDidLoad　から呼ばれる
*/
- (void)setupGL
{

	[EAGLContext setCurrentContext:self.context];

	GSLinit();
	CharaInit();

	GSLloadTexture(0, @"airi256.png");
	GSLloadTexture(1, @"ev_sp18.png");
	GSLloadTexture(2, @"erin.png");
	GSLloadTexture(10, @"font.png");

	GSLloadTexture(12, @"CRS_CLA.png");
	CharaLayerLoad(0, @"CRS_CLA_.lay");

	glEnable(GL_BLEND);										// αブレンドを有効にする
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// src * alpha + dst * (1 - alpha)

/*
	UIImagePickerController *pickerController;

    // カメラが使用可能かどうかチェックする
	if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
	{
		// イメージピッカーを作る
		pickerController = [[UIImagePickerController alloc] init];
		pickerController.sourceType = UIImagePickerControllerSourceTypeCamera;

		// カメラコントローラを隠す
		pickerController.showsCameraControls = NO;

		// カメラオーバーレイビューを追加する
		pickerController.cameraOverlayView = self.view;

		pickerController.delegate = self;

		// イメージピッカーを表示する
		[self presentModalViewController:pickerController animated:YES];
	}

//    [self.view startAnimation];
*/
}

/*
	終了処理
*/
- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    glDeleteBuffers(1, &_vertexBuffer);
    glDeleteVertexArraysOES(1, &_vertexArray);
    
    self.effect = nil;
    
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}




#pragma mark - GLKView and GLKViewController delegate methods
/*
	毎フレーム呼び出し　実行用
*/
- (void)update
{
	GLKMatrix4 modelViewMatrix;
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.1f, 100.0f);

	GetOrientation();

//	SetViewProjection();

//    self.effect.transform.projectionMatrix = projectionMatrix;
    
//    GLKMatrix4 baseModelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -4.0f);
//    baseModelViewMatrix = GLKMatrix4Rotate(baseModelViewMatrix, _rotation, 0.0f, 1.0f, 0.0f);
    
    // Compute the model view matrix for the object rendered with GLKit
//    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -1.5f);
//    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
//    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
//    self.effect.transform.modelviewMatrix = modelViewMatrix;
    
    // Compute the model view matrix for the object rendered with ES2
//    modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 1.5f);
//    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
//    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
//    _normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
//    _modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    
//    _rotation += self.timeSinceLastUpdate * 0.5f;

	GSLresetTexture();
}



/*
	毎フレーム呼び出し　描画用
*/
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	nowTime = [NSDate timeIntervalSinceReferenceDate];
	execTime = nowTime - oldTime;
	oldTime = nowTime;

//	glViewport(0,0,1024,1024);
//	glDepthRangef(0.0f, 1.0f);

	glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	[self movieMain];

//    glBindVertexArrayOES(_vertexArray);
    
    // Render the object with GLKit
//    [self.effect prepareToDraw];
    
//    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // Render the object again with ES2
//	glUseProgram(_program);

//	glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
//	glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);

//	glDrawArrays(GL_TRIANGLES, 0, 36);

//	DrawTest();

	drawChara(320, 360, 1, 2, 0, 0, 2000, 2000);
	drawChara(960, 360, 1, 3, 0, 0, 1000, 1000);
	drawChara(640, 360, 1, 1, 0, 0, 500, 500);

// 画面マスク描画

	GSLflatRectF(0.0f, -frontMaskY, 1280.0f, frontMaskY, 0, 255, 0);
	GSLflatRectF(0.0f, 720.0f, 1280.0f, frontMaskY, 0, 255, 0);


	nowTime = [NSDate timeIntervalSinceReferenceDate];
	drawTime = nowTime - oldTime;

	DrawFrameRate();
//	glFlush();
}



#pragma mark -  OpenGL ES 2 shader compilation
/*
	setupGL から呼ばれる
*/
- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    _program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname]) {
        NSLog(@"Failed to compile vertex shader");
        return NO;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname]) {
        NSLog(@"Failed to compile fragment shader");
        return NO;
    }
    
    // Attach vertex shader to program.
    glAttachShader(_program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(_program, GLKVertexAttribPosition, "position");
    glBindAttribLocation(_program, GLKVertexAttribNormal, "normal");
    
    // Link program.
    if (![self linkProgram:_program]) {
        NSLog(@"Failed to link program: %d", _program);
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (_program) {
            glDeleteProgram(_program);
            _program = 0;
        }
        
        return NO;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(_program, "modelViewProjectionMatrix");
    uniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(_program, "normalMatrix");
    
    // Release vertex and fragment shaders.
    if (vertShader) {
        glDetachShader(_program, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDetachShader(_program, fragShader);
        glDeleteShader(fragShader);
    }
    
    return YES;
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source) {
        NSLog(@"Failed to load vertex shader");
        return NO;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return NO;
    }
    
    return YES;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

@end
