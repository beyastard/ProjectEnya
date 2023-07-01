#include "AVisualUnit.h"
#include "AGame.h"

IMPLEMENT_ASERIAL(AVisualUnit,AUnit,1)

AVisualUnit::AVisualUnit():m_pA3DEngine(0){}
AVisualUnit::~AVisualUnit(){}
bool AVisualUnit::BindGame(AGame * _pG)
{
	AUnit::BindGame(_pG);
	m_pA3DEngine = _pG->GetA3DEngine();
	return true;
}
bool AVisualUnit::BindFinished(){return AUnit::BindFinished() && m_pA3DEngine;}
bool AVisualUnit::LogicRun(float __TimeSpan){return AUnit::LogicRun(__TimeSpan);}
bool AVisualUnit::Render(A3DViewport * pCurrentViewport){return true;}
int AVisualUnit::OnMsg(PAMSG pMsg){return AUnit::OnMsg(pMsg);}
bool AVisualUnit::Release(){return AUnit::Release();}
bool AVisualUnit::GetVisibility(){return true;}
void AVisualUnit::SetVisibility(bool b){}
