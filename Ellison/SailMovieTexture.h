/*
	! @file movietex.h
	! @bref	ムービーテクスチャクラス
	! @author	Masafumi TAKAHASHI
*/

#ifndef _SAIL_MOVIETEXTURE_
#define _SAIL_MOVIETEXTURE_

#include <atlbase.h>
#include <dshow.h>
#include "BaseClasses\streams.h"
#include "Rimlia\RimSignal.h"
#include "SailSprite.h"
//#include <d3d9types.h>

#ifdef _DEBUG
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "strmbase.lib")
#endif

//-----------------------------------------------------------------------------
// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

//! @class CMovieTexture 
//! @brief ビデオをDirect3DTexture9に書き込む処理を行うクラス
class CMovieTexture : public CBaseVideoRenderer ,public CTexture {
public:
	CMovieTexture( LPUNKNOWN pUnk, HRESULT *phr ):	CBaseVideoRenderer ( __uuidof(CLSID_TextureRenderer ), NAME ( "Texture Renderer"), pUnk, phr )/*m_bUseDynamicTextures(FALSE)*/ {
    // Store and AddRef the texture for our use.
    ASSERT(phr);
    if (phr)
        *phr = S_OK;
}
	~CMovieTexture();
protected:
	//! @param ダイナミックテクスチャを使うかどうかどうかのフラグ
	//BOOL	m_bUseDynamicTextures;
public:
	HRESULT CheckMediaType	( const CMediaType *pmt );     // Format acceptable?
    HRESULT SetMediaType	( const CMediaType *pmt );       // Video format notification
    HRESULT DoRenderSample	( IMediaSample *pMediaSample ); // New video sample
};

class CMovieSprite : public CSprite3D/*, public CSignal*/ {
public:
		CMovieSprite();	//コンストラクタ
		~CMovieSprite();	//デストラクタ

protected:
	double	m_EndTime;
	DWORD	m_CurrentTime;
	DWORD	m_dwF;
	bool	m_bLoop;
	double	m_Volume;						//ボリューム

	CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder
	CComPtr<IMediaControl>  m_pMC;          // Media Control
	CComPtr<IMediaPosition> m_pMP;          // Media Position
	CComPtr<IMediaEventEx>  m_pME;          // Media Event
	CComPtr<IBaseFilter>    m_pRenderer;    // our custom renderer
	CComPtr<IBasicAudio>	m_pBA;

	void CleanupDShow();

public:
	bool	Play ( WCHAR* Name , BOOL bLoop = FALSE );
	void	CheckMovieStatus();
	//! @fn HRESULT InitDShowTextureRenderer(WCHAR* wFileName, const BOOL bSound)
	//! @brief DirectShowからテクスチャへのレンダリングへの初期化
	//! @param *pd3dDevice (in) Direct3Dデバイス
	//! @param *wFileName (in) ムービーファイルのパス
	//! @param bSound (in) サウンド再生フラグ
	//! @return 関数の成否
	HRESULT InitDShowTextureRenderer( WCHAR* wFileName, const BOOL bSound );
	void	SetVolume(double vol);
	//! @brief ムービーのイベント取得
	//! @param lEventCode (out) イベント コードを受け取る変数へのポインタ
	//! @param lParam1 (out) 第 1 イベント引数を受け取る変数へのポインタ。
	//! @param lParam2 (out) 第 2 イベント引数を受け取る変数へのポインタ。
	//! @param msTimeout (in) タイムアウト時間 (ミリ秒単位)。イベントが到着するまで動作を停止するには、INFINITE を使う。
inline	void	GetEvent(long* lEventCode, long* lParam1, long* lParam2, long msTimeout) { m_pME->GetEvent(lEventCode, lParam1, lParam2, msTimeout); }
inline	void	Play()	{ m_pMC->Run(); };
inline	void	Stop()	{ m_pMC->Stop(); };
inline	void	SetSpeed ( double time ) { m_pMP->put_Rate(time); };	//! @brief 再生スピードの変更	//! @param time (in) 再生スピードの倍率
	//! @brief 現在の再生位置を指定位置にセット //! @param time (in) セットしたい再生位置
inline	void	SetTime(double time) { m_pMP->put_CurrentPosition(time); };
		double	GetStopTime()	//! @brief 終了時間の取得 //! @return 終了時間
	{
		REFTIME time;
		m_pMP->get_StopTime(&time);
		return time;
	};

		double GetDuration() 	//! @brief ストリームの時間幅の取得	//! @return 全ストリーム長
	{
		REFTIME time;
		m_pMP->get_Duration(&time);
		return time;
	}

	double GetCurrentPosition()	//! @brief 現在の再生位置の取得	//! @return 現在の再生位置
	{
		REFTIME time;
		m_pMP->get_CurrentPosition(&time);
		return time;
	}
	bool	IsFinished();

};

#endif