#include "A3DObject.h"

A3DObject::A3DObject()
{
	m_szName[0] = m_szName[MAX_NAME] = '\0';
}

A3DObject::~A3DObject()
{
}

bool A3DObject::Save(AFile * pFileToSave)
{
	return true;
}

bool A3DObject::Load(A3DDevice * pDevice, AFile * pFileToLoad)
{
	return true;
}
