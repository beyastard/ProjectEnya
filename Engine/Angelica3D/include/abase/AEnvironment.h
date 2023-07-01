/*
 * FILE: AEnvironment.H
 *
 * DESCRIPTION :
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 8 - 14
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 */

#ifndef _AENVIRONMENT_H_
#define _AENVIRONMENT_H_

#include "ABaseA3D.h"
#include "AWorldInit.h"

class AEnvironment
{
private:

	AWORLD_INIT_ENVIRONMENT		m_kernel_datum;

public:

	AEnvironment();
	virtual bool Create(const PAWORLD_INIT_ENVIRONMENT __pDatum);
	virtual bool Create(AEnvironmentInit * );
	virtual bool Release();
	const AWORLD_INIT_ENVIRONMENT* GetKernelDatum(){return &m_kernel_datum;}

	virtual ~AEnvironment();

};

#endif//_AENVIRONMENT_H_