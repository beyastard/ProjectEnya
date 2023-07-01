#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DErrLog.h"
#include "A3DFuncs.h"
#include "A3DCamera.h"
#include "A3DCursor.h"
#include "A3DRenderTarget.h"
#include "A3DViewport.h"
#include "A3DLightMan.h"
#include "AList.h"
#include "A3DConfig.h"

bool GetProperDisplayFreq(DWORD dwWidth, DWORD dwHeight, DWORD dwBitsPerPixel, DWORD * pdwFrequency)
{
	int indexMode = 0;
	DWORD dwHighestCertFreq = 0;

	DEVMODE WorkingDM;

	while( EnumDisplaySettings(NULL, indexMode, &WorkingDM) )
	{
		if( dwWidth == WorkingDM.dmPelsWidth 
			&& dwHeight == WorkingDM.dmPelsHeight 
			&& dwBitsPerPixel == WorkingDM.dmBitsPerPel )
		{
			if( dwHighestCertFreq < WorkingDM.dmDisplayFrequency )
			{
				if( DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&WorkingDM, CDS_TEST) )
				{
					dwHighestCertFreq = WorkingDM.dmDisplayFrequency;
					if( dwHighestCertFreq >= 85 )
						break;
				}
			}
		}
		
		indexMode ++;
	}

	*pdwFrequency = dwHighestCertFreq;
	return true;
}

A3DDevice::A3DDevice()
{
	m_bHWIDevice	= false;
	m_A3DDevType	= A3DDEVTYPE_HAL;

	m_hDeviceWnd	= NULL;
	m_pD3DDevice	= NULL;
	m_pBackBuffer	= NULL;
	m_pRenderTarget = NULL;
	m_pDepthBuffer  = NULL;
	m_pA3DEngine	= NULL;

	m_dwGammaLevel	= 100;

	m_A3DCullType	= A3DCULL_CCW;
	m_bTLDriver		= false;

	m_bShowCursor	= false;
	m_pA3DCursor	= NULL;

	m_bFogEnable		= false;
	m_bFogTableEnable	= false;

	// Flag to indicate whether the application has lost focus;
	m_bSwitchOff	= false;

	ZeroMemory(&m_CurrentMode, sizeof(m_CurrentMode));
	ZeroMemory(&m_DesktopMode, sizeof(m_DesktopMode));
}

A3DDevice::~A3DDevice()
{
}

bool A3DDevice::InitAsHWI(A3DEngine * pA3DEngine, HWND hDeviceWnd, A3DDEVFMT * pA3DDevFmt, A3DDEVTYPE devType, bool bAllowMultiThread, bool bForceWindowSwap)
{
	m_bHWIDevice = true;

	// Retrieve current desktop display mode and keep it;
	HDC					hdc = GetDC(NULL);
	m_DesktopMode.nBitsPerPixel		= GetDeviceCaps(hdc, BITSPIXEL);
	m_DesktopMode.nFrequency		= GetDeviceCaps(hdc, VREFRESH);
	m_DesktopMode.nWidth			= GetSystemMetrics(SM_CXSCREEN);
	m_DesktopMode.nHeight			= GetSystemMetrics(SM_CYSCREEN);
	ReleaseDC(NULL, hdc);

	m_pA3DEngine = pA3DEngine;
	m_hDeviceWnd = hDeviceWnd;
	m_A3DDevType = devType;
	
	//Keep them;
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth			= pA3DDevFmt->nWidth;
	d3dpp.BackBufferHeight			= pA3DDevFmt->nHeight;
	d3dpp.Windowed					= bForceWindowSwap ? true : pA3DDevFmt->bWindowed;
	d3dpp.hDeviceWindow				= hDeviceWnd;
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	d3dpp.BackBufferFormat			= D3DFMT_X8R8G8B8;
	m_d3dpp = d3dpp;

	pA3DDevFmt->fmtTarget			= A3DFMT_X8R8G8B8;
	pA3DDevFmt->fmtDepth			= A3DFMT_D16;
	m_A3DDevFmt	= *pA3DDevFmt;

	// Get Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;

	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}

	ConfigDevice();

	m_ListDynamicStreams.Init();
	return true;
}

