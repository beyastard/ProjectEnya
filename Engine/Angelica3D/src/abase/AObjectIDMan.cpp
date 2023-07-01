#include "AObjectIDMan.h"
#include "AGame.h"
#include "AIDObject.h"
#include "AIDRec.h"
#include "AGamePlay.h"
#include "AArchive.h"

IMPLEMENT_ASERIAL(AObjectIDMan,AManager,1)

AObjectIDMan::AObjectIDMan():AManager()
{
	m_managerId		= 0;
	m_IDRec			= NULL;
	m_nNum			= 0;
	m_objectID		= -1;

}

AObjectIDMan::~AObjectIDMan()
{
	Release();
}


int AObjectIDMan::OnGetID(AIDObject * pAObject)
{
	int id =  pAObject->GetId();
	return id;
}

bool AObjectIDMan::IsValidID(int __nID)
{
	if(__nID > m_objectID)
	{
		return false;
	}
	else if(__nID<0)
	{
		return false;
	}
	else
	{
		AIDObject * pIDObject = (AIDObject*)(m_IDRec->GetDataPtrByID(__nID));
		if(pIDObject == NULL)
			return false;
		if(pIDObject->GetId() != __nID)
			return false;
		else
			return true;
	}
}

bool AObjectIDMan::DelObject(AIDObject * __pObject)
{
	assert(__pObject);

	if(!IsValidPtr(__pObject))
		return false;
	int nID = __pObject->GetId();

	AIDObject * pIDObject = (AIDObject*)(m_IDRec->GetDataPtrByID(nID));
	if(!pIDObject)
		return true;
	assert(pIDObject->GetId() == nID);
	m_IDRec->SetDataPtr(nID,NULL);

	__pObject->Release();

	delete __pObject;

	m_nNum--;
	assert(m_nNum>=0);

	return true;
}

AIDObject * AObjectIDMan::GetPtrByID(int __nID)
{
	if(__nID > m_objectID) 
		return NULL;
	return (AIDObject*)(m_IDRec->GetDataPtrByID(__nID));
}


bool AObjectIDMan::Load(AArchive &ar,AGame * __pAGame)
{	
	if(AManager::Load(ar,__pAGame)==false)
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
	if(!ar.Read(&m_nNum,sizeof(m_nNum)))
	{
		ADebug::Msg(0,"%s %d read error",__FILE__,__LINE__);
		return false;
	}
	ADebug::Msg(0,"%d Loading ",m_nNum);
	int maxID = -1;
	for(int i=0;i<m_nNum;i++)
	{
		// 创建新对象
		AIDObject  *ptr = (AIDObject*)ar.CreateObject();
		
		if(OwnerLoad(ptr,ar,__pAGame)==false)
		{
			ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
			return false;
		}		
		m_IDRec->SetDataPtr(ptr,ptr->GetId());
		int id = ptr->GetId();
		if(id>maxID)
			maxID = id;
	}
	m_objectID = maxID;

	return true;
}

bool AObjectIDMan::Save(AArchive &ar)
{
	if(AManager::Save(ar)==false)
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	int nVersion = AObjectIDMan::GetClassInfo()->GetVersion();
	if(ar.Write(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	
	if(ar.Write(&m_nNum,sizeof(m_nNum)) == false)
	{
		ADebug::Msg(0," %s  %d AArchive write error",__FILE__,__LINE__);
		return false;
	}
	ADebug::Msg(0,"%d have write by AIMan",m_nNum);
	ADebug::Msg(0,"%d have max id AIMan",m_objectID);
	for(int i=0;i<=m_objectID;i++)
	{
		AIDObject * ptr = (AIDObject*)(m_IDRec->GetDataPtrByID(i));
		if(IsValidPtr(ptr))
		{
			ar.WriteClassInfo(ptr);
			if(ptr->Save(ar) == false)
			{
				ADebug::Msg(0,"%s %d Save error",__FILE__,__LINE__);
				return false;
			}
		}
	}
	return true;

}

bool AObjectIDMan::Init(AGame *__pAGame)
{
	AManager::Init(__pAGame);
	m_IDRec			= new AIDRec;
	if(!m_IDRec)
	{
		ADebug::Msg(0," %s %d error",__FILE__,__LINE__);
	}
	return true;
}

bool AObjectIDMan::Release()
{
	bool b =  Clear();
	m_pAGame = NULL;
	return b;
}

int AObjectIDMan::AddObject(AIDObject *pObject)
{
	assert(pObject);

	int nID = OnGetID(pObject);
	if(nID>m_objectID)
		m_objectID = nID;

	if( m_IDRec->SetDataPtr(pObject,nID) == false)
	{
		ADebug::Msg(0,"%s %d Add Data error",__FILE__,__LINE__);
		return -1;
	}
	m_nNum ++;
	return nID;
}

bool AObjectIDMan::IsValidPtr(AIDObject *__ptr)
{
	if(!__ptr)
		return false;
	int nID = __ptr->GetId();

	return IsValidID(nID);
}

bool AObjectIDMan::LogicRun(float __fSpanTime)
{
	if(OnLogicRun(__fSpanTime)==false)
		return true;
	for(int i=0;i<=GetTakedMaxID();i++)
	{
		AIDObject * pMatter = GetPtrByID(i);
		if(pMatter)
		{
			if(!OwnerProc(pMatter,__fSpanTime))
				return false;
		}
	}
	return true;
}

bool AObjectIDMan::OnRelease(AIDObject *__pAIDObject)
{
	__pAIDObject->Release();
	delete __pAIDObject;
	return true;
}

bool AObjectIDMan::OwnerProc(AIDObject *pAIDObject, float fSpanTime)
{
	return true;
}

bool AObjectIDMan::OwnerLoad(AIDObject *__p, AArchive &ar, AGame *__pAGame)
{
	return true;
}

bool AObjectIDMan::Clear()
{
	if(m_IDRec)
	{
		for(int i=0;i<=m_objectID;i++)
		{
			AIDObject * ptr = (AIDObject*)(m_IDRec->GetDataPtrByID(i));
			if(ptr)
			{
				if(OnRelease(ptr)==false) return false;
			}
		}
		m_IDRec->GetBufferSetLength(0);
		m_IDRec->Malloc(256); ;
	}
	m_nNum		= 0;
	m_objectID	= -1;

	if(m_IDRec)
	{
		m_IDRec->GetBufferSetLength(0);
		delete m_IDRec ;
		m_IDRec = NULL;
	}
	return true;
}

int AObjectIDMan::DeleteObject(AIDObject *__p)
{
	return 1;
}
