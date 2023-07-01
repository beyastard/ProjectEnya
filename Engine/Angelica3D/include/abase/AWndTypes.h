/*
 * FILE: AWndTypes.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-8-26
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AWNDTYPES_H__
#define	__AWNDTYPES_H__

#include "APlatform.h"

struct APOINT : public tagPOINT
{
public:

	APOINT()
	{
	}

	APOINT(LONG _x, LONG _y)
	{
		x	= _x;
		y	= _y;
	}

	operator DWORD ()
	{
		return ((DWORD)(WORD)x) + (((DWORD)(WORD)y) << 16);
	}

	void operator += (const APOINT& p)
	{
		x	+= p.x;
		y	+= p.y;
	}

	void operator -= (const APOINT& p)
	{
		x	-= p.x;
		y	-= p.y;
	}

	friend APOINT operator + (const APOINT& p1, const APOINT& p2)
	{
		return APOINT(p1.x+p2.x, p1.y+p2.y);
	}

	friend APOINT operator - (const APOINT& p1, const APOINT& p2)
	{
		return APOINT(p1.x-p2.x, p1.y-p2.y);
	}

	friend bool operator == (const APOINT& p1, const APOINT& p2)
	{
		return (p1.x == p2.x) && (p1.y == p2.y);
	}

	friend bool operator != (const APOINT& p1, const APOINT& p2)
	{
		return (p1.x != p2.x) || (p1.y != p2.y);
	}
};

struct ARECT : public tagRECT
{
public:

	ARECT()
	{
	}

	ARECT(LONG _left, LONG _top, LONG _right, LONG _bottom)
	{
		left	= _left;
		top		= _top;
		right	= _right;
		bottom	= _bottom;
	}

	void SetValue(LONG _left, LONG _top, LONG _right, LONG _bottom)
	{
		left	= _left;
		top		= _top;
		right	= _right;
		bottom	= _bottom;
	}

	LONG GetWidth() const
	{
		return right-left;
	}

	void SetWidth(LONG w)
	{
		right	= left + w;
	}

	LONG GetHeight() const
	{
		return bottom-top;
	}

	void SetHeight(LONG h)
	{
		bottom	= top + h;
	}

	APOINT& GetLeftTop()
	{
		return *((APOINT*)&left);
	}

	APOINT& GetRightBottom()
	{
		return *((APOINT*)&right);
	}

	const APOINT& GetLeftTop() const
	{
		return *((APOINT*)&left);
	}

	const APOINT& GetRightBottom() const
	{
		return *((APOINT*)&right);
	}

	void MoveHor(int nStep)
	{
		left	+= nStep;
		right	+= nStep;
	}

	void MoveVer(int nStep)
	{
		top		+= nStep;
		bottom	+= nStep;
	}

	void Move(LONG dx, LONG dy)
	{
		left	+= dx;
		right	+= dx;
		top		+= dy;
		bottom	+= dy;
	}

	void Move(const APOINT& pt)
	{
		left	+= pt.x;
		right	+= pt.x;
		top		+= pt.y;
		bottom	+= pt.y;
	}

	void MoveTo(LONG x, LONG y)
	{
		right	= x + GetWidth();
		bottom	= y + GetHeight();
		left	= x;
		top		= y;
	}

	void MoveTo(const APOINT& pt)
	{
		right	= pt.x + GetWidth();
		bottom	= pt.y + GetHeight();
		left	= pt.x;
		top		= pt.y;
	}

	friend bool operator && (const ARECT& r, const APOINT& p)
	{
		return
			(p.x >= r.left)		&&
			(p.x <  r.right)	&&
			(p.y >= r.top)		&&
			(p.y <  r.bottom);
	}

	friend bool operator && (const APOINT& p, const ARECT& r)
	{
		return
			(p.x >= r.left)		&&
			(p.x <  r.right)	&&
			(p.y >= r.top)		&&
			(p.y <  r.bottom);
	}
};

#endif	// __AWNDTYPES_H__