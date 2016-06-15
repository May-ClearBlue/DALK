#ifndef _SAIL_JOYSTICK_
#define _SAIL_JOYSTICK_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include "MultiStdAfx.h"
//#include "Sail\SailLogReporter.h"
//#include "Rimlia\RimWindowBase.h"

#ifndef _SAIL_MACHINTYPE_CONSOLE_
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#else
#include "host-win32\include\libpad.h"
#define LPDIRECTINPUT8 void*
#define LPDIRECTINPUTDEVICE8 void*
#define REFGUID const GUID&

struct DIJOYSTATE { BYTE buf[256]; };
struct DIMOUSESTATE { BYTE buf[256]; };

#endif

//#define _USE_DX_MOUSE_
#define _USE_DX_JOYSTICK_
#define _USE_DX_KEYBOARD_

#define AXIS_RANGE 1024
#define JOYSTICK_RANGE_MAX 1000
#define JOYSTICK_RANGE_MIN -1000

//PS3:CellPadInfo2のmax_connect
#define _SIG_PAD_MAX_CONNECT_ 7

//BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,void *pContext);

//-------------------------------インスタンス
class CDirectInput {
protected:
static	LPDIRECTINPUT8		m_pDirectInput;
public:
	CDirectInput()						{ m_pDirectInput = NULL; }
	~CDirectInput()						{ Release(); }
static	LPDIRECTINPUT8&	GetInstance()	{ return m_pDirectInput; }
static		BOOL		Initialize		( HINSTANCE hInstance = NULL );
static		BOOL		Release();
};

class IInputBase {
public:
virtual	void*	GetInputState() = 0;
virtual	void*	GetLastInputState() = 0;
virtual	void	Poll( HWND hWnd = NULL ) = 0;
};

//-------------------------------デバイス基底クラス
class CDirectInputDeviceBase : public IInputBase/*, public CDirectXBase*/ {
public:
	LPDIRECTINPUTDEVICE8	m_pDevice;
	BOOL					m_bAcquired;
	string_t					m_DeviceName;
	DWORD					m_DeviceType;

	union{
		BYTE				m_KeyBuffer[256];
		DIMOUSESTATE		m_MouseData;
		DIJOYSTATE			m_JoyStickData;
#ifdef _SAIL_TARGET_PS3_
		CellPadData			m_PadData;
#endif
	};
	union{
		BYTE				m_LastKeyBuffer[256];
		DIMOUSESTATE		m_LastMouseData;
		DIJOYSTATE			m_LastJoyStickData;
#ifdef _SAIL_TARGET_PS3_
		CellPadData			m_LastPadData;
#endif
	};

public:
		CDirectInputDeviceBase()	{ m_pDevice = NULL; m_bAcquired = FALSE; }
//		~CDirectInputDeviceBase()	{ /*Release();*/ }

		string_t&	GetDeviceName()	{ return m_DeviceName; }
		BOOL	IsAcquired()	{ return m_bAcquired; }
		BOOL	Create( HWND hWnd, REFGUID guid, LPCTSTR DeviceName );
		BOOL	SetBufferSize(DWORD BufferSize);
		void	Acquire();
		void	Unaquire();
//		void	Release();
};

#ifndef _SAIL_MACHINTYPE_CONSOLE_

class CDirectInputKeyBoard : public CDirectInputDeviceBase {
public:
inline	void*	GetInputState()			{ return m_KeyBuffer; }
inline	void*	GetLastInputState()		{ return m_LastKeyBuffer; }
inline	void	Poll(HWND hWnd = NULL)	{ memcpy( m_LastKeyBuffer, m_KeyBuffer, 256); if( m_pDevice->GetDeviceState(256,&m_KeyBuffer) == DIERR_INPUTLOST) m_pDevice->Acquire(); }
};

class CDirectInputMouse : public CDirectInputDeviceBase {
public:
inline	void*	GetInputState()			{ return &m_MouseData; }
inline	void*	GetLastInputState()		{ return &m_LastMouseData; }
inline	void	Poll(HWND hWnd = NULL)	{ memcpy( &m_LastMouseData, &m_MouseData, sizeof(DIMOUSESTATE) ); if( m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE),&m_MouseData) == DIERR_INPUTLOST) m_pDevice->Acquire(); }
};

#endif

class CDirectInputJoyStick : public CDirectInputDeviceBase {
protected:
	BOOL	m_bAttached;
	DWORD	m_Axes;
	DWORD	m_Buttons;

	BYTE	m_AxisData[4];		//※上下左右の順番で格納
	BYTE	m_LastAxisData[4];
public:
inline	BOOL	IsAttached()		{ return m_bAttached; }
inline	void*	GetInputState()		{ return &m_JoyStickData; }
inline	void*	GetLastInputState()	{ return &m_LastJoyStickData; }
inline	BYTE*	GetAxisData()		{ return m_AxisData; }
inline	BYTE*	GetLasrAxisData()	{ return m_LastAxisData; }
		void	GetCapabilities();
		void	Poll(HWND hWnd = NULL);
//inline	void	GetBufferData()					{ /*m_pDevice->GetDeviceData(DWORD cbObjectData,LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut,DWORD dwFlags);*/ }
//PS3
		void	Clear();

};

static vector<CDirectInputJoyStick*>g_JoySticks;

#if 0

	g_pDirectInput->GetInstance()->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumDirectInputDevicesCallback, &hWnd, DIEDFL_ALLDEVICES );

#ifdef _USE_DX_KEYBOARD_
	m_KeyBoard.Create( hWnd, GUID_SysKeyboard,"SysKeyboard" );
	m_KeyBoard.Acquire();
#endif

#ifdef _USE_DX_MOUSE_
	m_Mouse.Create( hWnd, pDirectInput->GetInstance(), GUID_SysMouse,"SysMouse" );
	m_Mouse.Acquire();
#endif

#endif

//HWNDのアドレスをpThisに
#ifndef _SAIL_MACHINTYPE_CONSOLE_
	BOOL CALLBACK EnumDirectInputDevicesCallback( LPCDIDEVICEINSTANCE pDeviceInstance, LPVOID pThis );
#endif

#endif