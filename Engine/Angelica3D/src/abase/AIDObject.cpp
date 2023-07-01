#include "AIDObject.h"

IMPLEMENT_ASERIAL(AIDObject,AObject,1)

AIDObject::AIDObject()
{
	m_nId		= INVALIDOBJECTID;
}

AIDObject::~AIDObject()
{

}

bool AIDObject::Load(AArchive &ar)
{
	if(AObject::Load(ar)==false)
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	int nVersion;
	if(ar.Read(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	//利用这个版本号进行版本控制

	if(!ar.Read(&m_nId,sizeof(m_nId)))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}

	return true;
}

bool AIDObject::Save(AArchive &ar)
{
	if(AObject::Save(ar)==false)
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	int nVersion = AIDObject::GetClassInfo()->GetVersion();
	if(ar.Write(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	if(!ar.Write(&m_nId,sizeof(m_nId)))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}

	return true;
}

bool AIDObject::Release()
{
	return true;
}