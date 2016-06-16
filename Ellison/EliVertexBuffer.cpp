#include <Multistdafx.h>
#include "EliVertexBuffer.h"

CVertexBuffer::CVertexBuffer() :
	m_Num(-1),
	m_Type(VX_INVALID),
	m_ObjectID(NULL)
{ ; }

BOOL CVertexBuffer::Create(DWORD num, VERTEX_TYPE type) {
	Release();

	_RES res = CDirect3D::GetDevice()->CreateVertexBuffer( num * VERTEX_SIZE[type], D3DUSAGE_WRITEONLY, FVF_PARAM[type], D3DPOOL_MANAGED, &m_ObjectID, NULL);

	SIG_ERRORRETURNBOOL(res,TEXT("Failed@CVertexBuffer::Create"));

	m_Type = type;
	m_Num = num;
	return TRUE;
}

void CVertexBuffer::Release() {
	SafeRelease(m_ObjectID);
}

BOOL CVertexBuffer::SetData(void* pData, DWORD num) {
	void* pDest = NULL;
	HRESULT res = m_ObjectID->Lock( 0, 0, &pDest, 0);

	SIG_ERRORRETURNBOOL(res,TEXT("Failed@CVertexBuffer::SetData"));
	
	_MEMCPY(pDest, pData, num * VERTEX_SIZE[m_Type]);

	m_ObjectID->Unlock();

	return TRUE;
}
