/*
 * FILE: A3DVSSIBLScene.h
 *
 * DESCRIPTION: Render scenes using Visible Surface Set.
 *
 * CREATED BY: duyuxin, 2002/7/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DVSSIBLSCENE_H_
#define _A3DVSSIBLSCENE_H_

#include "A3DIBLScene.h"
#include "A3DVSSRouteLoader.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DVSSIBLScene
//
///////////////////////////////////////////////////////////////////////////

class A3DVSSIBLScene : public A3DIBLScene
{
public:		//	Types

	enum
	{
		MAXNUM_ROUTE	= 1024		//	Maximum number of route can be handled at same time
	};

	typedef struct _ROUTE
	{
		A3DVSSRouteLoader*	pRoute;			//	Route object's address

		DWORD	dwID;			//	Route ID
		int*	aVisSurfs;		//	Visible surface indices buffer
		int		iNumVisSurf;	//	Number of visible surface
		int		iCurSection;	//	Current section
		int*	aVisObjs;		//	Visible scene object ID buffer
		int		iNumVisObj;		//	Number of visible scene object

	} ROUTE, *PROUTE;

public:		//	Constructors and Destructors

	A3DVSSIBLScene();
	virtual ~A3DVSSIBLScene() {}

public:		//	Attributes

public:		//	Operations

	virtual bool Init(A3DDevice* pA3DDevice);	//	Initialize object
	virtual bool Release();		//	Release resources

	virtual	bool UpdateVisibleSets(A3DViewport* pCurrentViewport);	//	Update visible set

	int			LoadRoute(char* szFileName, bool bAll);		//	Load one route from disk file
	int			LoadRoute(BYTE* pBuf);						//	Load one route from memory file
	bool		RemoveRoute(int iIndex);					//	Remove one route
	void		RemoveAllRoutes();							//	Remove all routes
	bool		SetCurrentRoute(int iIndex, int iSample);	//	Set current route
	bool		SetCurrentSection(int iSection);			//	Set current section

	int			GetSceneObjectNum();		//	Get scene object number
	bool		GetCurrentVisObjs(int* pBuf, int* piNumVisObj);		//	Get visible object's ID of specified section

	A3DVSSRouteLoader*	GetRouteLoader(int iIndex);		//	Get route loader object

	void		SetCurrentSample(int iSample)	{	m_iCurSample = iSample;		}
	int			GetCurrentSample()				{	return m_iCurSample;		}
	void		ForceToRenderAll(bool bTrue)	{	m_bRenderAll = bTrue;		}

protected:	//	Attributes

	ROUTE		m_aRoutes[MAXNUM_ROUTE];	//	Routes
	int			m_iNumRoute;				//	Number valid route in m_aRoutes
	int			m_iCurRoute;				//	Current route
	int			m_iCurSample;				//	Current sample

	bool		m_bRenderAll;				//	Force to render all surfaces
	bool		m_bNeedUpdate;				//	Need update vss ?

protected:	//	Operations

	ROUTE*		AllocRouteSlot();			//	Allocate a route slot
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DVSSIBLSCENE_H_

