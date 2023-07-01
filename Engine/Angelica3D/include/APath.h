/*
 * FILE: APath.h
 *
 * DESCRIPTION: A set of functions used to manipulate the path in Angelica Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _APATH_H_
#define _APATH_H_

inline void APath_GetRelativePath(char * szFullpath, char * szParentPath, char * szRelativepath)
{
	char * p1, * p2;
	p1 = szParentPath; p2 = szFullpath;
	while( *p1 && *p2 && (*p1 == *p2 || 
		(*p1 >= 'A' && *p1 <= 'Z' && *p1 + 0x20 == *p2) || 
		(*p2 >= 'A' && *p2 <= 'Z' && *p1 == *p2 + 0x20)) )
	{
		p1 ++;
		p2 ++;
	}

	if( *p1 ) // not found;
	{
		strcpy(szRelativepath, szFullpath);
		return;
	}

	if( *p2 == '\\' )
		p2 ++;

	strcpy(szRelativepath, p2);
}

inline void APath_GetFullPath(char * szFullpath, char * szBaseDir, char * szFilename)
{
	szFullpath[0] = '\0';

	int nStrLenName = strlen(szFilename);
	if( nStrLenName == 0 )
		return;
	if( nStrLenName > 1 && szFilename[0] == '\\' )
		return;
	//See if it is a absolute path;
	if( nStrLenName > 3 )
	{
		if( szFilename[1] == ':' && szFilename[2] == '\\' )
		{
			strcpy(szFullpath, szFilename);
			return;
		}
	}

	char * pszRealfile = szFilename;
	// Get rid of prefix .\, so to make a clean relative file path
	if( nStrLenName > 2 && szFilename[0] == '.' && szFilename[1] == '\\' )
		pszRealfile = szFilename + 2;
	
	if( szBaseDir[0] == '\0' )
		strcpy(szFullpath, pszRealfile);
	else if( szBaseDir[strlen(szBaseDir) - 1] == '\\' )
		sprintf(szFullpath, "%s%s", szBaseDir, pszRealfile);
	else
		sprintf(szFullpath, "%s\\%s", szBaseDir, pszRealfile);
	return;
}

inline void APath_TrimPath(char * pszPath, char * pszTrimedPath)
{
	int nStartPos, nEndPos;	

	nStartPos = 0;
	nEndPos = strlen(pszPath) - 1;

	while( pszPath[nStartPos] && (pszPath[nStartPos] == ' ' || pszPath[nStartPos] == '\t') )
		++ nStartPos;
	while( nEndPos >= nStartPos && pszPath[nEndPos] && (pszPath[nEndPos] == ' ' || pszPath[nStartPos] == '\t') )
		-- nEndPos;

	strncpy(pszTrimedPath, pszPath + nStartPos, nEndPos - nStartPos + 1);
	pszTrimedPath[nEndPos - nStartPos + 1] = '\0';
}

#endif//_APATH_H_