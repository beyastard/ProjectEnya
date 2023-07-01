// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ADeviceMouse.cpp
// Creator: Wei Hua (魏华)
// 鼠标设备

#include "ADeviceMouse.h"
#include "APlatform.h"
#include <ZMOUSE.H>
#include <string.h>

#include <DInput.h>

#define	ADEVICEMOUSE_NUM_VKEYS			4
enum
{
	ADEVICEMOUSE_WHEELMSG_UP	,
	ADEVICEMOUSE_WHEELMSG_DOWN	,
	ADEVICEMOUSE_WHEELMSG_MAX	,
};

#define PRESSING(vk)	((GetAsyncKeyState(vk) & 0x8000) ? true : false)

class	ADeviceMouse_Internal	: public ADeviceMouse
{
private:
	bool	m_bVKeyState[2][ADEVICEMOUSE_NUM_VKEYS];
	int		m_nGroupNow;								// 本次更新状态组的索引
	int		m_nGroupPrev;								// 上次状态组的索引
	int		m_nWheelRoll[2];
	int		m_nX, m_nY;
	int		m_nDX, m_nDY;
	struct
	{
		int	nLeft;
		int	nTop;
		int	nRight;
		int	nBottom;
	}m_CursorRange;
	LPDIRECTINPUT8			m_pDInput;
	LPDIRECTINPUTDEVICE8	m_pDIDMouse;
private:
	void	SwapGroup();
	void	ResetWheel();
public:
	ADeviceMouse_Internal();
	bool	CreateDXMouse(ADEVICEMOUSE_INFO_T *pInfo);
	bool	ReleaseDXMouse();
	//
	virtual			~ADeviceMouse_Internal();
	//
	virtual bool	UpdateIn();
	virtual bool	UpdateOut();
	// 返回指定虚拟键的按下状态
	virtual bool	VK_FirstPress(int nVK);
	virtual bool	VK_LastPress(int nVK);
	virtual bool	VK_Pressing(int nVK);
	virtual void	GetCursorPos(int *pnX, int *pnY);
	virtual void	GetCursorDeltaPos(int *pnDX, int *pnDY);
	virtual void	SetCursorRange(int nL, int nT, int nR, int nB);
	virtual int		GetWheelRoll();
	// 比如处理WM_MOUSEWHEEL消息
	virtual bool	FilterWinMsg(void *pWinMsg);
};
ADeviceMouse	* NewADeviceMouse(ADEVICEMOUSE_INFO_T *pInfo)
{
	ADeviceMouse_Internal	*pADeviceMouse;
	pADeviceMouse	= new ADeviceMouse_Internal;
	if( !pADeviceMouse )
	{
		return	NULL;
	}
	if( !pADeviceMouse->CreateDXMouse(pInfo) )
	{
		delete	pADeviceMouse;
		return	NULL;
	}
	return	pADeviceMouse;
}

ADeviceMouse_Internal::ADeviceMouse_Internal()
{
	memset(m_bVKeyState, 0, sizeof(m_bVKeyState));
	m_nGroupNow		= 0;
	m_nGroupPrev	= 0;
	memset(m_nWheelRoll, 0, sizeof(m_nWheelRoll));
	m_nX = m_nY		= 0;
	m_nDX = m_nDY	= 0;
	m_pDInput		= NULL;
	m_pDIDMouse		= NULL;
}
ADeviceMouse_Internal::~ADeviceMouse_Internal()
{
	ReleaseDXMouse();
}
void	ADeviceMouse_Internal::SwapGroup()
{
	m_nGroupPrev	= m_nGroupNow;
	m_nGroupNow		= 1 - m_nGroupNow;
	ResetWheel();
}
void	ADeviceMouse_Internal::ResetWheel()
{
	m_nWheelRoll[m_nGroupNow]	= 0;
}

