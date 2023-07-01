#include "AMatterData.h"
#include "AModelUnit.h"
#include "AGamePlay.h"
/*
IMPLEMENT_ASERIAL(AMatterData, AModelUnitData, 0x0100)

AMatterData::AMatterData()
{
}

AMatterData::~AMatterData()
{
}

bool AMatterData::Load(AArchive &ar)
{
	if(!AModelUnitData::Load(ar))
	{
		return false;
	}

	if(!ar.Read(&m_matter_dat, sizeof(m_matter_dat)))
	{
		return false;
	}

	return true;
}

bool AMatterData::Save(AArchive &ar)
{
	if(!AModelUnitData::Save(ar))
	{
		return false;
	}

	if(!ar.Write(&m_matter_dat, sizeof(m_matter_dat)))
	{
		return false;
	}

	return true;
}*/