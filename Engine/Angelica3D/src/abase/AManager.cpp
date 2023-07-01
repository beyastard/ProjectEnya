#include "AManager.h"
#include <assert.h>

IMPLEMENT_ASERIAL(AManager,AObject,1)

AManager::AManager() : AUnit()
{
	m_managerId = 0;
}

AManager::~AManager()
{

}

bool AManager::LogicRun(float __TimeSpan)
{
	if(OnLogicRun(__TimeSpan)==false)
		return true;
	return true;
}

bool AManager::Draw()
{
	return true;
}

bool AManager::Init(AGame* __pAGame)
{
	m_pAGame = __pAGame;
	return true;
}

bool AManager::Release()
{
	return true;
}

int AManager::OnMsg(AMSG * pMSG)
{

	return -1;
}

bool AManager::Clear()
{
	return true;
}

bool AManager::Load(AArchive &ar,AGame * p)
{
	return true;
}

bool AManager::Save(AArchive &ar)
{
	return true;
}

bool AManager::OnLogicRun(float fTimeSpan)
{
	return true;
}

bool AManager::LogicRun_Begin(float time)
{
	return true;
}
