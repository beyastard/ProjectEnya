/*
 * FILE: AFI.h
 *
 * DESCRIPTION: headers for interface functions in Angelica File Lib;
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFI_H_
#define _AFI_H_

#include "AFPlatform.h"

//	************************** Attention Please! *******************************
//
//	To Use File Module properly, you must first call AFileMod_Initialize() and 
//	then call AFileMod_SetBaseDir() to set a correct base dir, and at last you
//  should make a call to AFileMod_Finalize()
//
//	The default base dir is current directory, which is set when calling 
//	AFileMod_Initialize()
//
//  ****************************************************************************

bool AFileMod_Initialize(bool bCompressEnable=true);
bool AFileMod_SetBaseDir(char * szBaseDir);
bool AFileMod_Finalize();
const char * AFileMod_GetBaseDir();

void AFileMod_GetFullPath(char * szFullPath, char * szFolderName, char * szFileName);
void AFileMod_GetFullPath(char * szFullPath, char * szFileName);
void AFileMod_GetRelativePath(char * szFullPath, char * szFolderName, char * szRelativePath);
void AFileMod_GetRelativePath(char * szFullPath, char * szRelativePath);

// Get the file's title in the filename string;
// Note: lpszFile and lpszTitle should be different buffer;
bool AFileMod_GetFileTitle(char * lpszFile, char * lpszTitle, WORD cbBuf);

// Get the file's path in the filename string;
// Note: lpszFile and lpszPath should be different buffer;
bool AFileMod_GetFilePath(char * lpszFile, char * lpszPath, WORD cbBuf);
#endif