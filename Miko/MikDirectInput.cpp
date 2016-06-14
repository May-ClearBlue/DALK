#include "Multistdafx.h"

#include "SigDirectInput.h"

BOOL CDirectInput::Initialize ( HINSTANCE hInstance ) {
#ifndef _SAIL_MACHINTYPE_CONSOLE_
	if(hInstance == NULL)
		hInstance = ::GetModuleHandle(NULL);

	if(SUCCEEDED(DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&m_pDirectInput,NULL) ) )
		return TRUE;
	return FALSE;
#endif

#ifdef _SAIL_TARGET_PS3_
//CELL_PAD_ERROR_INVALID_PARAMETER  パラメータの設定値が不正
//CELL_PAD_ERROR_ALREADY_INITIALIZED 既に初期化済み(初回時のリソースは継続されます)
//CELL_PAD_ERROR_RESOURCE_ALLOCATION_FAILED リソースの確保に失敗した

	DWORD num = 1;
	if(hInstance != NULL)
		num = _SIG_PAD_MAX_CONNECT_;

	int32_t res = cellPadInit(num);

	if(res == CELL_PAD_OK)
		return true;

	return false;

#endif
}

BOOL CDirectInput::Release() {
#ifndef _SAIL_MACHINTYPE_CONSOLE_
	SafeRelease(m_pDirectInput);
	return true;
#else
	//int32_t
	cellPadEnd();
	return true;
#endif
}

BOOL CDirectInputDeviceBase::Create( HWND hWnd, REFGUID guid , LPCTSTR DeviceName ) {
#ifndef _SAIL_MACHINTYPE_CONSOLE_

	if( FAILED(CDirectInput::GetInstance()->CreateDevice( guid, &m_pDevice ,NULL) ) )
		return FALSE;

	if(guid == GUID_SysKeyboard){
		m_pDevice->SetDataFormat(&c_dfDIKeyboard);
		//キーボードは排他アクセス不可
		m_pDevice->SetCooperativeLevel( hWnd , DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
		_DEBUGPRINTF(_TEXT("キーボードデバイスの初期化\n"));
	}
	else if(guid == GUID_SysMouse){
		m_pDevice->SetDataFormat(&c_dfDIMouse2);
		//m_pDevice->SetCooperativeLevel(hWnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		_DEBUGPRINTF(_TEXT("マウスデバイスの初期化\n"));
	}
	else{
		//device->SetDataFormat(&c_dfDIJoystick2);
		m_pDevice->SetDataFormat(&c_dfDIJoystick);
		m_pDevice->SetCooperativeLevel( hWnd ,DISCL_EXCLUSIVE | DISCL_FOREGROUND );
		//m_pDevice->EnumObjects( EnumAxesCallback, (void *)pDevice, DIDFT_AXIS );

		DIPROPDWORD JoyStickWord;
		DIPROPRANGE JoyStickRange;

		JoyStickRange.diph.dwSize		= sizeof(DIPROPRANGE);
		JoyStickWord.diph.dwSize		= sizeof(DIPROPDWORD);	/* ジョイスティック設定 */
		JoyStickWord.diph.dwHeaderSize	= JoyStickRange.diph.dwHeaderSize
										= sizeof(DIPROPHEADER);
		JoyStickWord.diph.dwHow			= JoyStickRange.diph.dwHow = DIPH_BYOFFSET;

		JoyStickWord.dwData = 5000;					/* 入力しきい値 (0-10000) Dinput仕様*/
		JoyStickRange.lMin = JOYSTICK_RANGE_MIN;	/* 入力値範囲 */
		JoyStickRange.lMax = JOYSTICK_RANGE_MAX;

		/* Ｘ軸の設定 */
		JoyStickWord.diph.dwObj	= JoyStickRange.diph.dwObj
								= DIJOFS_X;
		if( FAILED( m_pDevice->SetProperty(DIPROP_RANGE, &JoyStickRange.diph) ) || FAILED( m_pDevice->SetProperty(DIPROP_DEADZONE, &JoyStickWord.diph) ) )
			return FALSE;

		/* Ｙ軸の設定 */
		JoyStickWord.diph.dwObj = JoyStickRange.diph.dwObj
								= DIJOFS_Y;
		
		if( FAILED( m_pDevice->SetProperty(DIPROP_RANGE, &JoyStickRange.diph) ) || FAILED( m_pDevice->SetProperty(DIPROP_DEADZONE, &JoyStickWord.diph) ) )
			return FALSE;

		_DEBUGPRINTF(_TEXT("ジョイスティックデバイスの作成\n"));
	}

	m_DeviceName = DeviceName;
	m_bAcquired = FALSE;

	return TRUE;
#else
	return true;
#endif
}

BOOL CDirectInputDeviceBase::SetBufferSize(DWORD BufferSize){
#ifndef _SAIL_MACHINTYPE_CONSOLE_
    DIPROPDWORD diprop = { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER) }; 
	diprop.diph.dwObj	= 0;
	diprop.diph.dwHow	= DIPH_DEVICE;
	diprop.dwData		= BufferSize;

	return SUCCEEDED( m_pDevice->SetProperty(DIPROP_BUFFERSIZE, &diprop.diph) );
#else
	return true;
#endif
}

