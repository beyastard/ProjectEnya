// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistTools.cpp
// Creator: Wei Hua (魏华)
// 用于简化应用的工具集，在引擎初始化后创建该对象。

#include "A3DAssistTools.h"
#include "APlatform.h"
#include "A3DAssistA3dString.h"
#include "A3DAssistA3dStringSet.h"
#include "A3DAssistLCD.h"
#include "A3DAssistSnd.h"

#include <a3d.h>
#include <am.h>

unsigned long	g_AA_nSystemTickcount	= 0;	// extern define in A3DAssistTools_Cmn.h

////////////////////////////////////////////////////////////////////
// 功能函数
////////////////////////////////////////////////////////////////////
/*void	SetCurTickToA3DAssistTools(int nTick)
{
	A3DAssistCache::m_nCurTick	= nTick;
}*/
int	CmnToHtml(char *__html, int __size, char *__cmn)
{
	int		i, j, len;
	bool	bTrans;
	char	szTrans[32];
	len	= strlen(__cmn);
	j	= 0;
	for(i=0;i<len;i++)
	{
		bTrans	= true;
		switch( __cmn[i] )
		{
			case	'<':
				strcpy(szTrans, "&lt;");
			break;
			case	'>':
				strcpy(szTrans, "&gt;");
			break;
			default:
				bTrans	= false;
			break;
		}
		if( bTrans )
		{
			int	tlen;
			tlen	= strlen(szTrans);
			memcpy(__html+j, szTrans, tlen);
			j	+= tlen;
		}
		else
		{
			__html[j]	= __cmn[i];
			j	++;
		}
		if( j>=__size )
		{
			j	= j-1;
			break;
		}
	}
	__html[j]	= 0;
	return	j;
}
int	HtmlToCmn(char *__cmn, int __size, char *__html)
{
	int		i, j, len, tlen;
	char	cTrans;
	len	= strlen(__html);
	j	= 0;
	for(i=0;i<len;i++)
	{
		tlen	= 0;
		if( __html[i]=='&' )
		{
			if( _strnicmp(__html+i, "&lt;", 4)==0 )
			{
				cTrans	= '<';
				tlen	= 4;		
			}
			else if( _strnicmp(__html+i, "&gt;", 4)==0 )
			{
				cTrans	= '>';
				tlen	= 4;		
			}
		}
		if( tlen>0 )
		{
			__cmn[j]	= cTrans;
			i			+= tlen-1;
		}
		else
		{
			__cmn[j]	= __html[i];
		}
		j			++;
		if( j>=__size )
		{
			j	= j-1;
			break;
		}
	}
	__cmn[j]	= 0;
	return	j;
}

////////////////////////////////////////////////////////////////////
// Fade工具
////////////////////////////////////////////////////////////////////
class	AATFadeTicker
{
private:
	unsigned long	m_nBeginTime;	// ms
	unsigned long	m_nEndTime;		// ms
	float	m_fInitial;				// 最初的音量
	float	m_fFrom;
	float	m_fTo;
	bool	m_bActive;
public:
	AATFadeTicker()
	{
		m_nBeginTime	= 0;
		m_nEndTime		= 0;
		m_fInitial		= 0;
		m_fFrom			= 0;
		m_fTo			= 0;
		m_bActive		= false;
	}
	~AATFadeTicker()
	{
	}
	bool	Begin(float fFrom, float fTo, float fTime)		// fTime单位为秒
	{
		m_fFrom			= fFrom;
		m_fTo			= fTo;
		m_nBeginTime	= g_AA_nSystemTickcount;
		m_nEndTime		= m_nBeginTime + int(fTime * 1000);
		m_bActive		= true;

		return			true;
	}
	bool	RevertBegin(float fTime)
	{
		return	Begin(m_fTo, m_fFrom, fTime);
	}
	void	SetInitial(float fI)
	{
		m_fInitial	= fI;
	}
	float	GetInitial()
	{
		return	m_fInitial;
	}
	float	GetFrom()
	{
		return	m_fFrom;
	}
	float	GetTo()
	{
		return	m_fTo;
	}
	bool	IsActive()
	{
		return	m_bActive;
	}
	float	GetCurVal()
	{
		if( !m_bActive || g_AA_nSystemTickcount>=m_nEndTime )
		{
			m_bActive	= false;
			return		m_fTo;
		}
		return
			( (g_AA_nSystemTickcount-m_nBeginTime) * m_fTo + (m_nEndTime-g_AA_nSystemTickcount) * m_fFrom )
			/ ( m_nEndTime - m_nBeginTime )
			;
	}
};

////////////////////////////////////////////////////////////////////
// A3DAssistTools具体实现
////////////////////////////////////////////////////////////////////
#define	A3DASSISTTOOLS_MAX_FONT		128
#define	A3DASSISTTOOLS_MAX_COLOR	128
#define	A3DASSISTTOOLS_MAX_LCDFONT	16

