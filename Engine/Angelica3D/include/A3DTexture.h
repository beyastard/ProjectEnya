/*
 * FILE: A3DTexture.h
 *
 * DESCRIPTION: Class representing one texture in A3D Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTEXTURE_H_
#define _A3DTEXTURE_H_

#include "A3DPlatform.h"
#include "A3DObject.h"
#include "A3DDevice.h"

typedef class A3DSurface * PA3DSurface;

#define A3DTF_DETAILMAP		0x1
#define A3DTF_LIGHTMAP		0x2
#define A3DTF_MIPLEVEL		0x4

class A3DTexture : public A3DObject
{
private:
	A3DDevice *			m_pA3DDevice;

protected:
	// flag to indicate whether the device is not create at all;
	bool				m_bHWITexture;

private:
	// Flag to say whether this is a detail texture;
	bool				m_bDetailTexture;
	int					m_nMipLevel;			// How many mip map really created;
	int					m_nDesiredMipLevel;		// The maximum mip level want to be created;	

	// Flag to say whether this is a lightmap texture;
	bool				m_bLightMap;

	char				m_szMapFile[MAX_PATH];
	char				m_szRelativeMapFile[MAX_PATH];
	IDirect3DTexture8 * m_pDXTexture;

protected:
	bool				m_bAlphaTexture;
	bool				m_bShaderTexture; // Flag indicates whether this is a multi-stage shader
	
	bool LoadNormalTexture(A3DDevice * pA3DDevice, char * szFullpath, char * szRelativePath);
	bool LoadDetailTexture(A3DDevice * pA3DDevice, char * szFullpath, char * szRelativePath);
	bool LoadLightMapTexture(A3DDevice * pA3DDevice, char * szFullpath, char * szRelativePath);

	bool CreateDXTexture(int nWidth, int nHeight, A3DFORMAT format, int nMipLevel);

public:
	A3DTexture();
	~A3DTexture();

	virtual bool Init(A3DDevice* pDevice);
	virtual bool Release();

	// bool Init(A3DDevice* pDevice, IDirect3DTexture8* pDXTexture);

	virtual bool TickAnimation();
	virtual bool Appear(int nLayer=0);
	virtual bool Disappear(int nLayer=0);

	bool LoadFromFile(A3DDevice * pA3DDevice, char * szFullpath, char * szRelativePath);
	
	bool ExportToFile(char * szFullpath); 
	
	bool Create(A3DDevice * pDevice, int iWidth, int iHeight, A3DFORMAT Format, int nMipLevel=1);
	bool LockRect(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel=0);
	bool UnlockRect();

	// Init with specified width height and use the format of a surface, finally, copy 
	// that part of the surface onto the texture using copy rects;
	bool CreateFromSurface(A3DDevice * pA3DDevice, A3DSurface * pA3DSurface, A3DRECT& rect, int * pTexWidth, int * pTexHeight, A3DFORMAT fmtSurface=A3DFMT_UNKNOWN);

	// Update the texture's content from an A3DSurface;
	bool UpdateFromSurface(A3DSurface * pA3DSurface, A3DRECT& rect);

	inline char * GetMapFile() { return m_szMapFile; }
	inline char * GetMapFileInFolder(char * szFolderName);
	inline bool IsAlphaTexture() { return m_bAlphaTexture; }
	inline bool IsShaderTexture() { return m_bShaderTexture; }
	inline IDirect3DTexture8* GetD3DTexture() { return m_pDXTexture; }
	inline void SetIsDetailTexture(bool bFlag) { m_bDetailTexture = bFlag; }
	inline bool IsDetailTexture() { return m_bDetailTexture; }
	inline void SetIsLightMap(bool bFlag) { m_bLightMap = bFlag; }
	inline bool IsLightMap() { return m_bLightMap; }
	inline void SetMapFile(char * szMapName) { strncpy(m_szMapFile, szMapName, MAX_PATH); }
	inline int GetDesiredMipLevel() { return m_nDesiredMipLevel; }
	inline void SetDesciredMipLevel(int nLevel) { m_nDesiredMipLevel = nLevel; }
};

char * A3DTexture::GetMapFileInFolder(char * szFolderName)
{
	APath_GetRelativePath(m_szMapFile, szFolderName, m_szRelativeMapFile);
	return m_szRelativeMapFile;
}

typedef A3DTexture * PA3DTexture;
#endif

