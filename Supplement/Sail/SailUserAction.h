#ifndef _SAIL_USER_ACTION_
#define _SAIL_USER_ACTION_

#include <MultiStdAfx.h>

enum DALK_DEVICE {
	DEVICE_KEYBOARD,		//マウスも含む
	DEVICE_TOUCH,
	DEVICE_ACTIVE,
	DEVICE_PAD0,
};

enum DALK_ACTION {
	ACTION_NUM0,
	ACTION_MAX = 128,
};

enum DALK_ACTION_STATE{
	ACTION_STATE_NONE,
	ACTION_STATE_ON,
	ACTION_STATE_OFF,
	ACTION_STATE_REPEAT,
};

enum DALK_INPUT_ANALOG {
	ACTION_ANALOG0,
	ACTION_ANALOGMAX = 8,
};

const int INPUT_NONE = 0;

enum DALK_INPUT_TOUCH {
	INPUT_TOUCH_NONE,
	INPUT_TAP,
	INPUT_DOUBLETAP,
	INPUT_PAN,
	INPUT_ZOOM,
	INPUT_ROTATE
};

enum DALK_INPUT_PAD {
	INPUT_PAD_NONE,
	INPUT_CURSOR_UP,
	INPUT_CURSOR_DOWN,
	INPUT_CURSOR_LEFT,
	INPUT_CURSOR_RIGHT,
	INPUT_BUTTON_0 = 8,
	INPUT_BUTTON_MAX = 63,	
	INPUT_BUTTON2_0 = 64,
	INPUT_BUTTON2_MAX = 127
};

#define _DALK_PAD_NUMMAX_ 2
#define _DALK_REPEAT_BEGIN_WAIT_FRAME 15

namespace UserAction {

//Result
	BYTE	m_ResultBuf[DEVICE_PAD0+1][256];
//Action
	BYTE*	m_ActionMap[256];
//
	DALK_ACTION_STATE	m_ActionState[256];

inline void SetAction			(int action, int input, DALK_DEVICE device)			{ m_ActionMap[device][input] = action; }
inline void SetKeyBoardAction	(int action, int input)								{ m_ActionMap[action] = &m_ResultBuf[DEVICE_KEYBOARD][input]; }
inline void SetTouchAction		(int action, DALK_INPUT_TOUCH input)				{ m_ActionMap[action] = &m_ResultBuf[DEVICE_TOUCH][input]; }
inline void SetPadAction		(int action, DALK_INPUT_PAD input, size_t num)		{ m_ActionMap[action] = &m_ResultBuf[DEVICE_PAD0 + num][input]; }

enum _COMMAND_ADV_DEFAULT_ {
	ACTION_UP,
	ACTION_DOWN,
	ACTION_LEFT,
	ACTION_RIGHT,
	ACTION_START,
	ACTION_OK,
	ACTION_CANCEL,
	ACTION_FORCESKIP,
	ACTION_SKIPMODE,
	ACTION_AUTOMODE,
	ACTION_MENU,
	ACTION_BACKLOG,
	ACTION_QUICKSAVE,
	ACTION_MOVIECANCEL,
	ACTION_STD_A,
	ACTION_STD_B,
	ACTION_STD_X,
	ACTION_STD_Y,
	ACTION_ALBUMDEF,
	ACTION_ALBUMADJUST,
	ACTION_TIPS,
	ACTION_MOBILE,
	ACTION_MESNEXT,

	ACTION_ALBUMNPAGE,
	ACTION_ALBUMBPAGE,
	ACTION_ALBUMOK,
	ACTION_ALBUMCANCEL,

	ACTION_TIPSMENU_UP,
	ACTION_TIPSMENU_DOWN,
	ACTION_TIPSMENU_LEFT,
	ACTION_TIPSMENU_RIGHT,
	ACTION_TIPSPAGE_UP,
	ACTION_TIPSPAGE_DOWN,
	ACTION_TIPSPAGE_LEFT,
	ACTION_TIPSPAGE_RIGHT,
	ACTION_ADV_MAX
};

inline void SetADVStyle() {
	SetKeyBoardAction(ACTION_UP,		VK_UP);
	SetKeyBoardAction(ACTION_DOWN,		VK_DOWN);
	SetKeyBoardAction(ACTION_LEFT,		VK_LEFT);
	SetKeyBoardAction(ACTION_RIGHT,		VK_RIGHT);
	SetKeyBoardAction(ACTION_OK,		VK_RETURN);
	SetKeyBoardAction(ACTION_CANCEL,	VK_ESCAPE);
	SetKeyBoardAction(ACTION_MENU,		VK_HOME);
	SetKeyBoardAction(ACTION_FORCESKIP,		VK_END);
	SetKeyBoardAction(ACTION_BACKLOG,	VK_BACK);
}

void InitADVAction() {
	_ZEROINIT(m_ResultBuf);
	_ZEROINIT(m_ActionMap);
	_ZEROINIT(m_ActionState);
	SetADVStyle();
}

void GetKeyBoardData() {
	BYTE TempBuf[256];
	::GetKeyboardState(TempBuf);

	for(int a = 0;a < ACTION_ADV_MAX;a ++) {
//ToDo:NULLチェック
		if(TempBuf[*m_ActionMap[a]] == 0) {
			*m_ActionMap[a] = 0;
		}
		else if(*m_ActionMap[a] >= _DALK_REPEAT_BEGIN_WAIT_FRAME)
			*m_ActionMap[a] ++;
	}
}

}; //namespace UserAction

#endif