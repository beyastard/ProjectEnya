/*
 * FILE: AMVideoClip.cpp
 *
 * DESCRIPTION: The class standing for one video clip in Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/5/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */

#include "AMVideoClip.h"
#include "AMSurfaceRenderer.h"
#include "AMPI.h"

AMVideoClip::AMVideoClip()
{
	m_pAMVideoEngine	= NULL;
	m_pSurfaceRenderer	= NULL;
	m_pScreenSprite		= NULL;

	m_state				= AMVIDEOCLIP_STATE_STOPPED;

	m_pGraphBuilder		= NULL;
	m_pMediaControl		= NULL;
	m_pMediaEvent		= NULL;
	m_pMediaPosition	= NULL;

	m_bHasSound = false;
	m_bLoop = false;
}

AMVideoClip::~AMVideoClip()
{
}

DWORD dwROTReg = 0xfedcba98;

HRESULT AddToROT(IUnknown *pUnkGraph) 
{
	return 0;
    IMoniker * pmk;
    IRunningObjectTable *pirot;
    if (FAILED(GetRunningObjectTable(0, &pirot))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x  pid %08x", (DWORD_PTR) 0, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pmk);
    if (SUCCEEDED(hr)) {
        hr = pirot->Register(0, pUnkGraph, pmk, &dwROTReg);
        pmk->Release();
    }
    pirot->Release();
    return hr;
}

bool AMVideoClip::Init(A3DDevice * pA3DDevice, AMVideoEngine * pAMVideoEngine, char * szVideoFile)
{
    HRESULT hr = S_OK;

    IPin *		            pSRPinIn;			// Surface Renderer Input Pin
	IPin *					pVRPinIn;			// Default video renderer's input pin
	IPin *					pVRConnectedPin;	// Connected pin with default video renderer's input pin

    InitializeCriticalSection(&m_SurfaceAccess);

    // Create the filter graph
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                    IID_IGraphBuilder, (void **)&m_pGraphBuilder);

#ifdef _DEBUG
    // Register the graph in the Running Object Table (for debug purposes)
    AddToROT(m_pGraphBuilder);
#endif
    
	strncpy(m_szClipName, szVideoFile, MAX_PATH);

    WCHAR wFileName[MAX_PATH];
    #ifndef UNICODE
        MultiByteToWideChar(CP_ACP, 0, szVideoFile, -1, wFileName, MAX_PATH);
    #else
        lstrcpy(wFileName, szVideoFile);
    #endif

	// Auto build the whole filter graph;
	if FAILED(hr = m_pGraphBuilder->RenderFile(wFileName, NULL))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not build filter graph! hr=0x%x", hr));
		return false;
	}

	// Enumerate filters sample
	/* 
	IEnumFilters *		pEnum = NULL;
    IBaseFilter *		pFilter;
    ULONG cFetched;
    m_pGraphBuilder->EnumFilters(&pEnum);
    while( pEnum->Next(1, &pFilter, &cFetched) == S_OK )
    {
        FILTER_INFO FilterInfo;
        char szName[256];
        
        pFilter->QueryFilterInfo(&FilterInfo);
        WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0);
        MessageBox(NULL, szName, "Filter name", MB_OK);

        FilterInfo.pGraph->Release();
        pFilter->Release();
    }
    pEnum->Release();
	*/

	// Now find the video renderer filter;
	IBaseFilter *		pVRFilter;
	if FAILED(hr = m_pGraphBuilder->FindFilterByName(L"Video Renderer", &pVRFilter))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not create find the video renderer filter!  hr=0x%x", hr));
        return false;
	}

	// Find the default video renderer's input pin; in fact you can use FindPin instead
	IEnumPins  *pEnumPins;
    IPin       *pPin;
    pVRFilter->EnumPins(&pEnumPins);
    while( pEnumPins->Next(1, &pPin, 0) == S_OK )
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
		if( PinDirThis == PINDIR_INPUT )
		{
			/*
			LPWSTR id;
			pPin->QueryId(&id);
			char szID[256];
			WideCharToMultiByte(CP_ACP, 0, id, -1, szID, 256, 0, 0);
			*/
			break;
		}
        pPin->Release();
    }
    pEnumPins->Release();
	pVRPinIn = pPin;

	// Find the connected pin of default video renderer's input pin;
	if FAILED(hr = pVRPinIn->ConnectedTo(&pVRConnectedPin))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not get connected pin of VRPinIn!  hr=0x%x", hr));
        return false;
	}

	// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	// If this addfilter is called before renderfile, 
	// the default video renderer will not be used!!!
	m_pSurfaceRenderer = new AMSurfaceRenderer(NULL, &hr);
    if (FAILED(hr))                                      
    {
        AMERRLOG(("AMVideoClip::Init(), Could not create AMSurfaceRenderer object! hr=0x%x", hr));
		return false;
    }

    if FAILED(hr = m_pGraphBuilder->AddFilter(m_pSurfaceRenderer, L"SURFACERENDERER"))
    {
        AMERRLOG(("AMVideoClip::Init(), Could not add renderer filter to graph!  hr=0x%x", hr));
        return false;
    }
    
	// Find the surface renderer's input pin
    if FAILED(hr = m_pSurfaceRenderer->FindPin(L"In", &pSRPinIn))
    {
        AMERRLOG(("AMVideoClip::Init(), Could not find input pin!  hr=0x%x", hr));
        return false;
    }

    if FAILED(hr = m_pGraphBuilder->Disconnect(pVRPinIn))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not disconnect pin of VRPinIn!  hr=0x%x", hr));
        return false;
	}

	if FAILED(hr = m_pGraphBuilder->Disconnect(pVRConnectedPin))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not disconnect connected pin of VRPinIn!  hr=0x%x", hr));
        return false;
	}

    // Connect these two filters, the pin connected with VR, and the input pin of texture renderer filter;
    if (FAILED(hr = m_pGraphBuilder->Connect(pVRConnectedPin, pSRPinIn)))
    {
        AMERRLOG(("AMVideoClip::Init(), Could not connect dsound renderer!  hr=0x%x", hr));
		return false;
    }

	// Get rid of unneccessary interface;
	pSRPinIn->Release();
	pVRPinIn->Release();
	pVRConnectedPin->Release();
	pVRFilter->Release();
	m_pGraphBuilder->RemoveFilter(pVRFilter);

	// Get the graph's media control, event & position interfaces
	m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
	m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (void **)&m_pMediaEvent);
	m_pGraphBuilder->QueryInterface(IID_IMediaPosition, (void **)&m_pMediaPosition);

	m_nWidth = m_pSurfaceRenderer->GetVidWidth();
	m_nHeight = m_pSurfaceRenderer->GetVidHeight();
	m_VideoFormat = m_pSurfaceRenderer->GetVidFormat();

	m_pA3DDevice = pA3DDevice;
	m_pAMVideoEngine = pAMVideoEngine;

	// Now we must create a surface that will be the destination surface for rendering
	A3DRECT rectVideo;
	rectVideo.left		= 0; 
	rectVideo.top		= 0;  
	rectVideo.right		= m_nWidth;
	rectVideo.bottom	= m_nHeight;

	m_pScreenSprite = new A2DSprite();
	if( NULL == m_pScreenSprite )
	{
		AMERRLOG(("AMVideoClip::Init(), Not enough memory!"));
		return false;
	}

	if( !m_pScreenSprite->InitWithoutSurface(m_pA3DDevice, m_nWidth, m_nHeight, m_VideoFormat, 1, &rectVideo) )
	{
		AMERRLOG(("AMVideoClip::Init(), Can not init a sprite from the DestSurface!"));
		return false;
	}

	m_state = AMVIDEOCLIP_STATE_STOPPED;

	return true;
}

