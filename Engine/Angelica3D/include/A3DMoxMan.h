/*
 * FILE: A3DMoxMan.h
 *
 * DESCRIPTION: Managing Class for frame objects
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMOXMAN_H_
#define _A3DMOXMAN_H_

#include "A3DPlatform.h"
#include "A3DData.h"
#include "A3DTypes.h"
#include "A3DDevice.h"
#include "A3DFrame.h"
#include "AList.h"

typedef struct _MOXRECORD
{
	char		szFilename[MAX_PATH];
	A3DFrame	* pFrame;
} MOXRECORD, * PMOXRECORD;

class A3DMoxMan : public A3DData
{
private:
	char		m_szFolderName[MAX_PATH];

	A3DDevice * m_pA3DDevice;
	AList	    m_ListMox;

	bool FindMoxFile(char * szFilename, A3DFrame ** ppFrame);
public:
	A3DMoxMan();
	~A3DMoxMan();

	bool Init(A3DDevice * pDevice);
	bool Release();
	bool Reset();

	bool LoadMoxFile(char * szFilename, A3DFrame ** ppFrame);
	bool ReleaseFrame(A3DFrame *& pFrame);
	bool DuplicateFrame(A3DFrame * pOrgFrame, A3DFrame ** ppNewFrame);
					
	inline void SetFolderName(char * szFolderName) { strcpy(m_szFolderName, szFolderName); }
	inline char * GetFolderName() { return m_szFolderName; }
	inline int GetMoxCount() { return m_ListMox.GetSize(); }
};

typedef A3DMoxMan * PA3DMoxMan;

#endif