#define A3DASSISTTOOLS_MAX_SOUNDTYPE 16

class	A3DAssistTools_I	: public A3DAssistTools
{
private:
	int						m_nCurFont;
	int						m_nFontHeight;
	float					m_fFontRatio;
	A3DAssistA3dString		*m_pCurAAA3dString;
	A3DAssistA3dStringSet	*m_pCurAAA3dStringSet;
	int						m_nCurLCDFont;
	float					m_fLCDFontRatio;
	A3DAssistLCD			*m_pCurAALCD;
	int						m_n3DSndTickRatio;
protected:
	A3DEngine				*m_pA3DEngine;
	AMSoundStream			*m_pAMSoundStream;
	HA3DFONT				m_hFont[A3DASSISTTOOLS_MAX_FONT];
	A3DAssistA3dString		*m_pAAA3dString[A3DASSISTTOOLS_MAX_FONT];
	A3DAssistA3dStringSet	*m_pAAA3dStringSet[A3DASSISTTOOLS_MAX_FONT];
	A3DCOLOR				m_color[A3DASSISTTOOLS_MAX_COLOR];
	A3DAssistLCD			*m_pA3DAssistLCD[A3DASSISTTOOLS_MAX_LCDFONT];
	A3DAssist2DOSOP			*m_pA3DAssist2DOSOP[A3DASSISTTOOLS_MAX_SOUNDTYPE];
	A3DAssist3DSnd			*m_aA3DAssist3DSnd[A3DASSISTTOOLS_MAX_SOUNDTYPE];
	A3DAssist2DSnd			*m_pA3DAssist2DSnd;
	AATFadeTicker			m_FT4SSFade;
	bool					m_bFadeNeedStop;
	char					m_szNextBMFile[MAX_PATH];
	float					m_fNextBMFadeIn;
public:
	A3DAssistTools_I();
	bool			Init(A3DASSISTTOOLS_INFO_T *pInfo);
	bool			Release();
	virtual			~A3DAssistTools_I();
	virtual void	SetSystemTickcount(unsigned long nT);							// 设置系统当前的时间（毫秒），用于声音的fade 。应该在主逻辑每次获得时间的时候设置这个
	virtual void	AddSystemTickcount(unsigned long nT);							// 累加当前的时间间隔（毫秒），用于声音的fade 。应该在主逻辑每次获得时间的时候设置这个
	virtual bool	LogicRun();														// 里面控制的一些东西需要Tick
	// 重置
	virtual bool	ResetResource();												// 重置所有资源（一般在一个游戏阶段结束时）
	// 字体相关
	virtual bool	F_RegisterFont(int nFont, A3DFONT_INFO_T *pInfo);				// 注册。在程序开始前确定总共需要那些字体，然后一次全部注册。
	virtual bool	F_RegisterColor(int nColor, A3DCOLOR acolor);					// 注册html中会使用到的颜色。
	virtual bool	F_SetCurFont(int nFont, int nNewSize = 0);						// 设置马上需要使用的2D字体。
	virtual	int		F_GetCurFont(){return m_nCurFont;}
	virtual int		F_GetFont(int nFont){return (int)m_pA3DEngine->GetA3DFontMan()->GetWindowsFont(m_hFont[nFont]);}
	virtual bool	F_SetCur3DFont(int nFont, float fRatio = 0);					// 设置马上需要使用的3D字体。
	virtual bool	F_GetTextExtent(char *szText, int *pnW, int *pnH);				// 获得当前字体下，指定字串的屏幕输出尺寸
	virtual bool	F_TextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade);		// 在屏幕指定位置输出指定颜色的字串
	virtual bool	F_HtmlTextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade);	// 输出字串中可以利用类html格式改变某一段的字体颜色
	virtual bool	F_TextOut3D(A3DVECTOR3 vecPos, char *szText, A3DCOLOR color);	// 在三维世界的一个位置输出字串
	//		以行表格字符串输出
	//		nX, nY是起始坐标
	virtual bool	F_TextOut_Begin();
	//		nDX是坐标增量
	virtual bool	F_TextOut_AddString(int nDX, A3DCOLOR color, char *szText);
	//		bShade表示是否有影子边
	virtual bool	F_TextOut_End(int nX, int nY, bool bShade);
	// LCD字符相关
	virtual bool	LCD_RegisterFont(int nLCDFont, A3DLCD_INFO_T *pInfo);			// 注册。可以注册多种LCD字体。
	virtual bool	LCD_GetTextExtent(char *szText, int *nW, int *nH);				// 获得如果变成LCD字串后的屏幕输出尺寸
	virtual bool	LCD_SetCurFont(int nLCDFont, float fRatio = 0);					// 设置马上需要使用的LCD字体。
	virtual bool	LCD_SetSep(int nSep);											// 设置字间距
	virtual bool	LCD_TextOut(int nX, int nY, char *szText, A3DCOLOR color);		// 在指定位置输出指定颜色的LCD样子的字串
	// 声音相关
	virtual bool	S_SndInit(int * n2DBufTypeBuf,int n2DBufTypeCount, int n2DDupBuf,
							  int* n3DBufTypeBuf, int n3DBufTypeCount, int nFNLen);	// 初始化缓冲个数
	virtual void	S_SetJustLoad(bool bSet);										// 设置声音只load不播
	virtual bool	S_2DSndPlay(int type,char *szSndFile, bool bImmEffect, bool bLoop, bool bCanFade);
	virtual bool	S_2DSndStop(int type,char *szSndFile);
	virtual bool	S_2DSndPause(int type,char *szSndFile);
	virtual bool	S_2DSndIsStopped(int type,char *szSndFile);
	virtual bool	S_2DDupSndPlay(char *szSndFile, bool bImmEffect, bool bCanFade);// 可以一个声音有多重
	virtual bool	S_3DSndPlay(int iType, char *szSndFile, A3DVECTOR3 &vecPos, float fMinDist, float fMaxDist
					, bool bImmEffect, bool bForce2D, bool bCanFade);
	virtual void	S_SetVolume(unsigned int nVol);
	virtual void	S_2DSndFadeIn(int type,int ms);											// 在这之后ms毫秒内的所有需要fade的2D声音都会fadein
	virtual void	S_2DSndFadeOut(int type);												// 当前正在播放的所有需要fade的2D声音都会fadeout
	virtual void	S_3DSndFadeIn(int iType, int ms);							// 在这之后ms毫秒内的所有需要fade的2D声音都会fadein
	virtual void	S_3DSndFadeOut(int iType);									// 当前正在播放的所有需要fade的2D声音都会fadeout
	// 背景音乐
	virtual bool	BM_Play(char *szSndFile);
	virtual bool	BM_Stop();
	virtual bool	BM_Pause(bool bPause);
	virtual bool	BM_IsStopped();
	virtual void	BM_SetLoop(bool bLoop);
	virtual bool	BM_GetLoop();
	virtual void	BM_SetVolume(unsigned int nVol);
	virtual unsigned int	BM_GetVolume();
	virtual bool	BM_FadeOut(float fOT, char *szNextFile, float fIT);				// 淡出当前音乐，然后淡入下一个音乐
	virtual bool	BM_FadeIn(char *szNextFile, float fIT);
};
A3DAssistTools *	NewA3DAssistTools(A3DASSISTTOOLS_INFO_T *pInfo)
{
	A3DAssistTools_I	*pA3DAssistTools;
	pA3DAssistTools	= new A3DAssistTools_I;
	if( pA3DAssistTools )
	{
		if( !pA3DAssistTools->Init(pInfo) )
		{
			delete	pA3DAssistTools;
			return	NULL;
		}
	}

	return	pA3DAssistTools;
}