bool AMVideoClip::Release()
{
	Stop();

	DeleteCriticalSection(&m_SurfaceAccess);
	if( m_pScreenSprite )
	{
		m_pScreenSprite->Release();
		delete m_pScreenSprite;
		m_pScreenSprite = NULL;
	}

	if( m_pMediaPosition )
	{
		m_pMediaPosition->Release();
		m_pMediaPosition = NULL;
	}
	if( m_pMediaEvent )
	{
		m_pMediaEvent->Release();
		m_pMediaEvent = NULL;
	}
	if( m_pMediaControl )
	{
		m_pMediaControl->Release();
		m_pMediaControl = NULL;
	}
	if( m_pGraphBuilder )
	{
		m_pGraphBuilder->Release();
		m_pGraphBuilder = NULL;
	}

	m_pAMVideoEngine = NULL;
	return true;
}

bool AMVideoClip::Start(bool bLoop)
{
	m_bLoop = bLoop;

	// Now let the surface renderer aim at out dest surface;
	m_pSurfaceRenderer->SetDestParameter(m_pScreenSprite, &m_SurfaceAccess);

	if FAILED(m_pMediaControl->Run())
	{
		AMERRLOG(("AMVideoClip::Start(), Can IMediaControl::Run() fail!"));
		return false;
	}

	m_state = AMVIDEOCLIP_STATE_PLAYING;
	return true;
}

