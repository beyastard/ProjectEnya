/*
 * FILE: A3DSurfaceMan.h
 *
 * DESCRIPTION: Managing class for surface object
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSURFACE_MAN_H_
#define _A3DSURFACE_MAN_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DDevice.h"
#include "A3DSurface.h"

class A3DSurfaceMan : public A3DData
{
private:
	A3DDevice *				m_pA3DDevice;
	char					m_szFolderName[MAX_PATH];

public:
	A3DSurfaceMan();
	~A3DSurfaceMan();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();
	bool Reset();

	bool LoadCursorSurfaceFromFile(int nWidth, int nHeight, char * szFileName, A3DCOLOR colorKey, A3DSurface ** ppSurface);
	bool LoadSurfaceFromFile(int nWidth, int nHeight, char * szFileName, A3DCOLOR colorKey, A3DSurface ** ppA3DSurface);
	bool ReleaseSurface(PA3DSurface& pSurface);

	inline void SetFolderName(char * szFolderName) { strncpy(m_szFolderName, szFolderName, MAX_PATH); }
};

typedef A3DSurfaceMan * PA3DSurfaceMan;

#endif
