
#include "AEnvironment.h"

AEnvironment::AEnvironment()
{
}

AEnvironment::~AEnvironment()
{
}

bool AEnvironment::Create(const PAWORLD_INIT_ENVIRONMENT __pDatum)
{
	m_kernel_datum = *__pDatum;
	return true;
}
bool AEnvironment::Create(AEnvironmentInit * pInit)
{
	AWORLD_INIT_ENVIRONMENT dat;
	pInit->ToKernelDatum(&dat);
	return Create(&dat);
}
bool AEnvironment::Release()
{
	return true;
}
