/*
 * FILE: PreDeclare.h
 *
 * DESCRIPTION: 类、结构和枚举量的预声明
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */
#ifndef __PREDECLARE__H__
#define __PREDECLARE__H__

#include <windows.h>

struct AMSG;
typedef AMSG * PAMSG;

struct tagCREATEFRMSTRUCT;
typedef tagCREATEFRMSTRUCT  CREATEFRMSTRUCT, *LPCREATEFRMSTRUCT;

struct tagINITAGAMESTRUCT;
typedef tagINITAGAMESTRUCT INITAGAMESTRUCT,*PINITAGAMESTRUCT;

#endif  // __PREDECLARE__H__