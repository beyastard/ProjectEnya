/*
 * FILE: A3DObject.h
 *
 * DESCRIPTION: Base Class of any class in A3D Engine
 *
 * CREATED BY: Hedi, 2001/7/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DOBJECT_H_
#define _A3DOBJECT_H_

#include "A3DPlatform.h"
#include "A3DClassID.h"

typedef class A3DDevice *	PA3DDevice;

#define MAX_NAME MAX_PATH

class A3DObject
{
protected:
	DWORD			m_dwClassID;	
	char			m_szName[MAX_NAME + 1];
public:
	A3DObject();
	~A3DObject();
	inline void SetName(char * pszName) { strncpy(m_szName, pszName, MAX_NAME); }
	inline char * GetName() { return m_szName; }
	virtual bool Save(AFile * pFileToSave);
	virtual bool Load(A3DDevice * pDevice, AFile * pFileToLoad);
	inline DWORD GetClassID() { return m_dwClassID; }
};

typedef A3DObject * PA3DObject;

#endif