bool A3DDevice::Init(A3DEngine * pA3DEngine, HWND hDeviceWnd, A3DDEVFMT * pA3DDevFmt, A3DDEVTYPE devType, bool bAllowMultiThread, bool bForceWindowSwap)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return InitAsHWI(pA3DEngine, hDeviceWnd, pA3DDevFmt, devType, bAllowMultiThread, bForceWindowSwap);

	D3DDISPLAYMODE		d3ddm;
	HRESULT				hval;

	// Init some members;
	m_pA3DEngine = pA3DEngine;
	m_hDeviceWnd = hDeviceWnd;
	m_bForceWindowSwap = bForceWindowSwap;
	m_A3DDevType = devType;
	
	// First let's find what is 16-bit target format and what is 32-bit target format;
	m_fmtTarget16 = A3DFMT_UNKNOWN;
	m_fmtTarget32 = A3DFMT_UNKNOWN;
	hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_R5G6B5, D3DFMT_R5G6B5, false);
	if( D3D_OK != hval )
	{
		hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X1R5G5B5, D3DFMT_X1R5G5B5, false);
		if( D3D_OK != hval )
		{
			g_pA3DErrLog->ErrLog("Can not determine the 16-bit target format!");
			return false;
		}
		else
			m_fmtTarget16 = A3DFMT_X1R5G5B5;
	}
	else
		m_fmtTarget16 = A3DFMT_R5G6B5;
	hval = hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false);
	if( D3D_OK == hval )
		m_fmtTarget32 = A3DFMT_X8R8G8B8;

	// Retrieve current desktop display mode and keep it;
	HDC					hdc = GetDC(NULL);
	m_DesktopMode.nBitsPerPixel		= GetDeviceCaps(hdc, BITSPIXEL);
	m_DesktopMode.nFrequency		= GetDeviceCaps(hdc, VREFRESH);
	m_DesktopMode.nWidth			= GetSystemMetrics(SM_CXSCREEN);
	m_DesktopMode.nHeight			= GetSystemMetrics(SM_CYSCREEN);
	
	if( !GetDeviceGammaRamp(hdc, (LPVOID)&m_DesktopGammaRamp) )
	{
		// If fail, we use a default gamma ramp table;
		for(int i=0; i<256; i++)
			m_DesktopGammaRamp.red[i] = m_DesktopGammaRamp.green[i] = m_DesktopGammaRamp.blue[i] = (WORD)min(65535, i * 255);
	}
	ReleaseDC(NULL, hdc);

	hval = m_pA3DEngine->GetD3D()->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("Can not get adapter display mode");
		return false;
	}
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth	= pA3DDevFmt->nWidth;
	d3dpp.BackBufferHeight	= pA3DDevFmt->nHeight;
	d3dpp.Windowed			= bForceWindowSwap ? true : pA3DDevFmt->bWindowed;
	d3dpp.hDeviceWindow		= hDeviceWnd;
	if( m_pA3DEngine->IsFSMotionBlur() )
		d3dpp.SwapEffect		= D3DSWAPEFFECT_COPY;
	else if( pA3DDevFmt->bWindowed || bForceWindowSwap )
		d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	else 
		d3dpp.SwapEffect		= D3DSWAPEFFECT_FLIP;

	d3dpp.BackBufferCount	= 1;
	
	if( pA3DDevFmt->bWindowed )
	{
		d3dpp.BackBufferFormat = d3ddm.Format;
	}
	else
	{
		if( pA3DDevFmt->fmtTarget == 0 )
		{
			// Use 32-bit first
			if( m_fmtTarget32 != A3DFMT_UNKNOWN )
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget32;
			else
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget16;
		}
		else 
		{
			if( pA3DDevFmt->fmtTarget == A3DFMT_X1R5G5B5 || pA3DDevFmt->fmtTarget == A3DFMT_R5G6B5 )
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget16;
			else
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget32;
		}
	}
	d3dpp.EnableAutoDepthStencil = TRUE;

TryAgain:
	if( d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat);
	if FAILED(hval)
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
	{
		hval = m_pA3DEngine->GetD3D()->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, d3dpp.BackBufferFormat, 
			d3dpp.BackBufferFormat, d3dpp.AutoDepthStencilFormat);
		if FAILED(hval)
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	
	}

	g_pA3DErrLog->ErrLog("Chosen Device Format: BackBuffer [%s], DepthStencil [%s]", GetFormatString((A3DFORMAT)d3dpp.BackBufferFormat), GetFormatString((A3DFORMAT)d3dpp.AutoDepthStencilFormat));

	// If using force swap, we have to set display mode using GDI function;
	if( !pA3DDevFmt->bWindowed && bForceWindowSwap )
	{
		// We have to use GDI function to set the display setting and 
		// only thus can we use window swap chain;
		DWORD dwFreq = 0;
		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(devMode));
		devMode.dmSize = sizeof(devMode);
		if( d3dpp.BackBufferFormat == (D3DFORMAT) A3DFMT_X8R8G8B8 )
			devMode.dmBitsPerPel = 32;
		else
			devMode.dmBitsPerPel = 16;
		devMode.dmPelsWidth = pA3DDevFmt->nWidth;
		devMode.dmPelsHeight = pA3DDevFmt->nHeight;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Get a proper display frequency
		GetProperDisplayFreq(devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, &dwFreq);
		if( dwFreq )
		{
			// For windows 98, all enumerated frequency is 0, but 
			// if you set devMode.dmDisplayFrequency to 0, it will be switched
			// to 60 Hz
			// if you did not supply DM_DISPLAYFREQUENCY, it will be changed
			// to current display settings of that width and height
			devMode.dmDisplayFrequency = dwFreq;
			devMode.dmFields |= DM_DISPLAYFREQUENCY; 
		}
		
		LONG ret = ChangeDisplaySettings(&devMode, 0);
		if( ret != DISP_CHANGE_SUCCESSFUL )
		{
			g_pA3DErrLog->ErrLog("A3DDevice::Init(), Can not change display mode!");
			return false;
		}

		m_CurrentMode.nWidth = devMode.dmPelsWidth;
		m_CurrentMode.nHeight = devMode.dmPelsHeight;
		m_CurrentMode.nBitsPerPixel = devMode.dmBitsPerPel;
		m_CurrentMode.nFrequency = dwFreq;

		g_pA3DErrLog->ErrLog("A3DDevice::Init(), We have changed desktop setting to [%dx%dx%d %dHz] by GDI function!", 
			devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, dwFreq);
	}

	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));	
	hval = m_pA3DEngine->GetD3D()->GetDeviceCaps(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, &m_d3dcaps);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::Init(), GetDeviceCaps() Fail.");
		return false;
	}

	DWORD		dwFlags = NULL;
	if( bAllowMultiThread )
		dwFlags = D3DCREATE_MULTITHREADED;

	if( m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::Init(), Chosen T&L Device...");
		hval = m_pA3DEngine->GetD3D()->CreateDevice(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
			dwFlags | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
		m_bTLDriver = true;
	}
	else
	{
		g_pA3DErrLog->ErrLog("A3DDevice::Init(), Chosen SOFTWARE Vertex Processing...");
		hval = m_pA3DEngine->GetD3D()->CreateDevice(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
			dwFlags | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
		m_bTLDriver = false;
	}
	if( D3D_OK != hval )
	{
		char szErrorCode[1024];
		D3DXGetErrorString(hval, szErrorCode, 1024);
		g_pA3DErrLog->ErrLog("Can not create device, reason: %s", szErrorCode);

		if( hval == D3DERR_OUTOFVIDEOMEMORY )
		{
			if( d3dpp.BackBufferFormat == D3DFMT_X8R8G8B8 || d3dpp.BackBufferFormat == D3DFMT_A8R8G8B8 )
			{
				hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, 
					(D3DDEVTYPE) m_A3DDevType, D3DFMT_R5G6B5, D3DFMT_R5G6B5, false);
				if FAILED(hval) 
					d3dpp.BackBufferFormat = D3DFMT_X1R5G5B5;
				else
					d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
				g_pA3DErrLog->ErrLog("Now switch to 16-bit color depth, and try again...");
				goto TryAgain;
			}
		}
		
		return false;
	}

	// Log chosen format;
	g_pA3DErrLog->ErrLog("A3DDevice::Init(), Create device %dx%dx%d", pA3DDevFmt->nWidth, pA3DDevFmt->nHeight, pA3DDevFmt->fmtTarget == A3DFMT_X8R8G8B8 ? 32 : 16);

	// Keep them;
	m_d3dpp = d3dpp;

	pA3DDevFmt->fmtTarget = (A3DFORMAT) d3dpp.BackBufferFormat;
	pA3DDevFmt->fmtDepth = (A3DFORMAT) d3dpp.AutoDepthStencilFormat;
	m_A3DDevFmt	= *pA3DDevFmt;

	//Get some surface pointers;
	hval = m_pD3DDevice->GetRenderTarget(&m_pRenderTarget);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("Can not get rendertarget pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetDepthStencilSurface(&m_pDepthBuffer);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("Can not get depth stencil surface pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("Can not get backbuffer pointer!");
		return false;
	}

	// Get Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;

	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}
	ConfigDevice();

	// For the first time, we set the fog data;
	SetFogEnable(true);
	SetFogTableEnable(true);
	SetFogColor(A3DCOLORRGBA(255, 255, 255, 255));
	SetFogStart(500.0f);
	SetFogEnd(800.0f);
	SetFogDensity(1.0f);

	m_ListDynamicStreams.Init();
	return true;
}

