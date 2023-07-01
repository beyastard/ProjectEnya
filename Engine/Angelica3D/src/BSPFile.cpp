/*
 * FILE: BSPFile.cpp
 *
 * DESCRIPTION: Data and operations for BSP file
 *
 * CREATED BY: Duyuxin, 2001/8/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "BSPFile.h"

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

/*	Write lump data into BSP file.

	fp: BSP file's pointer.
	pHeader: BSP file's header.
	lLump: lump will be written.
	pData: lump's data.
	lSize: total size of this lump.
*/
void BSP_WriteLump(FILE* fp, PBSPFILEHEADER pHeader, LONG lLump, void* pData, LONG lSize)
{
	PBFLUMP pLump = &pHeader->aLumps[lLump];

	pLump->lOff  = ftell(fp);
	pLump->lSize = lSize;

	fwrite(pData, 1, lSize, fp);
}

/*	Read lump data from BSP file.

	Return number of items in this lump for success, otherwise return -1.

	fp: BSP file's pointer.
	pHeader: BSP file's header.
	lLump: lump will be read.
	ppBuf (out): used to receive the lump data buffer's address.
	lSize: size of each item in this lump.
*/
int BSP_ReadLump(FILE* fp, PBSPFILEHEADER pHeader, LONG lLump, void** ppBuf, LONG lSize)
{
	PBFLUMP pLump = &pHeader->aLumps[lLump];

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



