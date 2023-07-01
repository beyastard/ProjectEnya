/*
 * FILE: A3DScene.h
 *
 * DESCRIPTION: Class representing the whole scene in A3D Engine
 *
 * CREATED BY: Hedi, 2002/3/25
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSCENE_H_
#define _A3DSCENE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DMaterial.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DBSP.h"

//#define A3DSCENE_USESTREAM

typedef struct _A3DSCENE_TEXTURE_RECORD
{
	int						nTexID;						// index in texture record buffer;
	char					szTextureName[MAX_PATH];	// Name of the texture;
	char					szDetailTextureName[MAX_PATH]; //Name of the detail texture;
	A3DMATERIALPARAM		material;					// Material parameter of this texture;
	A3DMATRIX4				matDetailTM;				// Texture Transformation Matrix for detail texture;
	bool					b2Sided;					// If this material is 2 sided;
	A3DMESH_PROP			prop;						// The mesh's property;

	int						nFaceNum;					// Total face using this texture;
	int						nFaceVisible;				// Current Visible Face Count;

	DWORD					dwRenderFlag;				// Flag to say this texture is to be rendered or not;
	A3DTexture *			pTexture;					// Pointer of this texture;
	A3DTexture *			pDetailTexture;				// Pointer of this texture's detail texture;
	A3DVERTEX  *			pVerts;						// Pointer of vertex buffer;
#ifdef A3DSCENE_USESTREAM
	A3DStream  *			pStream;					// The predefined stream which can contain all vertex of this texture;
#endif
} A3DSCENE_TEXTURE_RECORD, * PA3DSCENE_TEXTURE_RECORD;

typedef struct _A3DSCENE_FACE_RECORD
{
	WORD					nTexID;						// The texture id of this face;
	int						nRefTicks;					// The ticks last time referenced this face (in Engine tick unit);

} A3DSCENE_FACE_RECORD, * PA3DSCENE_FACE_RECORD;

typedef struct _A3DSCENE_OPTION
{
	int						nVertexType;

	union
	{
		char				foo[64];
	};
} A3DSCENE_OPTION, *PA3DSCENE_OPTION;

#define A3DSCENE_RENDER_SOLID		1
#define A3DSCENE_RENDER_ALPHA		2

class A3DScene : public A3DObject
{
	friend int FaceSortCompare(const void *arg1, const void *arg2);

private:
	typedef struct _A3DSCENE_SORTEDFACE
	{
		float							vDisToCam;
		A3DSCENE_TEXTURE_RECORD			*pTexRecord;
		WORD							wIndexInTexVisible;
	} A3DSCENE_SORTEDFACE;

protected:
	A3DDevice *					m_pA3DDevice;
	A3DBSP *					m_pA3DBSP;

	int							m_nOldCamLeaf;

	A3DSCENE_OPTION				m_Option;

	int							m_nNumFaces;		// Total faces of this scene;
	A3DVERTEX *					m_pAllFaces;		// Faces buffer that consist of seperate vertex;
	A3DSCENE_FACE_RECORD *		m_pFaceRecords;		// Buffer stores each face's texture id and reference info;
	A3DSCENE_SORTEDFACE *		m_pSortedFaces;		// Buffer used for sorts;

	int							m_nNumVisibleFaces;	// Number of Visible Faces;

	int							m_nNumTextures;		// Texture number;
	A3DSCENE_TEXTURE_RECORD *	m_pTextureRecords;	// Texture list;

	A3DAABB						m_SceneAABB;
protected:
	bool AddMesh(A3DMesh * pMesh);
	A3DSCENE_TEXTURE_RECORD * FindTextureRecord(char * szTextureName, A3DMATERIALPARAM material, bool b2Sided, A3DMESH_PROP prop);

public:
	A3DScene();
	~A3DScene();

	virtual bool Init(A3DDevice * pA3DDevice);
	virtual bool Release();

	bool SetBSPFile(char * szBSPFile);

	inline bool IsVisiblePos(A3DVECTOR3& vecPos);
	// Serialize methods;
	bool Load(A3DDevice * pDevice, AFile * pFileToLoad);
	bool Save(AFile * pFileToSave);

	virtual	bool UpdateVisibleSets(A3DViewport * pCurrentViewport);

	// Render current scene with the specified flag control;
	// See A3DScene.h for predefined flag
	virtual bool Render(A3DViewport * pCurrentViewport, DWORD dwFlag);
	virtual bool RenderSort(A3DViewport * pCurrentViewport, DWORD dwFlag, bool bNear2Far=false);

	// Import/Export methods;
	// For import, we will add this mox into current scene data, NOT REPLACE!
	virtual bool Import(char * szMoxFileName);
	virtual bool Export(char * szMoxFileName);

	virtual int GetFaceCount() { return m_nNumFaces; }
	A3DVERTEX * GetFaceBuffer() { return m_pAllFaces; }
	virtual bool GetAllFaceVerts(A3DVECTOR3 * pVertsBuffer, int * pnBufferSize);
	virtual int GetTextureNum() { return m_nNumTextures; }
	virtual A3DAABB& GetSceneAABB() { return m_SceneAABB; }
	virtual int GetNumVisibleFaces() { return m_nNumVisibleFaces; }
	virtual bool IsOnGround(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].prop.bOnGround; }
	virtual bool IsUsedForPVS(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].prop.bUsedForPVS; }
	virtual bool Is2Sided(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].b2Sided;	}
	virtual bool IsAlpha(int nFaceIndex) { return m_pTextureRecords[m_pFaceRecords[nFaceIndex].nTexID].dwRenderFlag == A3DSCENE_RENDER_ALPHA ? true : false;	}
};

bool A3DScene::IsVisiblePos(A3DVECTOR3& vecPos)
{
	if( !m_pA3DBSP )
		return true;

	if( m_nOldCamLeaf < 0)// || m_pA3DBSP->IsOutBSPSpace(vecPos) )
		return true;
	
	return m_pA3DBSP->CanBeSeenFromLeaf(vecPos, m_nOldCamLeaf);
}

typedef class A3DScene * PA3DScene;

#endif _A3DSCENE_H_
