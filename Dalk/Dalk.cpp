#include "MultiStdafx.h"
#include "DALK.h"

#if defined(_DALK_USE_DIRECT3D_)
#define _DALK_INIT_3DDEVICE_ CDirect3D::Initialize()
#define _DALK_RELEASE_3DDEVICE_ CDirect3D::Release()
#elif defined(_DALK_USE_OPENGL_)
#define _DALK_INIT_3DDEVICE_ CShaderManager::InitAllShader(APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT)
#define _DALK_RELEASE_3DDEVICE_ COpenGL::Release()
#endif    

#if defined(_DALK_USE_XAUDIO_)
#define _DALK_INIT_AUDIODEVICE CXAudio2::Init()
#define _DALK_RELEASE_AUDIODEVICE CXAudio2::Release()
#elif defined(_DALK_USE_OPENAL_)
#define _DALK_INIT_AUDIODEVICE COpenAL::Init()
#define _DALK_RELEASE_AUDIODEVICE COpenAL::Release()
#endif    

void Dalk::Init(int width, int height) {
	m_hWnd = NULL;

#if defined(_DALK_WINDOWS_)
	Create(_DALK_APP_SCREEN_WIDTH_, _DALK_APP_SCREEN_HEIGHT_, _DALK_APP_SCREEN_TITLE_);
	SetVisible(true);
#elif defined(_DALK_IOS_)
#endif

	CSystemInfo::Init();
	CSpecialPath::Init();
	CTimer::Initilaize();

	_DALK_INIT_3DDEVICE_;
	_DALK_INIT_AUDIODEVICE;

	m_Device.Initialize(m_hWnd, 0, 0, _DALK_APP_SCREEN_WIDTH_, _DALK_APP_SCREEN_HEIGHT_);
	m_Device.Set2DCustom();

	_DEBUG_HEAP_INFO();

#if defined(_DALK_USE_SQUIRREL_)
	InitSquirrel();
#endif

#if 0
	m_RenderTexture.Create(_DALK_APP_SCREEN_WIDTH_, _DALK_APP_SCREEN_HEIGHT_, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, TRUE);
	m_RenderSprite.SetTexture(&m_RenderTexture);
#endif

#if 0
	m_Texture.LoadImage("airi.png");
	m_Sprite.SetTexture(&m_Texture);
	m_Sprite.SetDiffuse(0.0f, 1.0f, 0.0f, 1.0f);
	m_Sprite.SetXYZ2D(0, 0, 512, 512);
	//	SaveBMP(m_Texture.GetInfo(), 0, "DDD.BMP");
#endif
}

#ifdef _DALK_WINDOWS_
LRESULT CALLBACK Dalk::MessageProc(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam) {
	if(id == WM_CLOSE)
		::PostQuitMessage(0);
//		::SendMessage(m_hWnd, WM_DESTROY, 0, 0);
//	else if ( id == WM_DESTROY )
//		::PostQuitMessage(0);
	else
		return ::DefWindowProc(hWnd,id,wParam,lParam);

	return 0;
}
#endif

#if defined(_DALK_USE_SQUIRREL_)
void Dalk::InitSquirrel(int stackSize)
{
	// VMを作成
	m_hSquirrel = sq_open(stackSize);

	// 標準のエラーハンドラを設定
	sqstd_seterrorhandlers(m_hSquirrel);

	// 文字列出力関数を設定
	sq_setprintfunc(m_hSquirrel, printfunc, printfunc);

	// 標準で使うVMの設定
	Sqrat::DefaultVM::Set(m_hSquirrel);

	_Bind_CWindow(m_hSquirrel);
}

void Dalk::RunSquirrel(_TCHAR fileName)
{
	Sqrat::Script _script(m_hSquirrel);
	_script.CompileFile(_TEXT("D:\\Users\\まひる。\\Desktop\\start.nut"));
	_script.Run();
}

#endif

DWORD Dalk::ShutDown()
{
#if defined(_DALK_USE_SQUIRREL_)
	//Sqratライブラリのクラスが破棄されるとき、vmを利用しているため、sq_closeする前にSqratライブラリのクラスを破棄する必要がある

	//	script.Release();

	// VMを解放
	sq_close(m_hSquirrel);
#endif

	m_Device.Release();
	
	_DALK_RELEASE_3DDEVICE_;
	_DALK_RELEASE_AUDIODEVICE;

#ifdef _DALK_WINDOWS_
	Destroy();
#endif

	return 0;
}

