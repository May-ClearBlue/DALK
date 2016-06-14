#ifndef _DALK_APPMAIN_
#define _DALK_APPMAIN_

#ifndef _DALK_APP_SCREEN_WIDTH_
#define _DALK_APP_SCREEN_WIDTH_ 1280
#endif
#ifndef _DALK_APP_SCREEN_HEIGHT_
#define _DALK_APP_SCREEN_HEIGHT_ 720
#endif
#ifndef _DALK_APP_SCREEN_TITLE_
#define _DALK_APP_SCREEN_TITLE_ _TEXT("-Dalk-")
#endif

#if defined(_DALK_WINDOWS_)
 #define _APP_MUTEXNAME_ "0C4F8A23-8D30-4226-87A6-2CB4FD604192"

#include <MultiStdAfx.h>

 #include "Rimlia/RimThread.h"
 #include "Rimlia/RimWindow.h"
#else
 #include "Ellison/EliShader.h"
#endif
#include "Rimlia/RimTimer.h"
#include "Rimlia/RimSysInfo.h"
#include "Siglune/SigDirect3DDevice.h"
#include "Ellison/EliSprite.h"

#include "Rimlia/RimSoundDecoder.h"
#include "Rinse/RinseAudioSource.h"

#include "Rimlia/bmfont.h"

#include "Rimlia/RimImageSaver.h"

#define _SQUIRREL_ENTRY_SCRIPT_ "Start.nut"

#ifdef _DALK_USE_OGG_
#pragma	comment( lib, "libogg_static.lib" )
#pragma	comment( lib, "libvorbis_static.lib" )
#pragma	comment( lib, "libvorbisfile_static.lib" )
#endif

#pragma	comment(lib, "Rimlia.lib")
#pragma comment( lib, "Siglune.lib")
#pragma comment( lib, "Rinse.lib")
#pragma comment( lib, "Ellison.lib")

class Dalk
#if defined(_DALK_WINDOWS_)
	: public CWindow//, public CThread
#endif
{
public:
	Dalk()
		: m_State(-1)
#if defined(_DALK_USE_SQUIRREL_)
		, m_hSquirrel(NULL)
#endif
	{ };

protected:
	CD3DDevice	m_Device;

	CRenderingBuffer	m_RenderingBuffer;
	CTexture	m_RenderTexture;
	CSprite2D	m_RenderSprite;

	CXAudioSource	m_Voice;
//	CSpriteBMFont	m_Font;
	CTexture	m_Texture;
	CSprite2D	m_Sprite;

	volatile int	m_State;

#if defined(_DALK_USE_SQUIRREL_)
	HSQUIRRELVM	 m_hSquirrel;
#endif

#if defined(_DALK_USE_SQUIRREL_)
	void	InitSquirrel(int stackSize = 1024);
	void	RunSquirrel(_TCHAR fileName);
#endif

public:
	void	Init(int width, int height);
	void	ShutDown();
	void	Update();
	void	Draw();
	void	Run();
};

#endif
