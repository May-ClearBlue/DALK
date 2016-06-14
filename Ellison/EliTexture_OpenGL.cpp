#include "EliTexture.h"

CTexture::CTexture()
: m_ObjectID(0), m_Format(0), m_TextureWidth(-1), m_TextureHeight(-1) {
	;
}

int CTexture::GetFormat( int Depth ) {
//基本的に8bitαと32bitRGBA以外はサポートしない
// D3DFMT_P8
// D3DFMT_X1R5G5B5:
// D3DFMT_A1R5G5B5:
// D3DFMT_R5G6B5:
// D3DFMT_R8G8B8:
// D3DFMT_R8G8B8;//サポートしてるカードが少ない？
// D3DFMT_X8R8G8B8:

	switch( Depth ) {
	case 8:
		return GL_ALPHA;
		break;
	case 32:
		return GL_RGBA;
	}
	return -1;
}

BOOL CTexture::Create(UINT width,UINT height, int Format, D3DPOOL type, BOOL isRender) {
	int tw = 1, th=1;

	while(width > tw)
		{ tw *= 2; }
	while(height > th)
		{ th *= 2; }

	GLenum res;

	glGenTextures(1, &m_ObjectID);
	SIG_ERRORRETURNBOOL(res,"Failed:glGenTextures");

	glBindTexture(GL_TEXTURE_2D, m_ObjectID);
	SIG_ERRORRETURNBOOL(res,"Failed:glBindTexture");
	
#if 1
 	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#endif

	glTexImage2D( GL_TEXTURE_2D, 0, Format/*GL_RGBA*/, tw, th, 0, Format/*GL_RGBA*/, GL_UNSIGNED_BYTE, NULL );
	SIG_ERRORRETURNBOOL(res,"Failed:glTexImage2D");

	m_TextureWidth	= tw;
	m_TextureHeight	= th;
	m_Format		= Format;
//	m_IsRenderTarget= isRender;

	if(isRender) {
		m_Info.X		= width;
		m_Info.Y		= height;
		m_Info.Depth	= 32;
	}

	return TRUE;
}

int CTexture::Create() {
	if(Create(m_Info.X, m_Info.Y, GetFormat(m_Info.Depth)) == FALSE )
		return 1;

	GLenum hr;

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Info.X, m_Info.Y, GetFormat(m_Info.Depth), GL_UNSIGNED_BYTE, m_Info.pData);
	SIG_ERRORCHECK(hr,"Failed@glTexSubImage2D");

	//Src解放
	_VFREE(m_Info.pData);

	return 0;
}

void CTexture::Release() {
	if(m_ObjectID != 0) {
        GLenum hr;
        glBindTexture(GL_TEXTURE_2D, m_ObjectID);
        SIG_ERRORCHECK(hr,"Failed@glBindTexture");
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
        SIG_ERRORCHECK(hr,"Failed@glBindTexture");
		glDeleteTextures(1, &m_ObjectID);
        SIG_ERRORCHECK(hr,"Failed@glDeleteTexture");
        glBindTexture(GL_TEXTURE_2D, 0);
        m_ObjectID = 0;
    }
}

//ToDo
void CTexture::Blt( CTexture* pDest ) {
	GLenum hr;

	glCopyTextureLevelsAPPLE( pDest->GetTexture(), GetTexture(), 0, 1);
	SIG_ERRORCHECK(hr,"Failed@glGetIntegerv");
#if 0
	GLint oldFB, newFB;

	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &oldFB );
	glGenFramebuffers(1, &newFB);
	glBindFramebuffer(GL_FRAMEBUFFER, newFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target[0].name, tex, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, oldFB);
#endif
}

void CTexture::ColorFill ( D3DCOLOR Color, RECT* pRect ) {
	RECT rect = { 0 };

	if(pRect != NULL)
		rect = *pRect;
	else
		SetRect(&rect, 0, 0, m_Info.X, m_Info.Y);

	size_t size = (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1) * 4;

	D3DCOLOR* pBuf = new D3DCOLOR[size];

	for(size_t a = 0; a < size; a ++)
		pBuf[a] = Color;

	PushTexture();
	glTexSubImage2D(GL_TEXTURE_2D, 0, rect.left, rect.top, (rect.right - rect.left + 1), (rect.bottom - rect.top + 1), GL_RGBA, GL_UNSIGNED_BYTE, pBuf);
	PopTexture();

	delete[] pBuf;

}

#pragma message("ToDo:CTexture::GetRenderTargetData@iOS")
void CTexture::GetRenderTargetData() {
	GLenum hr;
	GLint	active_unit;

	glGetIntegerv( GL_ACTIVE_TEXTURE, &active_unit );
	SIG_ERRORCHECK(hr,"Failed@glGetIntegerv");
	glActiveTexture(GL_TEXTURE0 + 7);
	SIG_ERRORCHECK(hr,"Failed@glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, m_ObjectID);
	SIG_ERRORCHECK(hr,"Failed@glBindTexture");
	glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_Info.X, m_Info.Y, 0 );
	SIG_ERRORCHECK(hr,"Failed@glCopyTexImage2D");
	glBindTexture(GL_TEXTURE_2D, 0);
	SIG_ERRORCHECK(hr,"Failed@glBindTexture");
	glActiveTexture(active_unit);
	SIG_ERRORCHECK(hr,"Failed@glActiveTexture");
}

