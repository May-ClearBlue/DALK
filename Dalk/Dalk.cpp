#include "MultiStdafx.h"
#include "DALK.h"


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

#if defined(_DALK_USE_DIRECT3D_)
	CDirect3D::Initialize();
#elif defined(_DALK_USE_OPENGL_)
	CShaderManager::InitAllShader(APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT);
#endif    

#if defined(_DALK_USE_XAUDIO_)
	CXAudio2::Init();
#elif defined(_DALK_USE_OPENAL_)
	COpenAL::Init();
#endif    

	m_Device.Initialize(m_hWnd, 0, 0, _DALK_APP_SCREEN_WIDTH_, _DALK_APP_SCREEN_HEIGHT_);
	m_Device.Set2DCustom();

	_DEBUG_HEAP_INFO();

	m_RenderTexture.Create(_DALK_APP_SCREEN_WIDTH_, _DALK_APP_SCREEN_HEIGHT_, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, TRUE);
	m_RenderSprite.SetTexture(&m_RenderTexture);

#if defined(_DALK_USE_SQUIRREL_)
	InitSquirrel();
#endif

	m_Texture.LoadImage("airi.png");
	m_Sprite.SetTexture(&m_Texture);
	m_Sprite.SetDiffuse(0.0f, 1.0f, 0.0f, 1.0f);
	m_Sprite.SetXYZ2D(0, 0, 512, 512);
	//	SaveBMP(m_Texture.GetInfo(), 0, "DDD.BMP");
}

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

void Dalk::ShutDown()
{
#if defined(_DALK_USE_SQUIRREL_)
	//Sqratライブラリのクラスが破棄されるとき、vmを利用しているため、sq_closeする前にSqratライブラリのクラスを破棄する必要がある

	//	script.Release();

	// VMを解放
	sq_close(m_hSquirrel);
#endif
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
				m_State = -1;
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
