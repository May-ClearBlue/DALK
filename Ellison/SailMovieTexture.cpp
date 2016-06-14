#include "Multistdafx.h"

#pragma warning(disable:4244)

/*
	! @file movietex.cpp
	! @bref	ムービーテクスチャクラス
	! @author	Masafumi TAKAHASHI
*/

#include "SailMovieTexture.h"

#define WM_GRAPHNOTIFY  WM_USER+40

//デストラクタ
CMovieTexture::~CMovieTexture() {

}

//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT CMovieTexture::CheckMediaType(const CMediaType *pmt) {
	HRESULT hr = E_FAIL;
	VIDEOINFO *pvi=0;

	CheckPointer(pmt,E_POINTER);

	// Reject the connection if this is not a video type
	if( *pmt->FormatType() != FORMAT_VideoInfo )
		{ return E_INVALIDARG; }

	// Only accept RGB24 video
	pvi = (VIDEOINFO *)pmt->Format();

	if( IsEqualGUID( *pmt->Type(), MEDIATYPE_Video )  && IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24 ) )
		{ hr = S_OK; }

	return hr;
}

//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made. 
//-----------------------------------------------------------------------------
HRESULT CMovieTexture::SetMediaType(const CMediaType *pmt) {
	VIDEOINFO *pviBmp;                      // Bitmap info header
	pviBmp = (VIDEOINFO *)pmt->Format();

	Create( pviBmp->bmiHeader.biWidth, abs(pviBmp->bmiHeader.biHeight ), D3DFMT_X8R8G8B8 );

	return S_OK;
}

//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT CMovieTexture::DoRenderSample( IMediaSample * pSample ) {
	BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer
	//LONG  lTxtPitch;                // Pitch of bitmap, texture

	BYTE  * pbS = NULL;
	DWORD * pdwS = NULL;
	DWORD * pdwD = NULL;
	UINT row, col, dwordWidth;
    
	CheckPointer( pSample,E_POINTER );
	CheckPointer( m_pTexture,E_UNEXPECTED );

	// Get the video bitmap buffer
	pSample->GetPointer( &pBmpBuffer );

	Lock();
	pTxtBuffer = (BYTE*)m_Info.pData;
	pTxtBuffer += (m_Info.Y - 1) * m_Info.Pitch;

	// Copy the bits    

//    if (m_TextureFormat == D3DFMT_X8R8G8B8) {
        // Instead of copying data bytewise, we use DWORD alignment here.
        // We also unroll loop by copying 4 pixels at once.
        //
        // original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
        //
        // aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
        //
        // We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
        // below, bitwise operations do exactly this.

		dwordWidth = m_Info.X /*m_lVidWidth*/ / 4; // aligned width of the row, in DWORDS
                                      // (pixel by 3 bytes over sizeof(DWORD))
		UINT lVidPitch = (m_Info.X * 3 + 3) & ~(3);

		for( row = 0; row< (UINT)m_Info.Y/*m_lVidHeight*/; row++) {
            pdwS = ( DWORD*)pBmpBuffer;
            pdwD = ( DWORD*)pTxtBuffer;

            for( col = 0; col < dwordWidth; col ++ )
            {
                pdwD[0] =  pdwS[0] | 0xFF000000;
                pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
                pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
                pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
                pdwD +=4;
                pdwS +=3;
            }

            // we might have remaining (misaligned) bytes here
            pbS = (BYTE*) pdwS;
            for( col = 0; col < (UINT)m_Info.X/*m_lVidWidth*/ % 4; col++)
            {
                *pdwD = 0xFF000000     |
                        (pbS[2] << 16) |
                        (pbS[1] <<  8) |
                        (pbS[0]);
                pdwD++;
                pbS += 3;           
            }

            pBmpBuffer += lVidPitch;
			pTxtBuffer -= m_Info.Pitch;//lTxtPitch;
        }// for rows
//    }
/*
    if (m_TextureFormat == D3DFMT_A1R5G5B5) 
    {
        for(int y = 0; y < m_lVidHeight; y++ ) 
        {
            BYTE *pBmpBufferOld = pBmpBuffer;
            BYTE *pTxtBufferOld = pTxtBuffer;   

            for (int x = 0; x < m_lVidWidth; x++) 
            {
                *(WORD *)pTxtBuffer = (WORD)
                    (0x8000 +
                    ((pBmpBuffer[2] & 0xF8) << 7) +
                    ((pBmpBuffer[1] & 0xF8) << 2) +
                    (pBmpBuffer[0] >> 3));

                pTxtBuffer += 2;
                pBmpBuffer += 3;
            }

            pBmpBuffer = pBmpBufferOld + m_lVidPitch;
            pTxtBuffer = pTxtBufferOld + lTxtPitch;
        }
    }
*/
	Unlock();