bool A3DDevice::Release()
{
	m_ListDynamicStreams.Release();

	if( m_pRenderTarget )
	{
		m_pRenderTarget->Release();
		m_pRenderTarget = NULL;
	}
	if( m_pDepthBuffer )
	{
		m_pDepthBuffer->Release();
		m_pDepthBuffer = NULL;
	}
	if( m_pBackBuffer )
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}
	if( m_pD3DDevice )
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	RestoreDesktopDisplayMode();

	return true;
}

bool A3DDevice::ConfigDevice()
{
	if( m_bHWIDevice )
		return true;

	HRESULT hval;

	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));
	hval = m_pD3DDevice->GetDeviceCaps(&m_d3dcaps);
	if FAILED(hval) return false;

	SetFaceCull(A3DCULL_CCW);

	//We use depth buffer;
	SetZTestEnable(true);
	SetZWriteEnable(true);

	//Initialize the device variables;
	SetAlphaBlendEnable(true);
	SetSourceAlpha(A3DBLEND_SRCALPHA);
	SetDestAlpha(A3DBLEND_INVSRCALPHA);

	SetDitherEnable(true);
	SetSpecularEnable(false); // If you want to make specular enable, you must set it explictly
	
	SetTextureFilterType(0, A3DTEXF_LINEAR);
	SetTextureColorOP(0, A3DTOP_MODULATE);
	SetTextureAlphaOP(0, A3DTOP_MODULATE);
	
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	return true;
}

bool A3DDevice::SetWorldMatrix(const A3DMATRIX4& matWorld)
{
	if( m_bHWIDevice )	return true;

	if FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld))
	{
		g_pA3DErrLog->ErrLog("D3DDevice SetTransform D3DTS_WORLD Failure"); 
		return false;
	}

	m_matWorldMatrix = matWorld;
	return true;
}

bool A3DDevice::SetViewMatrix(const A3DMATRIX4& matView)
{
	if( m_bHWIDevice )	return true;

	if FAILED(m_pD3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&matView))
	{
		g_pA3DErrLog->ErrLog("D3DDevice SetTransform D3DTS_VIEW Failure"); 
		return false;
	}

	m_matViewMatrix = matView;
	return true;
}

bool A3DDevice::SetProjectionMatrix(const A3DMATRIX4& matProject)
{
	if( m_bHWIDevice )	return true;

	if FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&matProject))
	{
		g_pA3DErrLog->ErrLog("D3DDevice SetTransform D3DTS_PROJECT Failure"); 
		return false;
	}

	m_matProjectionMatrix = matProject;
	return true;
}

bool A3DDevice::SetTextureMatrix(int iStage, const A3DMATRIX4& matTexture)
{
	if( m_bHWIDevice )	return true;

	iStage += D3DTS_TEXTURE0;
	if FAILED(m_pD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)iStage, (D3DMATRIX *)&matTexture))
	{
		g_pA3DErrLog->ErrLog("D3DDevice SetTextureMatrix Failure"); 
		return false;
	}

	return true;
}

bool A3DDevice::BeginRender()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->BeginScene();
	if FAILED(hval)
	{
		g_pA3DErrLog->ErrLog("A3DDevcie BeginScene Failure!");
		return false;
	}

	m_dwDrawVertCount = 0;
	m_dwDrawFaceCount = 0;
	m_dwDrawCount = 0;
	
	return true;
}

bool A3DDevice::EndRender()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	
	// We should show cursor here;
	if( m_pA3DCursor && m_bShowCursor )
	{
		m_pA3DCursor->TickAnimation();
		m_pA3DCursor->Draw();
	}

	hval = m_pD3DDevice->EndScene();
	if FAILED(hval)
	{
		g_pA3DErrLog->ErrLog("A3DDevcie EndScene Failure!");
		return false;
	}

	return true;
}

bool A3DDevice::Present()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
	if FAILED(hval)
	{
		if( hval == D3DERR_DEVICELOST )
		{
			hval = m_pD3DDevice->TestCooperativeLevel();
			if( hval == D3DERR_DEVICENOTRESET )
			{
				Reset();
				return true;
			}	
			else
			{
				return true;
			}
		}
		else
		{
			g_pA3DErrLog->ErrLog("A3DDevice: Present Failure!");
			return false;
		}
	}
	return true;
}

