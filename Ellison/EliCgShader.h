#ifndef _SIG_CG_SHADER_
#define _SIG_CG_SHADER_

#include <iostream>

#ifdef _DALK_WINDOWS_
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#pragma comment (lib,"cg.lib")
#pragma comment (lib,"cgGL.lib")
#else
//#include <glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#pragma comment (lib,"cg.lib")
#pragma comment (lib,"cgGL.lib")
#endif


//-------- 各種外部変数 -------------//
//Cg用変数
CGcontext CgContext; //コンテキスト
CGprofile CgVertexProfile;//頂点プロファイル
CGprogram CgVertexProgram;
CGprofile CgFragmentProfile;//フラグメントプロファイル
CGprogram CgFragmentProgram;

//-------- プロトタイプ宣言 -----//
void display();

//-------- Cgの初期設定 -----------//
//頂点シェーダ設定
void setUpCgVertex()
{
        //プロファイルの取得
        CgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
        if(CgVertexProfile == CG_PROFILE_UNKNOWN)
        {
                std::cerr << "Invalid profile type\n";
                exit(0);
        }
        cgGLSetOptimalOptions(CgVertexProfile);


        //ファイルを読んでコンパイル
        CgVertexProgram 
                = cgCreateProgramFromFile(
                                CgContext,//コンテキスト
                                CG_SOURCE,//Cgソースコード
                                "vertex.cg", //「.cgファイル名」
                                CgVertexProfile, //プロファイル
                                "CgVertexMain", //「.cgファイル」で最初にいく関数
                                NULL); //NULLでよい

        cgGLLoadProgram(CgVertexProgram);//プログラムのロード
        if(CgVertexProgram==NULL){
                std::cerr <<"Vertex Program Err. \n";
                exit(0);
        }
}

//フラグメントの初期設定
void setUpCgFragment()
{
        //プロファイルの取得
        CgFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
        if(CgFragmentProfile == CG_PROFILE_UNKNOWN){
                std::cerr << "Fragment : profile is unknown\n";
                exit(0);
        }
        cgGLSetOptimalOptions(CgFragmentProfile);

        //プログラムのコンパイル
        CgFragmentProgram 
                = cgCreateProgramFromFile(
                                CgContext,//コンテキスト
                                CG_SOURCE,//Cgソースコード
                                "fragment.cg", //「.cgファイル名」
                                CgFragmentProfile, //プロファイル
                                "CgFragmentMain", //「.cgファイル」で最初にいく関数
                                NULL); //NULLでよい

        cgGLLoadProgram(CgFragmentProgram); //プログラムのロード
        if(CgFragmentProgram==NULL){
                std::cerr <<"Fragment Program Err. \n";
                exit(0);
        }
}

void CgINIT()
{
        //コンテキスト作成
        CgContext = cgCreateContext();
        if(CgContext == NULL){
                std::cerr << "Can't Create Context\n";
                exit(0);
        }

        setUpCgVertex();
        setUpCgFragment();

}


//------------ GLUTの初期設定 ---------------//
void GLUT_CALL_FUNC()
{
//        glutDisplayFunc(display);
}

void GLUT_INIT(int argc,char **argv)
{
//        glutInit(&argc,argv);
//        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
//       glutInitWindowSize(640,480);
//        glutCreateWindow("Hello Cg with OpenGL");
        GLUT_CALL_FUNC();
}

//---------- メイン関数 ---------------//
int main(int argc,char **argv)
{
        GLUT_INIT(argc,argv);

        CgINIT();//Cgセットアップ

 //       glutMainLoop();

        return 0;
}


//----------- ここから各種コールバック -----------------//
void display()
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Cgの有効化
        cgGLBindProgram(CgVertexProgram);
        cgGLEnableProfile(CgVertexProfile);

        cgGLBindProgram(CgFragmentProgram);
        cgGLEnableProfile(CgFragmentProfile);

 //       glColor3f(0,1,0);
 //       glBegin(GL_TRIANGLES);
 //       glVertex2f(-0.8, 0.8);
 //       glVertex2f(0.8, 0.8);
 //       glVertex2f(0.0, -0.8);
 //       glEnd();

        //Cgの無効化
        cgGLDisableProfile(CgVertexProfile);
        cgGLDisableProfile(CgFragmentProfile);

 //       glutSwapBuffers();
}

#endif
