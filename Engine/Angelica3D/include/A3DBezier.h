/*
 * FILE: A3DBezier.h
 *
 * DESCRIPTION: 3D Bezier Curve Objects for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2002/7/24
 *
 * HISTORY:
 *
 * Copyright (c) 2002~2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DBEZIER_H_
#define _A3DBEZIER_H_

#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DViewport.h"

#define A3DBEZIER_MAX_POINTS			2048
#define A3DBEZIER_MAX_SEGMENTS			1024
#define A3DBEZIER_MAX_SUBDIVIDE			8

#define POINT_SELECTED_MASK				0x1
#define POINT_DELETED_MASK				0x2

#define SEGMENT_SELECTED_MASK			0x1
#define SEGMENT_DELETED_MASK			0x2

#define POINT_MODIFY_POS				0x1

#define SEGMENT_MODIFY_STARTINDEX		0x1
#define SEGMENT_MODIFY_ENDINDEX			0x2
#define SEGMENT_MODIFY_STARTCONTROL		0x4
#define SEGMENT_MODIFY_ENDCONTROL		0x8

class A3DBezierCurve
{
private:
	typedef struct _BEZIERPOINT
	{
		A3DVECTOR3			vecPos;				// position of the point
		A3DAABB				aabb;				// bounding box for raytrace

		BYTE				mask;				// mask indicates the state, whether it is selected or deleted

	} BEZIERPOINT;

	typedef struct _BEZIERSEGMENT
	{
		int					nStartIndex;		// index of the start point
		int					nEndIndex;			// index of the end point

		A3DVECTOR3			vecStartCtrl;		// start control point
		A3DAABB				aabbStart;			// aabb for start control point
		A3DVECTOR3			vecEndCtrl;			// end control point
		A3DAABB				aabbEnd;			// aabb for end control point

		A3DAABB				aabb;				// aabb for entire segment
		A3DOBB *			pOBBs;				// aabbs for each subdivided segment

		BYTE				mask;				// mask indicates the state, whether it is selected or deleted

	} BEZIERSEGMENT;

	A3DDevice *			m_pA3DDevice;
	
	float				m_vPointRadius;
	int					m_nNumPoints;
	BEZIERPOINT *		m_pPoints;
	A3DCOLOR			m_colorPoint;
	A3DCOLOR			m_colorSelPoint;
	A3DLVERTEX *		m_pPointsVertices;
	WORD *				m_pPointsIndices;
	
	float				m_vControlRadius;
	A3DCOLOR			m_colorControl;

	float				m_vSegmentRadius;
	int					m_nNumSegments;
	BEZIERSEGMENT *		m_pSegments;
	A3DCOLOR			m_colorSegment;
	A3DCOLOR			m_colorSelSegment;
	A3DLVERTEX *		m_pSegmentsVertices;
	WORD *				m_pSegmentsIndices;

	int					m_nSubDivide;

	bool				m_bModified;

protected:
	bool CompleteSegment(int nIndex); // complete the information needed by a segment
	bool CompletePoint(int nIndex);   // complete the information needed by a point

	// change the color of some point and segment;
	bool SetPointColor(int nIndex, A3DCOLOR color);
	bool SetSegmentColor(int nIndex, A3DCOLOR color);

	// function to test the point and segment state;
	bool IsPointSelected(int nIndex);
	bool IsSegmentSelected(int nIndex);
	bool IsPointDeleted(int nIndex);
	bool IsSegmentDeleted(int nIndex);

public:
	A3DBezierCurve();
	~A3DBezierCurve();

	// init and release
	bool Init(A3DDevice * pDevice, float vPointRadius=1.0f, float vSegmentRadius=0.3f, float vControlRadius=0.25f, int nSubDivide=5);
	bool Release();

	// manipulate a point
	bool AddPoint(const A3DVECTOR3& vecPos, int * pnIndex=NULL);
	bool ModifyPoint(int nPointIndex, const A3DVECTOR3& vecPos, DWORD dwFlags);
	bool DeletePoint(int nPointIndex, bool bDelete);
	bool DecreasePoint(); // Use this method to delete the last point permanently

	// manipulate a segment
	bool AddSegment(int nStartIndex, int nEndIndex, const A3DVECTOR3& vecStartCtrl, const A3DVECTOR3& vecEndCtrl, int * pnIndex=NULL);
	bool ModifySegment(int nSegmentIndex, int nStartIndex, int nEndIndex, const A3DVECTOR3& vecStartCtrl, const A3DVECTOR3& vecEndCtrl, DWORD dwFlags);
	bool DeleteSegment(int nSegmentIndex, bool bDelete);
	bool DecreaseSegment(); // Use this method to delete the last segment permanently

	// display it
	bool Render(A3DViewport * pA3DViewport);

	// Pick some point or segment, but not automatically selected, you
	// must use SelectPoint or SelectSegment to make one point(segment) selected;
	bool PickSegment(A3DVECTOR3& vecCamPos, A3DVECTOR3& vecDir, int * pnSegmentIndex);
	bool PickControl(A3DVECTOR3& vecCamPos, A3DVECTOR3& vecDir, int * pnSegmentIndex, bool * pbStart);
	bool PickPoint(A3DVECTOR3& vecCamPos, A3DVECTOR3& vecDir, int *pnPointIndex);

	// select one point or segment
	bool SelectPoint(int nIndex, bool bSelected);
	bool SelectSegment(int nIndex, bool bSelected);

	inline int GetNumPoints()				{ return m_nNumPoints;		 }
	inline int GetNumSegments()				{ return m_nNumSegments;	 }

	// inline function to preset the color schemes;
	inline void PresetPointColor(A3DCOLOR color)		{ m_colorPoint = color; }
	inline void PresetSelPointColor(A3DCOLOR color)		{ m_colorSelPoint = color; }
	inline void PresetControlColor(A3DCOLOR color)		{ m_colorControl = color; }
	inline void PresetSegmentColor(A3DCOLOR color)		{ m_colorSegment = color; }
	inline void PresetSelSegmentColor(A3DCOLOR color)	{ m_colorSelSegment = color; }
};

typedef class A3DBezierCurve * PA3DBezierCurve;

#endif//_A3DBEZIER_H_
