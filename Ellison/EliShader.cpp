#include "EliShader.h"
#include "Siglune/SigDirect3DInit.h"

#include <fstream>
//#include <glm/glm.hpp>	
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

CShader		CShader::g_ShaderInstance[_SHADER_NUM_MAX_];
CShader*	CShader::g_ShaderList[_SHADER_NUM_MAX_] = { 0 };
CShader*	CShader::g_pCurrentShader = NULL;

CShader::CShader() : 
//	m_VertexShaderID(0),
//	m_FragmentShaderID(0),
	m_ProgramID(0)
//	m_Format(0)
	{

	_ZEROINIT(m_UniformVar);

	_MATRIX _temp;
	_MATRIXIDENTITY(_temp);

	m_UniformVar.projection = _temp;// glm::mat4(1.0);
	m_UniformVar.view		= _temp; //glm::mat4(1.0);
	m_UniformVar.world		= _temp;// glm::mat4(1.0);
	m_UniformVar.texture0	= 0;
	m_UniformVar.texture1	= 1;
	m_UniformVar.texture2	= 2;
}

//コンパイルチェック
BOOL CShader::GetCompileError(GLuint id) {
	GLint res; 
	glGetShaderiv( id, GL_COMPILE_STATUS, &res ); 

	if(!res) { 
		int  len, num; 
		glGetShaderiv( id, GL_INFO_LOG_LENGTH, &len); 

		// エラーログを取得 
		char* pStr = new char[len]; 
		glGetShaderInfoLog( id, len, &num, pStr ); 

		// エラーログ出力 
		printf( "%s	", pStr );

		delete[] pStr;

		return FALSE;
	}
	return TRUE;
}

//リンクチェック
BOOL CShader::GetLinkError() {
	GLint linked;
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &linked);

	if(!linked) { 

		int  len, num; 
	    glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &len ); 

		// エラーログを取得 
		char* pStr = new char[len]; 
	    glGetProgramInfoLog(m_ProgramID, len, &num, pStr); 

		// エラーログ出力 
		printf("%s	", pStr); 

		delete[] pStr; 

		return FALSE;
	}
	return TRUE;
}

void CShader::SetUniformIndex() {
	for(SHADER_UNIFORM_INDEX* a = ShaderUniformIndexList[m_UniformType];*a != SH_UNI_TERMINATE;a++)
		m_UniformIndex[*a] = glGetUniformLocation(m_ProgramID, ShaderUniformRef[*a].name);
}

void CShader::SetUniform() {
	SetUniformIndex();

	CHECKUNIFORMINDEX(SH_UNI_DEFY)
	glUniform1i(m_UniformIndex[SH_UNI_DEFY], m_UniformVar.diffY);
	CHECKUNIFORMINDEX(SH_UNI_OFSX)
	glUniform1i(m_UniformIndex[SH_UNI_OFSX], m_UniformVar.offsetX);
	CHECKUNIFORMINDEX(SH_UNI_OFSY)
	glUniform1i(m_UniformIndex[SH_UNI_OFSY], m_UniformVar.offsetY);

	CHECKUNIFORMINDEX(SH_UNI_WORLD)
	glUniformMatrix4fv(m_UniformIndex[SH_UNI_WORLD], 1, GL_FALSE, _MATRIXARRAYPTR(m_UniformVar.world));
	CHECKUNIFORMINDEX(SH_UNI_VIEW)
	glUniformMatrix4fv(m_UniformIndex[SH_UNI_VIEW], 1, GL_FALSE, _MATRIXARRAYPTR(m_UniformVar.view));
	CHECKUNIFORMINDEX(SH_UNI_PROJECTION)
	glUniformMatrix4fv(m_UniformIndex[SH_UNI_PROJECTION], 1, GL_FALSE, _MATRIXARRAYPTR(m_UniformVar.projection));

	if( m_UniformType >= SH_UNI_ENUM_PTL0) {
		CHECKUNIFORMINDEX(SH_UNI_TEX0)
		glUniform1i(m_UniformIndex[SH_UNI_TEX0], m_UniformVar.texture0);
	}
	else if( m_UniformType >= SH_UNI_ENUM_PTL1) {
		CHECKUNIFORMINDEX(SH_UNI_TEX1)
		glUniform1i(m_UniformIndex[SH_UNI_TEX1], m_UniformVar.texture1);
	}
	else if( m_UniformType >= SH_UNI_ENUM_PTL2) {
		CHECKUNIFORMINDEX(SH_UNI_TEX2)
		glUniform1i(m_UniformIndex[SH_UNI_TEX2], m_UniformVar.texture2);
	}
}

