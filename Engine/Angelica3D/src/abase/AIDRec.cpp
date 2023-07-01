#include "AIDRec.h"
#include "AObject.h"
#include "AObjectIDMan.h"


AIDRec::AIDRec()
{
	m_nCaheSize = 256;
	Malloc(m_nCaheSize);
}

AIDRec::~AIDRec()
{

}

bool AIDRec::SetDataPtr(AObject * __p,int nID)
{
	if(nID > GetSize())
	{
		ReMalloc(GetSize() + m_nCaheSize,NULL);
	}
	byte * pData = (byte *)((byte *)m_pData + nID * sizeof(byte *));

	AObject ** p = (AObject **)pData;

	if(*p != NULL)
	{ 
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		assert(0);
	}
	*p = __p;

	return true;
}


AObject * AIDRec::GetDataPtrByID(int __nID)
{
	if(__nID >= GetSize())
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		assert(0);
	}

	byte * pData = (byte *)((byte *)m_pData + __nID * sizeof(byte *));

	AObject ** p = (AObject **)pData;

	return *p;
}

bool AIDRec::SetDataPtr(int __nID, AObject *__p)
{
	assert(__nID < GetSize());
	if(__nID > GetSize())
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);

		ReMalloc(GetSize() + m_nCaheSize,NULL);
	}

	byte * pData = (byte *)((byte *)m_pData + __nID * sizeof(byte *));

	AObject ** p = (AObject **)pData;

	*p = __p;

	return true;

}

int AIDRec::GetSize()
{
	return GetLength()/(sizeof(AObject *));
}

bool AIDRec::ReMalloc(int __newSize, int __newValue)
{
	if(__newSize ==0)
	{
		this->GetBufferSetLength(0);
		return true;
	}
	__newSize *= sizeof(AObject *);
	int nSize = GetLength();
	
	byte * pData = (byte*)GetBufferSetLength(__newSize);

	if(pData == NULL)
		return false;
	if(__newSize>nSize)
	{
		memset(pData+nSize,__newValue,__newSize-nSize);
	}
	return true;
}


bool AIDRec::Malloc(int __newSize, int __newValue)
{
	__newSize *= sizeof(AObject *);
	byte * pData = (byte*)GetBufferSetLength(__newSize);

	if(pData == NULL)
		return false;

	memset(pData,__newValue,__newSize);

	return true;
}
