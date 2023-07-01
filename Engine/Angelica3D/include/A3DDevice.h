/*
 * FILE: A3DDevice.h
 *
 * DESCRIPTION: The important class which represent the hardware in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DDEVICE_H_
#define _A3DDEVICE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "AList.h"

typedef class A3DEngine * PA3DEngine;
typedef class A3DLightMan * PA3DLightMan;	  
typedef class A3DViewport * PA3DViewport;
typedef class A3DRenderTarget * PA3DRenderTarget;
typedef class A3DStream * PA3DStream;
typedef class A3DCursor * PA3DCursor;

// A3D device flags
#define A3DDEV_ALLOWMULTITHREAD		0x0001		//	Allow multi-thread
#define A3DDEV_FORCESOFTWARETL		0x0002		//	Force software T&L
#define A3DDEV_ALPHATARGETFIRST		0x0004		//	Chose A8R8G8B8 first
#define A3DDEV_FPU_PRESERVE			0x0008		//	Create device with D3DCREATE_FPU_PRESERVE flag

#define SDM_WIDTH		0x1
#define SDM_HEIGHT		0x2
#define SDM_FORMAT		0x4
#define SDM_WINDOW		0x8

typedef struct _A3DDEVICE_DISPLAY_MODE
{
	int			nBitsPerPixel;
	int			nFrequency;
	int			nWidth;
	int			nHeight;

} A3DDEVICE_DISPLAY_MODE, * PA3DDEVICE_DISPLAY_MODE;

class A3DDevice
{
private:
	HWND					m_hDeviceWnd;
	
	bool					m_bSwitchOff;

	// Flag indicates whether using a hardware independent device;
	bool					m_bHWIDevice;

	A3DDEVTYPE				m_A3DDevType;

	A3DDEVFMT				m_A3DDevFmt;
	IDirect3DDevice8 *		m_pD3DDevice;
	
	// Cursor pointer;
	bool					m_bShowCursor;
	A3DCursor *				m_pA3DCursor;

	A3DDEVICE_DISPLAY_MODE	m_DesktopMode;
	A3DDEVICE_DISPLAY_MODE	m_CurrentMode;
	D3DGAMMARAMP			m_DesktopGammaRamp; // gamma ramp for desktop
	bool					m_bForceWindowSwap;
	
	D3DPRESENT_PARAMETERS	m_d3dpp;

	IDirect3DSurface8 *		m_pBackBuffer;
	IDirect3DSurface8 *		m_pRenderTarget;
	IDirect3DSurface8 *		m_pDepthBuffer;

	D3DCAPS8				m_d3dcaps;

	A3DEngine *				m_pA3DEngine;

	A3DCULLTYPE				m_A3DCullType;
	A3DFILLMODE				m_A3DFillMode;

	A3DFORMAT				m_fmtNormalTexture;
	A3DFORMAT				m_fmtColorKeyTexture;
	A3DFORMAT				m_fmtAlphaTexture;

	A3DFORMAT				m_fmtTarget16;
	A3DFORMAT				m_fmtTarget32;

	A3DMATRIX4				m_matWorldMatrix;
	A3DMATRIX4				m_matViewMatrix;
	A3DMATRIX4				m_matProjectionMatrix;

	DWORD					m_dwDrawVertCount;
	DWORD					m_dwDrawFaceCount;
	DWORD					m_dwDrawCount;

	A3DCOLOR				m_colorAmbient;
	A3DCOLORVALUE			m_colorValueAmbient;

	bool					m_bFogEnable; // if disabled, we just set vertex fog and pixel fog to none, but fogenable true
	bool					m_bFogTableEnable;
	A3DCOLOR				m_colorFog;
	FLOAT					m_vFogStart;
	FLOAT					m_vFogEnd;
	FLOAT					m_vFogDensity;

	// Gamma Level, range [0~200];
	DWORD					m_dwGammaLevel;

	// A list which contain all dynamic streams, this will be used when device will be reset;
	AList					m_ListDynamicStreams;

	bool					m_bTLDriver;
protected:
public:
	A3DDevice();
	~A3DDevice();
	bool Init(A3DEngine * pA3DEngine, HWND hDeviceWnd, A3DDEVFMT * pA3DDevFmt, A3DDEVTYPE devType=A3DDEVTYPE_HAL, bool bAllowMultiThread=false, bool bForceWindowSwap=false);
	bool Release();

	// Init one hardware independent device;
	// usually for a pure server version;
	bool InitAsHWI(A3DEngine * pA3DEngine, HWND hDeviceWnd, A3DDEVFMT * pA3DDevFmt, A3DDEVTYPE devType=A3DDEVTYPE_HAL, bool bAllowMultiThread=false, bool bForceWindowSwap=false);

	bool Reset();
	bool SetDisplayMode(int nWidth, int nHeight, A3DFORMAT format, bool bWindowed, DWORD dwFlags);
	bool SetDeviceWnd(HWND hWnd);

	// These function does not affect the current display setting, but
	// will change desktop display mode when in ForceSwap mode;
	bool RestoreDesktopDisplayMode();
	bool RestoreCurrentDisplayMode();

	bool ConfigDevice();
	char * GetFormatString(A3DFORMAT format);

	bool EnableHardwareVertexProcessing(bool bEnable);

	bool SetWorldMatrix(const A3DMATRIX4& matWorld);
	bool SetViewMatrix(const A3DMATRIX4& matView);
	bool SetProjectionMatrix(const A3DMATRIX4& matProject);
	bool SetTextureMatrix(int iStage, const A3DMATRIX4& matTexture);

	bool ClearTexture(int nLayer);

	bool BeginRender();
	bool EndRender();
	bool Present();

	bool Clear(DWORD dwFlags, A3DCOLOR color, FLOAT vZ, DWORD dwStencil);

	//Render State Control;
	
	//Z Buffer Control;
	bool SetZTestEnable(bool bEnable);
	bool SetZWriteEnable(bool bEnable);
	bool SetZBias(DWORD dwBias);
	bool SetZFunc(DWORD dwFunc);

	//Alpha Blend State Control;
	bool SetAlphaBlendEnable(bool bEnable);
	bool SetSourceAlpha(A3DBLEND alphaBlendParam);
	bool SetDestAlpha(A3DBLEND alphaBlendParam);
	
	//Alpha test control;
	bool SetAlphaTestEnable(bool bEnable);
	bool SetAlphaFunction(A3DCMPFUNC cmpFunc);
	bool SetAlphaRef(DWORD dwRefValue);

	//Specular control;
	bool SetSpecularEnable(bool bEnable);
	bool SetDitherEnable(bool bEnable);

	// Texture Filter Flag;
	bool SetTextureFilterType(int nTextureStage, A3DTEXTUREFILTERTYPE filterType);

	//Texture operation flag;
	bool SetTextureColorOP(int nTextureStage, A3DTEXTUREOP op);
	bool SetTextureColorArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2);
	bool SetTextureAlphaOP(int nTextureStage, A3DTEXTUREOP op);
	bool SetTextureAlphaArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2);

	// Texture address method
	bool SetTextureAddress(int nTextureStage, A3DTEXTUREADDR UAddrMethod, A3DTEXTUREADDR VAddrMethod);
	bool SetTextureCoordIndex(int nTextureStage, int iIndex);

	// Bump-mapping parameters
	bool SetTextureBumpMat(int nTextureStage, float _00, float _01, float _10, float _11);
	bool SetTextureBumpScale(int nTextureStage, float fScale);
	bool SetTextureBumpOffset(int nTextureStage, float fOffset);

	// Enable or disable texture transform
	bool SetTextureTransformFlags(int nTextureStage, A3DTEXTURETRANSFLAGS Flags);

	// Point sprite control;
	bool SetPointSize(FLOAT vPointSize);
	bool SetPointSpriteEnable(bool bEnable);
	
	//Device Caps;
	bool IsDetailMethodSupported();
	inline D3DCAPS8 GetD3DCaps() { return m_d3dcaps; }
	inline int GetMaxSimultaneousTextures()		{	return m_d3dcaps.MaxSimultaneousTextures;	}
	inline bool IsZBiasSupported() { return false; }//(m_d3dcaps.RasterCaps & D3DPRASTERCAPS_ZBIAS) ? true : false; }

	//Light Values;
	bool SetLightingEnable(bool bEnable);
	bool SetAmbient(A3DCOLOR colorAmbient);
	bool SetLightParam(int nLightID, A3DLIGHTPARAM * pLightParam);
	bool LightEnable(int nLightID, bool bEnable);
	inline A3DCOLORVALUE GetAmbientValue() { return m_colorValueAmbient; }
	inline A3DCOLOR GetAmbientColor() { return m_colorAmbient; }

	//Fog Values;
	bool SetFogEnable(bool bEnable);
	bool SetFogTableEnable(bool bEnable);
	bool SetFogColor(A3DCOLOR fogColor);
	bool SetFogStart(FLOAT vStart);
	bool SetFogEnd(FLOAT vEnd);
	bool SetFogDensity(FLOAT vDensity);
	inline bool GetFogEnable() { return m_bFogEnable; }
	inline bool GetFogTableEnable() { return m_bFogTableEnable; }
	inline A3DCOLOR GetFogColor() { return m_colorFog; }
	inline FLOAT GetFogStart() { return m_vFogStart; }
	inline FLOAT GetFogEnd() { return m_vFogEnd; }
	inline FLOAT GetFogDensity() { return m_vFogDensity; }
	
	// Gamma control;
	// Bright Level control; 
	int GetGammaLevel();
	bool SetGammaLevel(DWORD nLevel);
	// Restore desktop gamma ramp;
	void RestoreDesktopGammaRamp();

	//Material Control;
	bool SetMaterial(A3DMATERIALPARAM * pMaterialParam, bool b2Sided=false);

	//Viewports;
	bool SetViewport(A3DVIEWPORTPARAM * pViewportParam);

	//Face Culling;
	bool SetFaceCull(A3DCULLTYPE type);

	//Fill Mode;
	bool SetFillMode(A3DFILLMODE mode);

	//Render Target;
	bool SetRenderTarget(A3DRenderTarget * pRenderTarget);
	bool RestoreRenderTarget();

	//Dynamic Streams Mangement;
	bool AddDynamicStreams(A3DStream * pStream, ALISTELEMENT ** ppElement);
	bool RemoveDynamicStreams(ALISTELEMENT * pElement);

	//Creation of A3D Classes;
	bool CreateViewport(A3DViewport ** ppViewport, DWORD x, DWORD y, DWORD width, DWORD height, FLOAT minZ, FLOAT maxZ,
			bool bClearColor, bool bClearZ, A3DCOLOR colorClear=0);
	bool CreateRenderTarget(A3DRenderTarget ** ppRenderTarget, int width, int height,
			A3DFORMAT fmtTarget, A3DFORMAT fmtDepth, bool bNewTarget, bool bNewDepth);


	//Render;
	bool DrawIndexedPrimitive(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD StartIndex, DWORD PrimitiveCount);
	bool DrawPrimitive(A3DPRIMITIVETYPE Type, DWORD StartVertex, DWORD PrimitiveCount);

	bool DrawIndexedPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD PrimitiveCount, void* pIndexData, A3DFORMAT IndexDataFormat, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride);
	bool DrawPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD PrimitiveCount, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride);

	// Cursor functions;
	void ShowCursor(bool bShow);
	void SetCursor(A3DCursor * pA3DCursor);
	inline A3DCursor * GetCursor() { return m_pA3DCursor; }
	inline bool GetShowCursor() { return m_bShowCursor; }

	//Copy Rects
	bool CopyToBack(int x, int y, int width, int height, IDirect3DSurface8 * pSurface);

	bool ExportColorToFile(char * szFullpath);
	bool ExportDepthToFile(char * szFullpath);

	//Some inline functions;
	inline A3DEngine * GetA3DEngine() { return m_pA3DEngine; }
	inline IDirect3DDevice8 * GetD3DDevice() { return m_pD3DDevice; }
	inline IDirect3DSurface8 * GetBackBuffer() { return m_pBackBuffer; }
	inline A3DFORMAT GetNormalTextureFormat() { return m_fmtNormalTexture; }
	inline A3DFORMAT GetColorKeyTextureFormat() { return m_fmtColorKeyTexture; }
	inline A3DFORMAT GetAlphaTextureFormat() { return m_fmtAlphaTexture; }
	inline A3DFORMAT GetTarget16() { return m_fmtTarget16; }
	inline A3DFORMAT GetTarget32() { return m_fmtTarget32; }
	inline A3DMATRIX4 GetWorldMatrix() { return m_matWorldMatrix; }
	inline A3DMATRIX4 GetViewMatrix() { return m_matViewMatrix; }
	inline A3DMATRIX4 GetProjectionMatrix() { return m_matProjectionMatrix; }
	inline DWORD GetDrawVertCount() { return m_dwDrawVertCount; }
	inline DWORD GetDrawFaceCount() { return m_dwDrawFaceCount; }
	inline A3DDEVFMT GetDevFormat() { return m_A3DDevFmt; }
	inline DWORD GetDrawCount() { return m_dwDrawCount; }

	inline bool IsTLDriver() { return m_bTLDriver; }
	inline HWND GetDeviceWnd() { return m_hDeviceWnd; }
};

typedef A3DDevice * PA3DDevice;
#endif