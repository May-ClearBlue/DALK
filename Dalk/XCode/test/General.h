/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  File	:	General.c
 *  Contents:	共通定数ヘッダ
 *	Release	:	1997/07/01 		Nozomi		version 3.00
 *	UpDate	:	
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_INC_GENERAL_
#define	_INC_GENERAL_

/*--------------------------------------------------------------------------*/
/*	データ型宣言															*/
/*--------------------------------------------------------------------------*/

//typedef				int		BOOL;	/* 論理型（論理定数を値にとる）		*/
//typedef	unsigned	int		UINT;	/* 符号なしINT型（ツール用）		*/
//typedef	signed		int		SINT;	/* 符号つきINT型（ツール用）		*/
typedef	unsigned	char	UINT8;	/* 符号なし１バイト整数				*/
typedef	signed		char	SINT8;	/* 符号つき１バイト整数				*/
typedef	unsigned	short	UINT16;	/* 符号なし２バイト整数				*/
typedef	signed		short	SINT16;	/* 符号つき２バイト整数				*/
typedef	unsigned	long	UINT32;	/* 符号なし４バイト整数				*/
typedef	signed		long	SINT32;	/* 符号つき４バイト整数				*/
//typedef double		SINT64;

#ifndef	NULL
#define	NULL	((void *)0)			/* NULL 定義						*/
#endif
#define	FALSE	0	/* 論理定数（偽）	*/
#define	TRUE	1	/* 論理定数（真）	*/

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