bool A3DDevice::Clear(DWORD dwFlags, A3DCOLOR color, FLOAT vZ, DWORD dwStencil)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->Clear(0, NULL, dwFlags, color, vZ, dwStencil);
	if FAILED(hval)
	{
		g_pA3DErrLog->ErrLog("A3DDevice: Clear() Failure!");
		return false;
	}

	return true;
}

bool A3DDevice::SetTextureFilterType(int nTextureStage, A3DTEXTUREFILTERTYPE filterType)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_MAGFILTER, (D3DTEXTUREFILTERTYPE) filterType);
	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_MINFILTER, (D3DTEXTUREFILTERTYPE) filterType);

	if( g_pA3DConfig->GetFlagTrilinear() )
		m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_MIPFILTER, (D3DTEXTUREFILTERTYPE) filterType);
	else
		m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	
	return true;
}

bool A3DDevice::SetTextureColorOP(int nTextureStage, A3DTEXTUREOP op)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_COLOROP, (D3DTEXTUREOP) op);
	return true;
}

bool A3DDevice::SetTextureColorArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_COLORARG1, dwArg1);
	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_COLORARG2, dwArg2);
	return true;
}

bool A3DDevice::SetTextureAlphaOP(int nTextureStage, A3DTEXTUREOP op)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_ALPHAOP, (D3DTEXTUREOP) op);
	return true;
}

bool A3DDevice::SetTextureAlphaArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_ALPHAARG1, dwArg1);
	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_ALPHAARG2, dwArg2);
	return true;
}

bool A3DDevice::SetTextureAddress(int nTextureStage, A3DTEXTUREADDR UAddrMethod, A3DTEXTUREADDR VAddrMethod)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_ADDRESSU, (D3DTEXTUREADDRESS) UAddrMethod);
	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_ADDRESSV, (D3DTEXTUREADDRESS) VAddrMethod);
	return true;
}

bool A3DDevice::SetTextureCoordIndex(int nTextureStage, int iIndex)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_TEXCOORDINDEX, iIndex);
	return true;
}

bool A3DDevice::SetTextureBumpMat(int nTextureStage, float _00, float _01, float _10, float _11)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT00, *((DWORD*)(&_00)));
	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT01, *((DWORD*)(&_01)));
	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT10, *((DWORD*)(&_10)));
	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT11, *((DWORD*)(&_11)));
	return true;
}

bool A3DDevice::SetTextureBumpScale(int nTextureStage, float fScale)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_BUMPENVLSCALE, *((DWORD*)(&fScale)));
	return true;
}

bool A3DDevice::SetTextureBumpOffset(int nTextureStage, float fOffset)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_BUMPENVLOFFSET, *((DWORD*)(&fOffset)));
	return true;
}

bool A3DDevice::SetTextureTransformFlags(int nTextureStage, A3DTEXTURETRANSFLAGS Flags)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, Flags);
	return true;
}

bool A3DDevice::SetZTestEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, bEnable ? TRUE : FALSE);
	return true;								
}

bool A3DDevice::SetZWriteEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, bEnable ? TRUE : FALSE);
	return true;								
}

bool A3DDevice::SetZBias(DWORD dwBias)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ZBIAS, dwBias);
	return true;
}

bool A3DDevice::SetZFunc(DWORD dwFunc)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ZFUNC, dwFunc);
	return true;
}

bool A3DDevice::SetAlphaBlendEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetSourceAlpha(A3DBLEND alphaBlendParam)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, alphaBlendParam);
	return true;
}

bool A3DDevice::SetDestAlpha(A3DBLEND alphaBlendParam)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, alphaBlendParam);
	return true;
}

bool A3DDevice::SetAlphaTestEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetAlphaFunction(A3DCMPFUNC cmpFunc)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, cmpFunc);
	return true;
}

bool A3DDevice::SetAlphaRef(DWORD dwRefValue)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, dwRefValue);
	return true;
}

bool A3DDevice::SetLightingEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetLightParam(int nLightID, A3DLIGHTPARAM * pLightParam)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetLight(nLightID, (D3DLIGHT8 *)pLightParam);
	return true;
}

bool A3DDevice::LightEnable(int nLightID, bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->LightEnable(nLightID, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetAmbient(A3DCOLOR colorAmbient)
{
	if( m_bHWIDevice )	return true;

	m_colorAmbient = colorAmbient;
	m_colorValueAmbient = ColorRGBAToColorValue(colorAmbient);

	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, colorAmbient);
	return true;
}

bool A3DDevice::SetMaterial(A3DMATERIALPARAM * pMaterialParam, bool b2Sided)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetMaterial((D3DMATERIAL8 *)pMaterialParam);
	if( b2Sided )
	{
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else
	{
		//Set To Original Configuration;
		SetFaceCull(m_A3DCullType);
	}
	return true;
}

bool A3DDevice::SetViewport(A3DVIEWPORTPARAM * pViewportParam)
{
	if( m_bHWIDevice )	return true;

	D3DVIEWPORT8 viewport;

	memcpy(&viewport, pViewportParam, sizeof(A3DVIEWPORTPARAM));
	m_pD3DDevice->SetViewport(&viewport);

	return true;
}

bool A3DDevice::SetFaceCull(A3DCULLTYPE type)
{
	if( m_bHWIDevice )	return true;

	switch(type)
	{
	case A3DCULL_CW:
		m_A3DCullType = type;
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		break;
	case A3DCULL_CCW:
		m_A3DCullType = type;
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		break;
	case A3DCULL_NONE:
		m_A3DCullType = type;
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		break;
	default:
		return false;
	}

	return true;
}

bool A3DDevice::SetFillMode(A3DFILLMODE mode)
{
	if( m_bHWIDevice )	return true;

	switch(mode)
	{
	case A3DFILL_POINT:
		m_A3DFillMode = mode;
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		break;
	case A3DFILL_WIREFRAME:
		m_A3DFillMode = mode;
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	case A3DFILL_SOLID:
		m_A3DFillMode = mode;
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	default:
		return false;
	}

	return true;
}

bool A3DDevice::DrawIndexedPrimitive(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD StartIndex, DWORD PrimitiveCount)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	
	if( PrimitiveCount == 0 )
		return true; 

	m_pA3DEngine->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);

	hval = m_pD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)Type, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::DrawIndexedPrimitive() Fail");
		return false;
	}

	m_dwDrawVertCount += NumVertices;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);
	return true;
}

