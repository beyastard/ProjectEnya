#include "AObjectIDListMan.h"
#include "AModelUnit.h"
#include "AStat.h"

IMPLEMENT_ASERIAL(AObjectIDListMan,AManager,1)

AObjectIDListMan::AObjectIDListMan() : AManager()
{

}

AObjectIDListMan::~AObjectIDListMan()
{

}

bool AObjectIDListMan::AddObject(AIDObject *__p)
{
	int id = OnGetID(__p);
	__p->SetId(id);
	m_list.AddTail(__p);
	return true;
}

bool AObjectIDListMan::Load(AArchive &ar, AGame *__pAGame)
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
	int nNum = 0;
	if(!ar.Read(&nNum,sizeof(nNum)))
	{
		ADebug::Msg(0,"%s %d read error",__FILE__,__LINE__);
		return false;
	}
	for(int i=0;i<nNum;i++)
	{
		// 创建新对象
		AIDObject  *ptr = (AIDObject*)ar.CreateObject();
		if(OwnerLoad(ptr,ar,__pAGame)==false)
		{
			ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
			return false;
		}
		m_list.AddTail(ptr);
	}
	return true;

}

bool AObjectIDListMan::OwnerLoad(AIDObject *__p, AArchive &ar, AGame *__pAGame)
{
	return true;
}


bool AObjectIDListMan::Save(AArchive &ar)
{
	if(AManager::Save(ar)==false)
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	int nVersion = AObjectIDListMan::GetClassInfo()->GetVersion();
	if(ar.Write(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	int nNum = m_list.GetCount();
	if(ar.Write(&nNum,sizeof(nNum)) == false)
	{
		ADebug::Msg(0," %s  %d AArchive write error",__FILE__,__LINE__);
		return false;
	}
	AIDObject * p = NULL;
	AListPosition pos = m_list.GetHeadPosition();
	while(pos)
	{
		p = m_list.GetNext(pos);
		if(p==NULL)
		{
			ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
			assert(0);
			continue;
		}
		ar.WriteClassInfo(p);
		if(p->Save(ar)==false)
		{
			ADebug::Msg(0,"%s %d Save error",__FILE__,__LINE__);
			return false;
		}
	}
	return true;

}

bool AObjectIDListMan::Release()
{
	bool b =  Clear();
	m_pAGame = NULL;
	return b;
}

bool AObjectIDListMan::LogicRun(float __TimeSpan)
{
	if(OnLogicRun(__TimeSpan)==false)
		return true;
	AListPosition posThis;
	AListPosition pos = m_list.GetHeadPosition();
	BEGIN_STAT("管理器的LogicRun耗时");
	while(pos)
	{
		posThis = pos;
		AIDObject * pAIDObject = m_list.GetNext(pos);
		if(pAIDObject==NULL)
		{
			ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
			assert(0);
			return false;
		}

		if(OwnerProc(__TimeSpan,pAIDObject,posThis) == false)
			return false;
	}
	END_STAT();
	return true;
}

bool AObjectIDListMan::OwnerProc(float __f, AIDObject *__pAIDObject,AListPosition __pos)
{
	return true;
}

AIDObject * AObjectIDListMan::GetPtrByID(int id)
{
	AListPosition pos = m_list.GetTailPosition();
	while(pos)
	{
		AIDObject * pAIDObject = m_list.GetPrev(pos);
		if(pAIDObject->GetId() == id)
			return pAIDObject;
	}
	return NULL;
}

bool AObjectIDListMan::Draw()
{
	AListPosition pos = m_list.GetHeadPosition();
	while(pos)
	{
		AIDObject * pAIDObject = m_list.GetNext(pos);
		if(pAIDObject==NULL)
		{
			ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
			assert(0);
			return false;
		}
		if(OwnerDraw(pAIDObject) == false )
		{
			ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
			assert(0);
			return false;
		}
	}
	return true;

}

bool AObjectIDListMan::OwnerDraw(AIDObject *pAIDObject)
{
	return true;
}

int AObjectIDListMan::OnGetID(AIDObject *__p)
{
	return __p->GetId();
}

bool AObjectIDListMan::Clear()
{
	AIDObject * p = NULL;
	AListPosition pos = m_list.GetHeadPosition();
	while(pos)
	{
		p = m_list.GetNext(pos);
		if(p==NULL)
		{
			ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
			assert(0);
			continue;
		}
		OnRelease(p);
	}
	m_list.RemoveAll();
	return true;
}

bool AObjectIDListMan::OnRelease(AIDObject * p)
{
	if(p->IsKindOf(CLASS_INFO(AModelUnit)))
	{
		((AModelUnit*)p)->WithdrawModel();
	}
	p->Release();
	delete  p;
	return true;
}

AIDObject * AObjectIDListMan::GetPtrByIndex(int nIndex)
{
	if(nIndex<0 || nIndex>= (int)m_list.GetCount())
		return 0;
	return m_list.GetByIndex(nIndex);
}

bool AObjectIDListMan::DeleteObject(AIDObject *__p)
{
	return true;
}

int AObjectIDListMan::GetCount()
{ 
	return m_list.GetCount(); 
}