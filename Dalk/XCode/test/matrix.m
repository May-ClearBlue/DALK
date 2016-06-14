/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  File	:	matrix.m
 *  Contents:	
 *	Release	:	2012/10/012		Nozomi		version 1.00
 *	UpDate	:
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include	"General.h"
#include	"matrix.h"


float	sinbuf[50][2];
UINT32	sinbufnum = 0;
float	cosbuf[50][2];
UINT32	cosbufnum = 0;


float fSin(float rad)
{
	UINT32	i, j;
	float	a;

	for(i=0 ; i<sinbufnum ; i++)
	{
		if(sinbuf[i][0] == rad) break;
	}
	if(i<sinbufnum)
	{
		a = sinbuf[i][1];
	}
	else
	{
		a = sinf(rad);
		sinbufnum ++;
		if(sinbufnum > 50) sinbufnum = 50;
		i = sinbufnum - 1;
	}

	for( ; i>0 ; i--)
	{
		sinbuf[i][0] = sinbuf[i - 1][0];
		sinbuf[i][1] = sinbuf[i - 1][1];
	}
	sinbuf[0][0] = rad;
	sinbuf[0][1] = a;

	return a;
}
float fCos(float rad)
{
	UINT32	i, j;
	float	a;

	for(i=0 ; i<cosbufnum ; i++)
	{
		if(cosbuf[i][0] == rad) break;
	}
	if(i<cosbufnum)
	{
		a = cosbuf[i][1];
	}
	else
	{
		a = cosf(rad);
		cosbufnum ++;
		if(cosbufnum > 50) cosbufnum = 50;
		i = cosbufnum - 1;
	}

	for( ; i>0 ; i--)
	{
		cosbuf[i][0] = cosbuf[i - 1][0];
		cosbuf[i][1] = cosbuf[i - 1][1];
	}
	cosbuf[0][0] = rad;
	cosbuf[0][1] = a;

	return a;
}


/*
�^�p���͈ȉ��̃}�g���N�X�Ƃ��Ĉ���
|_11 _12 _13 _14|
|_21 _22 _23 _24|
|_31 _32 _33 _34|
|_41 _42 _43 _44|
*/

/*
�P�ʍs��𐶐�
*/
void MatrixIdentity(MATRIX4 *mat)
{
	zeroMem(&mat->m[0], sizeof(MATRIX4));

	mat->_11 = 1.0f;
	mat->_22 = 1.0f;
	mat->_33 = 1.0f;
	mat->_44 = 1.0f;
}


