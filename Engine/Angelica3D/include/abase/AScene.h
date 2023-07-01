/*
 * FILE: ASCENE.H
 *
 * DESCRIPTION : Wrap of A3DScene classes
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 8 - 14
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 */

#ifndef _ASCENE_H_
#define _ASCENE_H_

#include "ABaseA3D.h"
#include "AWorldInit.h"
class A3DVSSScene;
class A3DVSSIBLScene;
class AScene
{
protected:

	A3DDevice *				m_pA3DDevice;
	A3DESP *				m_pA3DESP;
	A3DIBLLightGrid	*		m_pA3DIBLLightGrid;
	A3DScene *				m_pA3DScene;
	A3DVSSScene*			m_pOldScene;
	A3DVSSIBLScene*			m_pIBLScene;
	int *					m_pPVSBuffer_Begin;
	int *					m_pPVSBuffer_End;
	int						m_MaxObjCnt;					
	int						m_iCurrentRouteID;
	int						m_iCurrentPointID;
	int						m_iLastRouteID;
	bool					m_bForceRenderAll;
	bool					m_bIBL;
	float					m_fDirLightWeight;
	float					m_fAmbLightWeight;

public:
	
	inline A3DIBLLightGrid*	GetLightGrid(){return m_pA3DIBLLightGrid;}
	inline int* PVSRenderPtrBegin(){return m_pPVSBuffer_Begin;}
	inline int* PVSRenderPtrEnd(){return m_pPVSBuffer_End;}
	void ForceRenderAll(bool b);
	bool LoadNewRoute(BYTE* pBuf_Point,BYTE* pBuf_Route);
	bool SetCurrentRoute(bool bRoute, int iSample);
	
	inline A3DScene * GetA3DScene()const{return m_pA3DScene;}
	inline A3DESP * GetA3DESP(){return m_pA3DESP;}
	inline void BindDevice(A3DDevice *  pD){m_pA3DDevice	= pD;}
	inline bool BindFinished()const{return m_pA3DDevice !=0;}
	AScene();
	virtual bool Create(const PAWORLD_INIT_SCENE);
	virtual bool Create(ASceneInit*);
	virtual bool Release();
	virtual ~AScene();
	virtual bool UpdateVisibleSets(A3DViewport * pCurrentViewport);

};

#endif//_ASCENE_H_