bool A3DDevice::DrawPrimitive(A3DPRIMITIVETYPE Type, DWORD StartVertex, DWORD PrimitiveCount)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true; 

	m_pA3DEngine->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);

	hval = m_pD3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)Type, StartVertex, PrimitiveCount);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::DrawPrimitive() Fail");
		return false;
	}

	m_dwDrawVertCount += PrimitiveCount * 3;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);
	return true;
}

bool A3DDevice::DrawIndexedPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD PrimitiveCount, void* pIndexData, A3DFORMAT IndexDataFormat, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);

	hval = m_pD3DDevice->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)Type, MinIndex, NumVertices, PrimitiveCount, pIndexData, (D3DFORMAT) IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::DrawIndexedPrimitiveUP() Fail");
		return false;
	}

	m_dwDrawVertCount += NumVertices;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);
	return true;
}

bool A3DDevice::DrawPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD PrimitiveCount, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);

	hval = m_pD3DDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)Type, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::DrawPrimitiveUP() Fail");
		return false;
	}

	m_dwDrawVertCount += PrimitiveCount * 3;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE);
	return true;
}

bool A3DDevice::CopyToBack(int x, int y, int width, int height, IDirect3DSurface8 * pSurface)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	RECT	rect;
	POINT	offset;

	rect.left		= 0;
	rect.top		= 0;
	rect.right		= width;
	rect.bottom		= height;
	offset.x		= x;
	offset.y		= y;

	hval = m_pD3DDevice->CopyRects(pSurface, &rect, 1, m_pBackBuffer, &offset);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::CopyToBack() Fail");
		return false;
	}

	return true;
}

bool A3DDevice::CreateViewport(A3DViewport ** ppViewport, DWORD x, DWORD y, DWORD width, DWORD height, FLOAT minZ, FLOAT maxZ,
		bool bClearColor, bool bClearZ, A3DCOLOR colorClear)
{
	A3DViewport *		pViewport;
	A3DVIEWPORTPARAM	param;

	pViewport = new A3DViewport();
	if( NULL == pViewport )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::CreateViewport() Not enough memory!");
		return false;
	}

	ZeroMemory(&param, sizeof(param));
	param.X = x;
	param.Y = y;
	param.Width = width;
	param.Height = height;
	param.MinZ = minZ;
	param.MaxZ = maxZ;

	if( !pViewport->Init(this, &param, bClearColor, bClearZ, colorClear) )
	{
		delete pViewport;
		pViewport = NULL;
		*ppViewport = NULL;
		g_pA3DErrLog->ErrLog("A3DDevice::CreateViewport() Viewport Init Fail!");
		return false;
	}

	*ppViewport = pViewport;
	return true;
}

bool A3DDevice::CreateRenderTarget(A3DRenderTarget ** ppRenderTarget, int width, int height,
								   A3DFORMAT fmtTarget, A3DFORMAT fmtDepth, bool bNewTarget, bool bNewDepth)
{
	A3DDEVFMT			devFmt;	
	A3DRenderTarget *	pRenderTarget;

	pRenderTarget = new A3DRenderTarget();
	if( NULL == pRenderTarget )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::CreateRenderTarget() Not enough memory!");
		return false;
	}

	ZeroMemory(&devFmt, sizeof(A3DDEVFMT));
	devFmt.nWidth = width;
	devFmt.nHeight = height;
	if( fmtTarget != A3DFMT_UNKNOWN )
		devFmt.fmtTarget = fmtTarget;
	else
		devFmt.fmtTarget = m_A3DDevFmt.fmtTarget;
	if( fmtDepth != A3DFMT_UNKNOWN )
		devFmt.fmtDepth = fmtDepth;
	else
		devFmt.fmtDepth = m_A3DDevFmt.fmtDepth;

	if( !pRenderTarget->Init(this, devFmt, bNewTarget, bNewDepth) )
	{
		delete pRenderTarget;
		pRenderTarget = NULL;
		*ppRenderTarget = NULL;
		g_pA3DErrLog->ErrLog("A3DDevice::CreateRenderTarget() RenderTarget Init Fail!");
		return false;
	}

	*ppRenderTarget = pRenderTarget;
	return true;
}

bool A3DDevice::SetRenderTarget(A3DRenderTarget * pRenderTarget)
{
	if( m_bHWIDevice )	return true;

	HRESULT				hval;

	IDirect3DSurface8 * pNewTarget = pRenderTarget->GetTargetSurface();
	IDirect3DSurface8 * pNewDepth = pRenderTarget->GetDepthSurface();

	if( NULL == pNewTarget )
		pNewTarget = m_pRenderTarget;
	if( NULL == pNewDepth )
		pNewDepth = m_pDepthBuffer;

	hval = m_pD3DDevice->SetRenderTarget(pNewTarget, pNewDepth);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::SetRenderTarget() D3DDevice SetRenderTarget Fail!");
		return false;
	}

	return true;
}

bool A3DDevice::RestoreRenderTarget()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->SetRenderTarget(m_pRenderTarget, m_pDepthBuffer);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::RestoreRenderTarget() D3DDevice SetRenderTarget Fail!");
		return false;
	}
 
	return true;
}

bool A3DDevice::IsDetailMethodSupported()
{
	if( m_bHWIDevice )	return true;

	if( (m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE) && 
		(m_d3dcaps.SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR) &&
		(m_d3dcaps.DestBlendCaps & D3DPBLENDCAPS_SRCCOLOR) )
		return true;

	return false;
}