void CDirectInputDeviceBase::Acquire(){
#ifndef _SAIL_MACHINTYPE_CONSOLE_
	if(m_bAcquired == FALSE && m_pDevice != NULL && m_pDevice->Acquire() == DI_OK)
		m_bAcquired = TRUE;
#endif
}

void CDirectInputDeviceBase::Unaquire(){
#ifndef _SAIL_MACHINTYPE_CONSOLE_
	if(m_bAcquired == TRUE && m_pDevice != NULL && m_pDevice->Unacquire() == DI_OK)
		m_bAcquired = FALSE;
#endif
}
/*
void CDirectInputDeviceBase::Release(){
	Unaquire();
	m_pDevice->Release();
}
*/
void CDirectInputJoyStick::GetCapabilities(){
#ifndef _SAIL_MACHINTYPE_CONSOLE_
	DIDEVCAPS oDeviceCaps;

	m_pDevice->GetCapabilities(&oDeviceCaps);
	m_Axes		= oDeviceCaps.dwAxes;
	m_Buttons	= oDeviceCaps.dwButtons;
	m_bAttached	= (oDeviceCaps.dwFlags & DIDC_ATTACHED) ? TRUE : FALSE;
#endif
}

void CDirectInputJoyStick::Poll( HWND hWnd ){
#ifndef _SAIL_MACHINTYPE_CONSOLE_
	memcpy( &m_LastJoyStickData, &m_JoyStickData, sizeof(DIJOYSTATE) );
	*(int*)m_LastAxisData = *(int*)m_LastAxisData;

	if( m_pDevice->Poll() == DIERR_INPUTLOST){
		m_pDevice->Acquire();

		*(int*)m_AxisData = 0x00000000;
		ZeroMemory( m_JoyStickData.rgbButtons, sizeof(m_JoyStickData.rgbButtons) );
	}
	else{
		if( m_pDevice->Poll() == DIERR_NOTACQUIRED)
			m_pDevice->Acquire();

		m_pDevice->GetDeviceState(sizeof(DIJOYSTATE),&m_JoyStickData);

		*(int*)m_AxisData = 0x00000000;

		//Ｘ軸判定
		if( m_JoyStickData.lX > (JOYSTICK_RANGE_MAX / 2) )
			m_AxisData[3] = 0x80;
		else if( m_JoyStickData.lX < (JOYSTICK_RANGE_MIN / 2) )
			m_AxisData[2] = 0x80;
		//Ｙ軸判定
		if( m_JoyStickData.lY < (JOYSTICK_RANGE_MIN / 2) )
			m_AxisData[0] = 0x80;
		else if( m_JoyStickData.lY > (JOYSTICK_RANGE_MAX / 2) )
			m_AxisData[1] = 0x80;
	}
#else
	//uint32_t
	//cellPadDbgGetData
	cellPadGetData(m_DeviceType, &m_PadData);

#endif
}

void CDirectInputJoyStick::Clear(){
	//int32_t
#ifdef _SAIL_TARGET_PS3_
	cellPadClearBuf(m_DeviceType);
#endif
}

#ifndef _SAIL_MACHINTYPE_CONSOLE_