/*
�Q�D�}�g���b�N�X���m�̐�
| a b c d |�@| A B C D |�@| aA+bE+cI+dM aB+bF+cJ+dN aC+bG+cK+dO aD+bH+cL+dP |
| e f g h |�~| E F G H |��| eA+fE+gI+hM eB+fF+gJ+hN eC+fG+gK+hO eD+fH+gL+hP |
| i j k l |�@| I J K L |�@| iA+jE+kI+lM iB+jF+kJ+lN iC+jG+kK+lO iD+jH+kL+lP |
| m n o p |�@| M N O P |�@| mA+nE+oI+pM mB+nF+oJ+pN mC+mG+oK+pO mD+nH+oL+pP |
*/
void MatrixCalcMxM(MATRIX4 *dmat, MATRIX4 *smat1, MATRIX4 *smat2)
{
	MATRIX4		w;

	w._11 = smat1->_11 * smat2->_11 + smat1->_12 * smat2->_21 + smat1->_13 * smat2->_31 + smat1->_14 * smat2->_41;
	w._21 = smat1->_21 * smat2->_11 + smat1->_22 * smat2->_21 + smat1->_23 * smat2->_31 + smat1->_24 * smat2->_41;
	w._31 = smat1->_31 * smat2->_11 + smat1->_32 * smat2->_21 + smat1->_33 * smat2->_31 + smat1->_34 * smat2->_41;
	w._41 = smat1->_41 * smat2->_11 + smat1->_42 * smat2->_21 + smat1->_43 * smat2->_31 + smat1->_44 * smat2->_41;

	w._12 = smat1->_11 * smat2->_12 + smat1->_12 * smat2->_22 + smat1->_13 * smat2->_32 + smat1->_14 * smat2->_42;
	w._22 = smat1->_21 * smat2->_12 + smat1->_22 * smat2->_22 + smat1->_23 * smat2->_32 + smat1->_24 * smat2->_42;
	w._32 = smat1->_31 * smat2->_12 + smat1->_32 * smat2->_22 + smat1->_33 * smat2->_32 + smat1->_34 * smat2->_42;
	w._42 = smat1->_41 * smat2->_12 + smat1->_42 * smat2->_22 + smat1->_43 * smat2->_32 + smat1->_44 * smat2->_42;

	w._13 = smat1->_11 * smat2->_13 + smat1->_12 * smat2->_23 + smat1->_13 * smat2->_33 + smat1->_14 * smat2->_43;
	w._23 = smat1->_21 * smat2->_13 + smat1->_22 * smat2->_23 + smat1->_23 * smat2->_33 + smat1->_24 * smat2->_43;
	w._33 = smat1->_31 * smat2->_13 + smat1->_32 * smat2->_23 + smat1->_33 * smat2->_33 + smat1->_34 * smat2->_43;
	w._43 = smat1->_41 * smat2->_13 + smat1->_42 * smat2->_23 + smat1->_43 * smat2->_33 + smat1->_44 * smat2->_43;

	w._14 = smat1->_11 * smat2->_14 + smat1->_12 * smat2->_24 + smat1->_13 * smat2->_34 + smat1->_14 * smat2->_44;
	w._24 = smat1->_21 * smat2->_14 + smat1->_22 * smat2->_24 + smat1->_23 * smat2->_34 + smat1->_24 * smat2->_44;
	w._34 = smat1->_31 * smat2->_14 + smat1->_32 * smat2->_24 + smat1->_33 * smat2->_34 + smat1->_34 * smat2->_44;
	w._44 = smat1->_41 * smat2->_14 + smat1->_42 * smat2->_24 + smat1->_43 * smat2->_34 + smat1->_44 * smat2->_44;

	copyMem(&dmat->m[0], &w.m[0], sizeof(MATRIX4));
}

/*
�P�D�x�N�g���ƃ}�g���b�N�X�̐�
           �@| A B C D |�@| Ax + Ey + Iz + M |
| x y z 1 |�~| E F G H |��| Bx + Fy + Jz + N |
           �@| I J K L |�@| Cx + Gy + Kz + O |
           �@| M N O P |�@| Dx + Hy + Lz + P |
*/
void MatrixCalcVxM(VECTOR4 *dvec, VECTOR4 *svec, MATRIX4 *smat)
{
	VECTOR4		w;

	w._1 = svec->_1 * smat->_11 + svec->_2 * smat->_21 + svec->_3 * smat->_31 + svec->_4 * smat->_41;
	w._2 = svec->_1 * smat->_12 + svec->_2 * smat->_22 + svec->_3 * smat->_32 + svec->_4 * smat->_42;
	w._3 = svec->_1 * smat->_13 + svec->_2 * smat->_23 + svec->_3 * smat->_33 + svec->_4 * smat->_43;
	w._4 = svec->_1 * smat->_14 + svec->_2 * smat->_24 + svec->_3 * smat->_34 + svec->_4 * smat->_44;

	copyMem(&dvec->v[0], &w.v[0], sizeof(VECTOR4));
}

