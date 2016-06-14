#include "Multistdafx.h"
#include "EliShader.h"

#include <fstream>
#include <iostream>

CShader::CShader() : 
	m_pVertexDeclaration(NULL),
	m_pVertexShader(NULL),
	m_pPixelShader(NULL)
	{

	_ZEROINIT(m_UniformVar);
	
	_MATRIXIDENTITY(m_UniformVar.projection);
	_MATRIXIDENTITY(m_UniformVar.view);
	_MATRIXIDENTITY(m_UniformVar.world);
}

void CShader::SetUniformIndex() {}

void CShader::SetUniform() {
	SetUniformIndex();

	HRESULT res;

	res = CDirect3D::GetDevice()->SetVertexShaderConstantI(m_UniformIndex[SH_UNI_DEFY], &m_UniformVar.diffY, 1);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@diffY");
	res = CDirect3D::GetDevice()->SetVertexShaderConstantI(m_UniformIndex[SH_UNI_OFSX], &m_UniformVar.offsetX, 1);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@offsetX");
	res = CDirect3D::GetDevice()->SetVertexShaderConstantI(m_UniformIndex[SH_UNI_OFSY], &m_UniformVar.offsetY, 1);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@offsetY");
	res = CDirect3D::GetDevice()->SetVertexShaderConstantI(m_UniformIndex[SH_UNI_TEX0], &m_UniformVar.texture0, 1);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@texture1");
	res = CDirect3D::GetDevice()->SetVertexShaderConstantF(m_UniformIndex[SH_UNI_WORLD], _MATRIXARRAYPTR(m_UniformVar.world), 4);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@world");
	res = CDirect3D::GetDevice()->SetVertexShaderConstantF(m_UniformIndex[SH_UNI_VIEW], _MATRIXARRAYPTR(m_UniformVar.view), 4);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@view");
	res = CDirect3D::GetDevice()->SetVertexShaderConstantF(m_UniformIndex[SH_UNI_PROJECTION], _MATRIXARRAYPTR(m_UniformVar.projection), 4);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@projection");
}

void CShader::SetProjectionMatrix() {
	HRESULT res;
	res = CDirect3D::GetDevice()->SetVertexShaderConstantF(m_UniformIndex[SH_UNI_PROJECTION], _MATRIXARRAYPTR(m_UniformVar.projection), 4);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@projection");
}

void CShader::SetViewMatrix() {
	HRESULT res;
	res = CDirect3D::GetDevice()->SetVertexShaderConstantF(m_UniformIndex[SH_UNI_VIEW], _MATRIXARRAYPTR(m_UniformVar.view), 4);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@view");
}

void CShader::SetWorldMatrix() {
	HRESULT res;
	res = CDirect3D::GetDevice()->SetVertexShaderConstantF(m_UniformIndex[SH_UNI_WORLD], _MATRIXARRAYPTR(m_UniformVar.world), 4);
	SIG_ERRORCHECK(res,"Error:SetUniformVar@world");
}

void CShader::Release() {
	SafeRelease(m_pPixelShader);
	SafeRelease(m_pVertexShader);
	SafeRelease(m_pVertexDeclaration);
}

BOOL CShader::CreateShaderFromString(const char* Ver, const char* Fl, SHADE_UNIFORM_ENUM  format) {
//	D3DXSHADER_PACKMATRIX_COLUMNMAJOR(列優先)の場合は事前に転置が必要。速度的に列優先の方がいいらしい
	LPD3DXBUFFER pBuffer;
	LPD3DXBUFFER pError;

#ifdef _DEBUG
	DWORD vflags = D3DXSHADER_PACKMATRIX_COLUMNMAJOR | D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT | D3DXSHADER_DEBUG;
	DWORD pflags = D3DXSHADER_PACKMATRIX_COLUMNMAJOR | D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT | D3DXSHADER_DEBUG;
	D3DXMACRO ShaderDefine[2] = { "_DEBUG","", NULL, NULL };

#else
	DWORD vflags = D3DXSHADER_PACKMATRIX_COLUMNMAJOR | D3DXSHADER_PARTIALPRECISION | D3DXSHADER_PREFER_FLOW_CONTROL;
	DWORD pflags = vflags;
	D3DXMACRO* ShaderDefine = NULL;
#endif

	HRESULT res;
	res = D3DXCompileShader(Ver, strlen(Ver), ShaderDefine,  NULL, _SIG_HLSL_ENTRYPOINT_, _SIG_VERTEXSHADER_PROFILE_, vflags, &pBuffer, &pError, NULL);

	if(FAILED(res)){
		_DEBUGPRINTF((const char*)pError->GetBufferPointer());
		return FALSE;
	}

	res = CDirect3D::GetDevice()->CreateVertexShader( (const DWORD*)pBuffer->GetBufferPointer(), &m_pVertexShader );
	SIG_ERRORRETURNBOOL(res, "Failed:CreateVertexShader");

	res = D3DXCompileShader(Ver, strlen(Fl), ShaderDefine,  NULL, _SIG_HLSL_ENTRYPOINT_, _SIG_PIXELSHADER_PROFILE_, pflags, &pBuffer, &pError, NULL);
	if(FAILED(res)){
		_DEBUGPRINTF((const char*)pError->GetBufferPointer());
		return FALSE;
	}

	res = CDirect3D::GetDevice()->CreatePixelShader( (const DWORD*)pBuffer->GetBufferPointer(), &m_pPixelShader );
	SIG_ERRORRETURNBOOL(res, "Failed:CreatePixelShader");

	return TRUE;
}

BOOL CShader::CreateShaderFromFile(const char* Ver, const char* Fl, SHADE_UNIFORM_ENUM  format) {
    //.vertファイルの中身を全て読み込む
    std::ifstream inputFile1(Ver);
    std::istreambuf_iterator<char> vdataBegin(inputFile1);
    std::istreambuf_iterator<char> vdataEnd;
    std::string fileData1(vdataBegin,vdataEnd);
    const char *vfile = fileData1.c_str();

    //.fragファイルの中身を全て読み込む
    std::ifstream inputFile2(Fl);
    std::istreambuf_iterator<char> fdataBegin(inputFile2);
    std::istreambuf_iterator<char> fdataEnd;
    std::string fileData2(fdataBegin,fdataEnd);
    const char *ffile = fileData2.c_str();

	return CreateShaderFromString(vfile, ffile, format);
}
