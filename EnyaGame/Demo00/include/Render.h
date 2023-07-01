#ifndef __RENDER_H__
#define __RENDER_H__

#include "A3D.h"

class CRender
{
public:
	CRender();
	~CRender();

	bool Init(HINSTANCE hInst, HWND hWnd, bool bFullScreen);
	void Release();
	bool ResizeDevice(int iWidth, int iHeight);
	bool BeginRender();
	bool EndRender();
	void ClearViewport();
	void OutText(int x, int y, const char* szText, DWORD color);

	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	A3DCamera* GetCamera() const { return m_pCamera; }
	A3DViewport* GetViewport() const { return m_pViewport; }
	A3DLight* GetDirectionalLight() const { return m_pDirLight; }
	A3DFont* GetSysFont() const { return m_pSysFont; }

	int GetRenderWidth() const { return m_iRenderWidth; }
	int GetRenderHeight() const { return m_iRenderHeight; }

	bool IsFullScreen() const { return m_bFullScreen; }

	bool EngineIsReady() const { return m_bEngineOK; }

protected:
	bool InitA3DEngine();
	void ReleaseA3DEngine();

private:
	A3DEngine* m_pA3DEngine;
	A3DDevice* m_pA3DDevice;
	A3DCamera* m_pCamera;
	A3DViewport* m_pViewport;
	A3DLight* m_pDirLight;
	A3DFont* m_pSysFont;

	HINSTANCE m_hInstance;
	HWND m_hRenderWnd;
	int m_iRenderWidth;
	int m_iRenderHeight;
	bool m_bFullScreen;

	bool m_bEngineOK;
};

extern CRender g_Render;

#endif