bool	ADeviceMouse_Internal::CreateDXMouse(ADEVICEMOUSE_INFO_T *pInfo)
{
	HRESULT		hval;
	HWND		hWnd;
	HINSTANCE	hInst;

	hWnd		= pInfo->hWnd;
	hInst		= pInfo->hInst;

	hval = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
		IID_IDirectInput8A, (LPVOID *)&m_pDInput, NULL);
	if( DI_OK != hval ) goto ErrEnd;

	hval = m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDMouse, NULL);
	if( DI_OK != hval ) goto ErrEnd;

	if( !pInfo->bWindow )
	{
		hval = m_pDIDMouse->SetCooperativeLevel(hWnd,
			DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		if( DI_OK != hval ) goto ErrEnd;
	}

	hval = m_pDIDMouse->SetDataFormat(&c_dfDIMouse);
	if( DI_OK != hval ) goto ErrEnd;

	hval = m_pDIDMouse->Acquire();
	if( DI_OK != hval ) goto ErrEnd;

	SetCursorRange(pInfo->nLeft, pInfo->nTop, pInfo->nRight, pInfo->nBottom);

	return	true;
ErrEnd:
	return	false;
}
bool	ADeviceMouse_Internal::ReleaseDXMouse()
{
	if( m_pDIDMouse )
	{
		// ??????还不知道怎么释放
		m_pDIDMouse->Unacquire();
		// By JiangLi.
		m_pDIDMouse->Release();
		// By JiangLi End.
		m_pDIDMouse	= NULL;
	}

	// By JiangLi.
	if(m_pDInput != NULL)
	{
		m_pDInput->Release();
		m_pDInput	= NULL;
	}
	// By JiangLi End.

	return	true;
}

//
bool	ADeviceMouse_Internal::UpdateIn()
{
	SwapGroup();

	// 和VK对应的键
	m_bVKeyState[m_nGroupNow][ADMOUSE_VK_LBUTTON]	= PRESSING(VK_LBUTTON);
	m_bVKeyState[m_nGroupNow][ADMOUSE_VK_RBUTTON]	= PRESSING(VK_RBUTTON);
	m_bVKeyState[m_nGroupNow][ADMOUSE_VK_MBUTTON]	= PRESSING(VK_MBUTTON);
	
	// 和滚轮对应的在m_nWheelRoll[m_nGroupPrev]里

	// 鼠标位置
	HRESULT			hval;
	DIMOUSESTATE	dims;

	memset(&dims, 0, sizeof(dims));
	hval	= m_pDIDMouse->GetDeviceState(sizeof(dims), &dims);
	if( DI_OK == hval )
	{
		m_nDX	= dims.lX;
		m_nDY	= dims.lY;

		m_nX	+= m_nDX;
		m_nY	+= m_nDY;
		if( m_nX<m_CursorRange.nLeft )
			m_nX	= m_CursorRange.nLeft;
		else if( m_nX>m_CursorRange.nRight )
			m_nX	= m_CursorRange.nRight;
		if( m_nY<m_CursorRange.nTop )
			m_nY	= m_CursorRange.nTop;
		else if( m_nY>m_CursorRange.nBottom )
			m_nY	= m_CursorRange.nBottom;
	}

	return	true;
}
bool	ADeviceMouse_Internal::UpdateOut()
{
	return	true;
}
bool	ADeviceMouse_Internal::VK_FirstPress(int nVK)
{
	return	m_bVKeyState[m_nGroupNow][nVK] && !m_bVKeyState[m_nGroupPrev][nVK];
}
bool	ADeviceMouse_Internal::VK_LastPress(int nVK)
{
	return	!m_bVKeyState[m_nGroupNow][nVK] && m_bVKeyState[m_nGroupPrev][nVK];
}
bool	ADeviceMouse_Internal::VK_Pressing(int nVK)
{
	return	m_bVKeyState[m_nGroupNow][nVK];
}

void	ADeviceMouse_Internal::GetCursorPos(int *pnX, int *pnY)
{
	*pnX	= m_nX;
	*pnY	= m_nY;
}
void	ADeviceMouse_Internal::GetCursorDeltaPos(int *pnDX, int *pnDY)
{
	*pnDX	= m_nDX;
	*pnDY	= m_nDY;
}
void	ADeviceMouse_Internal::SetCursorRange(int nL, int nT, int nR, int nB)
{
	m_CursorRange.nLeft		= nL;
	m_CursorRange.nTop		= nT;
	m_CursorRange.nRight	= nR;
	m_CursorRange.nBottom	= nB;
}
int		ADeviceMouse_Internal::GetWheelRoll()
{
	return	m_nWheelRoll[m_nGroupPrev];
}
bool	ADeviceMouse_Internal::FilterWinMsg(void *pWinMsg)
{
	MSG	*pMsg;
	pMsg	= (MSG *)pWinMsg;

	switch(pMsg->message)
	{
		case	WM_MOUSEWHEEL:
		{
			short fwKeys = LOWORD(pMsg->wParam);
			short zDelta = HIWORD(pMsg->wParam);
			if( zDelta>0 )			zDelta	= 1;
			else if( zDelta<0 )		zDelta	= -1;
			m_nWheelRoll[m_nGroupNow]	+= zDelta;
		}
		break;
	}

	return	true;
}