void CShader::SetProjectionMatrix() {
	m_UniformIndex[SH_UNI_PROJECTION] = glGetUniformLocation(m_ProgramID, ShaderUniformRef[SH_UNI_PROJECTION].name);

	CHECKUNIFORMINDEX(m_UniformIndex[SH_UNI_PROJECTION])
		glUniformMatrix4fv	(m_UniformIndex[SH_UNI_PROJECTION], 1, GL_FALSE, _MATRIXARRAYPTR(m_UniformVar.projection));
}

void CShader::SetViewMatrix() {
	m_UniformIndex[SH_UNI_VIEW] = glGetUniformLocation(m_ProgramID, ShaderUniformRef[SH_UNI_VIEW].name);

	CHECKUNIFORMINDEX(m_UniformIndex[SH_UNI_VIEW])
		glUniformMatrix4fv	(m_UniformIndex[SH_UNI_VIEW], 1, GL_FALSE, _MATRIXARRAYPTR(m_UniformVar.view));
}

void CShader::SetWorldMatrix() {
	m_UniformIndex[SH_UNI_WORLD] = glGetUniformLocation(m_ProgramID, ShaderUniformRef[SH_UNI_WORLD].name);

	CHECKUNIFORMINDEX(m_UniformIndex[SH_UNI_WORLD])
		glUniformMatrix4fv	(m_UniformIndex[SH_UNI_VIEW], 1, GL_FALSE, _MATRIXARRAYPTR(m_UniformVar.view));
}

void CShader::Release() {
	glUseProgram(0);

	if(m_ProgramID) {
#if 0
		glDetachShader(m_ProgramID,	m_VertexShaderID);
		glDetachShader(m_ProgramID,	m_FragmentShaderID);
		glDeleteShader(m_VertexShaderID);
		glDeleteShader(m_FragmentShaderID);
#endif
		glDeleteProgram(m_ProgramID);
#if 0
		m_VertexShaderID = 0;
		m_FragmentShaderID = 0;
		m_Format = 0;
#endif
		m_ProgramID = 0;
	}
}

BOOL CShader::CreateShaderFromString(const char* Ver, const char* Fl, SHADE_UNIFORM_ENUM format) {
	GLenum r;
	GLuint VertexShaderID = 0;
	GLuint FragmentShaderID = 0;
//バーテックス
	VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderID, 1, &Ver, NULL);
    glCompileShader(VertexShaderID);//コンパイル

	GetCompileError(VertexShaderID);
//フラグメント
	FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderID, 1, &Fl,NULL);
	glCompileShader(FragmentShaderID);//コンパイル
	GetCompileError(FragmentShaderID);

//プログラムオブジェクトの作成とアタッチ
	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID, VertexShaderID);
	SIG_ERRORCHECK(r,"Failed@glAttachShader");
	glAttachShader(m_ProgramID, FragmentShaderID);
	SIG_ERRORCHECK(r,"Failed@glAttachShader");

//アタッチした時点でDeleteしてOKという微妙に謎仕様
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	if(r != GL_NO_ERROR)
		return FALSE;

	for(ATTRIB_TYPE* a = _FIXED_VERTEX_FORMAT_[format];*a != SH_ATR_TERMINATE;a++) {
		glBindAttribLocation(m_ProgramID, *a, ShaderAttributeRef[*a].name);
		SIG_ERRORRETURNBOOL(r,"Failed@glBindAttribLocation");
	}

	m_UniformType = format;

	glLinkProgram(m_ProgramID);

//	m_Format = format;

	return GetLinkError();
}

BOOL CShader::CreateShaderFromFile(const char* Ver, const char* Fl, SHADE_UNIFORM_ENUM format) {
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

#ifdef _DALK_IOS_
BOOL CShader::CreateShaderFromFile(NSString* Ver, NSString* Fl, SHADE_UNIFORM_ENUM format) {

	NSString* path = [[NSBundle mainBundle] pathForResource:Ver ofType:nil];
	NSString* temp = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];

	const char* vfile = temp.UTF8String;

    path = [[NSBundle mainBundle] pathForResource:Fl ofType:nil];
	NSString* temp2 = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];

	const char* ffile = temp2.UTF8String;

	return CreateShaderFromString(vfile, ffile, format);
}
#endif

CShader CShaderManager::g_Shader[10];

void CShaderManager::InitAllShader(DWORD width, DWORD height) {
	//プロジェクション行列
	_MATRIX proj;
	//Eigen Only
	proj <<
		2.0f / (float)width, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / (float)height, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
		;

	_MATRIXTRANSPOSE(proj);
	//glm::transpose(proj);

	for(int a = 0; a < 4; a ++ ) {
		g_Shader[a].CreateShaderFromString(ShaderSourceList[a]._vert, ShaderSourceList[a]._flag, ShaderSourceList[a]._type );
		g_Shader[a].SetProjectionMatrix(&proj);
		g_Shader[a].RegistShaderList((_SHADERTYPE_)a);
	}
}
