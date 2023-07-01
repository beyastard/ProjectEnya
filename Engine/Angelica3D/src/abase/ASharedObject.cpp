#include "ASharedObject.h"


IMPLEMENT_ASERIAL(ASharedObject, AObject, 0x0100)

ASharedObject::ASharedList ASharedObject::m_DefSharedList;

ASharedObject::ASharedObject()
{
}

ASharedObject::~ASharedObject()
{
	Release();
}

ASharedObject::ASharedList& ASharedObject::GetSharedList()
{
	return m_DefSharedList;
}

VOID ASharedObject::DecRef()
{
	if(m_nRefCount > 1)
	{
		m_nRefCount --;
		return;
	}
	ASharedList& shlist = GetSharedList();

	AListPosition	Pos	= shlist.GetHeadPosition();
	while(Pos != NULL)
	{
		AListPosition PrePos	= Pos;
		ASharedObject* pObj	= shlist.GetNext(Pos);
		if(pObj == this)
		{
			shlist.RemoveAt(PrePos);
			break;
		}
	}
	delete this;
}


