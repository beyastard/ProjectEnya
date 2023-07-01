/*
 * FILE: AComboBox.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-10
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__ACOMBOBOX_H__
#define	__ACOMBOBOX_H__

#include "AWnd.h"
#include "AButton.h"

class AComboBox : public AWnd
{
public:
	DECLARE_ASERIAL(AComboBox)

protected:
	

public:
	AComboBox();
	virtual ~AComboBox();
};

#endif	// __ACOMBOBOX_H__