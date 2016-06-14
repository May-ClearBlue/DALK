#include "MultiStdafx.h"

#include "RimWindow.h"

#if 0
void CMessageManager::RegistMessenger(IMesProc* mes){
	m_CriticalSection.Enter();
	{
		m_MessengeList.push_back(mes);
	}
	m_CriticalSection.Leave();
}

void CMessageManager::DeleteMessenger(IMesProc* mes){
	m_CriticalSection.Enter();
	{
		for(CMessageList::iterator it = m_MessengeList.begin(); it != m_MessengeList.end();) {
			if (*it == mes)
				it = m_MessengeList.erase(it);
			else
				it ++;
		}
	}
	m_CriticalSection.Leave();
}

void CMessageManager::ClearMessenger(){
	m_CriticalSection.Enter();
	{
		m_MessengeList.clear();
	}
	m_CriticalSection.Leave();
}

//	message dispatcher
//	  CWindowのWndProcから呼び出されるので、このCriticalSectionは、各ウィンドゥにつき一つ存在することになる。
//	  つまりこのCriticalSectionは、メッセージループと、それに対応するメインスレッド間での相互排他のためのものである。
LRESULT CMessageManager::Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc){
	m_CriticalSection.Enter();
	{
		CMessageList::iterator it = m_MessengeList.begin();
		while (it != m_MessengeList.end()) {
			LRESULT l;

//			Err.Out("%.8x %.8x %.8x",(DWORD)this,(DWORD)*it,((*it)->WndProc));
//			DWORD d = reinterpret_cast<DWORD>((*it)->WndProc);

			// 返し値として０以外を持つならばメッセージは処理されたと見なす
			if( l = (*it)->MesProc(hWnd,uMsg,wParam,lParam) )
				return l;
			it++;
		}
	}
	m_CriticalSection.Leave();

	return (pWndProc == NULL) ? ::DefWindowProc(hWnd,uMsg,wParam,lParam) : ::CallWindowProc(pWndProc,hWnd,uMsg,wParam,lParam);
}
#endif

CCursor::CCursor():m_hCursor(NULL),m_id(-1)
 {}

bool CCursor::Load(LPCTSTR Name) {
	m_hCursor = (HCURSOR)::LoadImage( ::GetModuleHandle(NULL), Name, IMAGE_CURSOR, 0, 0, 0 );
	m_id = -1;
	return m_hCursor != NULL ? true : false;
}

