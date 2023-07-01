// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ADeviceKeyboard.cpp
// Creator: Wei Hua (魏华)
// 键盘设备

#include "ADeviceKeyboard.h"
#include "APlatform.h"
#include <string.h>

#define	ADEVICEKEYBOARD_NUM_VKEYS	256
#define	ADEVICEKEYBOARD_MAX_CHARS	256
#define	ADEVICEKEYBOARD_MAX_KEYS	256

#define PRESSING(vk)	((GetAsyncKeyState(vk) & 0x8000) ? true : false)

class	ADeviceKeyboard_Internal	: public ADeviceKeyboard
{
private:
	bool	m_bVKeyState[2][ADEVICEKEYBOARD_NUM_VKEYS];
	char	m_szChars[2][ADEVICEKEYBOARD_MAX_CHARS];
	char	m_szKeys[2][ADEVICEKEYBOARD_MAX_KEYS];
	int		m_nCharNum[2];
	int		m_nKeyNum[2];
	int		m_nGroupNow;								// 本次更新状态组的索引
	int		m_nGroupPrev;								// 上次状态组的索引
private:
	void	ClearChars();
	bool	AddToChars(unsigned int nChar);
	void	ClearKeys();
	bool	AddToKeys(unsigned int nKey);
	bool	RecordVKeyState();
	void	SwapGroup();
public:
	ADeviceKeyboard_Internal();
	virtual			~ADeviceKeyboard_Internal();
	//
	virtual bool	UpdateIn();
	virtual bool	UpdateOut();
	// 返回指定虚拟键的按下状态
	virtual bool	VK_FirstPress(int nVK);
	virtual bool	VK_LastPress(int nVK);
	virtual bool	VK_Pressing(int nVK);
	// 获得WM_CHAR的可见字串(返回字串长度)
	virtual int		GetChars(char *pszChars, int nSize);
	// 获得WM_KEYDOWN的功能键字串(返回字串长度)
	virtual int		GetKeys(char *pszChars, int nSize);
	// 比如处理WM_CHAR消息
	virtual bool	FilterWinMsg(void *pWinMsg);
};
ADeviceKeyboard	* NewADeviceKeyboard()
{
	return	new ADeviceKeyboard_Internal;
}

ADeviceKeyboard_Internal::ADeviceKeyboard_Internal()
{
	memset(m_bVKeyState, 0, sizeof(m_bVKeyState));
	memset(m_szChars, 0, sizeof(m_szChars));
	memset(m_szKeys, 0, sizeof(m_szKeys));
	memset(m_nCharNum, 0, sizeof(m_nCharNum));
	memset(m_nKeyNum, 0, sizeof(m_nKeyNum));
	m_nGroupNow		= 0;
	m_nGroupPrev	= 1;
}
ADeviceKeyboard_Internal::~ADeviceKeyboard_Internal()
{
}

void	ADeviceKeyboard_Internal::SwapGroup()
{
	m_nGroupPrev	= m_nGroupNow;
	m_nGroupNow		= 1 - m_nGroupNow;
	ClearChars();
	ClearKeys();
}

