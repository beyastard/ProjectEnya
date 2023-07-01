#include "Render.h"
#include "A3DMacros.h"

CRender	g_Render;

CRender::CRender()
{
	m_pA3DEngine = nullptr;
	m_pA3DDevice = nullptr;
	m_pCamera = nullptr;
	m_pViewport = nullptr;
	m_pDirLight = nullptr;
	m_pSysFont = nullptr;
	m_hInstance = nullptr;
	m_hRenderWnd = nullptr;
	m_iRenderWidth = 800;
	m_iRenderHeight = 600;
	m_bFullScreen = false;
	m_bEngineOK = false;
}

CRender::~CRender() {}

bool CRender::Init(HINSTANCE hInst, HWND hWnd, bool bFullScreen)
{
	m_hInstance = hInst;
	m_hRenderWnd = hWnd;
	m_bFullScreen = bFullScreen;

	if (!InitA3DEngine())
	{
		a_LogOutput(1, "CRender::Init, Failed to initialize A3D engine!");
		return false;
	}

	return true;
}

void CRender::Release()
{
	ReleaseA3DEngine();
}

bool CRender::ResizeDevice(int iWidth, int iHeight)
{
	if (!m_bEngineOK || !iWidth || !iHeight)
		return false;

	m_pA3DEngine->SetDisplayMode(iWidth, iHeight, A3DFMT_UNKNOWN, true, SDM_WIDTH | SDM_HEIGHT);

	m_iRenderWidth = iWidth;
	m_iRenderHeight = iHeight;

	// Adjust camera and viewport
	A3DVIEWPORTPARAM Params;
	Params.X = 0;
	Params.Y = 0;
	Params.Width = iWidth;
	Params.Height = iHeight;
	Params.MinZ = 0.0f;
	Params.MaxZ = 1.0f;

	m_pViewport->SetParam(&Params);
	m_pViewport->Active();

	// Adjust camera
	const float fRatio = static_cast<float>(iWidth) / static_cast<float>(iHeight);
	m_pCamera->SetProjectionParam(m_pCamera->GetFOV(), 0.1f, 2000.0f, fRatio);

	return true;
}

bool CRender::BeginRender()
{
	if (!m_bEngineOK)
		return false;

	return m_pA3DEngine->BeginRender();
}

bool CRender::EndRender()
{
	if (!m_bEngineOK)
		return false;

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
}

void CRender::ClearViewport()
{
	if (!m_bEngineOK)
		return;

	m_pViewport->Active();
	m_pViewport->ClearDevice();
}

void CRender::OutText(int x, int y, const char* szText, DWORD color)
{
	if (m_pSysFont)
		m_pSysFont->TextOut(x, y, const_cast<char*>(szText), color);
}

bool CRender::InitA3DEngine()
{
	if (!((m_pA3DEngine = new A3DEngine())))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Not enough memory!");
		return false;
	}

	RECT Rect;
	GetClientRect(m_hRenderWnd, &Rect);

	m_iRenderWidth = Rect.right;
	m_iRenderHeight = Rect.bottom;

	A3DDEVFMT devFmt;
	devFmt.bWindowed = !m_bFullScreen;
	devFmt.nWidth = m_iRenderWidth;
	devFmt.nHeight = m_iRenderHeight;
	devFmt.fmtTarget = A3DFMT_UNKNOWN;
	devFmt.fmtDepth = A3DFMT_UNKNOWN;

	//DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD | A3DDEV_FORCESOFTWARETL;  // Force software T&L
	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;  // Hardware render

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, static_cast<A3DDEVTYPE>(dwDevFlags)))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to initialize A3D engine.");
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	// Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight,
		A3DVECTOR3(0.0f, 0.0f, 0.0f),
		A3DVECTOR3(1.0f, -1.0f, 1.0f),
		A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f),
		A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f),
		A3DCOLORVALUE(0.4f, 0.4f, 0.4f, 1.0f));

	// Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(50, 50, 50));
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	// Create camera
	if (!((m_pCamera = new A3DCamera())))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to create camera object!");
		return false;
	}

	if (!m_pCamera->Init(g_Render.GetA3DDevice(), DEG2RAD(56.0f), 0.1f, 2000.0f))
	{
		a_LogOutput(1, 0, "CRender::InitA3DEngine: Failed to initialize camera.");
		return false;
	}

	// Move camera to original point and set it's direction as Z axis
	m_pCamera->SetPos(A3DVECTOR3(0.0F) - A3DVECTOR3(0.0f, 0.0f, 1.0f) * 10.0f);
	m_pCamera->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));

	// Create viewport
	if (!g_Render.GetA3DDevice()->CreateViewport(&m_pViewport, 0, 0,
		m_iRenderWidth, m_iRenderHeight,
		0.0f, 1.0f, true, true, A3DCOLORRGB(64, 64, 64)))
	{
		a_LogOutput(1, 0, "CRender::InitA3DEngine: Failed to create viewport.");
		return false;
	}

	m_pViewport->SetCamera(m_pCamera);
	m_bEngineOK = true;

	return true;
}

void CRender::ReleaseA3DEngine()
{
	A3DRELEASE(m_pCamera);
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pDirLight);
	A3DRELEASE(m_pA3DEngine);

	m_pSysFont = nullptr;
	m_pA3DDevice = nullptr;
}