A3DAssistTools_I::A3DAssistTools_I()
{
	m_nCurFont			= -1;
	m_nFontHeight		= 0;
	m_fFontRatio		= 0;
	m_nCurLCDFont		= -1;
	m_fLCDFontRatio		= 0;
	m_n3DSndTickRatio	= 2;
	m_pCurAAA3dString	= NULL;
	m_pCurAALCD			= NULL;
	m_pA3DEngine		= NULL;
	m_pAMSoundStream	= NULL;
	memset(m_hFont, 0, sizeof(m_hFont));
	memset(m_color, 0, sizeof(m_color));
	memset(m_pAAA3dString, 0, sizeof(m_pAAA3dString));
	memset(m_pAAA3dStringSet, 0, sizeof(m_pAAA3dStringSet));
	memset(m_pA3DAssistLCD, 0, sizeof(m_pA3DAssistLCD));
	memset(m_pA3DAssist2DOSOP,0,sizeof(m_pA3DAssist2DOSOP));
	memset(m_aA3DAssist3DSnd, 0, sizeof (m_aA3DAssist3DSnd));
	m_pA3DAssist2DSnd	= NULL;
	memset(m_szNextBMFile, 0, sizeof(m_szNextBMFile));
	m_fNextBMFadeIn		= 0;
	m_bFadeNeedStop		= false;
}
bool	A3DAssistTools_I::Init(A3DASSISTTOOLS_INFO_T *pInfo)
{
	// 重置tick
	A3DAssistCache::m_nCurTick	= 0;

	m_pA3DEngine		= pInfo->pA3DEngine;

	// 背景音乐
	m_pAMSoundStream	= new AMSoundStream;
	if( !m_pAMSoundStream )
	{
		goto	ErrEnd;
	}
	if( !m_pAMSoundStream->Init(m_pA3DEngine->GetAMSoundEngine(), NULL) )
	{
		goto	ErrEnd;
	}
	m_pAMSoundStream->SetLoopFlag(false);

	m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice()->SetDopplerFactor(1.0f);
	m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice()->UpdateChanges();
	
	return	true;
ErrEnd:
	Release();
	return	false;
}
bool	A3DAssistTools_I::Release()
{
	int		i;

	// 删除所有字体
	for(i=0;i<A3DASSISTTOOLS_MAX_FONT;i++)
	{
		if( m_pAAA3dString[i] )
		{
			m_pAAA3dString[i]->Release();
			delete	m_pAAA3dString[i];
			m_pAAA3dString[i]		= NULL;
		}
		if( m_pAAA3dStringSet[i] )
		{
			m_pAAA3dStringSet[i]->Release();
			delete	m_pAAA3dStringSet[i];
			m_pAAA3dStringSet[i]	= NULL;
		}
	}
	for(i=0;i<A3DASSISTTOOLS_MAX_LCDFONT;i++)
	{
		if( m_pA3DAssistLCD[i] )
		{
			m_pA3DAssistLCD[i]->Release();
			delete	m_pA3DAssistLCD[i];
			m_pA3DAssistLCD[i]	= NULL;
		}
	}

	// 音效对象
	for(i = 0;i<A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if( m_pA3DAssist2DOSOP[i] )
		{
			m_pA3DAssist2DOSOP[i]->Release();
			delete	m_pA3DAssist2DOSOP[i];
			m_pA3DAssist2DOSOP[i]	= NULL;
		}
	}
	
	for (i=0; i <A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if (m_aA3DAssist3DSnd[i])
		{
			m_aA3DAssist3DSnd[i]->Release();
			delete m_aA3DAssist3DSnd[i];
			m_aA3DAssist3DSnd[i] = NULL;
		}
	}

	if( m_pA3DAssist2DSnd )
	{
		m_pA3DAssist2DSnd->Release();
		delete	m_pA3DAssist2DSnd;
		m_pA3DAssist2DSnd	= NULL;
	}

	// 背景音乐
	if( m_pAMSoundStream )
	{
		m_pAMSoundStream->Stop();
		m_pAMSoundStream->Release();
		delete m_pAMSoundStream;
		m_pAMSoundStream	= NULL;
	}

	return	true;
}
A3DAssistTools_I::~A3DAssistTools_I()
{
	Release();
}