bool A3DDevice::ClearTexture(int nLayer)
{
	if( m_bHWIDevice )	return true;

	HRESULT		hval;

	hval = m_pD3DDevice->SetTexture(nLayer, NULL);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::ClearTexture() Fail!");
		return false;
	}
	return true;
}

bool A3DDevice::SetFogEnable(bool bEnable)
{
	m_bFogEnable = bEnable;

	if( m_bFogEnable )
	{
		SetFogTableEnable(m_bFogTableEnable);
	}
	else
	{
		// We set both fog algorithm to none to allow user defined fog data in LVertex;
		// And we can turn off Vertex's fog data by doint this!
		m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		m_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}

	return true;
}

bool A3DDevice::SetFogTableEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_bFogTableEnable = bEnable;
	
	if( m_bFogTableEnable && (m_d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		m_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}
	else
	{
		m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		m_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
	}

	return true;
}
	
bool A3DDevice::SetFogColor(A3DCOLOR fogColor)
{
	if( m_bHWIDevice )	return true;

	m_colorFog = fogColor;
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, m_colorFog);

	return true;
}

bool A3DDevice::SetFogStart(FLOAT vStart)
{
	if( m_bHWIDevice )	return true;

	m_vFogStart = vStart;
	m_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&m_vFogStart);

	return true;
}

bool A3DDevice::SetFogEnd(FLOAT vEnd)
{
	if( m_bHWIDevice )	return true;

	m_vFogEnd = vEnd;
	m_pD3DDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&m_vFogEnd);

	return true;
}

bool A3DDevice::SetFogDensity(FLOAT vDensity)
{
	if( m_bHWIDevice )	return true;

	m_vFogDensity = vDensity;
	m_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)&m_vFogDensity);

	return true;
}

char * A3DDevice::GetFormatString(A3DFORMAT format)
{
	switch(format)
	{
    case A3DFMT_R8G8B8:
		return "R8G8B8";

    case A3DFMT_A8R8G8B8:
		return "A8R8G8B8";

    case A3DFMT_X8R8G8B8:
		return "X8R8G8B8";

    case A3DFMT_R5G6B5:
		return "R5G6B5";

    case A3DFMT_X1R5G5B5:
		return "X1R5G5B5";

    case A3DFMT_A1R5G5B5:
		return "A1R5G5B5";

	case A3DFMT_A4R4G4B4:
		return "A4R4G4B4";

    case A3DFMT_R3G3B2:
		return "R3G3B2";

    case A3DFMT_A8:
		return "A8";

	case A3DFMT_A8R3G3B2:
		return "A8R3G3B2";

    case A3DFMT_X4R4G4B4:
		return "A8R3G3B2";

    case A3DFMT_A8P8:
		return "A8P8";

    case A3DFMT_P8:
		return "P8";

    case A3DFMT_L8:
		return "L8";

    case A3DFMT_A8L8:
		return "A8L8";

    case A3DFMT_A4L4:
		return "A4L4";

    case A3DFMT_V8U8:
		return "V8U8";

    case A3DFMT_L6V5U5:
		return "L6V5U5";

    case A3DFMT_X8L8V8U8:
		return "X8L8V8U8";

    case A3DFMT_Q8W8V8U8:
		return "Q8W8V8U8";

    case A3DFMT_V16U16:
		return "V16U16";

    case A3DFMT_W11V11U10:
		return "W11V11U10";

	case A3DFMT_D16_LOCKABLE:
		return "D16_LOCKABLE";

    case A3DFMT_D32:
		return "D32";

    case A3DFMT_D15S1:
		return "D15S1";

    case A3DFMT_D24S8:
		return "D24S8";

    case A3DFMT_D16:
		return "D16";

    case A3DFMT_D24X8:
		return "D24X8";

    case A3DFMT_D24X4S4:
		return "D24X4S4";
	}
	return "Unkown Format";
}

