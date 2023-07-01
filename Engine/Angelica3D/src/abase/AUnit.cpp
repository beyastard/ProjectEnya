#include "AUnit.h"
#include "APointerAuto.h"
#include "AUList.h"

IMPLEMENT_ASERIAL(AUnit,AIDObject,1)

AUnit::AUnit():m_pAGame(NULL),m_iTicks(0){}
AUnit::~AUnit(){}

int AUnit::OnMsg(PAMSG pMsg){return -1;}
bool AUnit::Release(){return true;}

bool AUnit::BindGame(AGame * m_pG)
{
	m_pAGame		= m_pG;
	return true;
}

bool AUnit::BindFinished()
{
	return m_pAGame != NULL;
}

bool AUnit::LogicRun(float __TimeSpan)
{
	m_iTicks++;
	if(m_iTicks>=0XFFFFFFF0)
	{
		m_iTicks = 0;
	}
	return true;
}

bool AUnit::Load(AArchive &ar, AGame *__pAGame)
{
	if(AIDObject::Load(ar)==false)
	{
		ADEBUG_TRACE("AUnit load error !");
		return false;
	}
	int nVersion;
	if(ar.Read(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	//利用这个版本号进行版本控制
	APointerAutoT<AData*> pData	= (AData*)ar.CreateObject();

	assert(pData != NULL);

	TRY_METHOD(
		pData->Load(ar), 
		"AUnit::Load error cannot read stream!");

	return Init(pData, __pAGame);
}

bool AUnit::Save(AArchive& ar)
{
	if(AIDObject::Save(ar)==false)
	{
		ADEBUG_TRACE("AUnit save error !");
		return false;
	}
	int nVersion = AUnit::GetClassInfo()->GetVersion();
	if(ar.Write(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}

	APointerAutoT<AData*> pData	= CreateData();
	if(!OnWriteData(pData))
	{
		return false;
	}
	ar.WriteClassInfo(pData);
	if(!pData->Save(ar))
	{
		return false;
	}
	return true;
}

bool AUnit::Init(AData *_pData, AGame *_pAGame)
{

	BindGame(_pAGame);
	
	if(!OnReadData(_pData))
	{
		return false;
	}
	if(_pData)
		SetId(_pData->GetId());
	return true;
}

AData* AUnit::CreateData()
{
	return new AData;
}

bool AUnit::OnWriteData(AData* pData)
{
	if(pData)
		VERIFY_OBJECT_TYPE(pData, AData);
	return true;
}

bool AUnit::OnReadData(AData* pData)
{
	if(pData)
		VERIFY_OBJECT_TYPE(pData, AData);
	return true;
}

bool AUnit::IsDead()
{
	return false;
}

bool AUnit::IsDel()
{
	return false;
}
