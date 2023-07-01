// File		: CommonFuncs.h
// Creator	: Wei Hua
// 大家都可以用的一些通用函数

#ifndef	__COMMONFUNCS_H__
#define	__COMMONFUNCS_H__

#include "A3DTypes.h"
#include "A3DFuncs.h"
// 公共函数
// 插值
float	_GetInterpolation(float f, float f1, float f2, float v1, float v2);
// 判断一行中是否有中文
bool	_HaveChineseInStr(char *__str);
float	_GetHDistance(A3DVECTOR3 Pos1, A3DVECTOR3 Pos2, float fMaxVLen);
int		_StrTrimC(char *__str, char __c);
float	_RandBetweenFloat(float __r1, float __r2);
int		_RandBetweenInt(int __r1, int __r2);		// 如果__r2 > __r1 , 随机范围是 __r1 到 __r2-1
bool	_IsPathExist(const char *__path, bool __searchpck = true);
int		_GetFileLen(const char *__file);
// 返回CDROM驱动器数目，__drives是由CDROM驱动器盘符组成的字串
int		_GetLogicalCDROM(char *__drives, int __len);
// 判断这些文件是否在光盘上
bool	_HaveTheseFilesOnCDROM(int __fnum, char **__files);

#endif	// __COMMONFUNCS_H__