/*
�P�D�x�N�g���ƃ}�g���b�N�X�̐�
         �@| A B C |�@| Ax + Ey + Iz |
| x y z |�~| E F G |��| Bx + Fy + Jz |
         �@| I J K |�@| Cx + Gy + Kz |
*/
void MatrixCalcVxM3(VECTOR4 *dvec, VECTOR4 *svec, MATRIX4 *smat)
{
	VECTOR4		w;

	w._1 = svec->_1 * smat->_11 + svec->_2 * smat->_21 + svec->_3 * smat->_31;
	w._2 = svec->_1 * smat->_12 + svec->_2 * smat->_22 + svec->_3 * smat->_32;
	w._3 = svec->_1 * smat->_13 + svec->_2 * smat->_23 + svec->_3 * smat->_33;
	w._4 = 0;

	copyMem(&dvec->v[0], &w.v[0], sizeof(VECTOR4));
}


/*
�P�D�x�N�g���ƃ}�g���b�N�X�̐�
       �@| A B |�@| Ax + Ey |
| x y |�~| E F |��| Bx + Fy |
*/
void MatrixCalcVxM2(VECTOR4 *dvec, VECTOR4 *svec, MATRIX4 *smat)
{
	VECTOR4		w;

	w._1 = svec->_1 * smat->_11 + svec->_2 * smat->_21;
	w._2 = svec->_1 * smat->_12 + svec->_2 * smat->_22;
	w._3 = 0;
	w._4 = 0;

	copyMem(&dvec->v[0], &w.v[0], sizeof(VECTOR4));
}


void MatrixCopyM(MATRIX4 *dmat, MATRIX4 *smat)
{
	copyMem(dmat, smat, sizeof(MATRIX4));
}
void MatrixCopyV(VECTOR4 *dvec, VECTOR4 *svec)
{
	copyMem(dvec, svec, sizeof(VECTOR4));
}

/*
�����s�ړ�
  (x, y, z)����(x + Tx, y + Ty, z + Tz)�ւ̕��s�ړ����s���s��B

|  1  0  0  0 |
|  0  1  0  0 |
|  0  0  1  0 |
| Tx Ty Tz  1 |
*/
void MatrixSetMove(MATRIX4 *dmat, float x, float y, float z)
{
	MatrixIdentity(dmat);

	dmat->_41 = x;
	dmat->_42 = y;
	dmat->_43 = z;
}


/*
����]�s��
�@��]�ɂ��Ă͌��_�𒆐S�Ƃ��ē_�̉�]�ŉ���ς݁B

�P�D�w���ŉ�]
|     1     0     0     0 |
|     0  cosX  sinX     0 |
|     0 -sinX  cosX     0 |
|     0     0     0     1 |
*/
void MatrixSetRotateX(MATRIX4 *dmat, float rad)
{
	float	fs, fc;

	MatrixIdentity(dmat);

	fs = fSin(rad);
	fc = fCos(rad);

	dmat->_22 = fc;
	dmat->_33 = fc;
	dmat->_32 = -fs;
	dmat->_23 = fs;
}

/*
�Q�D�x���ŉ�]
|  cosY     0 -sinY     0 |
|     0     1     0     0 |
|  sinY     0  cosY     0 |
|     0     0     0     1 |
*/
void MatrixSetRotateY(MATRIX4 *dmat, float rad)
{
	float	fs, fc;

	MatrixIdentity(dmat);

	fs = fSin(rad);
	fc = fCos(rad);

	dmat->_11 = fc;
	dmat->_33 = fc;
	dmat->_31 = fs;
	dmat->_13 = -fs;
}

/*
�R�D�y���ŉ�]
|  cosZ  sinZ     0     0 |
| -sinZ  cosZ     0     0 |
|     0     0     1     0 |
|     0     0     0     1 |
*/
void MatrixSetRotateZ(MATRIX4 *dmat, float rad)
{
	float	fs, fc;

	MatrixIdentity(dmat);

	fs = fSin(rad);
	fc = fCos(rad);

	dmat->_11 = fc;
	dmat->_22 = fc;
	dmat->_21 = -fs;
	dmat->_12 = fs;
}

