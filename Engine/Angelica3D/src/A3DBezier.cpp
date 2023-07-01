/*
 * FILE: A3DBezier.cpp
 *
 * DESCRIPTION: 3D Bezier Curve Objects for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2002/7/24
 *
 * HISTORY:
 *
 * Copyright (c) 2002~2008 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DBezier.h"
#include "A3DErrLog.h"
#include "A3DGDI.h"
#include "A3DCollision.h"

A3DBezierCurve::A3DBezierCurve()
{
	m_pA3DDevice			= NULL;

	m_vPointRadius			= 1.0f;
	m_colorPoint			= A3DCOLORRGBA(0, 0, 255, 200);
	m_colorSelPoint			= A3DCOLORRGBA(0, 255, 255, 200);
	m_nNumPoints			= 0;
	m_pPoints				= NULL;
	m_pPointsVertices		= NULL;
	m_pPointsIndices		= NULL;

	m_vControlRadius		= 0.25f;
	m_colorControl			= A3DCOLORRGBA(255, 0, 0, 200);

	m_vSegmentRadius		= 0.3f;
	m_colorSegment			= A3DCOLORRGBA(0, 255, 0, 255);
	m_colorSelSegment		= A3DCOLORRGBA(255, 255, 0, 255);
	m_nNumSegments			= 0;
	m_pSegments				= NULL;
	m_pSegmentsVertices		= NULL;
	m_pSegmentsIndices		= NULL;

	m_nSubDivide			= 5;
}

A3DBezierCurve::~A3DBezierCurve()
{
}

bool A3DBezierCurve::Init(A3DDevice * pDevice, float vPointRadius, float vSegmentRadius, float vControlRadius, int nSubDivide)
{
	m_pA3DDevice		= pDevice;

	if( nSubDivide > A3DBEZIER_MAX_SUBDIVIDE )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::Init(), Too many subdivide [%d]", nSubDivide);
		return false;
	}
	if( nSubDivide <= 0 )
		nSubDivide = 1;

	m_vPointRadius		= vPointRadius;
	m_vSegmentRadius	= vSegmentRadius;
	m_vControlRadius	= vControlRadius;

	m_nSubDivide		= nSubDivide;

	m_nNumPoints		= 0;
	m_nNumSegments		= 0;

	int	 i;

	// Now we pre alloc all needed buffer;
	m_pPoints = (BEZIERPOINT *) malloc(sizeof(BEZIERPOINT) * A3DBEZIER_MAX_POINTS);
	if( NULL == m_pPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::Init(), Not enough memory!");
		return false;
	}

	m_pPointsVertices = (A3DLVERTEX *) malloc(sizeof(A3DLVERTEX) * A3DBEZIER_MAX_POINTS * 4);
	if( NULL == m_pPointsVertices )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::Init(), Not enough memory!");
		return false;
	}

	m_pPointsIndices = (WORD *) malloc(sizeof(WORD) * A3DBEZIER_MAX_POINTS * 12);
	if( NULL == m_pPointsVertices )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::Init(), Not enough memory!");
		return false;
	}

	// we use a tetrahedron to represent the point
	// Now fill the indices at before hand;
	//
	// 0 is at the top tip
	//		  ^ Z
	//		  |
	//		  *	1
	//		/ |	\
	//	  /	  |	  \		   X
	// -------+------------>
	//	*-----+-----* 2
	//	3	  |
	//
	//
	//

	for(i=0; i<A3DBEZIER_MAX_POINTS; i++)
	{
		m_pPointsIndices[i * 12 + 0] = i * 4;
		m_pPointsIndices[i * 12 + 1] = i * 4 + 1;
		m_pPointsIndices[i * 12 + 2] = i * 4 + 2;

		m_pPointsIndices[i * 12 + 3] = i * 4;
		m_pPointsIndices[i * 12 + 4] = i * 4 + 2;
		m_pPointsIndices[i * 12 + 5] = i * 4 + 3;

		m_pPointsIndices[i * 12 + 6] = i * 4;
		m_pPointsIndices[i * 12 + 7] = i * 4 + 3;
		m_pPointsIndices[i * 12 + 8] = i * 4 + 1;

		m_pPointsIndices[i * 12 + 9] = i * 4 + 1;
		m_pPointsIndices[i * 12 + 10] = i * 4 + 3;
		m_pPointsIndices[i * 12 + 11] = i * 4 + 2;
	}

	m_pSegments = (BEZIERSEGMENT *) malloc(sizeof(BEZIERSEGMENT) * A3DBEZIER_MAX_SEGMENTS);
	if( NULL == m_pSegments )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::Init(), Not enough memory!");
		return false;
	}

	m_pSegmentsVertices = (A3DLVERTEX *) malloc(sizeof(A3DLVERTEX) * A3DBEZIER_MAX_SEGMENTS * 8 * m_nSubDivide);
	if( NULL == m_pSegmentsVertices )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::Init(), Not enough memory!");
		return false;
	}

	m_pSegmentsIndices = (WORD *) malloc(sizeof(WORD) * A3DBEZIER_MAX_SEGMENTS * 12 * m_nSubDivide);
	if( NULL == m_pSegmentsIndices )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::Init(), Not enough memory!");
		return false;
	}

	// we use a billboard to represent the segment line
	// now fill segments indices at before hand
	//
	//		  TOP View
	//		0--------1
	//		|        |				 Left View
	//		|		 |		   4-------------------5
	//		|		 |		   |				   |
	//		|		 |		   |				   |
	//		|		 |		   |				   |
	//		|		 |		   7-------------------6
	//		|		 |		   
	//	    3--------2
	//
	//
	for(i=0; i<A3DBEZIER_MAX_SEGMENTS * m_nSubDivide; i++)
	{
		m_pSegmentsIndices[i * 12 + 0] = i * 8;
		m_pSegmentsIndices[i * 12 + 1] = i * 8 + 1;
		m_pSegmentsIndices[i * 12 + 2] = i * 8 + 2;

		m_pSegmentsIndices[i * 12 + 3] = i * 8;
		m_pSegmentsIndices[i * 12 + 4] = i * 8 + 2;
		m_pSegmentsIndices[i * 12 + 5] = i * 8 + 3;

		m_pSegmentsIndices[i * 12 + 6] = i * 8 + 4;
		m_pSegmentsIndices[i * 12 + 7] = i * 8 + 5;
		m_pSegmentsIndices[i * 12 + 8] = i * 8 + 6;

		m_pSegmentsIndices[i * 12 + 9]  = i * 8 + 4;
		m_pSegmentsIndices[i * 12 + 10] = i * 8 + 6;
		m_pSegmentsIndices[i * 12 + 11] = i * 8 + 7;
	}

	return true;
}

bool A3DBezierCurve::Release()
{
	if( m_pPoints )
	{
		free(m_pPoints);
		m_pPoints = NULL;
	}

	if( m_pPointsVertices )
	{
		free(m_pPointsVertices);
		m_pPointsVertices = NULL;
	}

	if( m_pPointsIndices )
	{
		free(m_pPointsIndices);
		m_pPointsIndices = NULL;
	}

	if( m_pSegments )
	{
		free(m_pSegments);
		m_pSegments = NULL;
	}

	if( m_pSegmentsVertices )
	{
		free(m_pSegmentsVertices);
		m_pSegmentsVertices = NULL;
	}

	if( m_pSegmentsIndices )
	{
		free(m_pSegmentsIndices);
		m_pSegmentsIndices = NULL;
	}

	m_nNumPoints		= 0;
	m_nNumSegments		= 0;

	return true;
}

bool A3DBezierCurve::Render(A3DViewport * pCurrentViewport)
{	
	m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DLVERTEX);

	if( m_nNumSegments > 0 )
	{
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
			m_nNumSegments * m_nSubDivide * 8, m_nNumSegments * m_nSubDivide * 4, m_pSegmentsIndices, 
			A3DFMT_INDEX16, m_pSegmentsVertices, sizeof(A3DLVERTEX));
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
		m_pA3DDevice->SetLightingEnable(true);

		for(int i=0; i<m_nNumSegments; i++)
		{
			if( IsSegmentDeleted(i) )
				continue;

			if( !IsSegmentSelected(i) )
				continue;

			g_pA3DGDI->Draw3DLine(m_pPoints[m_pSegments[i].nStartIndex].vecPos, m_pSegments[i].aabbStart.Center, m_colorControl);
			g_pA3DGDI->Draw3DLine(m_pPoints[m_pSegments[i].nEndIndex].vecPos, m_pSegments[i].aabbEnd.Center, m_colorControl);
			g_pA3DGDI->DrawBox(m_pSegments[i].aabbStart, m_colorControl);
			g_pA3DGDI->DrawBox(m_pSegments[i].aabbEnd, m_colorControl);
		}
	}

	if( m_nNumPoints > 0 )
	{
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
			m_nNumPoints * 4, m_nNumPoints * 4, m_pPointsIndices, 
			A3DFMT_INDEX16, m_pPointsVertices, sizeof(A3DLVERTEX));
		m_pA3DDevice->SetLightingEnable(true);
	}

	return true;
}

bool A3DBezierCurve::AddPoint(const A3DVECTOR3& vecPos, int * pnIndex)
{
	if( m_nNumPoints >= A3DBEZIER_MAX_POINTS )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::AddPoint(), Too may points, can not add more!");
		return false;
	}

	m_nNumPoints ++;
	
	int			nIndex = m_nNumPoints - 1;

	m_pPoints[nIndex].vecPos	= vecPos;
	m_pPoints[nIndex].mask		= 0;

	if( !CompletePoint(nIndex) )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::AddPoint(), Call CompletePoint() fail!");
		return false;
	}

	if( pnIndex )
		*pnIndex = nIndex;

	return true;
}

bool A3DBezierCurve::ModifyPoint(int nPointIndex, const A3DVECTOR3& vecPos, DWORD dwFlags)
{
	if( nPointIndex >= m_nNumPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::ModifyPoint(), Point index [%d] out of range!", nPointIndex);
		return false;
	}

	if( dwFlags & POINT_MODIFY_POS )
		m_pPoints[nPointIndex].vecPos	= vecPos;
	
	if( !CompletePoint(nPointIndex) )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::ModifyPoint(), Call CompletePoint() fail!");
		return false;
	}

	return true;
}

bool A3DBezierCurve::DeletePoint(int nPointIndex, bool bDelete)
{
	if( nPointIndex >= m_nNumPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::DeletePoint(), Index [%d] out of range!", nPointIndex);
		return false;
	}

	if( bDelete )
		m_pPoints[nPointIndex].mask |= POINT_DELETED_MASK;
	else
		m_pPoints[nPointIndex].mask &= ~POINT_DELETED_MASK;

	m_pPoints[nPointIndex].mask &= ~POINT_SELECTED_MASK;

	if( bDelete )
		SetPointColor(nPointIndex, 0);
	else
		SetPointColor(nPointIndex, m_colorPoint);

	return true;
}

bool A3DBezierCurve::AddSegment(int nStartIndex, int nEndIndex, const A3DVECTOR3& vecStartCtrl, const A3DVECTOR3& vecEndCtrl, int * pnIndex)
{
	if( m_nNumSegments >= A3DBEZIER_MAX_SEGMENTS )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::AddSegment(), Too many segments now, can not add more!");
		return false;
	}

	if( nStartIndex < 0 || nStartIndex >= m_nNumPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::AddSegment(), start index [%d] beyond range!", nStartIndex);
		return false;
	}

	if( nEndIndex < 0 || nEndIndex >= m_nNumPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::AddSegment(), end index [%d] beyond range!", nEndIndex);
	}

	m_nNumSegments ++;

	int		nIndex = m_nNumSegments - 1;

	m_pSegments[nIndex].nStartIndex		= nStartIndex;
	m_pSegments[nIndex].nEndIndex 		= nEndIndex;
	m_pSegments[nIndex].vecStartCtrl	= vecStartCtrl;
	m_pSegments[nIndex].vecEndCtrl		= vecEndCtrl;
	m_pSegments[nIndex].pOBBs			= (A3DOBB *) malloc(sizeof(A3DOBB) * m_nSubDivide);
	m_pSegments[nIndex].mask			= 0;
	
	if( !CompleteSegment(nIndex) )
	{ 
		g_pA3DErrLog->ErrLog("A3DBezierCurve::AddSegment(), Call CompleteSegment() fail!");
		return false;
	}

	if( pnIndex )
		*pnIndex = nIndex;
	return true;
}

bool A3DBezierCurve::ModifySegment(int nSegmentIndex, int nStartIndex, int nEndIndex, const A3DVECTOR3& vecStartCtrl, const A3DVECTOR3& vecEndCtrl, DWORD dwFlags)
{
	if( nSegmentIndex >= m_nNumSegments )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::ModifySegment(), Segment Index [%d] out of range!", nSegmentIndex);
		return false;
	}

	if( nStartIndex < 0 || nStartIndex >= m_nNumPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::ModifySegment(), start index [%d] beyond range!", nStartIndex);
		return false;
	}

	if( nEndIndex < 0 || nEndIndex >= m_nNumPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::ModifySegment(), end index [%d] beyond range!", nEndIndex);
	}

	if( dwFlags & SEGMENT_MODIFY_STARTINDEX )
		m_pSegments[nSegmentIndex].nStartIndex	= nStartIndex;
	if( dwFlags & SEGMENT_MODIFY_ENDINDEX )
		m_pSegments[nSegmentIndex].nEndIndex 	= nEndIndex;
	if( dwFlags & SEGMENT_MODIFY_STARTCONTROL )
		m_pSegments[nSegmentIndex].vecStartCtrl	= vecStartCtrl;
	if( dwFlags & SEGMENT_MODIFY_ENDCONTROL )
		m_pSegments[nSegmentIndex].vecEndCtrl	= vecEndCtrl;
	
	if( !CompleteSegment(nSegmentIndex) )
	{ 
		g_pA3DErrLog->ErrLog("A3DBezierCurve::ModifySegment(), Call CompleteSegment() fail!");
		return false;
	}

	return true;
}

bool A3DBezierCurve::DeleteSegment(int nSegmentIndex, bool bDelete)
{
	if( nSegmentIndex >= m_nNumSegments )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::DeleteSegement(), Index [%d] out of range!", nSegmentIndex);
		return false;
	}

	if( bDelete )
		m_pSegments[nSegmentIndex].mask |= SEGMENT_DELETED_MASK;
	else
		m_pSegments[nSegmentIndex].mask &= ~SEGMENT_DELETED_MASK;

	// Clear select flag;
	m_pSegments[nSegmentIndex].mask &= ~SEGMENT_SELECTED_MASK;

	if( bDelete )
		SetSegmentColor(nSegmentIndex, 0);
	else
		SetSegmentColor(nSegmentIndex, m_colorSegment);
	return true;
}

bool A3DBezierCurve::CompleteSegment(int nIndex)
{
	int		i, s;

	// Now interpolate the segments for rendering;
	A3DVECTOR3	vecPoints[A3DBEZIER_MAX_SUBDIVIDE + 1];
	A3DVECTOR3	vecStart, vecStartCtrl, vecEndCtrl, vecEnd;

	vecStart		= m_pPoints[m_pSegments[nIndex].nStartIndex].vecPos;	// x0
	vecStartCtrl	= m_pSegments[nIndex].vecStartCtrl;						// x1
	vecEndCtrl		= m_pSegments[nIndex].vecEndCtrl;						// x2
	vecEnd			= m_pPoints[m_pSegments[nIndex].nEndIndex].vecPos;		// x3

	vecPoints[0]			= vecStart;
	vecPoints[m_nSubDivide] = vecEnd;
	// calucate the coefficients here;
	float a[3], b[3], c[3];
	for(i=0; i<3; i++)
	{
		c[i] = 3 * (vecStartCtrl.m[i] - vecStart.m[i]);			// 3(x1 - x0);
		b[i] = 3 * (vecEndCtrl.m[i] - vecStartCtrl.m[i]) - c[i]; // 3(x2 - x1) - cx
		a[i] = vecEnd.m[i] - vecStart.m[i] - c[i] - b[i];	//x3 - x0 - cx - bx
	}

	for(i=1; i<m_nSubDivide; i++)
	{
		float	t = i * 1.0f / m_nSubDivide;
		// Use Bezier Interpolate function to calculate the inner points on the curve;
		for(s=0; s<3; s++)
		{
			vecPoints[i].m[s] = a[s] * t * t * t + b[s] * t * t + c[s] * t + vecStart.m[s];
		}
	}
	
	float		vRadius = m_vSegmentRadius;
	A3DAABB		aabb;
	aabb.Mins = A3DVECTOR3(99999.9f);
	aabb.Maxs = A3DVECTOR3(-99999.9f);
	CompleteAABB(&aabb);

	A3DCOLOR	color;
	if( IsSegmentSelected(nIndex) )
		color = m_colorSelSegment;
	else
		color = m_colorSegment;

	// Now construct the mesh
	int nVertIndex = nIndex * m_nSubDivide * 8;
	for(i=0; i<m_nSubDivide; i++)
	{
		A3DVECTOR3	vecDir;
		A3DVECTOR3	vecLeft, vecLastLeft;
		A3DVECTOR3	vecUp, vecLastUp;
		float		vDis;
		
		// from vecPoints[i + 1], point to vecPoint[i];
		vecDir = Normalize(vecPoints[i] - vecPoints[i + 1]);
		vDis   = Magnitude(vecPoints[i] - vecPoints[i + 1]);
		
		if( fabs(vecDir.y) < 0.9f )
		{
			A3DVECTOR3 vecPerpendicular = A3DVECTOR3(0.0f, 1.0f, 0.0f);
			vecLeft = Normalize(CrossProduct(vecDir, vecPerpendicular));
		}
		else
		{
			A3DVECTOR3 vecPerpendicular = A3DVECTOR3(1.0f, 0.0f, 0.0f);
			vecLeft = Normalize(CrossProduct(vecDir, vecPerpendicular));
		}
		vecUp = Normalize(CrossProduct(vecLeft, vecDir));

		A3DOBB		obb;
		obb.Center	= 0.5f * (vecPoints[i] + vecPoints[i + 1]);
		obb.ZAxis	= vecDir;
		obb.XAxis	= -vecLeft;
		obb.YAxis	= vecUp;
		obb.Extents = A3DVECTOR3(vRadius, vRadius, vDis / 2.0f);
		CompleteOBB(&obb);
		m_pSegments[nIndex].pOBBs[i] = obb;

		ExpandAABB(&aabb, obb);
		
		if( i == 0 )
		{
			vecLastLeft = vecLeft;
			vecLastUp = vecUp;
		}

		m_pSegmentsVertices[nVertIndex + 0] = A3DLVERTEX(vecPoints[i] - vecLastLeft * vRadius,
			color, 0xff000000, 0.0f, 0.0f);
		m_pSegmentsVertices[nVertIndex + 1] = A3DLVERTEX(vecPoints[i] + vecLastLeft * vRadius,
			color, 0xff000000, 0.0f, 0.0f);
		m_pSegmentsVertices[nVertIndex + 2] = A3DLVERTEX(vecPoints[i + 1] + vecLeft * vRadius,
			color, 0xff000000, 0.0f, 0.0f);
		m_pSegmentsVertices[nVertIndex + 3] = A3DLVERTEX(vecPoints[i + 1] - vecLeft * vRadius,
			color, 0xff000000, 0.0f, 0.0f);

		m_pSegmentsVertices[nVertIndex + 4] = A3DLVERTEX(vecPoints[i] + vecLastUp * vRadius,
			color, 0xff000000, 0.0f, 0.0f);
		m_pSegmentsVertices[nVertIndex + 5] = A3DLVERTEX(vecPoints[i + 1] + vecUp * vRadius,
			color, 0xff000000, 0.0f, 0.0f);
		m_pSegmentsVertices[nVertIndex + 6] = A3DLVERTEX(vecPoints[i + 1] - vecUp * vRadius,
			color, 0xff000000, 0.0f, 0.0f);
		m_pSegmentsVertices[nVertIndex + 7] = A3DLVERTEX(vecPoints[i] - vecLastUp * vRadius,
			color, 0xff000000, 0.0f, 0.0f);

		nVertIndex += 8;

		vecLastLeft = vecLeft;
		vecLastUp = vecUp;
	}

	m_pSegments[nIndex].aabb = aabb;
	
	aabb.Mins = m_pSegments[nIndex].vecStartCtrl - A3DVECTOR3(m_vControlRadius);
	aabb.Maxs = aabb.Mins + 2.0f * A3DVECTOR3(m_vControlRadius);
	CompleteAABB(&aabb);
	m_pSegments[nIndex].aabbStart = aabb;

	aabb.Mins = m_pSegments[nIndex].vecEndCtrl - A3DVECTOR3(m_vControlRadius);
	aabb.Maxs = aabb.Mins + 2.0f * A3DVECTOR3(m_vControlRadius);
	CompleteAABB(&aabb);
	m_pSegments[nIndex].aabbEnd = aabb;

	return true;
}

bool A3DBezierCurve::CompletePoint(int nIndex)
{
	A3DAABB		aabb;
	A3DVECTOR3	vecPos = m_pPoints[nIndex].vecPos;

	aabb.Mins = vecPos - A3DVECTOR3(m_vPointRadius);
	aabb.Maxs = vecPos + A3DVECTOR3(m_vPointRadius);
	CompleteAABB(&aabb);
	
	m_pPoints[nIndex].aabb		= aabb;

	float		vRadius = m_vPointRadius;
	float		vRadiusLow = float(vRadius * cos(DEG2RAD(30.0f)));
	float		vHLow = -float(vRadius * sin(DEG2RAD(30.0f)));

	A3DCOLOR	color;
	if( IsPointSelected(nIndex) )
		color = m_colorSelPoint;
	else
		color = m_colorPoint;

	m_pPointsVertices[nIndex * 4 + 0] = A3DLVERTEX(vecPos + A3DVECTOR3(0.0f, vRadius, 0.0f),
		color, 0xff000000, 0.0f, 0.0f);
	m_pPointsVertices[nIndex * 4 + 1] = A3DLVERTEX(vecPos + A3DVECTOR3(0.0f, vHLow, vRadiusLow),
		color, 0xff000000, 0.0f, 0.0f);
	m_pPointsVertices[nIndex * 4 + 2] = A3DLVERTEX(vecPos + A3DVECTOR3(float(vRadiusLow * cos(DEG2RAD(30.0f))), vHLow, -float(vRadiusLow * sin(DEG2RAD(30.0f)))),
		color, 0xff000000, 0.0f, 0.0f);
	m_pPointsVertices[nIndex * 4 + 3] = A3DLVERTEX(vecPos + A3DVECTOR3(-float(vRadiusLow * cos(DEG2RAD(30.0f))), vHLow, -float(vRadiusLow * sin(DEG2RAD(30.0f)))),
		color, 0xff000000, 0.0f, 0.0f);
	return true;
}

bool A3DBezierCurve::SetPointColor(int nIndex, A3DCOLOR color)
{
	for(int i=0; i<4; i++)
	{
		m_pPointsVertices[nIndex * 4 + i].diffuse = color;
	}
	return true;
}

bool A3DBezierCurve::SetSegmentColor(int nIndex, A3DCOLOR color)
{
	for(int i=0; i<8 * m_nSubDivide; i++)
	{
		m_pSegmentsVertices[nIndex * 8 * m_nSubDivide + i].diffuse = color;
	}
	return true;
}

bool A3DBezierCurve::SelectPoint(int nIndex, bool bSelected)
{
	if( nIndex >= m_nNumPoints )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::SelectPoint(), Point Index [%s] out of range!", nIndex);
		return false;
	}

	if( bSelected )
		m_pPoints[nIndex].mask |= POINT_SELECTED_MASK;
	else
		m_pPoints[nIndex].mask &= ~POINT_SELECTED_MASK;
	
	if( bSelected )
		SetPointColor(nIndex, m_colorSelPoint);
	else
		SetPointColor(nIndex, m_colorPoint);
	return true;
}

bool A3DBezierCurve::SelectSegment(int nIndex, bool bSelected)
{
	if( nIndex >= m_nNumSegments )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::SelectSegment(), Segment Index [%d] out of range!", nIndex);
		return false;
	}

	if( bSelected )
		m_pSegments[nIndex].mask |= SEGMENT_SELECTED_MASK;
	else
		m_pSegments[nIndex].mask &= ~SEGMENT_SELECTED_MASK;
	
	if( bSelected )
		SetSegmentColor(nIndex, m_colorSelSegment);
	else
		SetSegmentColor(nIndex, m_colorSegment);
	return true;
}

// Methods for pick a elements from the curve;
bool A3DBezierCurve::PickSegment(A3DVECTOR3& vecCamPos, A3DVECTOR3& vecDir, int * pnSegmentIndex)
{
	float		vMinFraction = 99999.9f;
	float		vFraction;
	int			nMinIndex;
	A3DVECTOR3	vecDelta = vecDir * 1000.0f;

	A3DVECTOR3	vecPoint, vecNormal;

	for(int i=0; i<m_nNumSegments; i++)
	{
		// skip deleted ones;
		if( IsSegmentDeleted(i) )
			continue;

		A3DAABB  aabb = m_pSegments[i].aabb;

		if( CLS_RayToAABB3(vecCamPos, vecDelta, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
		{
			for(int j=0; j<m_nSubDivide; j++)
			{
				A3DOBB	obb = m_pSegments[i].pOBBs[j];
				if( CLS_RayToOBB3(vecCamPos, vecDelta, obb, vecPoint, &vFraction, vecNormal) )
				{
					if( vFraction < vMinFraction )
					{
						vMinFraction = vFraction;
						nMinIndex = i;
					}
				}
			}
		}

	}

	if( vMinFraction > 99999.0f )
		return false;

	if( pnSegmentIndex )
		*pnSegmentIndex = nMinIndex;
	return true;
}

bool A3DBezierCurve::PickControl(A3DVECTOR3& vecCamPos, A3DVECTOR3& vecDir, int * pnSegmentIndex, bool * pbStart)
{
	float		vMinFraction = 99999.9f;
	float		vFraction;
	int			nMinIndex;
	bool		bStart;
	A3DVECTOR3	vecDelta = vecDir * 1000.0f;

	A3DVECTOR3	vecPoint, vecNormal;

	for(int i=0; i<m_nNumSegments; i++)
	{
		// skip deleted ones;
		if( IsSegmentDeleted(i) )
			continue;

		if( !IsSegmentSelected(i) )
			continue;

		A3DAABB	aabb = m_pSegments[i].aabbStart;
		// First check start control point aabb;
		if( CLS_RayToAABB3(vecCamPos, vecDelta, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
		{
			if( vFraction < vMinFraction )
			{
				vMinFraction = vFraction;
				nMinIndex = i;
				bStart = true;
			}
		}

		aabb = m_pSegments[i].aabbEnd;
		// Then check end control point aabb;
		if( CLS_RayToAABB3(vecCamPos, vecDelta, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
		{
			if( vFraction < vMinFraction )
			{
				vMinFraction = vFraction;
				nMinIndex = i;
				bStart = false;
			}
		}
	}

	if( vMinFraction > 99999.0f )
		return false;

	if( pnSegmentIndex )
		*pnSegmentIndex = nMinIndex;
	if( pbStart )
		*pbStart = bStart;
	return true;
}

bool A3DBezierCurve::PickPoint(A3DVECTOR3& vecCamPos, A3DVECTOR3& vecDir, int *pnPointIndex)
{
	float		vMinFraction = 99999.9f;
	float		vFraction;
	int			nMinIndex;
	A3DVECTOR3	vecPoint, vecNormal;
	A3DVECTOR3	vecDelta = vecDir * 1000.0f;

	for(int i=0; i<m_nNumPoints; i++)
	{
		// skip deleted one;
		if( IsPointDeleted(i) )
			continue;

		A3DAABB		aabb = m_pPoints[i].aabb;
		if( CLS_RayToAABB3(vecCamPos, vecDelta, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
		{
			if( vFraction < vMinFraction )
			{
				vMinFraction = vFraction;
				nMinIndex    = i;
			}
		}
	}

	if( vMinFraction > 99999.0f )
		return false;

	if( pnPointIndex )
		*pnPointIndex = nMinIndex;

	return true;
}

bool A3DBezierCurve::IsPointDeleted(int nIndex)
{
	if( m_pPoints[nIndex].mask & POINT_DELETED_MASK )
		return true;

	return false;
}

bool A3DBezierCurve::IsSegmentDeleted(int nIndex)
{
	if( m_pSegments[nIndex].mask & SEGMENT_DELETED_MASK )
		return true;

	return false;
}

bool A3DBezierCurve::IsPointSelected(int nIndex)
{
	if( m_pPoints[nIndex].mask & POINT_SELECTED_MASK )
		return true;

	return false;
}

bool A3DBezierCurve::IsSegmentSelected(int nIndex)
{
	if( m_pSegments[nIndex].mask & SEGMENT_SELECTED_MASK )
		return true;

	return false;
}

bool A3DBezierCurve::DecreasePoint()
{
	if( m_nNumPoints == 0 )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::DecreasePoint(), No more points available!");
		return false;
	}

	m_nNumPoints --;
	return true;
}

bool A3DBezierCurve::DecreaseSegment()
{
	if( m_nNumSegments == 0 )
	{
		g_pA3DErrLog->ErrLog("A3DBezierCurve::DecreaseSegment(), No more segments available!");
		return false;
	}

	m_nNumSegments --;
	return true;
}