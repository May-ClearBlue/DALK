#ifndef _ELI_VERTEXTLV_
#define _ELI_VERTEXTLV_

#include "Siglune/SigDirect3DDevice.h"
#include "Ellison/EliVertexTypeDef.h"

//TLVertexを使ったテンプレ
class CVertexSquareTLV : public CVertexSquare<TLVERTEX, VX_PTL0>  {
public:
CVertexSquareTLV(){;}
//(左上－右上－左下－右下）//右回り：D3D D3DCULL_CCW：Ｚ字)
//(左上－左下－右上－右下）//左回り：GL D3DCULL_CW)：逆Ｎ字
//※OpenGLとDirectXでテクスチャのY軸は逆
//D3D:上が0、下がが1.0
//GL:下が0、上が1.0
//UV範囲指定(0.0f-1.0f)
inline	void SetUV(float left, float top, float right, float bottom) {
#if defined(_DALK_WINDOWS_)
		_Vertex[0].u = left;	_Vertex[0].v = top;
		_Vertex[1].u = right;	_Vertex[1].v = top;
		_Vertex[2].u = left;	_Vertex[2].v = bottom;
		_Vertex[3].u = right;	_Vertex[3].v = bottom;   
#elif defined(_DALK_IOS_)
		_Vertex[0].u = left;	_Vertex[0].v = /*1.0f - */top;
		_Vertex[1].u = left;	_Vertex[1].v = /*1.0f - */bottom;
		_Vertex[2].u = right;	_Vertex[2].v = /*1.0f - */top;
		_Vertex[3].u = right;	_Vertex[3].v = /*1.0f - */bottom;
#endif
	}
inline void SetUV(int left, int top, int right, int bottom, int tw, int th) { SetUV((float)left / (float)tw, (float)top / (float)th, (float)right / (float)tw, (float)bottom / (float)th); }
};

class CVertexSquareTLV2 : public CVertexSquare<TLVERTEX2, VX_PTL1>   {
public:
CVertexSquareTLV2(){;}

//(左上－右上－左下－右下）//右回り：D3D D3DCULL_CCW：Ｚ字)
//(左上－左下－右上－右下）//左回り：GL D3DCULL_CW)：逆Ｎ字
//※OpenGLとDirectXでテクスチャのY軸は逆
//D3D:上が0、下がが1.0
//GL:下が0、上が1.0
//UV範囲指定(0.0f-1.0f)
inline	void SetUV(float left, float top, float right, float bottom) {
#if defined(_DALK_WINDOWS_)
		_Vertex[0].u = left;	_Vertex[0].v = top;
		_Vertex[1].u = right;	_Vertex[1].v = top;
		_Vertex[2].u = left;	_Vertex[2].v = bottom;
		_Vertex[3].u = right;	_Vertex[3].v = bottom;   
#elif defined(_DALK_IOS_)
		_Vertex[0].u = left;	_Vertex[0].v = /*1.0f - */top;
		_Vertex[1].u = left;	_Vertex[1].v = /*1.0f - */bottom;
		_Vertex[2].u = right;	_Vertex[2].v = /*1.0f - */top;
		_Vertex[3].u = right;	_Vertex[3].v = /*1.0f - */bottom;
#endif
	}

inline	void SetUV2(float left, float top, float right, float bottom) {
#if defined(_DALK_WINDOWS_)
		_Vertex[0].u2 = left;	_Vertex[0].v2 = top;
		_Vertex[1].u2 = right;	_Vertex[1].v2 = top;
		_Vertex[2].u2 = left;	_Vertex[2].v2 = bottom;
		_Vertex[3].u2 = right;	_Vertex[3].v2 = bottom;   
#elif defined(_DALK_IOS_)
		_Vertex[0].u2 = left;	_Vertex[0].v2 = /*1.0f - */top;
		_Vertex[1].u2 = left;	_Vertex[1].v2 = /*1.0f - */bottom;
		_Vertex[2].u2 = right;	_Vertex[2].v2 = /*1.0f - */top;
		_Vertex[3].u2 = right;	_Vertex[3].v2 = /*1.0f - */bottom;
#endif
	}

inline void SetUV(int left, int top, int right, int bottom, int tw, int th) { SetUV((float)left / (float)tw, (float)top / (float)th, (float)right / (float)tw, (float)bottom / (float)th); }
inline void SetUV2(int left, int top, int right, int bottom, int tw, int th) { SetUV2((float)left / (float)tw, (float)top / (float)th, (float)right / (float)tw, (float)bottom / (float)th); }
};


#endif