/*
�R����]
	Z->X->Y
*/
void MatrixRotationYawPitchRoll(MATRIX4 *dmat, float yaw, float pitch, float roll)
{
	MATRIX4		mx;	// pitch
	MATRIX4		my;	// yaw
	MATRIX4		mz;	// roll

	MatrixSetRotateX(&mx, pitch);
	MatrixSetRotateY(&my, yaw);
	MatrixSetRotateZ(&mz, roll);

	MatrixCalcMxM(dmat, &mz, &mx);
	MatrixCalcMxM(dmat, dmat, &my);
}

/*
���X�P�[�����O
�@(Sx, Sy, Sz)�̔{���Ŋg��E�k������s��B

| Sx  0  0  0 |
|  0 Sy  0  0 |
|  0  0 Sz  0 |
|  0  0  0  1 |
*/
void MatrixStretch(MATRIX4 *dmat, float sizex, float sizey, float sizez)
{
	MatrixIdentity(dmat);

	dmat->_11 = sizex;
	dmat->_22 = sizey;
	dmat->_33 = sizez;
}


/*
�������ϊ��s��
�@���̍s��̓������ɂ��č���͉�����Ȃ��B

�����̂P
| cot(FovW / 2)             0                   0 0 |
|             0 cot(FovH / 2)                   0 0 |
|             0             0      Zf / (Zf - Zn) 1 |
|             0             0 -Zn(Zf / (Zf - Zn)) 0 |
��fov(Field of view)�͎���p�̂���

FovW�F���������̎���p
FovH�F���������̎���p
Zn  �F���_����t�����g�N���b�s���O�v���[���܂ł̋���
Zf  �F���_����o�b�N�N���b�s���O�v���[���܂ł̋���


�����̂Q
|      2n / (r - l)                 0                  0  0 |
|                 0      2n / (t - b)                  0  0 |
| (r + l) / (r - l) (t + b) / (t - b) -(f + n) / (f - n) -1 |
|                 0                 0     -2fn / (f - n)  0 |

l:left  (NEAR�ʒu)
t:top   (NEAR�ʒu)
r:right (NEAR�ʒu)
b:bottom(NEAR�ʒu)
n:near
f:far
*/






/*


���C�ӎ���]�̍s��
�@�C�Ӄx�N�g��R(Rx, Ry, Rz)�����Ƃ���T��]����s��B
�@���̍s��̓������ɂ��č���͉�����Ȃ��B�i�����ꑼ�̃y�[�W�ɂĥ���j

|    cosT+(1-cosT)Rx2 (1-cosT)RxRy - RzsinT (1-cosT)RzRx+RysinT 0 |
| (1-cosT)RxRy+RzsinT      cosT+(1-cosT)Ry2 (1-cosT)RyRz-RxsinT 0 |
| (1-cosT)RzRx+RysinT   (1-cosT)RyRz+RxsinT    cosT+(1-cosT)Rz2 0 |
|                   0                     0                   0 1 |


�������ϊ��s��
�@���̍s��̓������ɂ��č���͉�����Ȃ��B

�����̂P
| cot(FovW / 2)             0                   0 0 |
|             0 cot(FovH / 2)                   0 0 |
|             0             0      Zf / (Zf - Zn) 1 |
|             0             0 -Zn(Zf / (Zf - Zn)) 0 |
��fov(Field of view)�͎���p�̂���

FovW�F���������̎���p
FovH�F���������̎���p
Zn  �F���_����t�����g�N���b�s���O�v���[���܂ł̋���
Zf  �F���_����o�b�N�N���b�s���O�v���[���܂ł̋���


�����̂Q
|      2n / (r - l)                 0                  0  0 |
|                 0      2n / (t - b)                  0  0 |
| (r + l) / (r - l) (t + b) / (t - b) -(f + n) / (f - n) -1 |
|                 0                 0     -2fn / (f - n)  0 |

l:left  (NEAR�ʒu)
t:top   (NEAR�ʒu)
r:right (NEAR�ʒu)
b:bottom(NEAR�ʒu)
n:near
f:far

��ʓI��FAR�ʒu�ł̋�`��(�}1, �}1)�ɐݒ肷��
----------------*/







