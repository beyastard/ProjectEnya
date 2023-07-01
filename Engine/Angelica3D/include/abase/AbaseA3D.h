/*
 * FILE: ABaseA3D.h
 *
 * DESCRIPTION: A "new" operator which is used to detect memory leaks
 *				in debug mode. 
 *
 * CREATED BY: JiangLi, 2002/08/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.	
 */

#ifndef __ABASEA3D_H__
#define __ABASEA3D_H__

#ifndef ADEBUG_NEW

#include <A3D.h>

#else	// ADEBUG_NEW

#ifndef	new

#include <A3D.h>

#else	// new

#undef	new

#include <A3D.h>

#ifndef	_MFC_VER

#define new ADEBUG_NEW

#endif	// _MFC_VER

#endif	// new

#endif	// ADEBUG_NEW

#endif	// __ABASEA3D_H__