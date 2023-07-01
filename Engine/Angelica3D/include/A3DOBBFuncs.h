/*
 * FILE: A3DOBBFuncs.h
 *
 * DESCRIPTION: Some functions used in calculating the obbs from a group of vertex data
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DOBBFUNCS_H_
#define _A3DOBBFUNCS_H_

#include "A3DTypes.h"
#include "A3DFuncs.h"
#include "A3DVertex.h"

A3DOBB GetOBB(A3DVERTEX * pVertex, int nVertCount);
A3DOBB GetOBB(A3DOBB& obb1, A3DOBB& obb2);
A3DAABB GetAABB(A3DVERTEX * pVertex, int nVertCount);

bool AABBToVertex(A3DVECTOR3 * pVertexPos, WORD * pIndices, const A3DAABB& aabb, bool bWire);
bool OBBToVertex(A3DVECTOR3 * pVertexPos, WORD * pIndices, const A3DOBB& obb, bool bWire=true);

#endif