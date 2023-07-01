#include "AMath.h"

// 认为最小的float
static const	float	MIN_FLOAT	= float(0.00001);
static const	double	PIE			= 3.1415926;
static const	double	PIE_M2		= PIE * 2;
static const	double	PIE_D2		= PIE / 2;

// 看一个浮点数是否可以认为是0
int	IsFloatZero(float __f)
{
	if( fabs(__f) < MIN_FLOAT )
	{
		return	1;
	}
	return	0;
}

// 将方向向量转化为水平转角(0~2PIE)和垂直转角(-PIE/2~PIE/2)，因为上下看的范围应该只有前面的(-PIE/2~PIE/2)
// 然后按等分格数转化为整数
// __angleh和__anglev分别是角度的水平数值和竖直数值
// __htotal和__vtotal分别是水平圆周和竖直半园周的等分格数
int	Dir2TwoAngle(float __dx, float __dy, float __dz
	, int *__angleh, int *__anglev, int __htotal, int __vtotal)
{
	double	rh, rv, ah, av;
	rh	= sqrt(__dx*__dx + __dz*__dz);
	if( IsFloatZero(float(rh)) )
	{
		*__angleh	= 0;
		*__anglev	= __dy >0 ? 0 : __vtotal-1;
		return	0;
	}
	ah	= acos(__dx / rh);
	if( __dz<=0 )
	{
		ah	= PIE_M2-ah;
	}
	rv	= sqrt(__dy*__dy + rh*rh);
	av	= asin(__dy / rv);
	*__angleh	= int(ah * __htotal / PIE_M2 + 0.5);
	if( *__angleh >= __htotal )	*__angleh = 0;
	*__anglev	= int((PIE_D2 - av) * __vtotal / (PIE) + 0.5);
	if( *__anglev >= __vtotal )	*__anglev = __vtotal-1;
	return	0;
}

int	TwoAngle2Dir(int __angleh, int __anglev, int __htotal, int __vtotal
	, float *__dx, float *__dy, float *__dz)
{
	double	rh, rv, ah, av, x, y, z;
	ah	= ((double)__angleh) * PIE_M2 / __htotal;
	x	= cos(ah);
	z	= sqrt(1 - x*x);
	if( ah>PIE )	z = -z;
	rh	= sqrt(x*x + z*z);
	av	= PIE_D2 - ((double)__anglev) * PIE / __vtotal;
	rv	= cos(av);
	if( IsFloatZero(float(rv)) )
	{
		*__dy	= float(av>0 ? 1 : -1);
		*__dx	= float(0);
		*__dz	= float(0);
		return	0;
	}
	rv	= rh / rv;
	y	= rv * sin(av);
	*__dx	= float(x);
	*__dy	= float(y);
	*__dz	= float(z);
	return	0;
}

int	TwoAngle2Dir(float __angleh, float __anglev
	, float *__dx, float *__dy, float *__dz)
{
	double	rh, rv, ah, av, x, y, z;
	ah	= __angleh;
	x	= cos(ah);
	z	= sqrt(1 - x*x);
	if( ah>PIE )	z = -z;
	rh	= sqrt(x*x + z*z);
	av	= __anglev;
	rv	= cos(av);
	if( IsFloatZero(float(rv)) )
	{
		*__dy	= float(av>0 ? 1 : -1);
		*__dx	= float(0);
		*__dz	= float(0);
		return	0;
	}
	rv	= rh / rv;
	y	= rv * sin(av);
	*__dx	= float(x);
	*__dy	= float(y);
	*__dz	= float(z);
	return	0;
}

int	NormalizeDir(float *__dx, float *__dy, float *__dz)
{
	double	r;
	r	= sqrt(*__dx * *__dx + *__dy * *__dy + *__dz * *__dz);
	*__dx	= float(*__dx/r);
	*__dy	= float(*__dy/r);
	*__dz	= float(*__dz/r);
	return	0;
}