void	A3DAssistTools_I::SetSystemTickcount(unsigned long nT)
{
	g_AA_nSystemTickcount	= nT;	
}
void	A3DAssistTools_I::AddSystemTickcount(unsigned long nT)
{
	g_AA_nSystemTickcount	+= nT;	
}
bool	A3DAssistTools_I::LogicRun()
{
	A3DAssistCache::m_nCurTick	++;
	m_pAMSoundStream->Tick();
	
	A3DCamera	* pA3DCamera;
	pA3DCamera	= m_pA3DEngine->GetActiveCamera();

	// 三维音效
	if( pA3DCamera && (A3DAssistCache::m_nCurTick % m_n3DSndTickRatio) == 0 )
	{
		m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice()->SetPosition(pA3DCamera->GetPos());
		m_pA3DEngine->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_MEDIA);
		m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice()->SetOrientation(
			pA3DCamera->GetDir(), pA3DCamera->GetUp());
		m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice()->UpdateChanges();
		m_pA3DEngine->EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_MEDIA);
	}
	// 各个声音管理器
	int i;
	for(i = 0;i<A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if( m_pA3DAssist2DOSOP[i] )	
			m_pA3DAssist2DOSOP[i]->LogicRun();
	}

	for(i = 0;i<A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if( m_aA3DAssist3DSnd[i] )	
			m_aA3DAssist3DSnd[i]->LogicRun();
	}

	if (m_pA3DAssist2DSnd)
		m_pA3DAssist2DSnd->LogicRun();

	// 背景音乐
	if( m_FT4SSFade.IsActive() )
	{
		unsigned long	lCurVol;
		lCurVol	= (unsigned long)m_FT4SSFade.GetCurVal();
		if( !m_FT4SSFade.IsActive() )
		{
			// 刚刚Fade完, Stop之
			if( m_bFadeNeedStop )
			{
				BM_Stop();
			}
			// 有紧接着要播放的
			if( m_szNextBMFile[0] )
			{
				m_bFadeNeedStop		= false;
				BM_Play(m_szNextBMFile);
				m_szNextBMFile[0]	= 0;
				m_FT4SSFade.RevertBegin( m_fNextBMFadeIn );
			}
			else
			{
				// 恢复原来的音量
				lCurVol	= (unsigned long)m_FT4SSFade.GetInitial();
			}
		}
		BM_SetVolume(lCurVol);
	}

	return	true;
}
bool	A3DAssistTools_I::ResetResource()
{
	// 重置所有资源（一般在一个游戏阶段结束时）

	int		i;

	// 字体
	for(i=0;i<A3DASSISTTOOLS_MAX_FONT;i++)
	{
		if( m_pAAA3dString[i] )
		{
			m_pAAA3dString[i]->ResetAll();
		}
		if( m_pAAA3dStringSet[i] )
		{
			m_pAAA3dStringSet[i]->ResetAll();
		}
	}
	// 音效
	for(i=0;i<A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if( m_pA3DAssist2DOSOP[i] )
		{
			m_pA3DAssist2DOSOP[i]->ResetAll();
		}
	}

	for(i=0;i<A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if( m_aA3DAssist3DSnd[i] )
		{
			m_aA3DAssist3DSnd[i]->ResetAll();
		}
	}

	if( m_pA3DAssist2DSnd )
	{
		m_pA3DAssist2DSnd->ResetAll();
	}

	return	true;
}

