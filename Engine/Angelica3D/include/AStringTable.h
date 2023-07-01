/*
 * FILE: AStringTable.h
 *
 * DESCRIPTION: A string table for angelica engine
 *
 * CREATED BY: Hedi, 2002/5/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ASTRINGTABLE_H_
#define _ASTRINGTABLE_H_

#include "AFPlatform.h"

typedef struct _ASTRING_ENTRY
{
	int				pEntryName; // Offset relative to the beginning of m_pCharBuffer;
	int				pEntryData; // Offset relative to the beginning of m_pCharBuffer;
}ASTRING_ENTRY, * PASTRING_ENTRY;

class AStringTable
{
private:
	int					m_nCharBufferLen;		// the buffer length of char buffer
	char *				m_pNextCharBuffer;		// a pointer to next not used char position;
	char *				m_pCharBuffer;			// a pointer to all the text involed with this string talble;

	int					m_nEntryCount;			// current string entry count
	int					m_nEntryBufferCapacity;	// the capacity of m_pStringEntries, in structure count
	ASTRING_ENTRY *		m_pStringEntries;		// pointer to entry list table

	bool				m_bHasSorted;			// Flag indicating that the entries has been sorted according to its entry name;

protected:
	int CompareTwoEntry(int nEntry1, int nEntry2);

public:
	AStringTable();
	~AStringTable();

	bool Init(char * szFilename);
	bool Release();

	// Get a entry's string value
	// In:
	//		szEntryName:		the entry's name, case ignored
	//		dwBufLen			the buffer's length
	// Out: 
	//		pszEntryData:	 	the string value of the entry if found
	//		pdwBufOutLen:       can be NULL. the real buffer len of pszEntryData
	//	
	// Return
	//		false		if pEntryData is too short, and pdwBufferLen will contain the length needed;
	//		true		success
	bool GetEntry(char * szEntryName, char * pszEntryData, DWORD dwBufLen, DWORD * pdwBufOutLen);
	bool GetEntryDataByIndex(int nIndex, char * pszEntryData, DWORD dwBufLen, DWORD * pdwBufOutLen);
	bool GetEntryNameByIndex(int nIndex, char * pszEntryName, DWORD dwBufLen, DWORD * pdwBufOutLen);

	// Add a string entry into the string table;
	// In:
	//		szEntryName:		the entry's name, will be converted to all upper case
	//		szEntryData:		the string value of this entry
	bool AddEntry(char * szEntryName, char * szEntryData);

	// Sort the entries according to each entry's name;
	bool ResortEntry();

	inline int GetEntryCount() { return m_nEntryCount; }
};

typedef class AStringTable * PAStringTable;

#endif
