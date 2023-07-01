/*
 * FILE: A3DMacros.h
 *
 * DESCRIPTION: Fundermental macros for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMACROS_H_
#define _A3DMACROS_H_

#include "A3DTypes.h"

#define A3D_PI		3.1415926535f
#define A3D_2PI		6.2831853072f

//	Plane's type
#define A3DPLANE_BAD		-1		//	Bad plane
#define A3DPLANE_PX			0		//	Positive x axis
#define A3DPLANE_PY			1		//	Positive y axis
#define A3DPLANE_PZ			2		//	Positive z axis
#define A3DPLANE_NX			3		//	Negative x axis
#define A3DPLANE_NY			4		//	Negative y axis
#define A3DPLANE_NZ			5		//	Negative z axis
#define A3DPLANE_MAJORX		6		//	Major axis is x
#define A3DPLANE_MAJORY		7		//	Major axis is y
#define A3DPLANE_MAJORZ		8		//	Major axis is z
#define A3DPLANE_UNKNOWN	9		//	unknown

//Warning: you must supply byte values as r, g and b or the result may be undetermined
#define A3DCOLORRGB(r, g, b)		((A3DCOLOR) (0xff000000 | ((r) << 16) |\
	((g) << 8) | (b)))						

//Warning: you must supply byte values as r, g, b and a, or the result may be undetermined
#define A3DCOLORRGBA(r, g, b, a)	((A3DCOLOR) (((a) << 24) | ((r) << 16) |\
	((g) << 8) | (b)))	

#define A3DCOLOR_GETRED(color) ((BYTE)(((color) & 0x00ff0000) >> 16))
#define A3DCOLOR_GETGREEN(color) ((BYTE)(((color) & 0x0000ff00) >> 8))
#define A3DCOLOR_GETBLUE(color) ((BYTE)(((color) & 0x000000ff)))
#define A3DCOLOR_GETALPHA(color) ((BYTE)(((color) & 0xff000000) >> 24))

#define min2(a, b) (((a) > (b)) ? (b) : (a))
#define min3(a, b, c) (min2(min2((a), (b)), (c)))
#define max2(a, b) (((a) > (b)) ? (a) : (b))
#define max3(a, b, c) (max2(max2((a), (b)), (c)))
#define min4(a, b, c, d) (min2(min2((a), (b)), min2((c), (d))))
#define max4(a, b, c, d) (max2(max2((a), (b)), max2((c), (d))))

#define DEG2RAD(deg) ((deg) * A3D_PI / 180.0f)
#define RAD2DEG(rad) ((rad) * 180.0f / A3D_PI)

#define FLOATISZERO(x)	((x) > -1e-7f && (x) < 1e-7f)

#define CLAMPVALUE(x, a, b)	if ((x) < (a))	(x) = (a);	else if ((x) > (b))	(x) = (b);
#define CLAMPMAXVALUE(x, a) if ((x) > (a))	(x) = (a);
#define CLAMPMINVALUE(x, a) if ((x) < (a))	(x) = (a);

//	Release A3D objects
#define A3DRELEASE(p) \
{ \
	if (p) \
	{ \
		(p)->Release(); \
		delete (p); \
		(p) = NULL; \
	} \
}

//	Release A3D interfaces
#define A3DINTERFACERELEASE(p) \
{ \
	if (p) \
	{ \
	(p)->Release(); \
	(p) = NULL; \
	} \
}

#define MEMBER_OFFSET(_CLS, _MEMBER)   ((size_t)(&(((_CLS*)0)->_MEMBER)))

#endif