bool	A3DAssistTools_I::F_RegisterFont(int nFont, A3DFONT_INFO_T *pInfo)
{
	if( nFont<0 || nFont>=A3DASSISTTOOLS_MAX_FONT )
	{
		return	false;
	}
	HA3DFONT	hFont;
	if( !m_hFont[nFont] )
	{
		hFont	= m_pA3DEngine->GetA3DFontMan()->RegisterFont(
				pInfo->pszName
				, pInfo->nHeight
				, pInfo->nFlag
			);
		if( !hFont )
		{
			return	false;
		}
		m_hFont[nFont]	= hFont;
	}
	else
	{
		return	false;
	}
	if( pInfo->nBufNum>0 )
	{
		A3DAssistA3dString	*pA3DAssistA3dString;
		pA3DAssistA3dString	= m_pAAA3dString[nFont];
		if( pA3DAssistA3dString )
		{
			pA3DAssistA3dString->Release();
			delete	pA3DAssistA3dString;
			m_pAAA3dString[nFont]	= NULL;
		}
		pA3DAssistA3dString	= new A3DAssistA3dString;
		if( !pA3DAssistA3dString )
		{
			return	false;
		}
		if( !pA3DAssistA3dString->Init(m_pA3DEngine, hFont, pInfo->nBufNum, pInfo->nStrLen) )
		{
			return	false;
		}

		m_pAAA3dString[nFont]	= pA3DAssistA3dString;
	}
	if( pInfo->nSetBufNum>0 )
	{
		A3DAssistA3dStringSet	*pA3DAssistA3dStringSet;
		pA3DAssistA3dStringSet	= m_pAAA3dStringSet[nFont];
		if( pA3DAssistA3dStringSet )
		{
			pA3DAssistA3dStringSet->Release();
			delete	pA3DAssistA3dStringSet;
			m_pAAA3dStringSet[nFont]	= NULL;
		}
		pA3DAssistA3dStringSet	= new A3DAssistA3dStringSet;
		if( !pA3DAssistA3dStringSet )
		{
			return	false;
		}
		if( !pA3DAssistA3dStringSet->Init(m_pA3DEngine, hFont, pInfo->nSetBufNum, pInfo->nSetStrLen) )
		{
			return	false;
		}

		m_pAAA3dStringSet[nFont]	= pA3DAssistA3dStringSet;
	}

	if(m_nCurFont < 0)
	{/*
		m_nCurFont			= nFont;
		m_nFontHeight		= nNewSize;
		m_pCurAAA3dString	= m_pAAA3dString[nFont];
		m_pCurAAA3dStringSet= m_pAAA3dStringSet[nFont];*/
		F_SetCurFont(nFont);
	}

	return	true;
}
bool	A3DAssistTools_I::F_RegisterColor(int nColor, A3DCOLOR acolor)
{
	if( nColor<0 || nColor>=A3DASSISTTOOLS_MAX_COLOR )
	{
		return	false;
	}
	m_color[nColor]	= acolor;

	return	true;
}
bool	A3DAssistTools_I::F_SetCurFont(int nFont, int nNewSize)
{
	if( nFont<0 || nFont>=A3DASSISTTOOLS_MAX_FONT )
	{
		return	false;
	}
	m_nCurFont			= nFont;
	m_nFontHeight		= nNewSize;
	m_pCurAAA3dString	= m_pAAA3dString[nFont];
	m_pCurAAA3dStringSet= m_pAAA3dStringSet[nFont];
	return	true;
}
bool	A3DAssistTools_I::F_SetCur3DFont(int nFont, float fRatio)
{
	m_nCurFont			= nFont;
	m_fFontRatio		= fRatio;
	m_pCurAAA3dString	= m_pAAA3dString[nFont];
	return	true;
}
bool	A3DAssistTools_I::F_GetTextExtent(char *szText, int *pnW, int *pnH)
{
	bool	bval;
	bval	= m_pA3DEngine->GetA3DFontMan()->
		GetTextExtent(m_hFont[m_nCurFont], szText, strlen(szText), pnW, pnH);
	return	bval;
}
bool	A3DAssistTools_I::F_TextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade)
{
	if( !m_pCurAAA3dString )
	{
		return	false;
	}
	return	m_pCurAAA3dString->TextOut(nX, nY, szText, color, bShade, m_nFontHeight);
}
bool	A3DAssistTools_I::F_HtmlTextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade)
{
	F_TextOut_Begin();

	// 分析
	const int	cst_DFT_MAX_SECT	= 8;	// 最多的段数
	A3DCOLOR	sectColor;					// 一小段内的颜色
	char		*p, *pEnd;
	char		szPrevStr[AATS_HTMLSTR_MAX_LEN];
	char		szCmnPrevStr[AATS_HTMLSTR_MAX_LEN];
	int			nCount;
	int			nLen, nDX;
	int			nW, nH;
	int			nColorIdx;
	bool		bCanStop;

	nCount		= 0;
	bCanStop	= false;
	nLen		= 0;
	nDX			= 0;
	sectColor	= color;
	do
	{
		p	= strchr(szText, '<');
		if( !p
		||  nCount>=cst_DFT_MAX_SECT
		||  !(pEnd = strchr(p, '>'))
		)
		{
			// 没有html数据，直接输出整个字串
			strcpy(szPrevStr, szText);
			nLen		= strlen(szPrevStr);
			bCanStop	= true;
		}
		else
		{
			// 保存上一个字串
			nLen	= p - szText;
			memcpy(szPrevStr, szText, nLen);
			szPrevStr[nLen]	= 0;
		}
		// 输出上一个字串
		if( nLen )
		{
			HtmlToCmn(szCmnPrevStr, sizeof(szCmnPrevStr), szPrevStr);
			F_TextOut_AddString(nDX, sectColor, szCmnPrevStr);
			// 获得上一个字串的显示长度
			F_GetTextExtent(szCmnPrevStr, &nW, &nH);
			nDX			= nW;
		}
		else
		{
			nDX			= 0;
		}
		// 结束
		if( bCanStop )
		{
			break;
		}
		// 找下一个颜色
		nColorIdx	= atoi(p+1);
		if( pEnd-p>1 && nColorIdx>=0 && nColorIdx<A3DASSISTTOOLS_MAX_COLOR )
		{
			sectColor	= m_color[nColorIdx];
		}
		else
		{
			sectColor	= color;
		}
		// 指针后移
		szText	= pEnd + 1;
		nCount	++;
	}while(!bCanStop);

	F_TextOut_End(nX, nY, bShade);

	return	true;
}
bool	A3DAssistTools_I::F_TextOut3D(A3DVECTOR3 vecPos, char *szText, A3DCOLOR color)
{
	if( !m_pCurAAA3dString )
	{
		return	false;
	}
	return	m_pCurAAA3dString->TextOut3D(vecPos, szText, color, m_fFontRatio);
}

