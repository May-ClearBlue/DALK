//
//  shader.m
//  test
//
//  Created by 中野 利哉 on 2012/08/31.
//  Copyright (c) 2012年 中野 利哉. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include	"General.h"

BOOL	ShaderUse[50];
GLuint	VShaderID[50];
GLuint	FShaderID[50];
GLuint	ShaderPrg[50];

GLuint	ShaderUni[50][3];


#define		SH_UNI_DEFY		0	// 16:9でない場合の表示位置補正
#define		SH_UNI_OFSX		1	// 解像度補正X
#define		SH_UNI_OFSY		2	// 解像度補正Y
#define		SH_UNI_TEX1		3	// Texture 1



enum
{
	SH_ATR_POSITION,
	SH_ATR_COLOR,
	SH_ATR_UV,
	SH_ATR_UV2,
	SH_ATR_UV3
};

/*
	シェーダーをコンパイルする
*/
GLuint MakeShader(GLuint type, const GLchar *src)
{
    GLint	status;
	GLuint	id;
	
	if(type == 0)
	{
		id = glCreateShader(GL_VERTEX_SHADER);			// シェーダー生成
	}
	else
	{
		id = glCreateShader(GL_FRAGMENT_SHADER);			// シェーダー生成
	}
    glShaderSource(id, 1, &src, NULL);	// ソースをロード
    glCompileShader(id);				// コンパイル

// シェーダーコンパイルログを取得＆表示
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(id, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif

// コンパイル成功かどうか確認
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if(status == 0)
	{
		glDeleteShader(id);
		return 0xffffffff;				// 失敗
	}
    
	return id;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		シェーダー定義

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//	頂点αを使用する
const GLchar *vsh000 = 
	"	attribute vec4 position;						\n"
	"	attribute vec4 color;							\n"
	"													\n"
	"	varying vec4 v_color;							\n"
	"													\n"
//	"	uniform mat4 vp_mtr;							\n"
	"	uniform float defy;								\n"
	"	uniform float ofsx;								\n"
	"	uniform float ofsy;								\n"
	"	void main()										\n"
	"	{												\n"
	"		v_color = color;							\n"
//	"		gl_Position = vp_mtr * position;			\n"
	"		gl_Position = position;						\n"
	"		gl_Position.x *= ofsx;						\n"
	"		gl_Position.y *= ofsy;						\n"
	"		gl_Position.x -= 1.0;						\n"
	"		gl_Position.y += 1.0 - defy;				\n"
	"		gl_Position.w = 1.0;						\n"
	"	}												\n"
	"";

const GLchar *fsh000 = 
	"	precision mediump float;						\n"
	"	varying vec4 v_color;							\n"
	"													\n"
	"	void main()										\n"
	"	{												\n"
	"	    gl_FragColor = v_color;						\n"
	"	}												\n"
	"";


/*
	シェーダーをコンパイルして準備する
*/
BOOL SetupShader00()
{
    GLint status;

	ShaderUse[0] = FALSE;

	NSLog(@"Shader 00.\n");
	NSLog(@"Start compile vertex shader.\n");
	VShaderID[0] = MakeShader(0, vsh000);
	if(VShaderID[0] == 0xffffffff)
	{
		return FALSE;
	}
	NSLog(@"Start compile flagment shader.\n");
	FShaderID[0] = MakeShader(1, fsh000);
	if(FShaderID[0] == 0xffffffff)
	{
		glDeleteShader(VShaderID[0]);
		VShaderID[0] = 0;
		return FALSE;
	}

	// シェーダープログラムオブジェクトを作成
	ShaderPrg[0] = glCreateProgram();

	NSLog(@"Set program to vertex shader.\n");
	// 頂点シェーダーを設定
	glAttachShader(ShaderPrg[0], VShaderID[0]);

	NSLog(@"Set program to flagment shader.\n");
	// フラグメントシェーダーを設定
	glAttachShader(ShaderPrg[0], FShaderID[0]);

	// 頂点フォーマットを設定
	NSLog(@"Bind program to Attributes.\n");
	glBindAttribLocation(ShaderPrg[0], SH_ATR_POSITION, "position");
	glBindAttribLocation(ShaderPrg[0], SH_ATR_COLOR, "color");

	// Link program.
	NSLog(@"Link program.\n");
	glLinkProgram(ShaderPrg[0]);

#if defined(DEBUG)
	GLint logLength;
	glGetProgramiv(ShaderPrg[0], GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(ShaderPrg[0], logLength, &logLength, log);
		NSLog(@"Program link log:\n%s", log);
		free(log);
	}
#endif

	glGetProgramiv(ShaderPrg[0], GL_LINK_STATUS, &status);
	if(status == 0)
	{
		NSLog(@"Failed to link program: %d", 1);
	    
		glDeleteShader(VShaderID[0]);
		VShaderID[0] = 0;
		glDeleteShader(FShaderID[0]);
		FShaderID[0] = 0;
		glDeleteProgram(ShaderPrg[0]);
		ShaderPrg[0] = 0;

		return FALSE;
	}
    
	// uniform 変数の位置を取得しておく
//	ShaderUni[0][0] = glGetUniformLocation(ShaderPrg[0], "vp_mtr");
	ShaderUni[0][SH_UNI_DEFY] = glGetUniformLocation(ShaderPrg[0], "defy");
	ShaderUni[0][SH_UNI_OFSX] = glGetUniformLocation(ShaderPrg[0], "ofsx");
	ShaderUni[0][SH_UNI_OFSY] = glGetUniformLocation(ShaderPrg[0], "ofsy");

	// サンプルだとここで解放しているが、意味不明。
//	if (vertShader) {
//		glDetachShader(_program, vertShader);
//		glDeleteShader(vertShader);
//	}
//	if (fragShader) {
//		glDetachShader(_program, fragShader);
//		glDeleteShader(fragShader);
//	}

	ShaderUse[0] = TRUE;

	return TRUE;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		シェーダー定義

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//	テクスチャα、頂点αを使用する
const GLchar *vsh001 = 
	"	attribute vec4 position;						\n"
	"	attribute vec4 color;							\n"
	"	attribute vec2 texCoord0;						\n"
	"													\n"
	"	varying vec4 v_color;							\n"
	"	varying vec2 v_texCoord0;						\n"
	"													\n"
//	"	uniform mat4 vp_mtr;							\n"
	"	uniform float defy;								\n"
	"	uniform float ofsx;								\n"
	"	uniform float ofsy;								\n"
	"	void main()										\n"
	"	{												\n"
	"		v_color = color;							\n"
	"		v_texCoord0 = texCoord0;					\n"
//	"		gl_Position = vp_mtr * position;			\n"
	"		gl_Position = position;						\n"
	"		gl_Position.x *= ofsx;						\n"
	"		gl_Position.y *= ofsy;						\n"
	"		gl_Position.x -= 1.0;						\n"
	"		gl_Position.y += 1.0 - defy;				\n"
	"		gl_Position.w = 1.0;						\n"
	"	}												\n"
	"";

/*

//	テクスチャα、頂点αを使用する
const char *ps_src1 = 
	"	TEXTURE  TexSrc;							\n"
	"	sampler  DiffuseSampler = sampler_state		\n"
	"	{											\n"
	"		Texture = (TexSrc);						\n"
	"	};											\n"
	"	void ps_main(								\n"
	"		in  float2 vTex : TEXCOORD0,			\n"
	"		in	float4 vDif : COLOR0,				\n"
	"		out float4 oCol : COLOR0)				\n"
	"	{											\n"
	"		oCol = tex2D(DiffuseSampler, vTex);		\n"
//	"		oCol.a *= vDif.a;						\n"
	"		oCol *= vDif;							\n"
	"	}											\n"
*/

const GLchar *fsh001 = 
	"	precision mediump float;						\n"
	"	varying vec2 v_texCoord0;						\n"
	"	varying vec4 v_color;							\n"
	"													\n"
	"	uniform sampler2D tex_base;						\n"
	"	void main()										\n"
	"	{												\n"
	"		vec4 col;									\n"
	"													\n"
	"		col = texture2D(tex_base, v_texCoord0);		\n"
	"	    gl_FragColor = col * v_color;				\n"
//	"	    gl_FragColor.a = 1.0;						\n"
	"	}												\n"
	"";



/*
	シェーダーをコンパイルして準備する
*/
BOOL SetupShader01()
{
    GLint status;

	ShaderUse[1] = FALSE;

	NSLog(@"Shader 01.\n");
	NSLog(@"Start compile vertex shader.\n");
	VShaderID[1] = MakeShader(0, vsh001);
	if(VShaderID[1] == 0xffffffff)
	{
		return FALSE;
	}
	NSLog(@"Start compile flagment shader.\n");
	FShaderID[1] = MakeShader(1, fsh001);
	if(FShaderID[1] == 0xffffffff)
	{
		glDeleteShader(VShaderID[1]);
		VShaderID[1] = 0;
		return FALSE;
	}

	// シェーダープログラムオブジェクトを作成
	ShaderPrg[1] = glCreateProgram();

	NSLog(@"Set program to vertex shader.\n");
	// 頂点シェーダーを設定
	glAttachShader(ShaderPrg[1], VShaderID[1]);

	NSLog(@"Set program to flagment shader.\n");
	// フラグメントシェーダーを設定
	glAttachShader(ShaderPrg[1], FShaderID[1]);

	// 頂点フォーマットを設定
	NSLog(@"Bind program to Attributes.\n");
	glBindAttribLocation(ShaderPrg[1], SH_ATR_POSITION, "position");
	glBindAttribLocation(ShaderPrg[1], SH_ATR_COLOR, "color");
	glBindAttribLocation(ShaderPrg[1], SH_ATR_UV, "texCoord0");

	// Link program.
	NSLog(@"Link program.\n");
	glLinkProgram(ShaderPrg[1]);

#if defined(DEBUG)
	GLint logLength;
	glGetProgramiv(ShaderPrg[1], GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(ShaderPrg[1], logLength, &logLength, log);
		NSLog(@"Program link log:\n%s", log);
		free(log);
	}
#endif

	glGetProgramiv(ShaderPrg[1], GL_LINK_STATUS, &status);
	if(status == 0)
	{
		NSLog(@"Failed to link program: %d", 1);
	    
		glDeleteShader(VShaderID[1]);
		VShaderID[1] = 0;
		glDeleteShader(FShaderID[1]);
		FShaderID[1] = 0;
		glDeleteProgram(ShaderPrg[1]);
		ShaderPrg[1] = 0;

		return FALSE;
	}
    
	// uniform 変数の位置を取得しておく
//	ShaderUni[1][0] = glGetUniformLocation(ShaderPrg[1], "vp_mtr");
	ShaderUni[1][SH_UNI_DEFY] = glGetUniformLocation(ShaderPrg[1], "defy");
	ShaderUni[1][SH_UNI_OFSX] = glGetUniformLocation(ShaderPrg[1], "ofsx");
	ShaderUni[1][SH_UNI_OFSY] = glGetUniformLocation(ShaderPrg[1], "ofsy");
	ShaderUni[1][SH_UNI_TEX1] = glGetUniformLocation(ShaderPrg[1], "tex_base");

	// サンプルだとここで解放しているが、意味不明。
//	if (vertShader) {
//		glDetachShader(_program, vertShader);
//		glDeleteShader(vertShader);
//	}
//	if (fragShader) {
//		glDetachShader(_program, fragShader);
//		glDeleteShader(fragShader);
//	}

	ShaderUse[1] = TRUE;

	return TRUE;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		シェーダー定義 - font

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//	テクスチャα、頂点αを使用する
const GLchar *vsh004 = 
	"	attribute vec4 position;						\n"
	"	attribute vec4 color;							\n"
	"	attribute vec2 texCoord0;						\n"
	"													\n"
	"	varying vec4 v_color;							\n"
	"	varying vec2 v_texCoord0;						\n"
	"													\n"
//	"	uniform mat4 vp_mtr;							\n"
	"	uniform float defy;								\n"
	"	uniform float ofsx;								\n"
	"	uniform float ofsy;								\n"
	"	void main()										\n"
	"	{												\n"
	"		v_color = color;							\n"
	"		v_texCoord0 = texCoord0;					\n"
//	"		gl_Position = vp_mtr * position;			\n"
	"		gl_Position = position;						\n"
	"		gl_Position.x *= ofsx;						\n"
	"		gl_Position.y *= ofsy;						\n"
	"		gl_Position.x -= 1.0;						\n"
	"		gl_Position.y += 1.0 - defy;				\n"
	"		gl_Position.w = 1.0;						\n"
	"	}												\n"
	"";

/*
const char *ps_src4 = 
	"	TEXTURE  TexSrc;							\n"
	"	sampler  DiffuseSampler = sampler_state		\n"
	"	{											\n"
	"		Texture = (TexSrc);						\n"
	"	};											\n"
	"	void ps_main(								\n"
	"		in  float2 vTex : TEXCOORD0,			\n"
	"		in	float4 vDif : COLOR0,				\n"
	"		out float4 oCol : COLOR0)				\n"
	"	{											\n"
	"		oCol = tex2D(DiffuseSampler, vTex);		\n"
	"		oCol.r = vDif.r;						\n"
	"		oCol.g = vDif.g;						\n"
	"		oCol.b = vDif.b;						\n"
	"		oCol.a *= vDif.a;						\n"
	"	}											\n"
	"";
*/

const GLchar *fsh004 = 
	"	precision mediump float;						\n"
	"	varying vec2 v_texCoord0;						\n"
	"	varying vec4 v_color;							\n"
	"													\n"
	"	uniform sampler2D tex_base;						\n"
	"	void main()										\n"
	"	{												\n"
	"		vec4 col;									\n"
	"													\n"
	"		col = texture2D(tex_base, v_texCoord0);		\n"
	"	    gl_FragColor = v_color;						\n"
	"	    gl_FragColor.a *= col.r;					\n"
	"	}												\n"
	"";



/*
	シェーダーをコンパイルして準備する
*/
BOOL SetupShader04()
{
    GLint status;

	ShaderUse[4] = FALSE;

	NSLog(@"Shader 04.\n");
	NSLog(@"Start compile vertex shader.\n");
	VShaderID[4] = MakeShader(0, vsh004);
	if(VShaderID[4] == 0xffffffff)
	{
		return FALSE;
	}
	NSLog(@"Start compile flagment shader.\n");
	FShaderID[4] = MakeShader(4, fsh004);
	if(FShaderID[4] == 0xffffffff)
	{
		glDeleteShader(VShaderID[4]);
		VShaderID[4] = 0;
		return FALSE;
	}

	// シェーダープログラムオブジェクトを作成
	ShaderPrg[4] = glCreateProgram();

	NSLog(@"Set program to vertex shader.\n");
	// 頂点シェーダーを設定
	glAttachShader(ShaderPrg[4], VShaderID[4]);

	NSLog(@"Set program to flagment shader.\n");
	// フラグメントシェーダーを設定
	glAttachShader(ShaderPrg[4], FShaderID[4]);

	// 頂点フォーマットを設定
	NSLog(@"Bind program to Attributes.\n");
	glBindAttribLocation(ShaderPrg[4], SH_ATR_POSITION, "position");
	glBindAttribLocation(ShaderPrg[4], SH_ATR_COLOR, "color");
	glBindAttribLocation(ShaderPrg[4], SH_ATR_UV, "texCoord0");

	// Link program.
	NSLog(@"Link program.\n");
	glLinkProgram(ShaderPrg[4]);

#if defined(DEBUG)
	GLint logLength;
	glGetProgramiv(ShaderPrg[4], GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(ShaderPrg[4], logLength, &logLength, log);
		NSLog(@"Program link log:\n%s", log);
		free(log);
	}
#endif

	glGetProgramiv(ShaderPrg[4], GL_LINK_STATUS, &status);
	if(status == 0)
	{
		NSLog(@"Failed to link program: %d", 1);
	    
		glDeleteShader(VShaderID[4]);
		VShaderID[4] = 0;
		glDeleteShader(FShaderID[4]);
		FShaderID[4] = 0;
		glDeleteProgram(ShaderPrg[4]);
		ShaderPrg[4] = 0;

		return FALSE;
	}
    
	// uniform 変数の位置を取得しておく
//	ShaderUni[4][0] = glGetUniformLocation(ShaderPrg[4], "vp_mtr");
	ShaderUni[4][SH_UNI_DEFY] = glGetUniformLocation(ShaderPrg[4], "defy");
	ShaderUni[4][SH_UNI_OFSX] = glGetUniformLocation(ShaderPrg[4], "ofsx");
	ShaderUni[4][SH_UNI_OFSY] = glGetUniformLocation(ShaderPrg[4], "ofsy");
	ShaderUni[4][SH_UNI_TEX1] = glGetUniformLocation(ShaderPrg[4], "tex_base");

	// サンプルだとここで解放しているが、意味不明。
//	if (vertShader) {
//		glDetachShader(_program, vertShader);
//		glDeleteShader(vertShader);
//	}
//	if (fragShader) {
//		glDetachShader(_program, fragShader);
//		glDeleteShader(fragShader);
//	}

	ShaderUse[4] = TRUE;

	return TRUE;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		シェーダー定義 - 板ポリ用

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//	頂点αを使用する
const GLchar *vsh005 = 
	"	attribute vec4 position;						\n"
	"	attribute vec4 color;							\n"
	"													\n"
	"	varying vec4 v_color;							\n"
	"													\n"
//	"	uniform mat4 vp_mtr;							\n"
	"	uniform float defy;								\n"
	"	uniform float ofsx;								\n"
	"	uniform float ofsy;								\n"
	"	void main()										\n"
	"	{												\n"
	"		v_color = color;							\n"
//	"		gl_Position = vp_mtr * position;			\n"
	"		gl_Position = position;						\n"
	"		gl_Position.x *= ofsx;						\n"
	"		gl_Position.y *= ofsy;						\n"
	"		gl_Position.x -= 1.0;						\n"
	"		gl_Position.y += 1.0 - defy;				\n"
	"		gl_Position.w = 1.0;						\n"
	"	}												\n"
	"";

const GLchar *fsh005 = 
	"	precision mediump float;						\n"
	"	varying vec4 v_color;							\n"
	"													\n"
	"	void main()										\n"
	"	{												\n"
	"	    gl_FragColor = v_color;						\n"
	"	}												\n"
	"";


/*
	シェーダーをコンパイルして準備する
*/
BOOL SetupShader05()
{
    GLint status;

	ShaderUse[5] = FALSE;

	NSLog(@"Shader 05.\n");
	NSLog(@"Start compile vertex shader.\n");
	VShaderID[5] = MakeShader(0, vsh005);
	if(VShaderID[5] == 0xffffffff)
	{
		return FALSE;
	}
	NSLog(@"Start compile flagment shader.\n");
	FShaderID[5] = MakeShader(1, fsh005);
	if(FShaderID[5] == 0xffffffff)
	{
		glDeleteShader(VShaderID[5]);
		VShaderID[5] = 0;
		return FALSE;
	}

	// シェーダープログラムオブジェクトを作成
	ShaderPrg[5] = glCreateProgram();

	NSLog(@"Set program to vertex shader.\n");
	// 頂点シェーダーを設定
	glAttachShader(ShaderPrg[5], VShaderID[5]);

	NSLog(@"Set program to flagment shader.\n");
	// フラグメントシェーダーを設定
	glAttachShader(ShaderPrg[5], FShaderID[5]);

	// 頂点フォーマットを設定
	NSLog(@"Bind program to Attributes.\n");
	glBindAttribLocation(ShaderPrg[5], SH_ATR_POSITION, "position");
	glBindAttribLocation(ShaderPrg[5], SH_ATR_COLOR, "color");

	// Link program.
	NSLog(@"Link program.\n");
	glLinkProgram(ShaderPrg[5]);

#if defined(DEBUG)
	GLint logLength;
	glGetProgramiv(ShaderPrg[5], GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(ShaderPrg[5], logLength, &logLength, log);
		NSLog(@"Program link log:\n%s", log);
		free(log);
	}
#endif

	glGetProgramiv(ShaderPrg[5], GL_LINK_STATUS, &status);
	if(status == 0)
	{
		NSLog(@"Failed to link program: %d", 1);
	    
		glDeleteShader(VShaderID[5]);
		VShaderID[5] = 0;
		glDeleteShader(FShaderID[5]);
		FShaderID[5] = 0;
		glDeleteProgram(ShaderPrg[5]);
		ShaderPrg[5] = 0;

		return FALSE;
	}
    
	// uniform 変数の位置を取得しておく
//	ShaderUni[0][0] = glGetUniformLocation(ShaderPrg[0], "vp_mtr");
	ShaderUni[5][SH_UNI_DEFY] = glGetUniformLocation(ShaderPrg[0], "defy");
	ShaderUni[5][SH_UNI_OFSX] = glGetUniformLocation(ShaderPrg[0], "ofsx");
	ShaderUni[5][SH_UNI_OFSY] = glGetUniformLocation(ShaderPrg[0], "ofsy");

	// サンプルだとここで解放しているが、意味不明。
//	if (vertShader) {
//		glDetachShader(_program, vertShader);
//		glDeleteShader(vertShader);
//	}
//	if (fragShader) {
//		glDetachShader(_program, fragShader);
//		glDeleteShader(fragShader);
//	}

	ShaderUse[5] = TRUE;

	return TRUE;
}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		シェーダー定義

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//	頂点αを使用する
const GLchar *vsh030 = 
	"	attribute vec4 position;						\n"
	"													\n"
//	"	uniform mat4 vp_mtr;							\n"
	"	uniform float defy;								\n"
	"	uniform float ofsx;								\n"
	"	uniform float ofsy;								\n"
	"	void main()										\n"
	"	{												\n"
//	"		gl_Position = vp_mtr * position;			\n"
	"		gl_Position = position;						\n"
	"		gl_Position.x *= ofsx;						\n"
	"		gl_Position.y *= ofsy;						\n"
	"		gl_Position.x -= 1.0;						\n"
	"		gl_Position.y += 1.0 - defy;				\n"
	"		gl_Position.w = 1.0;						\n"
	"	}												\n"
	"";

const GLchar *fsh030 = 
	"	precision mediump float;						\n"
	"													\n"
	"	void main()										\n"
	"	{												\n"
	"	    gl_FragColor.r = 1.0;						\n"
	"	    gl_FragColor.g = 1.0;						\n"
	"	    gl_FragColor.b = 0.0;						\n"
	"	    gl_FragColor.a = 1.0;						\n"
	"	}												\n"
	"";


/*
	シェーダーをコンパイルして準備する
*/
BOOL SetupShader30()
{
    GLint status;

	ShaderUse[30] = FALSE;

	NSLog(@"Shader 30.\n");
	NSLog(@"Start compile vertex shader.\n");
	VShaderID[30] = MakeShader(0, vsh030);
	if(VShaderID[30] == 0xffffffff)
	{
		return FALSE;
	}
	NSLog(@"Start compile flagment shader.\n");
	FShaderID[30] = MakeShader(1, fsh030);
	if(FShaderID[30] == 0xffffffff)
	{
		glDeleteShader(VShaderID[30]);
		VShaderID[30] = 0;
		return FALSE;
	}

	// シェーダープログラムオブジェクトを作成
	ShaderPrg[30] = glCreateProgram();

	NSLog(@"Set program to vertex shader.\n");
	// 頂点シェーダーを設定
	glAttachShader(ShaderPrg[30], VShaderID[30]);

	NSLog(@"Set program to flagment shader.\n");
	// フラグメントシェーダーを設定
	glAttachShader(ShaderPrg[30], FShaderID[30]);

	// 頂点フォーマットを設定
	NSLog(@"Bind program to Attributes.\n");
	glBindAttribLocation(ShaderPrg[30], SH_ATR_POSITION, "position");

	// Link program.
	NSLog(@"Link program.\n");
	glLinkProgram(ShaderPrg[30]);

#if defined(DEBUG)
	GLint logLength;
	glGetProgramiv(ShaderPrg[30], GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(ShaderPrg[30], logLength, &logLength, log);
		NSLog(@"Program link log:\n%s", log);
		free(log);
	}
#endif

	glGetProgramiv(ShaderPrg[30], GL_LINK_STATUS, &status);
	if(status == 0)
	{
		NSLog(@"Failed to link program: %d", 1);
	    
		glDeleteShader(VShaderID[30]);
		VShaderID[30] = 0;
		glDeleteShader(FShaderID[30]);
		FShaderID[30] = 0;
		glDeleteProgram(ShaderPrg[30]);
		ShaderPrg[30] = 0;

		return FALSE;
	}
    
	// uniform 変数の位置を取得しておく
//	ShaderUni[30][0] = glGetUniformLocation(ShaderPrg[30], "vp_mtr");
	ShaderUni[30][SH_UNI_DEFY] = glGetUniformLocation(ShaderPrg[30], "defy");
	ShaderUni[30][SH_UNI_OFSX] = glGetUniformLocation(ShaderPrg[30], "ofsx");
	ShaderUni[30][SH_UNI_OFSY] = glGetUniformLocation(ShaderPrg[30], "ofsy");



	// サンプルだとここで解放しているが、意味不明。
//	if (vertShader) {
//		glDetachShader(_program, vertShader);
//		glDeleteShader(vertShader);
//	}
//	if (fragShader) {
//		glDetachShader(_program, fragShader);
//		glDeleteShader(fragShader);
//	}

	ShaderUse[30] = TRUE;

	return TRUE;
}







/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

BOOL SetupShader()
{
	SetupShader00();
	SetupShader01();
	SetupShader04();
	SetupShader05();


	SetupShader30();

	return TRUE;
}


/*
	レンダリング用にシェーダーを設定
*/
//void SetShader(UINT32 sno, GLKMatrix4 *mat)
void SetShader(UINT32 sno, float defy, float ofsx, float ofsy)
{
	if(ShaderUse[sno] == FALSE) return;

	glUseProgram(ShaderPrg[sno]);
//	glUniformMatrix4fv(ShaderUni[sno][0], 1, 0, mat->m);
	glUniform1f(ShaderUni[sno][SH_UNI_DEFY], defy);
	glUniform1f(ShaderUni[sno][SH_UNI_OFSX], ofsx);
	glUniform1f(ShaderUni[sno][SH_UNI_OFSY], -ofsy);
}

/*
	SH_ATR_POSITION,	// x, y, z
	SH_ATR_COLOR,		// r, g, b, a
	SH_ATR_UV,			// u, v
*/
void SetTLVertex(float *pbuf)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glEnableVertexAttribArray(0);	// SH_ATR_POSITION,	// x, y, z
	glEnableVertexAttribArray(1);	// SH_ATR_COLOR,		// r, g, b, a
	glEnableVertexAttribArray(2);	// SH_ATR_UV,			// u, v

	// index, num, type, normalized, datalen, bufp
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*4, pbuf);	// Pos
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9*4, pbuf+3);	// Col
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9*4, pbuf+7);	// UV
}

/*
	SH_ATR_POSITION,	// x, y, z
	SH_ATR_COLOR,		// r, g, b
*/
void SetLVertex(float *pbuf)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glEnableVertexAttribArray(0);	// SH_ATR_POSITION,	// x, y, z
	glEnableVertexAttribArray(1);	// SH_ATR_COLOR,		// r, g, b, a


	// index, num, type, normalized, datalen, bufp
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*4, pbuf);	// Pos
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7*4, pbuf+3);	// Col
}

/*
	SH_ATR_POSITION,	// x, y, z
*/
void SetVertex(float *pbuf)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glEnableVertexAttribArray(0);	// SH_ATR_POSITION,	// x, y, z

	// index, num, type, normalized, datalen, bufp
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*4, pbuf);	// Pos
}

void DrawPrimitiveTRISTRIP(UINT32 st, UINT32 ct)
{
	glDrawArrays(GL_TRIANGLE_STRIP, st, ct);
}

void DrawPrimitiveTRI(UINT32 st, UINT32 ct)
{
	glDrawArrays(GL_TRIANGLES, st, ct);
}

void SetTexture(UINT32 shno, UINT32 id, UINT32 texid)
{
	switch(id)
	{
	case 0:	// Texture 0
		glActiveTexture(GL_TEXTURE0);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, texid);
	glUniform1i(ShaderUni[shno][SH_UNI_TEX1], 0);
}