BOOL CALLBACK EnumDirectInputDevicesCallback(LPCDIDEVICEINSTANCE pDeviceInstance,LPVOID pThis) {
	HWND hWnd = *(HWND*)pThis; 

	CDirectInputJoyStick* pJoyStick = new CDirectInputJoyStick;

	if( pJoyStick->Create( hWnd, pDeviceInstance->guidInstance, pDeviceInstance->tszProductName ) ){
		pJoyStick->GetCapabilities();

		string_t OutStr = _T("デバイス名 = \"");
		OutStr += pJoyStick->GetDeviceName();
		OutStr += _T("\"\n");
		::OutputDebugString( OutStr.data() );

		g_JoySticks.push_back(pJoyStick);
		return DIENUM_CONTINUE; 
	}

	delete pJoyStick;

	return DIENUM_STOP;
}
#endif
/*
BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,void *pContext ){
	//軸の範囲設定
	DIPROPRANGE diprg = { sizeof(DIPROPRANGE), sizeof(DIPROPHEADER) };
	diprg.diph.dwHow = DIPH_BYID; 
	diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
	diprg.lMin = AXIS_RANGE * -1; 
	diprg.lMax = AXIS_RANGE;

	LPDIRECTINPUTDEVICE8 lpJoy = (LPDIRECTINPUTDEVICE8)pContext;

	if( FAILED(lpJoy->SetProperty(DIPROP_RANGE,&diprg.diph) ) )
		return DIENUM_STOP;

	return DIENUM_CONTINUE;
}
*/
/*
typedef struct JOYSTICKDATA
{ 
　LONG lX; // X軸
　LONG lY; // Y軸 
　BYTE Button1; // ボタン1
　BYTE Button2; // ボタン2
　BYTE Button3; // ボタン3
　BYTE Button4; // ボタン4
}
JOYSTICKDATA, *LPJOYSTICKDATA; // DWORDアライメント必須 

まずこれが大元のジョイスティック用のデータです。
特にどうということは在りません。

DIOBJECTDATAFORMAT JoystickDataFormat[] =
{ 
　{ &GUID_XAxis, FIELD_OFFSET(JOYSTICKDATA, lX), DIDFT_AXIS | DIDFT_ANYINSTANCE, 0, }, 
　{ &GUID_YAxis, FIELD_OFFSET(JOYSTICKDATA, lY), DIDFT_AXIS | DIDFT_ANYINSTANCE, 0, }, 
　{ &GUID_Button, FIELD_OFFSET(JOYSTICKDATA, Button1), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0, },
　{ &GUID_Button, FIELD_OFFSET(JOYSTICKDATA, Button2), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0, },
　{ &GUID_Button, FIELD_OFFSET(JOYSTICKDATA, Button3), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0, },
　{ &GUID_Button, FIELD_OFFSET(JOYSTICKDATA, Button4), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0, },
};

DIDATAFORMAT DIJoystickDataFormat = { 
　sizeof(DIDATAFORMAT), // この構造体のサイズ
　sizeof(DIOBJECTDATAFORMAT), // オブジェクト データ形式のサイズ
　DIDF_ABSAXIS, // 絶対軸の座標
　sizeof(JOYSTICKDATA), // デバイス データのサイズ
　6, // オブジェクト数
　JoystickDataFormat, // データ位置
}; 

まずDIOBJECTDATAFORMAT JoystickDataFormat[]で
ジョイスティック用の構造体のどこにどのデータが入るか設定しています。

次にDIDATAFORMAT DIJoystickDataFormatで詳細な
データを設定しています。

この辺は定型処理であとは欲しいデータを追加していくだけですから、
一度作ってしまえば後は簡単です。

lpDevJoystick[i]->SetDataFormat( &DIJoystickDataFormat );
で実際に設定します。

void RefreshJoystick( void )
{
　for ( DWORD i = 0; i < JoystickCount; i++ )
　{
　　if ( lpDevJoystick != NULL )
　　{
　　　// ポーリングが必要な場合
　　　if ( bPolling[i] ) lpDevJoystick[i]->Poll();

　　　HRESULT hr = lpDevJoystick[i]->GetDeviceState( sizeof(JOYSTICKDATA), &JoystickData[i] );
　　　if SUCCEEDED( hr )
　　　{
　　　　continue;
　　　}
　　　// デバイス再アクセス
　　　lpDevJoystick[i]->Acquire();
　　　ZeroMemory( &JoystickData[i], sizeof(JOYSTICKDATA) );
　　}
　}
}
*/