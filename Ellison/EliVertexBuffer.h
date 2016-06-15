#include "Siglune/SigDirect3D.h"
#include "EliVertexTypeDef.h"

#ifndef _DALK_WINDOWS_
#include "EliShader.h"
#endif

//頂点バッファ
class CVertexBuffer {
public:
	CVertexBuffer();
	~CVertexBuffer(){ Release(); }

protected:
	VERTEX_TYPE		m_Type;
	DWORD			m_Num;
	_VERTEXBUFFER	m_ObjectID;

	void	SetFormat();
public:
//	void	Init();
	BOOL	Create(DWORD num, VERTEX_TYPE type);
	void	Release();
	BOOL	SetData(void* pData, DWORD num);
#if defined(_DALK_WINDOWS_)
inline	_RES	SetStream(int num = 0)	{ return CDirect3D::GetDevice()->SetStreamSource( 0, m_ObjectID, 0, VERTEX_SIZE[m_Type] ); }
#elif defined(_DALK_IOS_)
		void	SetStream(int num = 0);		//OpenGLESだとパイプラインの指定はできないので引数は使わない
#endif
};
