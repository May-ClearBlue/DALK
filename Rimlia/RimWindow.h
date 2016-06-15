#ifndef _SAIL_CLIENT_BASE_
#define _SAIL_CLIENT_BASE_

#include "MultiStdafx.h"
#include "RimCriticalSection.h"

#ifndef _DALK_WINDOWS_
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#else

#define _DALK_WINCLASSNAME_ _T("_DALK_WINDOWCLASS_")

//通常ウィンドウ情報格納構造体。
struct CWindowConfig {
//public:
	string_t	caption;	///	キャプション
	string_t	classname;	///	クラス名(captionと同じでも良い)
	LPCTSTR dialog;		///	ダイアログの場合は、ここでリソース名を指定すればok

	int		width;
	int		height;
	LONG	style;
	LONG	exstyle;
	bool	bCentering;
};

//Windowsデフォルトのクラス名かどうか
inline bool IsWindowsClassName(const string_t& strClassName){
	return	( strClassName== _T("BUTTON") )		|| ( strClassName == _T("COMBOBOX") )	||
			( strClassName== _T("EDIT") )		|| ( strClassName == _T("LISTBOX") )	||
			( strClassName== _T("MDICLIENT") )	|| ( strClassName == _T("SCROLLBAR") )	||
			( strClassName== _T("STATIC") );
}

//キーボードとマウスメッセージを除去
inline void ClearKeyMessage(HWND hWnd){
	MSG msg;	
	while(	::PeekMessage(&msg,hWnd,WM_KEYFIRST,WM_KEYLAST,PM_REMOVE) ||
			::PeekMessage(&msg,hWnd,WM_MOUSEFIRST,WM_MOUSELAST,PM_REMOVE) )
		Sleep(1);
}

//カーソルクラス
class CCursor{
protected:
	string_t	m_Name;
	int		m_id;
	HCURSOR	m_hCursor;
public:

	CCursor();
	~CCursor()				{ Release(); }
	
		bool	Load(LPCTSTR Name);
		bool	Load(int id);
inline	BOOL	Release()		{ return m_hCursor != NULL ? ::DestroyCursor(m_hCursor) : FALSE; }
inline	HCURSOR	GetHandle()		{ return m_hCursor; }
inline	HCURSOR	Set()			{ return ::SetCursor(m_hCursor); }
};

#if 0
//ウィンドウ基底クラス。CWindowMessangerを継承するのが基本。
class IMesProc {
typedef void (*PMESFUNC)( HWND, UINT, WPARAM, LPARAM );
public:
virtual	LRESULT	MesProc		( HWND, UINT, WPARAM, LPARAM ) = 0;
};

//ウィンドウメッセージ管理クラス
class CMessageManager{
protected:
	typedef std::list<IMesProc*> CMessageList;

	CMessageList		m_MessengeList;
	CCriticalSection	m_CriticalSection;
public:
	void	RegistMessenger(IMesProc* mes);	//	自分自身をフックに加える
	void	DeleteMessenger(IMesProc* mes);	//	自分自身をフックから外す
	void	ClearMessenger();				//	すべてをクリアする
	LRESULT Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc = NULL);
};
#endif

//ウィンドウクラス
class CWindow {
public:
		CWindow();
		~CWindow();
//virtual	void Bind(HSQUIRRELVM vm);

protected:
	CWindowConfig	m_Config;			//	ウィンドゥオプション
	bool			m_bShowCursor;		//	マウスカーソルの表示状態
	bool			m_bShowWindow;		//
	DWORD			m_FillColor;		//	背景色
//
		HWND				m_hWnd;			//	ウィンドウハンドル
//		WNDPROC				m_pWndProc;		//　ウィンドウメッセージ処理
virtual	LRESULT	CALLBACK	MessageProc(HWND hWnd, UINT id, WPARAM wParam, LPARAM lParam);

public:

static	LRESULT	CALLBACK	gWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//	BOOL	ChangeWindowStyle(DWORD style,DWORD exstyle);
		_ERROR	Create		( int width, int height, LPCTSTR pCaption ); //, HWND hParent = NULL );
		BOOL	Resize		( int sx, int sy );
		BOOL	Adjust		( RECT& );

inline	void	Destroy()	{ ::DestroyWindow(m_hWnd); m_hWnd = NULL; }
			//setter
		BOOL	SetCaption	( LPCTSTR name );
		LRESULT	SetPos		( int x, int y );
		void	SetCursor	( bool bShow );
		void	SetVisible	( bool bShow );
//getter
inline	HWND	GetHWnd()	{ return m_hWnd; }
CWindowConfig*	GetWindowConfig()	{ return &m_Config; }
};


#if defined _DALK_USE_SQUIRREL_

#undef _CLASS_NAME_
#define _CLASS_NAME_ CWindow

SQUIRREL_BEGIN_BIND(CWindow, "Window")
	SQUIRREL_MEMBERFUNC_BIND(CWindow::Create, "Create")
	SQUIRREL_MEMBERFUNC_BIND(CWindow::Resize, "Resize")
	SQUIRREL_MEMBERFUNC_BIND(CWindow::Adjust, "Adjust")
	SQUIRREL_MEMBERFUNC_BIND(CWindow::SetCaption, "SetCaption")
	SQUIRREL_MEMBERFUNC_BIND(CWindow::SetPos, "SetPos")
	SQUIRREL_MEMBERFUNC_BIND(CWindow::SetCursor, "SetCursos")
	SQUIRREL_MEMBERFUNC_BIND(CWindow::SetVisible, "SetVisible")
SQUIRREL_END_BIND(CWindow);

#endif

#endif

#endif