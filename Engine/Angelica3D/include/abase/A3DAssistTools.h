// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistTools.h
// Creator: Wei Hua (魏华)
// 用于简化应用的工具集，在引擎初始化后创建该对象。

#ifndef	__A3DASSISTTOOLS_H__
#define	__A3DASSISTTOOLS_H__

////////////////////////////////////////////////////////////////////
// 功能函数
////////////////////////////////////////////////////////////////////
// 用于在普通字串和html格式字串间转化
int	CmnToHtml(char *__html, int __size, char *__cmn);
int	HtmlToCmn(char *__cmn, int __size, char *__html);
// 设置当前时间，用于确定一个缓冲单元的最后使用时间
// 只需要在LogicRun中自己增加自己的Tick就可以了
// （这个函数取消）
// void	SetCurTickToA3DAssistTools(int nTick);

////////////////////////////////////////////////////////////////////
// A3DAssistTools接口定义
////////////////////////////////////////////////////////////////////
#include <A3DTypes.h>
class	A3DEngine;

#define	AATS_HTMLSTR_MAX_LEN				1024

// 用于注册普通字体
typedef struct	__A3DFONT_INFO_T
{
	char	*pszName;
	int		nHeight;
	int		nFlag;
	int		nBufNum;							// A3DString的缓冲各数
	int		nStrLen;							// 最大字串长度
	int		nSetBufNum;							// A3DStringSet的缓冲各数
	int		nSetStrLen;							// 最大字串长度
}A3DFONT_INFO_T;
// 用于注册LCD图片字体
typedef struct	__A3DLCD_INFO_T
{
	char	*pszChars;							// 能输出的字符列表
	char	*pszCharGraphFile;					// 和输出字符对应的一行等宽小图文件
	int		nRow;								// 图的行数
	int		nCol;								// 图的列数
	int		nUnitW;								// 每个小图宽
	int		nUnitH;								// 每个小图高
}A3DLCD_INFO_T;

