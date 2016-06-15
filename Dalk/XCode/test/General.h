/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  File	:	General.c
 *  Contents:	���ʒ萔�w�b�_
 *	Release	:	1997/07/01 		Nozomi		version 3.00
 *	UpDate	:	
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_INC_GENERAL_
#define	_INC_GENERAL_

/*--------------------------------------------------------------------------*/
/*	�f�[�^�^�錾															*/
/*--------------------------------------------------------------------------*/

//typedef				int		BOOL;	/* �_���^�i�_���萔��l�ɂƂ�j		*/
//typedef	unsigned	int		UINT;	/* �����Ȃ�INT�^�i�c�[���p�j		*/
//typedef	signed		int		SINT;	/* ������INT�^�i�c�[���p�j		*/
typedef	unsigned	char	UINT8;	/* �����Ȃ��P�o�C�g����				*/
typedef	signed		char	SINT8;	/* �������P�o�C�g����				*/
typedef	unsigned	short	UINT16;	/* �����Ȃ��Q�o�C�g����				*/
typedef	signed		short	SINT16;	/* �������Q�o�C�g����				*/
typedef	unsigned	long	UINT32;	/* �����Ȃ��S�o�C�g����				*/
typedef	signed		long	SINT32;	/* �������S�o�C�g����				*/
//typedef double		SINT64;

#ifndef	NULL
#define	NULL	((void *)0)			/* NULL ��`						*/
#endif
#define	FALSE	0	/* �_���萔�i�U�j	*/
#define	TRUE	1	/* �_���萔�i�^�j	*/

#define LF 0x0a
#define CR 0x0d
#define TAB 0x09

#define	readWordData(p)		((*(p+1) << 8) + *(p))
#define	readDWordData(p)		((*(p+3) << 24) + (*(p+2) << 16) + (*(p+1) << 8) + *(p))

//#define	copyMem(a, b, c)	CopyMemory(a, b, c)
//#define	fillMem(a, b, c)	FillMemory(a, b, c)
//#define	zeroMem(a, b)	ZeroMemory(a, b)
//#define		allocMem(a)		HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, a)
//#define		freeMem(a)		if(a)	HeapFree(GetProcessHeap(), 0, a), a = NULL

#define	copyMem(a, b, c)	memcpy(a, b, c)
#define	fillMem(a, b, c)	memset(a, c, b)
#define	zeroMem(a, b)	memset(a, 0, b)

#define		allocMem(a)		malloc(a)
#define		freeMem(a)		if(a)	free(a), a = NULL





#ifdef _WINDOWS
#ifdef __cplusplus
#define		Release(a)	if(a) (a)->Release(), a = NULL
#else
#define		Release(a)	if(a) (a)->lpVtbl->Release(a), a = NULL
#endif //__cplusplus
#endif



#endif
