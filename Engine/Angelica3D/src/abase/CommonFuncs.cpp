// File		: CommonFuncs.cpp
// Creator	: Wei Hua
// 大家都可以用的一些通用函数

#include <assert.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "CommonFuncs.h"

#define A_RAND_MAX 65536

// 插值
float	_GetInterpolation(float f, float f1, float f2, float v1, float v2)
{
	float	r;
	r		= (f-f1) / (f2-f1);
	return	v1 * (1-r) + v2 * (r);
}

// 判断一行中是否有中文
bool	_HaveChineseInStr(char *__str)
{
	static const int cst_MaxStrLen	= 1024;
	int	i;
	i	= 0;
	while(__str[i])
	{
		if( __str[i]<0 )	return true;
		i	++;
		assert(i<cst_MaxStrLen);
	}
	return	false;
}

float	_GetHDistance(A3DVECTOR3 Pos1, A3DVECTOR3 Pos2, float fMaxVLen)
{
	if( fabs(Pos1.y - Pos2.y) > fMaxVLen )
	{
		return	-1;
	}
	return	MagnitudeH(Pos1 - Pos2);
}

// 将字串两头为__c的当作空字符去掉
int		_StrTrimC(char *__str, char __c)
{
	int	i, l;
	char	*p;
	l	= strlen(__str);
	for(i=l-1; i>=0; i--)
	{
		if( __c == __str[i] )
		{
			__str[i]	= 0;
		}
		else
		{
			break;
		}
	}
	p	= __str;
	for(i=0; i<l; i++)
	{
		if( __c == __str[i] )
		{
			p = __str + i + 1;
		}
		else
		{
			break;
		}
	}
	strcpy( __str, p );

	return	0;
}

float	_RandBetweenFloat(float __r1, float __r2)
{
	return	__r1 + (__r2 - __r1) * rand()/ A_RAND_MAX;
}

int		_RandBetweenInt(int __r1, int __r2)
{
	return	int(__r1 + float(__r2 - __r1) * rand() / A_RAND_MAX);
}

#include <AFilePackage.h>
bool	_IsPathExist(const char *__path, bool __searchpck)
{
	if( _access(__path, 0) == -1 )
	{
		return false;
	}
	return	true;
}

int	_GetFileLen(const char *__file)
{
	struct stat st;
	if( stat(__file, &st) < 0 )
	{
		return	-1;
	}
	return	st.st_size;
}

// return number of CDROMs
// __drives must have a buffer of at least 32
// 返回CDROM驱动器数目，__drives是由CDROM驱动器盘符组成的字串
int _GetLogicalCDROM(char *__drives, int __len)
{
	DWORD dwdrv, dw1=1;
	char cdrv='A';
	int count=0;
	
	memset(__drives, 0, __len);
	dwdrv = GetLogicalDrives();
	for(int i=0;i<sizeof(dwdrv)*8;i++)
	{
		if( dw1 & dwdrv )
		{
			// it's a drive letter
			char sdrv[3];
			memset(sdrv, 0, 3);
			sprintf(sdrv, "%c:", cdrv);
			int rst = GetDriveType(sdrv);
			if( DRIVE_CDROM == rst )
			{
				__drives[count] = cdrv;
				count++;
				if( count>=__len )
				{
					goto	End;
				}
			}
		}
		dw1 <<= 1;
		cdrv ++;
	}
End:
	return count;
}

// 判断这些文件是否在光盘上
bool	_HaveTheseFilesOnCDROM(int __fnum, char **__files)
{
	int		i, j, cdnum;
	char	drives[32];

	cdnum	= _GetLogicalCDROM(drives, sizeof(drives));
	if( cdnum<=0 )
	{
		// 没有光驱
		return	false;
	}
	char	file[MAX_PATH];
	bool	bHaveIt;
	if( __fnum<=0 )
	{
		__fnum	= 128;
	}
	for(i=0;i<__fnum && __files[i];i++)
	{
		bHaveIt	= false;
		for(j=0;j<cdnum&&!bHaveIt;j++)
		{
			sprintf(file, "%c:/%s", drives[j], __files[i]);
			if( _IsPathExist(file, false) )
			{
				bHaveIt	= true;
			}
		}
		if( !bHaveIt )
		{
			// 在所有的光驱上都没有找到
			return	false;
		}
	}

	return	true;
}