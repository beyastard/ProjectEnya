/*
 * FILE: AFI.cpp
 *
 * DESCRIPTION: interface functions in Angelica File Module;
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AFI.h"

ALog *		g_pAFErrLog = NULL;
char		g_szBaseDir[MAX_PATH] = "";
bool		g_bCompressEnable = false;

bool AFileMod_Initialize(bool bCompressEnable)
{
	if( g_pAFErrLog )
		AFileMod_Finalize();

	g_bCompressEnable = bCompressEnable;
	
	// By default, we should set the g_szBaseDir to current directory
	GetCurrentDirectory(MAX_PATH, g_szBaseDir);
	if( g_szBaseDir[strlen(g_szBaseDir) - 1] == '\\' )
		g_szBaseDir[strlen(g_szBaseDir) - 1] = '\0';

	if( !g_pAFErrLog )
	{
		g_pAFErrLog = new ALog();
		if( NULL == g_pAFErrLog )
			return false; // Not enough memory!

		g_pAFErrLog->Init("AF.log", "Angelica File Module Error Log");
	}

	return true;
}

bool AFileMod_SetBaseDir(char * szBaseDir)
{
	strncpy(g_szBaseDir, szBaseDir, MAX_PATH);

	// Get rid of last '\\'
	if( g_szBaseDir[0] && g_szBaseDir[strlen(g_szBaseDir) - 1] == '\\' )
		g_szBaseDir[strlen(g_szBaseDir) - 1] = '\0';

	return true;
}

const char * AFileMod_GetBaseDir()
{
	return g_szBaseDir;
}

bool AFileMod_Finalize()
{
	if( g_pAFErrLog )
	{
		g_pAFErrLog->Release();
		delete g_pAFErrLog;
		g_pAFErrLog = NULL;
	}

	g_szBaseDir[0] = '\0';
	return true;
}

void AFileMod_GetFullPath(char * szFullPath, char * szFolderName, char * szFileName)
{
	char szBaseDir[MAX_PATH];
	sprintf(szBaseDir, "%s\\%s", g_szBaseDir, szFolderName);
	APath_GetFullPath(szFullPath, szBaseDir, szFileName);
}

void AFileMod_GetFullPath(char * szFullPath, char * szFileName)
{
	APath_GetFullPath(szFullPath, g_szBaseDir, szFileName);
}

void AFileMod_GetRelativePath(char * szFullPath, char * szFolderName, char * szRelativePath)
{
	char szBaseDir[MAX_PATH];

	sprintf(szBaseDir, "%s\\%s", g_szBaseDir, szFolderName);
	APath_GetRelativePath(szFullPath, szBaseDir, szRelativePath);
}

void AFileMod_GetRelativePath(char * szFullPath, char * szRelativePath)
{
	APath_GetRelativePath(szFullPath, g_szBaseDir, szRelativePath);
}

bool AFileMod_GetFileTitle(char * lpszFile, char * lpszTitle, WORD cbBuf)
{
	lpszTitle[0] = '\0';
	if( lpszFile[0] == '\0' )
		return true;

	char * pszTemp;
    pszTemp = (char *)lpszFile + strlen(lpszFile);
	--pszTemp;
    if ('\\' == (* pszTemp) || '/' == (* pszTemp)) return false;
    while (true) {
        if ('\\' == (* pszTemp) || '/' == (* pszTemp))
		{
            ++pszTemp;
            break;
        }
        if (pszTemp == lpszFile) break;
        --pszTemp;
    }
    strcpy(lpszTitle, pszTemp);
    return true;
}

bool AFileMod_GetFilePath(char * lpszFile, char * lpszPath, WORD cbBuf)
{
	lpszPath[0]= '\0';
	if( lpszFile[0] == '\0' )
		return true;

	char * pszTemp;
    
	strncpy(lpszPath, lpszFile, cbBuf);
    pszTemp = (char *)lpszPath + strlen(lpszPath);

    --pszTemp;
    while (true) {
        if ('\\' == (* pszTemp) || '/' == (* pszTemp)) 
		{
            break;
        }
        if (pszTemp == lpszPath) break;
        --pszTemp;
    }
    *pszTemp = '\0';
    return true;
}