class	A3DAssistTools
{
public:
	virtual			~A3DAssistTools()										{;}
	virtual void	SetSystemTickcount(unsigned long nT)					= 0;	// 设置系统当前的时间（毫秒），用于声音的fade 。应该在主逻辑每次获得时间的时候设置这个
	virtual void	AddSystemTickcount(unsigned long nT)					= 0;	// 累加当前的时间间隔（毫秒），用于声音的fade 。应该在主逻辑每次获得时间的时候设置这个
	virtual bool	LogicRun()												= 0;	// 里面控制的一些东西需要Tick
	// 重置
	virtual bool	ResetResource()											= 0;	// 重置所有资源（一般在一个游戏阶段结束时）
	// 字体相关
	virtual bool	F_RegisterFont(int nFont, A3DFONT_INFO_T *pInfo)		= 0;	// 注册。在程序开始前确定总共需要那些字体，然后一次全部注册。
	virtual bool	F_RegisterColor(int nColor, A3DCOLOR acolor)			= 0;	// 注册html中会使用到的颜色。
	virtual bool	F_SetCurFont(int nFont, int nNewSize = 0)				= 0;// 设置马上需要使用的2D字体。
	virtual	int		F_GetCurFont()											= 0;
	virtual int		F_GetFont(int nFont)									= 0;
	virtual bool	F_SetCur3DFont(int nFont, float fRatio = 0)				= 0;	// 设置马上需要使用的3D字体。
	virtual bool	F_GetTextExtent(char *szText, int *pnW, int *pnH)		= 0;	// 获得当前字体下，指定字串的屏幕输出尺寸
	virtual bool	F_TextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade)= 0;	// 在屏幕指定位置输出指定颜色的字串
	virtual bool	F_HtmlTextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade)=0;	// 输出字串中可以利用类html格式改变某一段的字体颜色
	virtual bool	F_TextOut3D(A3DVECTOR3 vecPos, char *szText, A3DCOLOR color)		= 0;	// 在三维世界的一个位置输出字串
	//		以行表格字符串输出
	//		nX, nY是起始坐标
	virtual bool	F_TextOut_Begin()										= 0;
	//		nDX是坐标增量
	virtual bool	F_TextOut_AddString(int nDX, A3DCOLOR color, char *szText)			= 0;
	//		bShade表示是否有影子边
	virtual bool	F_TextOut_End(int nX, int nY, bool bShade)				= 0;
	// LCD字符相关
	virtual bool	LCD_RegisterFont(int nLCDFont, A3DLCD_INFO_T *pInfo)	= 0;	// 注册。可以注册多种LCD字体。
	virtual bool	LCD_GetTextExtent(char *szText, int *nW, int *nH)		= 0;	// 获得如果变成LCD字串后的屏幕输出尺寸
	virtual bool	LCD_SetCurFont(int nLCDFont, float fRatio = 0)			= 0;	// 设置马上需要使用的LCD字体。
	virtual bool	LCD_SetSep(int nSep)									= 0;	// 设置字间距
	virtual bool	LCD_TextOut(int nX, int nY, char *szText, A3DCOLOR color)			= 0;	// 在指定位置输出指定颜色的LCD样子的字串
	// 声音相关
	virtual bool	S_SndInit(int * n2DBufTypeBuf,int n2DBufTypeCount, int n2DDupBuf,
							  int* n3DBufTypeBuf, int n3DBufTypeCount, int nFNLen)		= 0;	// 初始化缓冲个数（nFNLen是文件名对大长度）
	virtual void	S_SetJustLoad(bool bSet)								= 0;	// 设置声音只load不播
	virtual bool	S_2DSndPlay(int type,char *szSndFile, bool bImmEffect, bool bLoop, bool bCanFade)	= 0;
	virtual bool	S_2DSndStop(int type,char *szSndFile)							= 0;
	virtual bool	S_2DSndPause(int type,char *szSndFile)							= 0;
	virtual bool	S_2DSndIsStopped(int type,char *szSndFile)						= 0;
	virtual bool	S_2DDupSndPlay(char *szSndFile, bool bImmEffect, bool bCanFade)		= 0;	// 可以一个声音有多重
	virtual bool	S_3DSndPlay(int iType, char *szSndFile, A3DVECTOR3 &vecPos, float fMinDist, float fMaxDist
					, bool bImmEffect, bool bForce2D, bool bCanFade)		= 0;
	virtual void	S_SetVolume(unsigned int nVol)							= 0;	// 音量
	virtual void	S_2DSndFadeIn(int type,int ms)									= 0;	// 在这之后ms毫秒内的所有需要fade的2D声音都会fadein
	virtual void	S_2DSndFadeOut(int type)										= 0;	// 当前正在播放的所有需要fade的2D声音都会fadeout
	virtual void	S_3DSndFadeIn(int iType, int ms)								= 0;	// 在这之后ms毫秒内的所有需要fade的2D声音都会fadein
	virtual void	S_3DSndFadeOut(int iType)										= 0;	// 当前正在播放的所有需要fade的2D声音都会fadeout
	// 背景音乐
	virtual bool	BM_Play(char *szSndFile)								= 0;
	virtual bool	BM_Stop()												= 0;
	virtual bool	BM_Pause(bool bPause)									= 0;
	virtual bool	BM_IsStopped()											= 0;
	virtual void	BM_SetLoop(bool bLoop)									= 0;
	virtual bool	BM_GetLoop()											= 0;
	virtual void	BM_SetVolume(unsigned int nVol)							= 0;	// 音量
	virtual unsigned int	BM_GetVolume()									= 0;
	virtual bool	BM_FadeOut(float fOT, char *szNextFile = NULL, float fIT = 0)	= 0;
	virtual bool	BM_FadeIn(char *szNextFile, float fIT)					= 0;
																					// 淡出当前音乐，然后淡入下一个音乐
};

// 在NewA3DAssistTools中用于初始化该对象
typedef struct	__A3DASSISTTOOLS_INFO_T
{
	A3DEngine	*pA3DEngine;
}A3DASSISTTOOLS_INFO_T;
A3DAssistTools *	NewA3DAssistTools(A3DASSISTTOOLS_INFO_T *pInfo);

#endif