void	ADeviceKeyboard_Internal::ClearChars()
{
	m_nCharNum[m_nGroupNow]		= 0;
}
bool	ADeviceKeyboard_Internal::AddToChars(unsigned int nChar)
{
	int		idx;
	idx		= m_nCharNum[m_nGroupNow];
	if( idx>= ADEVICEKEYBOARD_MAX_CHARS-1 )
	{
		return	false;
	}
	if( nChar<32 )
	{
		switch(nChar)
		{
			case	MYVK_UNDO:
			case	MYVK_REDO:
				AddToKeys(nChar);
			break;
		}
		// 其他的可能包括
		// case	0x0D:				// /n
		// case	0x0A:				// /r
		// case	0x1B:				// ESC
		// case	0x09:				// TAB
		// case	0x08:				// BACKSPACE
		// 都是这个范围的
		return	true;
	}
	// 还有一些不能作为字符输入
	switch(nChar)
	{
		case	'`':				// 用于开关console的
		case	'~':				// 用于开关console的
			return	true;
		break;
	}

	m_szChars[m_nGroupNow][idx]		= nChar;
	m_nCharNum[m_nGroupNow]			= idx + 1;

	return	true;
}
void	ADeviceKeyboard_Internal::ClearKeys()
{
	m_nKeyNum[m_nGroupNow]		= 0;
}
bool	ADeviceKeyboard_Internal::AddToKeys(unsigned int nKey)
{
	int		idx;
	idx		= m_nKeyNum[m_nGroupNow];
	if( idx>= ADEVICEKEYBOARD_MAX_KEYS-1 )
	{
		return	false;
	}

	switch(nKey)
	{
		case	VK_LEFT:
		case	VK_RIGHT:
		case	VK_UP:
		case	VK_DOWN:
		case	VK_HOME:
		case	VK_END:
		case	VK_RETURN:
		case	VK_PRIOR:			// page up
		case	VK_NEXT:			// page down
		case	VK_DELETE:			// 删除后一个
		case	VK_BACK:			// 删除前一个
		case	MYVK_UNDO:
		case	MYVK_REDO:
		break;
		default:
		return	true;				// 剩下的不用考虑
	}

	m_szKeys[m_nGroupNow][idx]		= nKey;
	m_nKeyNum[m_nGroupNow]			= idx + 1;

	return	true;
}

bool	ADeviceKeyboard_Internal::RecordVKeyState()
{
	int	i;
	// VK == 0 没有意义
	for( i=1;i<ADEVICEKEYBOARD_NUM_VKEYS;i++ )
	{
		m_bVKeyState[m_nGroupNow][i] = PRESSING(i);
	}
	return	true;
}

bool	ADeviceKeyboard_Internal::UpdateIn()
{
	SwapGroup();

	RecordVKeyState();

	return	true;
}
bool	ADeviceKeyboard_Internal::UpdateOut()
{
	return	true;
}

bool	ADeviceKeyboard_Internal::VK_FirstPress(int nVK)
{
	return	m_bVKeyState[m_nGroupNow][nVK] && !m_bVKeyState[m_nGroupPrev][nVK];
}
bool	ADeviceKeyboard_Internal::VK_LastPress(int nVK)
{
	return	!m_bVKeyState[m_nGroupNow][nVK] && m_bVKeyState[m_nGroupPrev][nVK];
}
bool	ADeviceKeyboard_Internal::VK_Pressing(int nVK)
{
	return	m_bVKeyState[m_nGroupNow][nVK];
}

int		ADeviceKeyboard_Internal::GetChars(char *pszChars, int nSize)
{
	int		size;
	char	*psz;

	psz		= m_szChars[m_nGroupPrev];
	size	= m_nCharNum[m_nGroupPrev];
	if( size>nSize-1 )
	{
		return	-1;
	}
	psz[size]	= 0;
	strcpy(pszChars, psz);

	return	size;
}

int		ADeviceKeyboard_Internal::GetKeys(char *pszChars, int nSize)
{
	int		size;
	char	*psz;

	psz		= m_szKeys[m_nGroupPrev];
	size	= m_nKeyNum[m_nGroupPrev];
	if( size>nSize-1 )
	{
		return	-1;
	}
	psz[size]	= 0;
	strcpy(pszChars, psz);

	return	size;
}

bool	ADeviceKeyboard_Internal::FilterWinMsg(void *pWinMsg)
{
	MSG	*pMsg;
	pMsg	= (MSG *)pWinMsg;

	switch(pMsg->message)
	{
		case	WM_CHAR:
			AddToChars(pMsg->wParam);
		break;
		case	WM_KEYDOWN:
			AddToKeys(pMsg->wParam);
		break;
	}

	return	true;
}

