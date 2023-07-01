/*
 * FILE: AMath.h
 *
 * DESCRIPTION: 基本数学函数
 *
 * CREATED BY: 王远明, 2002/8/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */


#ifndef __AMATH__H__
#define __AMATH__H__

#include <math.h>
#include "abasea3d.h"

// 看一个浮点数是否可以认为是0
int	IsFloatZero(float __f);

// 将方向向量转化为水平转角(0~2PIE)和垂直转角(-PIE/2~PIE/2)，
// __angleh和__anglev分别是角度的水平数值和竖直数值
// __htotal和__vtotal分别是水平圆周和竖直半园周的等分格数
int	Dir2TwoAngle(float __dx, float __dy, float __dz
	, int *__angleh, int *__anglev, int __htotal, int __vtotal);
int	TwoAngle2Dir(int __angleh, int __anglev, int __htotal, int __vtotal
	, float *__dx, float *__dy, float *__dz);
int	TwoAngle2Dir(float __angleh, float __anglev
	, float *__dx, float *__dy, float *__dz);
int	NormalizeDir(float *__dx, float *__dy, float *__dz);

// 水平角度(0~2PIE)，垂直角度(-PIE/2~PIE/2)
int	TwoAngle2Int(float __fh, float __fv, int *__ih, int *__iv, int __htotal, int __vtotal);
int	Int2TwoAngle(int __ih, int __iv, int __htotal, int __vtotal, float *__fh, float *__fv);

//水平为向量，垂直为(-PIE/2~PIE/2)
int	DirAngle2Vector(float __dx, float __dz, float __av, float *__dirx, float *__diry, float *__dirz);
int	Vector2DirAngle(float __dirx, float __diry, float __dirz, float *__dx, float *__dz, float *__av);
// 方向向量变成角度[0,2PIE)，返回-1表示没有角度
int	Dir2Degree(float __dx, float __dz, float *__degree);
int	Degree2Dir(float __degree, float *__dx, float *__dz);
// 变成[1,360]，0表示没有角度
int	Dir2IntDegree(float __dx, float __dz, int *__degree);
//返回-1表示向量(x1,z1)相对于向量(x,z)左偏，否则右偏
int GetPointWay(float x,float z,float x1,float z1);
// 插值
float	GetInterpolation(float f, float f1, float f2, float v1, float v2);
A3DVECTOR3 UpProduct(A3DVECTOR3 vecDir);

#endif  //__AMATH__H__