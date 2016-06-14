/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  File	:	matrix.h
 *  Contents:	
 *	Release	:	2012/10/012		Nozomi		version 1.00
 *	UpDate	:
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




/*
�^�p���͈ȉ��̃}�g���N�X�Ƃ��Ĉ���
|_11 _12 _13 _14|
|_21 _22 _23 _24|
|_31 _32 _33 _34|
|_41 _42 _43 _44|
*/
typedef struct tagMATRIX4
{
	union
	{
		struct
		{
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};
		float	m[16];
	};
} MATRIX4;


typedef struct tagVECTOR4
{
	union
	{
		struct
		{
			float	_1, _2, _3, _4;
		};
		struct
		{
			float	x, y, z, w;
		};
		struct
		{
			float	r, g, b, a;
		};
		float	v[4];
	};
} VECTOR4;




float fSin(float rad);
float fCos(float rad);

/*
�P�ʍs��𐶐�
*/
void MatrixIdentity(MATRIX4 *mat);

/*
�Q�D�}�g���b�N�X���m�̐�
*/
void MatrixCalcMxM(MATRIX4 *dmat, MATRIX4 *smat1, MATRIX4 *smat2);

/*
�P�D�x�N�g���ƃ}�g���b�N�X�̐�
*/
void MatrixCalcVxM(VECTOR4 *dvec, VECTOR4 *svec, MATRIX4 *smat);
void MatrixCalcVxM2(VECTOR4 *dvec, VECTOR4 *svec, MATRIX4 *smat);
void MatrixCalcVxM3(VECTOR4 *dvec, VECTOR4 *svec, MATRIX4 *smat);

void MatrixCopyM(MATRIX4 *dmat, MATRIX4 *smat);
void MatrixCopyV(VECTOR4 *dvec, VECTOR4 *svec);

/*
�����s�ړ�
*/
void MatrixSetMove(MATRIX4 *dmat, float x, float y, float z);


/*
����]�s��
*/
void MatrixSetRotateX(MATRIX4 *dmat, float rad);

/*
*/
void MatrixSetRotateY(MATRIX4 *dmat, float rad);

/*
�R�D�y���ŉ�]
*/
void MatrixSetRotateZ(MATRIX4 *dmat, float rad);

/*
�R����]	Z->X->Y
*/
void MatrixRotationYawPitchRoll(MATRIX4 *dmat, float x, float y, float z);

/*
���X�P�[�����O
*/
void MatrixStretch(MATRIX4 *dmat, float sizex, float sizey, float sizez);





