#include "AData.h"
#include "ADebug.h"

// class AData

IMPLEMENT_ASERIAL(AData, AIDObject, 1)

AData::AData()
{
	INITIALIZE_STATICPROPERIES(AData)
}

AData::~AData()
{
}

AUString AData::GetName()
{
	return m_str_name;
}

void AData::SetName(AUString name)
{
	m_str_name = name;
}

bool AData::Save(AArchive &ar)
{
	if(!AIDObject::Save(ar))
	{
		return false;
	}
	int nVersion;
	nVersion = AData::GetClassInfo()->GetVersion();
	if(ar.Write(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}

	if(!ar.WriteString(m_str_name)) 
		return false;

	return true;
}

bool AData::Load(AArchive &ar)
{
	if(!AIDObject::Load(ar))
	{
		return false;
	}
	int nVersion;
	if(ar.Read(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	//利用这个版本号进行版本控制

	if(!ar.ReadString(m_str_name))
		return false;

	return true;
}

void AData::CollectionResourceRec(AListT<AUString> &resources)
{

}
