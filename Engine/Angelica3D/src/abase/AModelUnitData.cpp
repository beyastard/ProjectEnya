#include "AModelUnitData.h"
IMPLEMENT_ASERIAL(AModelUnitData, AData, 1)

void AModelUnitData::SetDefault()
{
	m_model_datum.aabb_trace_enable = true;
	m_model_datum.ray_trace_enable	= true;
	m_model_datum.visibility		= true;
}