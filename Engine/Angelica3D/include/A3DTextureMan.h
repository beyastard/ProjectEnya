/*
 * FILE: A3DTextureMan.h
 *
 * DESCRIPTION: Class that manage all textures in A3D Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTEXTUREMAN_H_
#define _A3DTEXTUREMAN_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "AList.h"

typedef struct _TEXTURERECORD
{
	char				szFilename[MAX_PATH];
	A3DTexture			*pA3DTexture;
	int					nRefCount;
} TEXTURERECORD, * PTEXTURERECORD;

class A3DTextureMan : public A3DData
{
private:
	A3DDevice * m_pA3DDevice;
	AList	    m_ListTexture;

	bool FindTexture(char * szFilename, A3DTexture ** ppA3DTexture);

public:
	A3DTextureMan();
	~A3DTextureMan();

	bool Init(A3DDevice * pDevice);
	bool Release();
	bool Reset();

	bool TickAnimation();

	// Use dwTextureFlags to specify special texture usage, eg. detail map or lightmap
	// if dwTextureFlags is 0, then it is a diffuse texture map only;
	// if dwTextureFlags contains A3DTF_MIPLEVEL, then nMipLevel will be used when
	// create mipmap chain;
	bool LoadTextureFromFile(char * pszFilename, A3DTexture ** ppA3DTexture, DWORD dwTextureFlags=0, int nMipLevel=0);
	bool LoadTextureFromFileInFolder(char * pszFilename, char * szFolder, A3DTexture ** ppA3DTexture, DWORD dwTextureFlags=0, int nMipLevel=0);
	bool ReleaseTexture(PA3DTexture&  pA3DTexture);
	
	bool PrecacheAllTexture();

	inline int GetTextureCount() { return m_ListTexture.GetSize(); }
};

typedef A3DTextureMan * PA3DTextureMan;

#endif
