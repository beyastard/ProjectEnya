/*
 * FILE: A3DVertex.h
 *
 * DESCRIPTION: Structures representing the Vertex, LVerex or TLVertex
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DVERTEX_H_
#define _A3DVERTEX_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"

enum A3DVERTEXTYPE
{
	A3DVT_VERTEX = 0,
	A3DVT_LVERTEX,
	A3DVT_TLVERTEX,
	A3DVT_IBLVERTEX
};

typedef struct _A3DVERTEX
{
    FLOAT		x, y, z;
	FLOAT		nx, ny, nz;
	FLOAT       tu, tv;

public:

	_A3DVERTEX() {}
	_A3DVERTEX(const A3DVECTOR3& vPos, const A3DVECTOR3& vNormal, FLOAT _tu, FLOAT _tv)
	{
		x = vPos.x;		y = vPos.y;		z = vPos.z;
		nx = vNormal.x;	ny = vNormal.y;	nz = vNormal.z;
		tu = _tu;		tv = _tv;
	}

} A3DVERTEX, * PA3DVERTEX;

typedef struct _A3DLVERTEX
{
	FLOAT x, y, z;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu, tv;
public:
	inline _A3DLVERTEX() {}
	inline _A3DLVERTEX(const A3DVECTOR3& _vecPos, A3DCOLOR _diffuse, A3DCOLOR _specular, FLOAT _tu, FLOAT _tv) 
		{x = _vecPos.x; y = _vecPos.y; z = _vecPos.z; diffuse = _diffuse; specular = _specular; tu = _tu; tv = _tv;}
} A3DLVERTEX, * PA3DLVERTEX;

typedef struct _A3DTLVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu, tv;

	_A3DTLVERTEX()	{}
	_A3DTLVERTEX(const A3DVECTOR4& vPos, DWORD _diffuse, DWORD _specular, FLOAT _tu, FLOAT _tv)
	{
		x = vPos.x;	y = vPos.y;	z = vPos.z;	rhw = vPos.w;
		diffuse  = _diffuse;
		specular = _specular;
		tu = _tu;	tv = _tv;
	}

} A3DTLVERTEX, * PA3DTLVERTEX;

typedef struct _A3DIBLVERTEX
{
	FLOAT x, y, z;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu1, tv1;
	FLOAT tu2, tv2;
public:
	inline _A3DIBLVERTEX() {}
	inline _A3DIBLVERTEX(const A3DVECTOR3& _vecPos, const A3DCOLOR& _diffuse, const A3DCOLOR& _specular, FLOAT _tu1, FLOAT _tv1, FLOAT _tu2, FLOAT _tv2)
	{
		x = _vecPos.x; y = _vecPos.y; z = _vecPos.z;
		diffuse		= _diffuse;
		specular	= _specular;
		tu1 = _tu1; tv1 = _tv1;
		tu2 = _tu2; tv2 = _tv2;
	}

} A3DIBLVERTEX, * PA3DIBLVERTEX;

#define A3DFVF_A3DVERTEX  (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define A3DFVF_A3DLVERTEX  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define A3DFVF_A3DTLVERTEX  (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define A3DFVF_A3DIBLVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)

#endif