bool A3DDevice::SetDisplayMode(int nWidth, int nHeight, A3DFORMAT format, bool bWindowed, DWORD dwFlags)
{
	if( m_bHWIDevice )
	{
		// We only take notes of the new parameters, assuming we have set it already!
		if( dwFlags & SDM_WIDTH )
			m_d3dpp.BackBufferWidth = nWidth;
		if( dwFlags & SDM_HEIGHT )
			m_d3dpp.BackBufferHeight = nHeight;
		if( dwFlags & SDM_FORMAT )
			m_d3dpp.BackBufferFormat = (D3DFORMAT) format;
		if( dwFlags & SDM_WINDOW )
			m_d3dpp.Windowed = bWindowed;

		if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		else
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

		m_A3DDevFmt.nWidth = m_d3dpp.BackBufferWidth;
		m_A3DDevFmt.nHeight = m_d3dpp.BackBufferHeight;
		if( !m_bForceWindowSwap )
			m_A3DDevFmt.bWindowed = m_d3dpp.Windowed ? true : false;
		m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;

		// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
		m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
		switch(m_A3DDevFmt.fmtTarget)
		{
		case A3DFMT_X1R5G5B5:
		case A3DFMT_R5G6B5:
			m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
			m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
			break;
		case A3DFMT_X8R8G8B8:
		case A3DFMT_A8R8G8B8:
			m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
			m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
			break;
		}
		return true;
	}

	// Now it is a real hardware related device, we have to do normal work;
	if( dwFlags & SDM_WIDTH )
		m_d3dpp.BackBufferWidth = nWidth;
	if( dwFlags & SDM_HEIGHT )
		m_d3dpp.BackBufferHeight = nHeight;
	if( (dwFlags & SDM_FORMAT) && !m_A3DDevFmt.bWindowed )
		m_d3dpp.BackBufferFormat = (D3DFORMAT) format;
	if( dwFlags & SDM_WINDOW )
		m_d3dpp.Windowed = bWindowed;

	if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	HRESULT hval;
	hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, m_d3dpp.AutoDepthStencilFormat);
	if FAILED(hval)
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if( !m_A3DDevFmt.bWindowed && m_bForceWindowSwap )
	{
		DWORD dwFreq;
		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(devMode));
		devMode.dmSize = sizeof(devMode);
		if( m_d3dpp.BackBufferFormat == D3DFMT_X8R8G8B8 )
			devMode.dmBitsPerPel = 32; 
		else
			devMode.dmBitsPerPel = 16;
		devMode.dmPelsWidth = m_d3dpp.BackBufferWidth;
		devMode.dmPelsHeight = m_d3dpp.BackBufferHeight;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 
		
		// Get a proper display frequency
		GetProperDisplayFreq(devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, &dwFreq);
		if( dwFreq )
		{
			// For windows 98, all enumerated frequency is 0, but 
			// if you set devMode.dmDisplayFrequency to 0, it will be switched
			// to 60 Hz
			// if you did not supply DM_DISPLAYFREQUENCY, it will be changed
			// to current display settings of that width and height
			devMode.dmDisplayFrequency = dwFreq;
			devMode.dmFields |= DM_DISPLAYFREQUENCY; 
		}

		LONG ret = ChangeDisplaySettings(&devMode, 0);
		if( ret != DISP_CHANGE_SUCCESSFUL )
		{
			// Just log a warning message, but let the program run continue;
			g_pA3DErrLog->ErrLog("A3DDevice::SetDisplayMode(), Can not change display mode!");
		}

		// Store it for restorecurrentdisplaymode use
		m_CurrentMode.nWidth = devMode.dmPelsWidth;
		m_CurrentMode.nHeight = devMode.dmPelsHeight;
		m_CurrentMode.nBitsPerPixel = devMode.dmBitsPerPel;
		m_CurrentMode.nFrequency = dwFreq;

		g_pA3DErrLog->ErrLog("A3DDevice::SetDisplayMode(), We have changed desktop setting to [%dx%dx%d %dHz] by GDI function!", 
			devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, dwFreq);
	}

	bool bval = Reset();
	if( bval )
	{
		// Update Device information;
		m_A3DDevFmt.nWidth = m_d3dpp.BackBufferWidth;
		m_A3DDevFmt.nHeight = m_d3dpp.BackBufferHeight;
		if( !m_bForceWindowSwap )
			m_A3DDevFmt.bWindowed = m_d3dpp.Windowed ? true : false;
		m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;
	}

	// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}

	g_pA3DErrLog->ErrLog("A3DDevice::SetDisplayMode(), Set to %dx%dx%d", m_A3DDevFmt.nWidth, m_A3DDevFmt.nHeight, m_A3DDevFmt.fmtTarget == A3DFMT_X8R8G8B8 ? 32 : 16);
	return bval;
}

bool A3DDevice::Reset()
{
	if( m_bHWIDevice ) return true;
	if( m_bSwitchOff ) return true;

	HRESULT hval;

	// We must test the cooperative level here again to ensure we can call to reset;
	hval = m_pD3DDevice->TestCooperativeLevel();
	if( hval != D3DERR_DEVICENOTRESET && hval != D3D_OK )
	{
		return true;
	}
	
	if( m_pRenderTarget )
		m_pRenderTarget->Release();
	if( m_pDepthBuffer )
		m_pDepthBuffer->Release();
	if( m_pBackBuffer )
		m_pBackBuffer->Release();

	ALISTELEMENT * pDynamicStreamElement = m_ListDynamicStreams.GetFirst();
	while( pDynamicStreamElement != m_ListDynamicStreams.GetTail() )
	{
		A3DStream * pDynamicStream = (A3DStream *) pDynamicStreamElement->pData;

		pDynamicStream->ReleaseResource();
		pDynamicStreamElement = pDynamicStreamElement->pNext;
	}

	g_pA3DErrLog->ErrLog("A3DDevice::Reset() Device Lost, now try to reset it...");
	hval = m_pD3DDevice->Reset(&m_d3dpp);
	if FAILED(hval)
	{
		g_pA3DErrLog->ErrLog("A3DDevice::Reset() Reset Device Failure!");
		return false;
	}
	g_pA3DErrLog->ErrLog("A3DDevice::Reset() Reset Device Succeed.");

	//Get some surface pointers;
	hval = m_pD3DDevice->GetRenderTarget(&m_pRenderTarget);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::Reset() Can not get rendertarget pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetDepthStencilSurface(&m_pDepthBuffer);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::Reset() Can not get depth stencil surface pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	if( D3D_OK != hval )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::Reset() Can not get backbuffer pointer!");
		return false;
	}
	ConfigDevice();

	// Now set some special parameters;
	SetAmbient(m_colorAmbient);
	SetFogTableEnable(m_bFogTableEnable);
	SetFogColor(m_colorFog);
	SetFogStart(m_vFogStart);
	SetFogEnd(m_vFogEnd);
	SetFogDensity(m_vFogDensity);
	m_pA3DEngine->GetA3DLightMan()->RestoreLightResource();

	pDynamicStreamElement = m_ListDynamicStreams.GetFirst();
	while( pDynamicStreamElement != m_ListDynamicStreams.GetTail() )
	{
		A3DStream * pDynamicStream = (A3DStream *) pDynamicStreamElement->pData;

		pDynamicStream->RebuildResource();
		pDynamicStreamElement = pDynamicStreamElement->pNext;
	}
	return true;
}

bool A3DDevice::SetSpecularEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	m_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, bEnable ? TRUE : FALSE);	
	return true;
}

bool A3DDevice::SetDitherEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	m_pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, bEnable ? TRUE : FALSE);	
	return true;
}

bool A3DDevice::SetPointSize(FLOAT vPointSize)
{
	if( m_bHWIDevice ) return true;

	m_pD3DDevice->SetRenderState(D3DRS_POINTSIZE, *(DWORD *)&vPointSize);
	return true;
}

