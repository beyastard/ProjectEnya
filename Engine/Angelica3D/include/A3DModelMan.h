/*
 * FILE: A3DModelMan.h
 *
 * DESCRIPTION: Managing Class for model objects
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMODELMAN_H_
#define _A3DMODELMAN_H_

#include "A3DPlatform.h"
#include "A3DData.h"
#include "A3DTypes.h"
#include "A3DDevice.h"
#include "A3DModel.h"
#include "AList.h"
#include "A3DModelCollector.h"

typedef struct _MODELRECORD
{
	char		szFilename[MAX_PATH];
	A3DModel	* pModel;
} MODELRECORD, * PMODELRECORD;

class A3DModelMan : public A3DData
{
private:
	char					m_szFolderName[MAX_PATH];

	A3DDevice *				m_pA3DDevice;
	AList					m_ListModel;

	A3DModelCollector *		m_pModelCollector;

protected:
	bool FindModelFile(char * szFilename, A3DModel ** ppFrame);

public:
	A3DModelMan();
	~A3DModelMan();

	bool Init(A3DDevice * pDevice);
	bool Release();
	bool Reset();

	bool LoadModelFile(char * szFilename, A3DModel ** ppModel, bool bChild=false);
	bool ReleaseModel(A3DModel *& pModel);

	inline void SetFolderName(char * szFolderName) { strcpy(m_szFolderName, szFolderName); }
	inline char * GetFolderName() { return m_szFolderName; }
	inline int GetModelCount() { return m_ListModel.GetSize(); }
};

typedef A3DModelMan * PA3DModelMan;

#endif