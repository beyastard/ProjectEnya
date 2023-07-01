/*
 * FILE: A3DBox.h
 *
 * DESCRIPTION: Class used to create and show a box according to an OBB or an AABB
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DBOX_H_
#define _A3DBOX_H_

#include "A3DPlatform.h"
#include "A3DDevice.h"
#include "A3DVertex.h"
#include "A3DTexture.h"
#include "A3DTypes.h"
#include "A3DStream.h"

class A3DBox
{
private:
	A3DDevice *			m_pA3DDevice;
	A3DStream *			m_pA3DStream;
	A3DTexture			m_Texture;

	A3DOBB				m_a3dOBB;

	A3DCOLOR			m_diffuse;
	A3DCOLOR			m_specular;

public:
	A3DBox();
	~A3DBox();

	bool Init(A3DDevice * pA3DDevice, A3DCOLOR diffuse=0xffffffff, A3DCOLOR specular=0xff000000);

	bool UpdateBox(const A3DAABB& a3dAABB, A3DCOLOR diffuse=0xffffffff, A3DCOLOR specular=0xff000000);
	bool UpdateBox(const A3DOBB& a3dOBB, A3DCOLOR diffuse=0xffffffff, A3DCOLOR specular=0xff000000);

	bool Release();
	bool Render(A3DViewport * pCurrentViewport);
};
#endif