void CTexture::PushTexture(int Stage) {
	GLenum hr;
	glGetIntegerv( GL_ACTIVE_TEXTURE, &m_TextureStageHistory );
	SIG_ERRORCHECK(hr,"Failed@glGetIntegerv");
	glActiveTexture(GL_TEXTURE0 + Stage);
	SIG_ERRORCHECK(hr,"Failed@glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, m_ObjectID);
	SIG_ERRORCHECK(hr,"Failed@glBindTexture");
}

void CTexture::PopTexture() {
	GLenum hr;
	glBindTexture(GL_TEXTURE_2D, 0);
	SIG_ERRORCHECK(hr,"Failed@glBindTexture");
	glActiveTexture(m_TextureStageHistory);
	SIG_ERRORCHECK(hr,"Failed@glActiveTexture");
	m_TextureStageHistory = 0;
};

void CTexture::SetTexture(int Stage) {
	GLenum hr;

	glActiveTexture(GL_TEXTURE0 + Stage);
	SIG_ERRORCHECK(hr,"Failed@glActiveTexture");

	glBindTexture(GL_TEXTURE_2D, m_ObjectID);
	SIG_ERRORCHECK(hr,"Failed@glBindTexture");

//シェーダにユニットナンバーを渡してやる
//	glUniform1i(ShaderUni[shno][SH_UNI_TEX1], 0);
}

CRenderingBuffer::CRenderingBuffer()
{ ; }

BOOL CRenderingBuffer::SetRenderTarget(CTexture& tex, int Index) {
	CRenderingTargetInfo	OldInfo = { 0 };
	GLenum res;

	if(m_InfoList.size() <= 0) {
		// 以前のFBOを保存
		GLint temp;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &temp);
		SIG_ERRORRETURNBOOL(res,"Failed@CTexture::GetRenderTarget");
		
		//glGetIntegervはGLintだけどFrameBufferのIDは本来GLuint・・・
		OldInfo.m_RenderBuffer = temp;

		GLint viewport[4] = { 0 };
		glGetIntegerv(GL_VIEWPORT, viewport);
		SIG_ERRORRETURNBOOL(res,"Failed@CTexture::GetViewport");

		OldInfo.m_ViewPort.X		= viewport[0];
		OldInfo.m_ViewPort.Y		= viewport[1];
		OldInfo.m_ViewPort.Width	= viewport[2];
		OldInfo.m_ViewPort.Height	= viewport[3];
		OldInfo.m_ViewPort.MinZ	= 0.0f;
		OldInfo.m_ViewPort.MaxZ	= 1.0f;
	}
	else {
		OldInfo.m_RenderBuffer = m_Info.m_RenderBuffer;
		OldInfo.m_ViewPort = m_Info.m_ViewPort;
	}

	CRenderingTargetInfo	NewInfo = { 0 };

	ISurfaceInfo& iInfo = tex.GetInfo();

	NewInfo.m_ViewPort.X		= 0;
	NewInfo.m_ViewPort.Y		= 0;
	NewInfo.m_ViewPort.Width	= iInfo.X;
	NewInfo.m_ViewPort.Height	= iInfo.Y;
	NewInfo.m_ViewPort.MinZ	= 0.0f;
	NewInfo.m_ViewPort.MaxZ	= 1.0f;

	// FramebufferObjectを作成
	glGenFramebuffers(1, &NewInfo.m_RenderBuffer);
	SIG_ERRORRETURNBOOL(res,"glGenFramebuffers Failed@CTexture::SetRenderTarget");
	glBindFramebuffer(GL_FRAMEBUFFER, NewInfo.m_RenderBuffer);
	SIG_ERRORRETURNBOOL(res,"glBindFramebuffers Failed@CTexture::SetRenderTarget");
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.GetTexture(), 0);
	SIG_ERRORRETURNBOOL(res,"glFramebufferTecture2D Failed@CTexture::SetRenderTarget");
#if 0
	// 深度バッファを作成
    glGenRenderbuffers(1, &offscreenDepth);
    glBindRenderbuffer(GL_RENDERBUFFER_OES, offscreenDepth);
    glRenderbufferStorage(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, 256, 256);
    glBindRenderbuffer(GL_RENDERBUFFER_OES, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, offscreenDepth);
#endif

	// FramebufferObjectの有効性チェック
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"FBOの作成に失敗しました : %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }

	glViewport(NewInfo.m_ViewPort.X, NewInfo.m_ViewPort.Y, NewInfo.m_ViewPort.Width, NewInfo.m_ViewPort.Height);
	SIG_ERRORRETURNBOOL(res,"Failed@CD3DDevice::SetViewPort");

	m_Info = NewInfo;

	m_InfoList.push_back(OldInfo);

	return TRUE;
}

BOOL CRenderingBuffer::ResetRenderTarget( int Index ) {
	if(m_InfoList.size() <= 0)
		return FALSE;

	_RES res;

	CRenderingTargetInfo& Info = m_InfoList.back();
	glBindFramebuffer(GL_FRAMEBUFFER, Info.m_RenderBuffer);
	SIG_ERRORRETURNBOOL(res,"Failed@CTexture::SetRenderTarget");

	glViewport(Info.m_ViewPort.X, Info.m_ViewPort.Y, Info.m_ViewPort.Width, Info.m_ViewPort.Height);
	SIG_ERRORRETURNBOOL(res,"Failed@CD3DDevice::SetViewPort");

	glDeleteFramebuffers(1, &m_Info.m_RenderBuffer);
	SIG_ERRORRETURNBOOL(res,"Failed@CTexture::glDeleteFramebuffers");

	m_Info.m_RenderBuffer = Info.m_RenderBuffer;
	m_Info.m_ViewPort = Info.m_ViewPort;
	m_InfoList.pop_back();

	return TRUE;
}