// 水平角度(0~2PIE)，垂直角度(-PIE/2~PIE/2)
int	TwoAngle2Int(float __fh, float __fv, int *__ih, int *__iv, int __htotal, int __vtotal)
{
	double	ah, av;
	ah		= __fh;
	av		= __fv;
	*__ih	= int(ah * __htotal / PIE_M2 + 0.5);
	if( *__ih >= __htotal )	*__ih = 0;
	*__iv	= int((PIE_D2 - av) * __vtotal / (PIE) + 0.5);
	if( *__iv >= __vtotal )	*__iv = __vtotal-1;
	return	0;
}

int	Int2TwoAngle(int __ih, int __iv, int __htotal, int __vtotal, float *__fh, float *__fv)
{
	double	ah, av;
	ah	= ((double)__ih) * PIE_M2 / __htotal;
	av	= PIE_D2 - ((double)__iv) * PIE / __vtotal;
	*__fh	= (float)ah;
	*__fv	= (float)av;
	return	0;
}

//水平为向量，垂直为(-PIE/2~PIE/2)
int	DirAngle2Vector(float __dx, float __dz, float __av, float *__dirx, float *__diry, float *__dirz)
{
	double	dy;
	dy		= cos(__av);
	if(IsFloatZero((float)dy))
	{
		*__dirx	= 0;
		*__dirz	= 0;
		*__diry	= (float)sin(__av);
		return	0;
	}
	*__diry	= float(sqrt(__dx*__dx + __dz*__dz) / dy * sin(__av));
	*__dirx	= __dx;
	*__dirz	= __dz;

	return	0;
}
int	Vector2DirAngle(float __dirx, float __diry, float __dirz, float *__dx, float *__dz, float *__av)
{
	double	rh, rv, av;
	rh		= sqrt(__dirx*__dirx + __dirz*__dirz);
	if(IsFloatZero((float)rh))
	{
		// 水平角度不变，竖直角度为pie/2 ~ -pie/2
		*__av	= float(__diry>0 ? PIE_D2 : -PIE_D2);
		return	0;
	}
	rv	= sqrt(__diry*__diry + rh*rh);
	av	= asin(__diry / rv);
	*__av	= (float)av;
	*__dx	= __dirx;
	*__dz	= __dirz;
	return	0;
}

// 方向向量变成角度[0,2PIE)，返回-1表示没有角度
int	Dir2Degree(float __dx, float __dz, float *__degree)
{
	double	r, degree;
	r		= sqrt(__dx*__dx + __dz*__dz);
	if( IsFloatZero(float(r)) )
	{
		return	-1;
	}
	degree	= acos(__dx / r);
	if( __dz<0 )
	{
		degree	= PIE_M2 - degree;
	}
	*__degree	= (float)degree;
	return	0;
}
// 变成[1,360]，0表示没有角度
int	Dir2IntDegree(float __dx, float __dz, int *__degree)
{
	float	fd;
	int		rst;
	rst		= Dir2Degree(__dx, __dz, &fd);
	if( rst<0 )
	{
		*__degree	= 0;
		return	0;
	}
	if( IsFloatZero(fd) )
	{
		*__degree	= 360;
	}
	else
	{
		*__degree	= int(fd * 360 / PIE_M2);
	}
	return	0;
}

int	Degree2Dir(float __degree, float *__dx, float *__dz)
{
	*__dx	= (float)cos(__degree);
	*__dz	= (float)sin(__degree);
	return	0;
}

int GetPointWay(float x,float z,float x1,float z1)
{
	float a=z * x1 + (-x) * z1 ;
	return a>0? 1 : -1;
}

float	GetInterpolation(float f, float f1, float f2, float v1, float v2)
{
	float	r;
	r		= (f-f1) / (f2-f1);
	return	v1 * (1-r) + v2 * (r);
}

A3DVECTOR3 UpProduct(A3DVECTOR3 vecDir)
{
	if(fabs(vecDir.x)<0.001 && fabs(vecDir.z)<0.001)
		return A3DVECTOR3(1.f,0.f,1.f);
	A3DVECTOR3 vecDir2 = vecDir;
	A3DVECTOR3 vecUp,vecRight;
	vecDir2.y -= 5.f;
	vecRight = CrossProduct(vecDir2,vecDir);
	vecUp = CrossProduct(vecRight,vecDir);
	if(vecUp.y <0)
		vecUp = vecUp *(-1);
	return vecUp;
}
