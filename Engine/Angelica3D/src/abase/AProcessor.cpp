#include "AProcessor.h"

IMPLEMENT_ASERIAL(AProcessor,AUnit,1)

AProcessor::AProcessor() : AUnit()
{
	m_nActive	= 0;
}

AProcessor::~AProcessor()
{

}

bool AProcessor::Release()
{

	return true;
}

bool AProcessor::Draw()
{
	return true;
}