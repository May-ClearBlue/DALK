#ifndef _SIG_DIRECT3D_
#define _SIG_DIRECT3D_

#include "SigDirect3DInit.h"

class CD3DDevice;

#if defined(_DALK_USE_DIRECT3D_)
//ToDo:マルチディスプレイ対応
class CDirect3D {
private:
static	LPDIRECT3D9			m_pDirect3D;
static	LPDIRECT3DDEVICE9	m_pDirect3DDevice;
static	CD3DDevice*			m_pDevice;
public:
static	void				SetDevice(LPDIRECT3DDEVICE9 pDevice) { m_pDirect3DDevice = pDevice; }
static	void				SetDeviceClass(CD3DDevice* pDevice) { m_pDevice = pDevice; }
static	BOOL				Initialize()	{ return (m_pDirect3D = ::Direct3DCreate9(D3D_SDK_VERSION)) ? TRUE : FALSE; }
static	LPDIRECT3D9& 		GetInstance()	{ return m_pDirect3D; }
static	LPDIRECT3DDEVICE9	GetDevice()		{ return m_pDirect3DDevice; }
static	CD3DDevice*			GetDeviceClass(){ return m_pDevice; }
static	void				Release()		{ SafeRelease(m_pDirect3D); }
};
#elif defined(_DALK_USE_OPENGL_)
class COpenGL {
private:
//static	GLuint	g_FrontDepthBuffer;
static	GLuint	g_FrontRenderBuffer;
static	GLuint	g_FrontFrameBuffer;
static	CD3DDevice*			m_pDevice;
public:

static	BOOL	PreInitialize()	{
//OpenGLのコンテキスト生成前に呼ぶ
		GLenum hr;
		glGenRenderbuffers(1, &g_FrontRenderBuffer);
		SIG_ERRORRETURNBOOL(hr,"glGenRenderbuffers@COpenGL::Initialize")
		glBindRenderbuffer(GL_RENDERBUFFER, g_FrontRenderBuffer);
		SIG_ERRORRETURNBOOL(hr,"glBindRenderbuffers@COpenGL::Initialize")
		return TRUE;
	}
static	BOOL	AfterInitialize(BOOL isUseDepthBuffer = FALSE)	{
		GLenum hr;
		glGenFramebuffers(1, &g_FrontFrameBuffer);
		SIG_ERRORRETURNBOOL(hr,"glGenFramebuffers@COpenGL::Initialize")
		glBindFramebuffer(GL_FRAMEBUFFER, g_FrontFrameBuffer);
		SIG_ERRORRETURNBOOL(hr,"glBindFramebuffers@COpenGL::Initialize")

#if 0
		if(isUseDepthBuffer)
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_FrontDepthbuffer);
#endif
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, g_FrontRenderBuffer);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return FALSE;

		return TRUE;
	}
static	CD3DDevice*		GetDeviceClass()					{ return m_pDevice; }
static	void			SetDeviceClass(CD3DDevice* pDevice) { m_pDevice = pDevice; }
static	const GLuint	GetRenderBuffer()		{ return g_FrontRenderBuffer; }
static	const GLuint	GetFrameBuffer()		{ return g_FrontRenderBuffer; }
static		void		Release()	{ ; }

};
#endif

namespace Siglune {
inline 	_RES ResetTexture(int Stage=0) {
#if defined(_DALK_USE_DIRECT3D_)
return CDirect3D::GetDevice()->SetTexture(Stage, NULL);
#elif defined(_DALK_USE_OPENGL_)
	GLenum hr;

	glActiveTexture(GL_TEXTURE0+Stage);
	SIG_ERRORCHECK(hr,"glActiveTexture");

	glBindTexture(GL_TEXTURE_2D, 0);
	SIG_ERRORCHECK(hr,"glBindTexture");
	return hr;
#endif
}

};	//Siglune

#endif