bool AMVideoClip::Stop()
{
	if( m_pMediaControl )
	{
		if FAILED(m_pMediaControl->Stop())
		{
			AMERRLOG(("AMVideoClip::Stop(), Can IMediaControl::Stop() fail!"));
			return false;
		}
	}

	if( m_pSurfaceRenderer )
		m_pSurfaceRenderer->SetDestParameter(NULL, NULL);

	m_state = AMVIDEOCLIP_STATE_STOPPED;
	return true;
}

bool AMVideoClip::Tick()
{
	long evCode, param1, param2;
	HRESULT hr;

	if( m_pMediaEvent == NULL )
		return true;

	while( hr = m_pMediaEvent->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr) )
	{
		if( EC_COMPLETE == evCode )
		{ 
			if( m_bLoop )
			{
				hr = m_pMediaPosition->put_CurrentPosition(0);
				goto NEXT;
			}

			Stop();
			break;
		} 
		else if( EC_USERABORT == evCode )
		{
			// We will not handle this, because the user can not stop the
			// video playback directly;
		}

NEXT:
		hr = m_pMediaEvent->FreeEventParams(evCode, param1, param2);
	}
	return true;
}

bool AMVideoClip::DrawToBack()
{
	if( m_pScreenSprite )
	{
		EnterCriticalSection(&m_SurfaceAccess);
		m_pScreenSprite->DrawToBack();
		LeaveCriticalSection(&m_SurfaceAccess);
		return true;
	}
	return true;
}

bool AMVideoClip::SetAutoScaleFlags(DWORD dwFlags)
{
	if( (dwFlags & AMVID_FIT_X) || (dwFlags & AMVID_FIT_Y) )
	{
		int nNewWidth = m_nWidth, nNewHeight = m_nHeight;
		
		if( dwFlags & AMVID_FIT_X )
		{
			nNewWidth = m_pA3DDevice->GetDevFormat().nWidth;
		}
		if( dwFlags & AMVID_FIT_Y )
		{
			nNewHeight = m_pA3DDevice->GetDevFormat().nHeight;
		}

		FLOAT vScaleX = nNewWidth * 1.0f / m_nWidth;
		FLOAT vScaleY = nNewHeight * 1.0f / m_nHeight;
		// Try to keep the contraint and center the frame;
		if( (dwFlags & AMVID_FIT_X ) && (dwFlags & AMVID_FIT_Y) &&
			(dwFlags & AMVID_FIT_KEEPCONSTRAINT) )
		{
			if( fabs(vScaleX - vScaleY) > 1e-3 )
			{
				A3DPOINT2 ptPos;
				ptPos.x = ptPos.y = 0;

				// We have to use the smaller scale factor;
				if( vScaleX > vScaleY )
				{
					vScaleX = vScaleY;
					ptPos.x = int((nNewWidth - m_nWidth * vScaleX) / 2);
				}
				else
				{
					vScaleY = vScaleX;
					ptPos.y = int((nNewHeight - m_nHeight * vScaleY) / 2);
				}
				SetVidPos(ptPos);
			}
		}
		SetScaleFactors(vScaleX, vScaleY);
	}
	else
	{

	}
	return true;
}

bool AMVideoClip::SetScaleFactors(FLOAT vScaleX, FLOAT vScaleY)
{
	m_vScaleX = vScaleX;
	m_vScaleY = vScaleY;
	
	if( m_pScreenSprite )
	{
		m_pScreenSprite->SetScaleX(m_vScaleX);
		m_pScreenSprite->SetScaleY(m_vScaleY);

		if( (m_vScaleX > 1.0f && fabs(m_vScaleX - 1.0f) > 1e-3f) ||
			(m_vScaleY > 1.0f && fabs(m_vScaleY - 1.0f) > 1e-3) )
		{
			m_pScreenSprite->SetLinearFilter(true);
		}
		else
		{
			m_pScreenSprite->SetLinearFilter(false);
		}

	}
	return true;
}

bool AMVideoClip::SetColor(A3DCOLOR color)
{
	m_color = color;

	if( m_pScreenSprite )
		m_pScreenSprite->SetColor(m_color);
	return true;
}

bool AMVideoClip::SetVidPos(A3DPOINT2 ptVidPos)
{
	m_ptVidPos = ptVidPos;

	if( m_pScreenSprite )
		m_pScreenSprite->SetPosition(m_ptVidPos.x, m_ptVidPos.y);
	return true;
}

bool AMVideoClip::SetBlendMode(A3DBLEND srcBlend, A3DBLEND destBlend)
{
	if( m_pScreenSprite )
	{
		m_pScreenSprite->SetSrcBlend(srcBlend);
		m_pScreenSprite->SetDestBlend(destBlend);
	}
	return true;
}