bool	A3DAssistTools_I::F_TextOut_Begin()
{
	if( !m_pCurAAA3dStringSet )
	{
		return	false;
	}
	m_pCurAAA3dStringSet->TextOut_Begin();
	return	true;
}
bool	A3DAssistTools_I::F_TextOut_AddString(int nDX, A3DCOLOR color, char *szText)
{
	return	m_pCurAAA3dStringSet->TextOut_Add(nDX, color, szText);
}
bool	A3DAssistTools_I::F_TextOut_End(int nX, int nY, bool bShade)
{
	return	m_pCurAAA3dStringSet->TextOut_End(nX, nY, bShade);
}

bool	A3DAssistTools_I::LCD_RegisterFont(int nLCDFont, A3DLCD_INFO_T *pInfo)
{
	if( nLCDFont<0 || nLCDFont>= A3DASSISTTOOLS_MAX_LCDFONT )
	{
		return	false;
	}
	if( m_pA3DAssistLCD[nLCDFont] )
	{
		return	false;
	}

	bool			bval;
	A3DAssistLCD	*pA3DAssistLCD;
	pA3DAssistLCD	= new A3DAssistLCD;
	if( !pA3DAssistLCD )
	{
		return	false;
	}
	bval			= pA3DAssistLCD->Init(
					m_pA3DEngine
					, pInfo->pszChars
					, pInfo->pszCharGraphFile
					, pInfo->nRow
					, pInfo->nCol
					, pInfo->nUnitW
					, pInfo->nUnitH
					);
	if( !bval )
	{
		return	false;
	}

	m_pA3DAssistLCD[nLCDFont]	= pA3DAssistLCD;

	return	bval;
}
bool	A3DAssistTools_I::LCD_GetTextExtent(char *szText, int *nW, int *nH)
{
	if( !m_pCurAALCD )
	{
		return	false;
	}
	return	m_pCurAALCD->GetTextExtent(szText, nW, nH);
}
bool	A3DAssistTools_I::LCD_SetCurFont(int nLCDFont, float fRatio)
{
	if( nLCDFont<0 || nLCDFont>=A3DASSISTTOOLS_MAX_LCDFONT )
	{
		return	false;
	}

	m_nCurLCDFont	= nLCDFont;
	m_pCurAALCD		= m_pA3DAssistLCD[m_nCurLCDFont];
	m_pCurAALCD->SetRatio(fRatio);

	return	true;
}
bool	A3DAssistTools_I::LCD_SetSep(int nSep)
{
	m_pCurAALCD->SetSep(nSep);
	return	true;
}
bool	A3DAssistTools_I::LCD_TextOut(int nX, int nY, char *szText, A3DCOLOR color)
{
	if( !m_pCurAALCD )
	{
		return	false;
	}
	return	m_pCurAALCD->TextOut(nX, nY, szText, color);
}