void Dalk::Update() {
//	m_RenderingBuffer.SetRenderTarget(m_RenderTexture);

	_RINSERESULT res;
#ifdef _DALK_WINDOWS_
	res =
#endif
	m_Device.BeginScene();
	SIG_ERRORCHECK(res, _TEXT("FAILED::Siglune.BeginScene"));

#ifdef _DALK_WINDOWS_
	res =
#endif
	m_Device.Clear(0, 0, 0);
	SIG_ERRORCHECK(res, _TEXT("FAILED::Siglune.Clear"));

#if defined(_DALK_USE__OPENGL_)
	//			CShader& shader2 = CShaderManager::GetShader(1);
	//			shader2.SetShader();
	//			shader2.SetUniform();
#endif

	Draw();

#if defined(_DALK_WINDOWS_)
	res = m_Device.EndScene();
	res = m_Device.Present();
#endif

//	m_RenderingBuffer.ResetRenderTarget();
}

void Dalk::Draw() {
	m_Sprite.DrawUp();

#if 0
	if (GetAsyncKeyState(VK_SPACE)){
		//				m_Sprite.Draw();
		//				m_Font.SetDiffuse(0xFF000000);
		//				m_Font.DrawChar(0x938c,0.0f, 0.0f);

		STable[78]._Sprite->SetXYZ2D(200, 200, 700, 500);
		STable[78]._Sprite->SetUV(0, 0, 500, 300, 2048, 1440);
		STable[78]._Sprite->SetDiffuse(0xFFFFFFFF);
		STable[78].lpTEXT.SetTexture();
		STable[78]._Sprite->DrawPrimitiveUp();

		//				m_Sprite.SetUV(0, 150, 640, 300,2048,1024);
		//				m_Sprite->SetDiffuse(col);

		//				m_Texture.SetTexture();
		//				m_Sprite.DrawUp();

		//				m_RenderTexture.SetTexture();
		//				m_RenderSprite.Draw();

	}
#endif

#if defined (_DALK_IOS_)
	CShader::ChangeShader(_SHADER_NOTEXTURE_);
	for (int a = 0; a < 6; a++) {
		CVertexSquareLV gg;
		gg.SetXYZ2D(PADRect[a].left, PADRect[a].top, PADRect[a].right, PADRect[a].bottom);
		gg.SetDiffuse(1.0f, 0.0f, 0.0f, 0.5f);
		gg.DrawPrimitiveUp();
	}
	CShader::ChangeShader(_SHADER_DEFAULT_);
#endif
}

#if defined(_DALK_WINDOWS_)
void Dalk::Run() {
	MSG msg;
	do{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!::GetMessage(&msg, NULL, 0, 0))/* メッセージ処理 */
				m_State = 1;

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else 
			Update();

		::Sleep(10);
	} while(m_State < 0);
}

#endif

/*
//			if(GetAsyncKeyState(VK_RETURN))
//				this->On();
#if defined(_APP_TESTMODE_)
m_Texture.LoadImageFile(_TEXT(_APP_RESOURCE_PATH_ "airi.png"));
m_Texture.Create();
m_Sprite.SetTexture(&m_Texture);
//		m_Font.LoadFont(_APP_RESOURCE_PATH_"FTEST");
#endif

m_Texture.SetTexture();
m_Sprite.Draw();
CExtAudioStream ADS;
CExtAudioDecoder ADC;
ADS.Open(@"TEST.mp3");
ADC.SetStream(&ADS);
DWORD frame = ADS.GetSize() * 2;
void* pBuf = malloc(frame);
frame = ADC.Decode(pBuf, frame, frame);
//		m_Font.DrawChar(L'鎌',0.0f, 0.0f)
m_Font.DrawChar(0x938c, 0.0f, 0.0f);
#if 0
m_Sprite.SetXYZ(925, 486, 100 + 18 + 36, 100 + 32 + 64);
m_Sprite.SetUV(501, 222, 501 + 18, 222 + 32, 2048, 1024);
//			m_Sprite->SetDiffuse(col);

m_Texture.SetTexture();
m_Sprite.DrawUp();
#endif

*/