/*
    // Unlock the Texture
    if (FAILED(m_pTexture->UnlockRect(0)))
        return E_FAIL;
*/    
    return S_OK;
}


//コンストラクタ
CMovieSprite::CMovieSprite() {
	m_pGB = NULL;
	m_pMC = NULL;
	m_pMP = NULL;
	m_pME = NULL;
	m_pRenderer = NULL;
	m_Volume = 0;
//	m_fu = m_fv = 1.0f;
}

//デストラクタ
CMovieSprite::~CMovieSprite() {
	CleanupDShow();
	//m_pTexture = NULL;
/*
	if ( m_pTexture ) {
		delete (CMovieTexture*)m_pTexture;
		m_pTexture = NULL;
	}
*/
}

//! @brief DirectShowからテクスチャへのレンダリングへの初期化
//! @param *pd3dDevice (in) Direct3Dデバイス
//! @param *wFileName (in) ムービーファイルのパス
//! @param bSound (in) サウンド再生フラグ
//! @return 関数の成否
HRESULT CMovieSprite::InitDShowTextureRenderer( WCHAR* wFileName, const BOOL bSound ) {
    HRESULT hr = S_OK;
    CComPtr<IBaseFilter>    pFSrc;			// Source Filter
    CComPtr<IPin>           pFSrcPinOut;	// Source Filter Output Pin   
    CMovieTexture			*pCTR = NULL;	// DirectShow Texture renderer

	// フィルタグラフの作成
	if (FAILED(m_pGB.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
		{ return E_FAIL; }

#ifdef REGISTER_FILTERGRAPH
	// Register the graph in the Running Object Table (for debug purposes)
	AddToROT(m_pGB);
#endif

	// Create the Texture Renderer object
	m_pTexture = new CMovieTexture( NULL, &hr );
	pCTR = (CMovieTexture*)m_pTexture;	

	// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	
	m_pRenderer = pCTR;

	if (FAILED(hr = m_pGB->AddFilter(m_pRenderer, L"TextureRenderer"))) {
		//Msg(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);
		return hr;
	}

	// Add the source filter to the graph.
	hr = m_pGB->AddSourceFilter (wFileName, L"SOURCE", &pFSrc);

	// If the media file was not found, inform the user.
	if (hr == VFW_E_NOT_FOUND) {
		//Msg(TEXT("Could not add source filter to graph!  (hr==VFW_E_NOT_FOUND)\r\n\r\n")
		//TEXT("This sample reads a media file from the DirectX SDK's media path.\r\n")
		//TEXT("Please install the DirectX 9 SDK on this machine."));
		return hr;
	}
	else if(FAILED(hr)) {
		//Msg(TEXT("Could not add source filter to graph!  hr=0x%x"), hr);
        return hr;
	}

	if (SUCCEEDED(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut)))     {
		//AVIとかMPEG1とかVFWでいけるやつ
		//オーディオトラックを再生しない場合　NO_AUDIO_RENDERER　を定義
		if(!bSound) {
			// If no audio component is desired, directly connect the two video pins
			// instead of allowing the Filter Graph Manager to render all pins.
			CComPtr<IPin> pFTRPinIn;      // Texture Renderer Input Pin

			// Find the source's output pin and the renderer's input pin
			if (FAILED(hr = pCTR->FindPin(L"In", &pFTRPinIn))) {
				//Msg(TEXT("Could not find input pin!  hr=0x%x"), hr);
				return hr;
			}

			// Connect these two filters
			if (FAILED(hr = m_pGB->Connect(pFSrcPinOut, pFTRPinIn))) {
				//Msg(TEXT("Could not connect pins!  hr=0x%x"), hr);
				return hr;
			}
		}
		else {
			// Render the source filter's output pin.  The Filter Graph Manager
			// will connect the video stream to the loaded CTextureRenderer
			// and will load and connect an audio renderer (if needed).
			if (FAILED(hr = m_pGB->Render(pFSrcPinOut))) {
				//Msg(TEXT("Could not render source output pin!  hr=0x%x"), hr);
				return hr;
			}
		}
	}
	else {
		hr = m_pGB.QueryInterface(&m_pBA);
		//WMVとかMPEG2とか
		hr = m_pGB->RenderFile( wFileName, NULL );
		if(FAILED(hr))
			return hr;
		if(!bSound) {
			// 無音
			hr = m_pBA->put_Volume(-10000);
		}
		else {
			//音の出力
			hr = m_pBA->put_Volume(0);
		}
	}

	// Get the graph's media control, event & position interfaces
	m_pGB.QueryInterface(&m_pMC);
	m_pGB.QueryInterface(&m_pMP);
	m_pGB.QueryInterface(&m_pME);

	m_pME->SetNotifyWindow( NULL, WM_GRAPHNOTIFY, 0 );

	// Start the graph running;
	if (FAILED(hr = m_pMC->Run())) {
		//Msg(TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr);
		return hr;
	}

	//テクスチャの取得
	//m_pTexture = pCTR;//->GetTexture();
	//pCTR->GetVideoDesc(&m_lWidth, &m_lHeight, &m_lPitch);

	//D3DSURFACE_DESC desc;
	//m_pTexture->GetLevelDesc(0, &desc);

	//テクスチャが2の累乗でしか確保できない場合に余剰分を表示しないためのUV計算
	//m_fu = (FLOAT)m_lWidth / (FLOAT)desc.Width;
	//m_fv = (FLOAT)m_lHeight / (FLOAT)desc.Height;

	return S_OK;
}


//-----------------------------------------------------------------------------
// CheckMovieStatus: If the movie has ended, rewind to beginning
//-----------------------------------------------------------------------------
void CMovieSprite::CheckMovieStatus(void) {
	long lEventCode;
	long lParam1;
	long lParam2;
	HRESULT hr;

	if (!m_pME)
		return;
        
	// Check for completion events
	hr = m_pME->GetEvent(&lEventCode, (LONG_PTR *) &lParam1, (LONG_PTR *) &lParam2, 0);

	if (SUCCEEDED(hr)) {
        // If we have reached the end of the media file, reset to beginning
		if (EC_COMPLETE == lEventCode)  
			{ hr = m_pMP->put_CurrentPosition(0); }
        // Free any memory associated with this event
		hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
	}
}

//-----------------------------------------------------------------------------
// CleanupDShow
//-----------------------------------------------------------------------------
void CMovieSprite::CleanupDShow(void) {
#ifdef REGISTER_FILTERGRAPH
    // Pull graph from Running Object Table (Debug)
    RemoveFromROT();
#endif
    // Shut down the graph
    if (!(!m_pMC)) m_pMC->Stop();

	//各種オブジェクトの解放
    if (!(!m_pBA)) m_pBA.Release();
    if (!(!m_pMC)) m_pMC.Release();
    if (!(!m_pME)) m_pME.Release();
    if (!(!m_pMP)) m_pMP.Release();
    if (!(!m_pGB)) m_pGB.Release();
    if (!(!m_pRenderer)) m_pRenderer.Release(); m_pTexture = NULL;

}

void CMovieSprite::SetVolume(double vol){
	double v =  ( 20.0 * log10( m_Volume / 100.0 ) ) * 100.0;
	if(v > 0.0)
		v = 0.0;
	if(v < -10000.0)
		v = -10000.0;
	m_pBA->put_Volume( (long) v);
	m_Volume = vol;
}

bool CMovieSprite::Play( WCHAR* Name , BOOL bLoop ) {
	//Off();

	if (InitDShowTextureRenderer( Name, TRUE ) == S_OK ) {
		m_EndTime = GetStopTime();	//ムービーの終了時間

		CTextureInfo& oInfo = GetTextureInfo();

		FLOATRECT pm = { 0, 0, m_Info.X, m_Info.Y };
		FLOATRECT uv = { 0, 0, m_Info.X, m_Info.Y };

		SetVertex( &pm, &uv );

		Play();	//再生
	/*

		HRESULT hRes;

		while( Wait(0) == 0 ) {
			//ムービーの再生時間が終了した場合
			if( m_MoviePlayer.GetCurrentPosition() >= m_EndTime ) {
				m_MoviePlayer.Stop();		//停止
				if ( bLoop )
					{ On(); return 1; }
				m_MoviePlayer.SetTime(0.0);	//タイムラインを開始位置にセット
				m_MoviePlayer.Play();		//再生
			}
			//hRes = Render();	//描画
		}
	*/
		return true;
	}
	return false;
}

bool CMovieSprite::IsFinished() {
	//ムービーの再生時間が終了した場合
	if( GetCurrentPosition() >= m_EndTime ) {
		Stop();		//停止
		if ( m_bLoop )
			{ return true; }
		SetTime(0.0);	//タイムラインを開始位置にセット
		Play();		//再生
	}
	return false;
	//hRes = Render();	//描画
}