bool	A3DAssistTools_I::S_SndInit(int* n2DBufTypeBuf,int n2DBufTypeCount, int n2DDupBuf, 
									int* n3DBufTypeBuf, int n3DBufTypeCount, int nFNLen)
{
	bool bval;
	int i;

	for(i=0;i<n2DBufTypeCount;i++)
	{
		if( n2DBufTypeBuf[i]<=0 )
			continue;
		m_pA3DAssist2DOSOP[i]	= new A3DAssist2DOSOP;
		if( !m_pA3DAssist2DOSOP[i] )
		{
			return	false;
		}
		bval	= m_pA3DAssist2DOSOP[i]->Init(m_pA3DEngine, n2DBufTypeBuf[i], nFNLen);
		if( !bval )
		{
			return	false;
		}
	}
	if( n2DDupBuf>0 )
	{
		m_pA3DAssist2DSnd	= new A3DAssist2DSnd;
		if( !m_pA3DAssist2DSnd )
		{
			return	false;
		}
		bval	= m_pA3DAssist2DSnd->Init(m_pA3DEngine, n2DDupBuf, nFNLen);
		if( !bval )
		{
			return	false;
		}
	}
	for (i=0; i < n3DBufTypeCount; i++)
	{
		if (n3DBufTypeBuf[i]<=0)
			continue;

		m_aA3DAssist3DSnd[i] = new A3DAssist3DSnd;
		if( !m_aA3DAssist3DSnd[i] )
		{
			return	false;
		}
		bval	= m_aA3DAssist3DSnd[i]->Init(m_pA3DEngine, n3DBufTypeBuf[i], nFNLen);
		if( !bval )
		{
			return	false;
		}
	}

	return	true;
}
void	A3DAssistTools_I::S_SetJustLoad(bool bSet)
{
	int i;
	for(i=0;i<A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if( m_pA3DAssist2DOSOP[i] )
		{
			m_pA3DAssist2DOSOP[i]->SetJustLoad(bSet);
		}
	}

	for(i=0;i<A3DASSISTTOOLS_MAX_SOUNDTYPE;i++)
	{
		if( m_aA3DAssist3DSnd[i] )
		{
			m_aA3DAssist3DSnd[i]->SetJustLoad(bSet);
		}
	}

	if( m_pA3DAssist2DSnd )
	{
		m_pA3DAssist2DSnd->SetJustLoad(bSet);
	}
}
bool	A3DAssistTools_I::S_2DSndPlay(int type,char *szSndFile, bool bImmEffect, bool bLoop, bool bCanFade)
{
	if(type<0 || type >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return true;
	if(m_pA3DAssist2DOSOP[type])
	{
		return	m_pA3DAssist2DOSOP[type]->Play(szSndFile, bImmEffect, bLoop, bCanFade);
	}
	return true;
}
bool	A3DAssistTools_I::S_2DSndStop(int type,char *szSndFile)
{
	if(type<0 || type >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return true;
	if(m_pA3DAssist2DOSOP[type])
	{
		return	m_pA3DAssist2DOSOP[type]->Stop(szSndFile);
	}
	return true;
}
bool	A3DAssistTools_I::S_2DSndPause(int type,char *szSndFile)
{
	if(type<0 || type >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return true;
	if(m_pA3DAssist2DOSOP[type])
	{
		return	m_pA3DAssist2DOSOP[type]->Pause(szSndFile);
	}
	return true;
}
bool	A3DAssistTools_I::S_2DSndIsStopped(int type,char *szSndFile)
{
	if(type<0 || type >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return true;
	if(m_pA3DAssist2DOSOP[type])
	{
		return	m_pA3DAssist2DOSOP[type]->IsStopped(szSndFile);
	}
	return true;
}
bool	A3DAssistTools_I::S_2DDupSndPlay(char *szSndFile, bool bImmEffect, bool bCanFade)
{
	return	m_pA3DAssist2DSnd->Play(szSndFile, bImmEffect, bCanFade);
}
bool	A3DAssistTools_I::S_3DSndPlay(int iType, char *szSndFile, A3DVECTOR3 &vecPos, float fMinDist, float fMaxDist, bool bImmEffect, bool bForce2D, bool bCanFade)
{
	if(iType<0 || iType >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return true;

	if(m_aA3DAssist3DSnd[iType])
		return m_aA3DAssist3DSnd[iType]->Play(szSndFile, vecPos, fMinDist, fMaxDist, bImmEffect, bForce2D, bCanFade);

	return true;
}
void	A3DAssistTools_I::S_SetVolume(unsigned int nVol)
{
	m_pA3DEngine->GetAMSoundEngine()->SetVolume(nVol);
}
void	A3DAssistTools_I::S_2DSndFadeIn(int type,int ms)
{
	if(type<0 || type >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return;
	if(m_pA3DAssist2DOSOP[type])
	{
		m_pA3DAssist2DOSOP[type]->FadeIn(ms);
		m_pA3DAssist2DSnd->FadeIn(ms);
	}
}
void	A3DAssistTools_I::S_2DSndFadeOut(int type)
{
	if(type<0 || type >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return;
	if(m_pA3DAssist2DOSOP[type])
	{
		m_pA3DAssist2DOSOP[type]->FadeOut();
		m_pA3DAssist2DSnd->FadeOut();
	}
}
void	A3DAssistTools_I::S_3DSndFadeIn(int iType, int ms)
{
	if(iType<0 || iType >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return;

	if(m_aA3DAssist3DSnd[iType])
		m_aA3DAssist3DSnd[iType]->FadeIn(ms);
}
void	A3DAssistTools_I::S_3DSndFadeOut(int iType)
{
	if(iType<0 || iType >= A3DASSISTTOOLS_MAX_SOUNDTYPE)
		return;

	if(m_aA3DAssist3DSnd[iType])
		m_aA3DAssist3DSnd[iType]->FadeOut();
}

bool	A3DAssistTools_I::BM_Play(char *szSndFile)
{
	return	m_pAMSoundStream->Play(szSndFile);
}
bool	A3DAssistTools_I::BM_Stop()
{
	return	m_pAMSoundStream->Stop();
}
bool	A3DAssistTools_I::BM_Pause(bool bPause)
{
	return	m_pAMSoundStream->Pause(bPause);
}
bool	A3DAssistTools_I::BM_IsStopped()
{
	return	m_pAMSoundStream->IsStopped();
}
void	A3DAssistTools_I::BM_SetLoop(bool bLoop)
{
	m_pAMSoundStream->SetLoopFlag(bLoop);
}
bool	A3DAssistTools_I::BM_GetLoop()
{
	return	m_pAMSoundStream->GetLoopFlag();
}
void	A3DAssistTools_I::BM_SetVolume(unsigned int nVol)
{
	m_pAMSoundStream->SetVolume(nVol);
}
unsigned int	A3DAssistTools_I::BM_GetVolume()
{
	return	m_pAMSoundStream->GetVolume();
}
bool	A3DAssistTools_I::BM_FadeOut(float fOT, char *szNextFile, float fIT)
{
	float	fVol;
	m_bFadeNeedStop		= true;
	if( m_FT4SSFade.IsActive() )
	{
		fVol	= m_FT4SSFade.GetInitial();
	}
	else
	{
		fVol	= (float)BM_GetVolume();
	}
	m_FT4SSFade.SetInitial(fVol);
	m_FT4SSFade.Begin(fVol, 0, fOT);
	if( szNextFile )
	{
		strcpy(m_szNextBMFile, szNextFile);
		m_fNextBMFadeIn	= fIT;
	}
	else
	{
		m_szNextBMFile[0]	= 0;
	}
	return	true;
}
bool	A3DAssistTools_I::BM_FadeIn(char *szNextFile, float fIT)
{
	float	fVol;
	m_bFadeNeedStop		= false;
	if( m_FT4SSFade.IsActive() )
	{
		fVol	= m_FT4SSFade.GetInitial();
	}
	else
	{
		fVol	= (float)BM_GetVolume();
	}
	m_FT4SSFade.SetInitial(fVol);
	BM_Play(szNextFile);
	BM_SetVolume(0);
	m_FT4SSFade.Begin(0, fVol, fIT);
	m_szNextBMFile[0]	= 0;
	return	true;
}