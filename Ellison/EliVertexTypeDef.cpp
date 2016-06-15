#include "Multistdafx.h"
#include "EliVertexSquareTLV.h"
#if 0
//美しくない・・・設計見直すかも。
void CVertexSquareTLV::DrawPrimitiveUp() {
	_RES r;
#if defined(_DALK_WINDOWS_)
	r = CDirect3D::GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, _Vertex, sizeof(TLVERTEX) );
	if(FAILED(r))
		_DEBUGPRINTF("FAILED@CVertexSquareTLV::DrawPrimitiveUp");
#elif defined(_DALK_IOS_)
//	SetPrimitiveUp();
    EliVertex::SetVertexPointer(GetVertexType(), _Vertex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
    SIG_ERRORCHECK(r,"Failed@CVertexSquareTLV::DrawPrimitiveUp()");
}


void CVertexSquareTLV2::DrawPrimitiveUp() {
	_RES r;
#if defined(_DALK_WINDOWS_)
	r = CDirect3D::GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, _Vertex, sizeof(TLVERTEX2) );
#elif defined(_DALK_IOS_)
//	SetPrimitiveUp();
    EliVertex::SetVertexPointer(GetVertexType(), _Vertex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
    SIG_ERRORCHECK(r,"Failed@CVertexSquareTLV2::DrawPrimitiveUp()");
}
#endif