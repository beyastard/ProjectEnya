/*
 * FILE: AGame.h
 *
 * DESCRIPTION: 游戏整体的实现类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AGAME__H__
#define __AGAME__H__

#include "AProcessor.h"

#include "PreDeclare.h"
#include <AMConfig.h>
#include <A3DMeshSorter.h>

#define	RELEASE(ptr)		{ if(ptr){ptr->Release(); delete ptr; ptr=NULL;} }

class ADeviceMan;
class AGamePlay;
class A3DEngine;
class ADeviceMouse;
class ADeviceKeyboard;
class A3DCamera;
class A3DLight;
class A3DViewport;
class A3DAssistTools;
class AWinMsgDeviceGrp;

typedef struct tagINITA3DENGINE
{
	HINSTANCE	hInstance;
	HWND		hWnd;
	int			nWidth;
	int			nHeight;
	bool		bIsWindow;
}INITA3DENGINE,*PINITA3DENGINE;

typedef struct tagINITAGAMESTRUCT
{
	INITA3DENGINE InitA3DEngine;
	
}INITAGAMESTRUCT,*PINITAGAMESTRUCT;

enum
{
	EXIT_CODE_NULL,
	EXIT_CODE_NORMAL,
	EXIT_CODE_RESET,
	EXIT_CODE_ERROR,
};

class AGame : public AProcessor  
{
public:
	DECLARE_ASERIAL(AGame)
	
protected:
	bool				m_bIsWindow;
	bool				m_bFastPossible;
	float				m_fRunSpan;
	long				m_logfps;
	bool				m_bActive;
	HWND				m_hWnd;
	HINSTANCE			m_hInstance;
	int					m_nWidth;
	int					m_nHeight;
	int					m_nDepth;
	A3DViewport *		m_pA3DViewport;
	A3DLight *			m_pA3DLight;
	A3DLight *			m_pA3DDynamicLight;
	A3DCamera *			m_pA3DCamera;
	A3DEngine *			m_pA3DEngine;
	ADeviceMan *		m_pADeviceMan;
	AGamePlay *			m_pAGamePlay;
	ADeviceMouse *		m_pAMouse;
	ADeviceKeyboard *	m_pAKeyBoard;
	A3DAssistTools *	m_pA3DAssistTools;
	AWinMsgDeviceGrp *	m_pAWinMsgDevice;
	A3DRain*			m_pA3DRain;
	A3DSnow*			m_pA3DSnow;
	A3DRainSplash *		m_pA3DRainSplash;
	bool				m_bReadFromPck;
	
protected:
	virtual bool OnRunQueryActive();
	virtual void QueryPckFileName(AUString& strPckName);
	virtual bool ReleaseA3DTools();
	virtual bool ReleaseGameCore();
	virtual bool ReleaseDevice();
	virtual bool ReleaseA3DEngine();
	virtual bool InitGameCore();
	virtual bool InitDevice(bool bWindow);
	virtual bool InitA3DEngine(PINITAGAMESTRUCT pInitStruct);
	virtual bool InitA3DTools();
	virtual int OnDispatchWndMsg(MSG * pMSG);

	
public:
	bool DispatchWndMsg();
	virtual bool ReleaseRainSnow();
	virtual bool InitRainSnow();
	virtual bool Reset();
	void SetForceRender(bool bForceRender);
	void SetLogicfps(int logicfps);
	bool DeleteDevice();
	bool CreateDevice(bool bWindow);
	bool SetHWND(HWND hWnd);
	virtual bool ResizeDevice(int __cx,int __cy);
	bool InitExtraGamePlay(AGamePlay * __pGamePlay);
	bool logic_run(float fTimeSpan);
	virtual int Run();
	//重载后要调用基类的实现
	virtual bool Release();
	virtual bool Init(PINITAGAMESTRUCT pInitStruct);

	bool WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	inline int GetClientWidth() { return m_nWidth;}
	inline int GetClientHeight() { return m_nHeight;}
	inline void SetClientWidth(int __nWidth) { m_nWidth = __nWidth;}
	inline void SetClientHeight(int __nHeight) { m_nHeight = __nHeight;}

	inline AGamePlay * SetAGamePlay(AGamePlay* __pAGamePlay)
	{ 
		AGamePlay * pPre = m_pAGamePlay;
		m_pAGamePlay = __pAGamePlay;
		return pPre;
	}
	A3DCamera * SetA3DCamera(A3DCamera * p);
	inline AGamePlay * GetAGamePlay() { return m_pAGamePlay;}
	inline A3DEngine * GetA3DEngine() { return m_pA3DEngine;}
	inline A3DCamera * GetA3DCamera() { return m_pA3DCamera;}
	inline ADeviceMan * GetADeviceMan() { return m_pADeviceMan;}
	inline A3DLight * GetA3DLight() { return m_pA3DLight;}
	inline A3DViewport * GetA3DViewport() { return m_pA3DViewport;}
	inline ADeviceMouse * GetADeviceMouse() { return m_pAMouse;}
	inline ADeviceKeyboard * GetADeviceKeyboard() { return m_pAKeyBoard;}
	inline A3DAssistTools * GetA3DAssistTools() { return m_pA3DAssistTools; }
	inline bool	GetIsWindow() { return m_bIsWindow; }
	inline HWND GetHWND() { return m_hWnd; }
	inline A3DRain* GetA3DRain(){ return m_pA3DRain;}
	inline A3DSnow* GetA3DSnow() { return m_pA3DSnow;}
	inline bool IsReadFromPck() { return m_bReadFromPck;}
	inline A3DLight* GetDynamicLight() { return m_pA3DDynamicLight;}
	inline void SetColorDepth(int nDepth) { m_nDepth = nDepth;}
	inline int GetColorDepth() { return m_nDepth;}

	AGame();
	virtual ~AGame();

};
extern AGame * g_pAGame;

#endif // __AGAME__H__