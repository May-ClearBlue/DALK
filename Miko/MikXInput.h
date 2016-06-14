#include <MultiStdAfx.h>

#include <XInput.h>
#pragma comment(lib,"xinput.lib")

#define MAX_CONTROLLERS 2

//ボタン定数
/*
XINPUT_GAMEPAD_DPAD_UP          0x00000001
XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
XINPUT_GAMEPAD_START            0x00000010
XINPUT_GAMEPAD_BACK             0x00000020
XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080
XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
XINPUT_GAMEPAD_A                0x1000
XINPUT_GAMEPAD_B                0x2000
XINPUT_GAMEPAD_X                0x4000
XINPUT_GAMEPAD_Y                0x8000
*/

class CXInputController {
public:
	CXInputController() : m_ID(-1), m_PacketNumber(0)/*m_isActive(FALSE)*/
		{ _ZEROINIT(m_State); }

protected:
//	BOOL		m_isActive;
	DWORD		m_PacketNumber;
	int			m_ID;
XINPUT_STATE			m_State; 
XINPUT_CAPABILITIES		m_Capabilities;
public:
		BOOL	UpdateState() { return ::XInputGetState( m_ID, &m_State ) == ERROR_SUCCESS ? TRUE : FALSE; }
		BOOL	GetCapabilities(){ return ::XInputGetCapabilities( m_ID, XINPUT_FLAG_GAMEPAD, &m_Capabilities) == ERROR_SUCCESS ? TRUE : FALSE; }
inline	BOOL	IsStateChanged() { return m_State.dwPacketNumber != m_PacketNumber; }
inline	WORD	GetButtonState() { return m_State.Gamepad.wButtons; }
};

class CXInput {
protected:
	CXInputController	m_Controller[MAX_CONTROLLERS];
public:
	void				Init();
	inline	CXInputController&	GetInstance(int num) { return m_Controller[num]; }
};