bool A3DDevice::SetPointSpriteEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	m_pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, bEnable ? TRUE : FALSE);
	return true;	
}

bool A3DDevice::EnableHardwareVertexProcessing(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	if( !IsTLDriver() )
		return true;
	
	if( bEnable )
		m_pD3DDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);
	else
		m_pD3DDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE);

	return true;
}

bool A3DDevice::AddDynamicStreams(A3DStream * pStream, ALISTELEMENT ** ppElement)
{
	if( m_bHWIDevice ) return true;

	return m_ListDynamicStreams.Append((LPVOID) pStream, ppElement);
}

bool A3DDevice::RemoveDynamicStreams(ALISTELEMENT * pElement)
{
	if( m_bHWIDevice ) return true;

	return m_ListDynamicStreams.Delete(pElement);
}

int A3DDevice::GetGammaLevel()
{
	return m_dwGammaLevel;
}

bool A3DDevice::SetGammaLevel(DWORD dwLevel)
{
	m_dwGammaLevel = dwLevel;

	if( m_bHWIDevice ) return true;

	D3DGAMMARAMP gammaRamp;

	for(int i=0; i<256; i++)
		gammaRamp.red[i] = gammaRamp.green[i] = gammaRamp.blue[i] = (WORD)min(65535, dwLevel * i * 255 / 100);

	if( GetVersion() < 0x80000000 )
	{
		// For 2000 we must always use direct 3d gamma control
		m_pD3DDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION, &gammaRamp);
	}
	else
	{
		if( !m_bForceWindowSwap )
		{
			m_pD3DDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION, &gammaRamp);
		}
		else
		{
			HDC hdc = GetDC(NULL);
			SetDeviceGammaRamp(hdc, (LPVOID)&gammaRamp);
			ReleaseDC(NULL, hdc);
		}
	}
	return true;
}

void A3DDevice::ShowCursor(bool bShow)
{
	m_bShowCursor = bShow;
}

void A3DDevice::SetCursor(A3DCursor * pA3DCursor)
{
	m_pA3DCursor = pA3DCursor;
}

bool A3DDevice::ExportColorToFile(char * szFullpath)
{
	if( m_bHWIDevice ) return true;

	if( !m_pBackBuffer )
		return false;

	HRESULT hval;
	hval = D3DXSaveSurfaceToFile(szFullpath, D3DXIFF_BMP, m_pBackBuffer, NULL, NULL);
	if FAILED(hval)
	{
		g_pA3DErrLog->ErrLog("A3DDevice::ExportColorToFile(), Can not save color surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

bool A3DDevice::ExportDepthToFile(char * szFullpath)
{
	if( m_bHWIDevice ) return true;

	// Currently not working;
	if( !m_pDepthBuffer )
		return false;

	HRESULT hval;

	hval = D3DXSaveSurfaceToFile(szFullpath, D3DXIFF_BMP, m_pDepthBuffer, NULL, NULL);
	if FAILED(hval)
	{
		g_pA3DErrLog->ErrLog("A3DDevice::ExportDepthToFile(), Can not save depth surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

void A3DDevice::RestoreDesktopGammaRamp()
{
	if( m_bHWIDevice ) return;

	if( m_bForceWindowSwap )
	{
		HDC hdc = GetDC(NULL);
		SetDeviceGammaRamp(hdc, (LPVOID)&m_DesktopGammaRamp);
		ReleaseDC(NULL, hdc);
	}
}

bool A3DDevice::RestoreDesktopDisplayMode()
{	
	RestoreDesktopGammaRamp();

	if( m_bForceWindowSwap && !m_A3DDevFmt.bWindowed )
	{
		// Restore the desktop display mode;
		LONG ret = ChangeDisplaySettings(NULL, 0);
		if( ret != DISP_CHANGE_SUCCESSFUL )
		{
			// just log a warning message, but let program run continue
			g_pA3DErrLog->ErrLog("A3DDevice::Release(), Can not restore desktop display mode!");
		}
	}

	m_bSwitchOff = true;
	return true;
}

bool A3DDevice::RestoreCurrentDisplayMode()
{
	if( m_bForceWindowSwap && !m_A3DDevFmt.bWindowed )
	{
		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(devMode));
		devMode.dmSize = sizeof(devMode);
		devMode.dmBitsPerPel = m_CurrentMode.nBitsPerPixel; 
		devMode.dmPelsWidth = m_CurrentMode.nWidth;
		devMode.dmPelsHeight = m_CurrentMode.nHeight;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 

		if( m_CurrentMode.nFrequency > 0 )
		{
			devMode.dmDisplayFrequency = m_CurrentMode.nFrequency;
			devMode.dmFields |= DM_DISPLAYFREQUENCY; 
		}
		
		LONG ret = ChangeDisplaySettings(&devMode, 0);
		if( ret != DISP_CHANGE_SUCCESSFUL )
		{
			// Just log a warning message, but let the program run continue
			g_pA3DErrLog->ErrLog("A3DDevice::RestoreCurrentDisplayMode(), Can not change display mode!");
		}

		g_pA3DErrLog->ErrLog("A3DDevice::RestoreCurrentDisplayMode(), Restore to %dx%dx%d %dHz", 
			m_CurrentMode.nWidth, m_CurrentMode.nHeight, m_CurrentMode.nBitsPerPixel, m_CurrentMode.nFrequency);
	}
	SetGammaLevel(m_dwGammaLevel);

	m_bSwitchOff = false;
	return true;
}

bool A3DDevice::SetDeviceWnd(HWND hWnd)
{
	if( !m_d3dpp.Windowed )
	{
		g_pA3DErrLog->ErrLog("A3DDevice::SetDeviceWnd(), can not change device window in fullscreen mode!");
		return false;
	}

	m_hDeviceWnd = hWnd;
	m_d3dpp.hDeviceWindow = m_hDeviceWnd;

	if( !Reset() )
	{
		return false;
	}

	return true;
}