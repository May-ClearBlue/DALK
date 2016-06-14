#include "Multistdafx.h"

#include "SailGrid.h"

void CGrid::SetGrid(int width,int height){
	m_GridWidth = width;
	m_GridHeight= height;

	m_GridX = m_pTexture->GetTextureInfo().TextureWidth / width;
	m_GridY = m_pTexture->GetTextureInfo().TextureHeight/ height;
}

void CGrid::DrawGrid(int GridNum,int x,int y){
	float x1 = (GridNum % m_GridX) * m_GridWidth;
	float y1 = (GridNum / m_GridX) * m_GridHeight;
	float x2 = x1 + m_GridWidth;
	float y2 = y1 + m_GridHeight;

	SAILVERTEX Vertex[4];

	SetSAILVERTEX(Vertex[0], x1 + x , y1 + y, 0.0f,1.0f, D3DCOLOR_RGBA(0xFF,0xFF,0xFF,0xFF),x1 , y1);
	SetSAILVERTEX(Vertex[1], x1 + x , y2 + y, 0.0f,1.0f, D3DCOLOR_RGBA(0xFF,0xFF,0xFF,0xFF),x1 , y2);
	SetSAILVERTEX(Vertex[2], x2 + x , y1 + y, 0.0f,1.0f, D3DCOLOR_RGBA(0xFF,0xFF,0xFF,0xFF),x2 , y1);
	SetSAILVERTEX(Vertex[3], x2 + x , y2 + y, 0.0f,1.0f, D3DCOLOR_RGBA(0xFF,0xFF,0xFF,0xFF),x2 , y2);

	g_pD3DDevice.GetInstance()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof(SAILVERTEX) );
}