bool CCursor::Load(int id) {
	m_hCursor = (HCURSOR)::LoadImage( ::GetModuleHandle(NULL), MAKEINTRESOURCE(id), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
	m_Name = _T("");
	return m_hCursor != NULL ? true : false;
}

CWindow::CWindow() :
	m_FillColor(0)			//	起動時黒がディフォルト
,	m_hWnd(NULL)
,	m_bShowCursor(true)		//	デフォルトでマウスカーソルを表示
,	m_bShowWindow(false)		//	デフォルトでマウスカーソルを表示
	//m_pWndProc		=	NULL;		//	hookしたWndProc

{
	//デフォルトセッティング
	m_Config.caption	= _DALK_WINCLASSNAME_;
	m_Config.classname	= _T("Saya-System");
	m_Config.style		= WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	m_Config.exstyle	= 0;
	m_Config.width		= 800;
	m_Config.height		= 600;
	m_Config.bCentering	= true;
	m_Config.dialog		= NULL;
}

CWindow::~CWindow(){
	//::DestroyWindow(m_hWnd);
}

LRESULT CALLBACK CWindow::MessageProc(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam) {
	if(id == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd,id,wParam,lParam);
}

LRESULT CALLBACK CWindow::gWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
//	CWindow* pWindow = (CWindow*)::GetWindowLong(hWnd, GWL_USERDATA);	//	ここにCWindow*を隠しておいた
	CWindow* pWindow = (CWindow*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);	//	ここにCWindow*を隠しておいた
	return (pWindow != NULL) ?	pWindow->MessageProc(hWnd, uMsg, wParam, lParam) : ::DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//	ウィンドゥサイズのadjust処理
BOOL CWindow::Adjust(RECT&rc){
	::SetRect(&rc,0,0,m_Config.width,m_Config.height);
	LONG	lStyle = m_Config.style;
/*
	bool	bMenu = false;
	if (lStyle & WS_SYSMENU){
		//	m_hWndがNULLの状態でGetMenuを呼び出すのは不正？
		if (m_bUseMenu || (m_hWnd!=NULL && ::GetMenu(m_hWnd)!=NULL)){
			bMenu = true;
		} else {
			bMenu = false;
			lStyle &= ~WS_SYSMENU;	//	SYSMENUフラグを外す
		}
	//	lStyle |= WS_CAPTION;	//	しかし、キャプションの指定は行なう（高さが狂うため）
	}
*/
	return ::AdjustWindowRect(&rc,lStyle,FALSE);
}

_ERROR CWindow::Create( int width, int height, LPCTSTR pCaption){//, HWND hParent ) {
	HWND hParent = NULL;
	//CCriticalLock cl(GetCriticalSection());
	// まだ登録されていないウィンドウクラス名で、かつWindows標準クラス
	// 以外の名前の場合はRegisterClassEx()してやる
	HINSTANCE hInst = ::GetModuleHandle(NULL);
    WNDCLASSEX wndclass;

	if( !::GetClassInfoEx(hInst, m_Config.classname.data() , &wndclass) && !IsWindowsClassName(m_Config.classname)) {
        // ウインドウクラス生成処理
		wndclass.cbSize		= sizeof(WNDCLASSEX);
		wndclass.style		= 0;	//	ダブルクリック感知するなら→CS_DBLCLKS;
		wndclass.lpfnWndProc= gWndProc;
		wndclass.cbClsExtra	= 0;
		wndclass.cbWndExtra	= 0;
		wndclass.hInstance	= hInst;
		wndclass.hIcon		= NULL;
		wndclass.hIconSm	= NULL;
		wndclass.hCursor	= LoadCursor(NULL,IDC_ARROW);
		wndclass.hbrBackground = (m_FillColor == 0) ? (HBRUSH)GetStockObject(BLACK_BRUSH) : (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName  = NULL;
		wndclass.lpszClassName = m_Config.classname.data();

		if (!::RegisterClassEx(&wndclass))
			return RIM_ERROR_REGISTWINDOWCLASS;
	}

	LONG style = (hParent != NULL) ? WS_CHILDWINDOW : 0;

	RECT rect = { 0, 0, width, height };
	LONG lStyle = m_Config.style;

	if(::AdjustWindowRect(&rect,lStyle,FALSE) == FALSE)
		::SetRect(&rect, 0, 0, width, height);
//	Adjust(rect);

	m_hWnd = ::CreateWindow( m_Config.classname.data(), pCaption, style | m_Config.style,
				CW_USEDEFAULT, CW_USEDEFAULT, (rect.right - rect.left), (rect.bottom - rect.top),
				hParent, NULL, hInst, NULL);

	if (m_hWnd == NULL)
		return RIM_ERROR_CREATEWINDOW;

	m_Config.width = width;
	m_Config.height = height;

	//	コールバックされたときに、それぞれのWindowClassにdispatch出来るように
	//	GWL_USERDATAにthisを隠しておく。
//	::SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)this);
	::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	//	画面のセンターに生成するように修正
	if(m_Config.bCentering){
		int	sx = ::GetSystemMetrics(SM_CXSCREEN);
		int sy	= ::GetSystemMetrics(SM_CYSCREEN);
		sx	 = (sx - m_Config.width) >> 1;
		sy	 = (sy - m_Config.height)>> 1;
		SetPos(sx,sy);
	}
#if 0
	//	標準のWindowClassならば、コールバック関数として、これを
	//	書き込んでやる。
	if (IsWindowsClassName(m_Config.classname)){
		m_pWndProc = (WNDPROC)::SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)gWndProc);
		//	こういうhookは、illegalか...
	}
	else
		m_pWndProc = NULL;
#endif
	return RIM_ERROR_NONE;
}

//	ウィンドゥサイズのadjust処理
BOOL CWindow::Resize(int sx,int sy) {
	RECT  rc;
	::SetRect(&rc,0,0,sx,sy);

	if( ::AdjustWindowRect(&rc,m_Config.style,FALSE) &&
		::MoveWindow( m_hWnd, 0,0,(rc.right - rc.left), (rc.bottom - rc.top), TRUE ) ) {
		m_Config.width	= sx;
		m_Config.height	= sy;
		return TRUE;
	}
	return FALSE;
}

LRESULT CWindow::SetPos(int x,int y) {
	//フルスクリーン時は動かさないようにすること
	if(m_hWnd){
		//	ここでトップオーダーにしておいても問題ないのだろう...
		return ! ::SetWindowPos(m_hWnd,HWND_TOP,x,y,NULL,NULL,SWP_NOSIZE|SWP_NOZORDER);
	}
	return 0;
}

BOOL CWindow::SetCaption(LPCTSTR name) {
	if( ::SetWindowText(m_hWnd,name) == FALSE)
		return FALSE;
	
	m_Config.caption = name;
	return TRUE;
}

void CWindow::SetCursor(bool bShow){
	//	これはCreateWindowしたスレッドが処理しなくてはならない
	if(m_bShowCursor == bShow)
		return ;

	m_bShowCursor = bShow;
	::ShowCursor(bShow);
}

void CWindow::SetVisible(bool bShow){
	if(bShow){
		::ShowWindow(m_hWnd, SW_SHOW);
		::SetFocus(m_hWnd);
	}
	else
		::ShowWindow(m_hWnd, SW_HIDE);

	m_bShowWindow = bShow;
}
