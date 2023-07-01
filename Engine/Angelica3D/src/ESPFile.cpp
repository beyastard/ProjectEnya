/*
 * FILE: ESPFile.cpp
 *
 * DESCRIPTION: Common operations for ESP file
 *
 * CREATED BY: Duyuxin, 2001/10/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "ESPFile.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Extern variables and functions
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Declaration of local functions
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

/*	Write lump data into ESP file.

	fp: ESP file's pointer.
	pHeader: ESP file's header.
	lLump: lump will be written.
	pData: lump's data.
	lSize: total size of this lump.
*/
void ESP_WriteLump(FILE* fp, PESPFILEHEADER pHeader, LONG lLump, void* pData, LONG lSize)
{
	PEFLUMP pLump = &pHeader->aLumps[lLump];

	pLump->lOff  = ftell(fp);
	pLump->lSize = lSize;

	fwrite(pData, 1, lSize, fp);
}

/*	Read lump data from ESP file.

	Return number of items in this lump for success, otherwise return -1.

	fp: ESP file's pointer.
	pHeader: ESP file's header.
	lLump: lump will be read.
	ppBuf (out): address of a variable that will be set to lump data buffer's pointer.
	lSize: size of each item in this lump.
*/
int ESP_ReadLump(FILE* fp, PESPFILEHEADER pHeader, LONG lLump, void** ppBuf, LONG lSize)
{
	PEFLUMP pLump = &pHeader->aLumps[lLump];

	if (pLump->lSize % lSize)
		return -1;
	
	void *pBuf;
	if (!(pBuf = malloc(pLump->lSize)))
		return -1;

	fseek(fp, pLump->lOff, SEEK_SET);
	fread(pBuf, 1, pLump->lSize, fp);

	*ppBuf = pBuf;

	return pLump->lSize / lSize;
}



