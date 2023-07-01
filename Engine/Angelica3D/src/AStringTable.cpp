/*
 * FILE: AStringTable.cpp
 *
 * DESCRIPTION: A string table for angelica engine
 *
 * CREATED BY: Hedi, 2002/5/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.
 */

#include "AStringTable.h"
#include "AFileImage.h"
#include "AScriptFile.h"
#include "AFPI.h"

AStringTable::AStringTable()
{
	m_pStringEntries		= NULL;
	m_nEntryBufferCapacity	= 0;
	m_nEntryCount			= 0;

	m_pCharBuffer			= NULL;
	m_pNextCharBuffer		= NULL;
	m_nCharBufferLen		= 0;

	m_bHasSorted			= true;
}

AStringTable::~AStringTable()
{
}

bool AStringTable::Init(char * szFileName)
{
	AFileImage fileImage;
	if( !fileImage.Open(szFileName, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		AFERRLOG(("AStringTable::Init(), Can not open file [%s]", szFileName));
		return false;
	}

	AScriptFile scriptFile;
	if( !scriptFile.Open(&fileImage) )
	{
		AFERRLOG(("AStringTable::Init(), Can not open script file!", szFileName));
		return false;
	}

	char		szEntryName[256];
	char		szEntryData[1024];

	while(1)
	{
		if( !scriptFile.GetNextToken(true) )
			break;
		strncpy(szEntryName, scriptFile.m_szToken, 256);
		scriptFile.GetNextToken(true);
		strncpy(szEntryData, scriptFile.m_szToken, 1024);

		if( !AddEntry(szEntryName, szEntryData) )
		{
			AFERRLOG(("AStringTable::Init(), Call AddEntry() Fail!"));
			goto Failure;
		}
	}

	scriptFile.Close();
	fileImage.Close();

	ResortEntry();

	return true;

Failure:
	scriptFile.Close();
	fileImage.Close();
	return false;
}

bool AStringTable::Release()
{
	if( m_pStringEntries )
	{
		free(m_pStringEntries);
		m_pStringEntries = NULL;
	}
	if( m_pCharBuffer )
	{
		free(m_pCharBuffer);
		m_pCharBuffer = NULL;
	}

	m_nEntryCount = 0;
	m_nEntryBufferCapacity = 0;
	
	m_nCharBufferLen = 0;
	m_pNextCharBuffer = NULL;
	return true;
}

bool AStringTable::GetEntry(char * szEntryName, char * pszEntryData, DWORD dwBufLen, DWORD * pdwBufOutLen)
{
	pszEntryData[0] = '\0';

	if( !m_bHasSorted )
	{
		// Use linear search;
		for(int i=0; i<m_nEntryCount; i++)
		{
			if( 0 == _stricmp(m_pCharBuffer + m_pStringEntries[i].pEntryName, szEntryName) )
			{
				return GetEntryDataByIndex(i, pszEntryData, dwBufLen, pdwBufOutLen);
			}
		}
	}
	else
	{
		// Use binary search;
		if( m_nEntryCount == 0 )
			return false;

		int		nLeft = 0; 
		int		nRight = m_nEntryCount - 1;

		int		nMiddle;
		char	*szName;

		while(nLeft <= nRight)
		{
			nMiddle = (nLeft + nRight) / 2;
			szName = m_pCharBuffer + m_pStringEntries[nMiddle].pEntryName;

			int nCompare = _stricmp(szName, szEntryName);
			if( nCompare == 0 )
				return GetEntryDataByIndex(nMiddle, pszEntryData, dwBufLen, pdwBufOutLen);
			else if( nCompare < 0 )
				nLeft = nMiddle + 1;
			else if( nCompare > 0 )
				nRight = nMiddle - 1;
		}
	}
	return false;
}

bool AStringTable::GetEntryDataByIndex(int nIndex, char * pszEntryData, DWORD dwBufLen, DWORD * pdwBufOutLen)
{
	int nDataLen = strlen(m_pCharBuffer + m_pStringEntries[nIndex].pEntryData) + 1;

	if( pdwBufOutLen )
		*pdwBufOutLen = nDataLen;

	if( dwBufLen < (DWORD)nDataLen )
		return false;
	strcpy(pszEntryData, m_pCharBuffer + m_pStringEntries[nIndex].pEntryData);
	return true;
}

bool AStringTable::GetEntryNameByIndex(int nIndex, char * pszEntryName, DWORD dwBufLen, DWORD * pdwBufOutLen)
{
	int nNameLen = strlen(m_pCharBuffer + m_pStringEntries[nIndex].pEntryName) + 1;

	if( pdwBufOutLen )
		*pdwBufOutLen = nNameLen;

	if( dwBufLen < (DWORD)nNameLen )
		return false;
	strcpy(pszEntryName, m_pCharBuffer + m_pStringEntries[nIndex].pEntryName);
	return true;
}

bool AStringTable::AddEntry(char * szEntryName, char * szEntryData)
{
	int nNameLen = strlen(szEntryName);
	int nDataLen = strlen(szEntryData);

	if( m_pCharBuffer + m_nCharBufferLen < m_pNextCharBuffer + nNameLen + 1 + nDataLen + 1 )
	{
		int nCurrentOffset = m_pNextCharBuffer - m_pCharBuffer;

		m_nCharBufferLen += max(1024, nNameLen + 1 + nDataLen + 1);

		m_pCharBuffer = (char *) realloc(m_pCharBuffer, m_nCharBufferLen);
		if( NULL == m_pCharBuffer )
		{
			AFERRLOG(("AStringTable::AddEntry(), Not enough memory!"));
			return false;
		}

		m_pNextCharBuffer = m_pCharBuffer + nCurrentOffset;
	}

	ASTRING_ENTRY		entry;

	strcpy(m_pNextCharBuffer, szEntryName);
	entry.pEntryName = m_pNextCharBuffer - m_pCharBuffer;
	m_pNextCharBuffer += nNameLen + 1;

	strcpy(m_pNextCharBuffer, szEntryData);
	entry.pEntryData = m_pNextCharBuffer - m_pCharBuffer;
	m_pNextCharBuffer += nDataLen + 1;

	if( m_nEntryCount >= m_nEntryBufferCapacity )
	{
		m_nEntryBufferCapacity += 256;
		m_pStringEntries = (ASTRING_ENTRY *) realloc(m_pStringEntries, sizeof(ASTRING_ENTRY) * m_nEntryBufferCapacity);
		if( NULL == m_pStringEntries )
		{
			AFERRLOG(("AStringTable::Init(), Not enough memory!"));
			return false;
		}
	}

	m_pStringEntries[m_nEntryCount] = entry;
	m_nEntryCount ++;

	m_bHasSorted = false;
	return true;
}

bool AStringTable::ResortEntry()
{
	char			*szName;
	
	int				nBestIndex = 0;
	char			*szBestName;
	ASTRING_ENTRY	temp;

	// Use selection sorting
	for(int i=0; i<m_nEntryCount-1; i++)
	{
		nBestIndex = i;
		szBestName = m_pCharBuffer + m_pStringEntries[i].pEntryName;

		for(int j=i+1; j<m_nEntryCount; j++)
		{
			szName = m_pCharBuffer + m_pStringEntries[j].pEntryName;
									 
			if( _stricmp(szBestName, szName) > 0 )
			{
				nBestIndex = j;
				szBestName = szName;
			}
		}

		if( nBestIndex != i )
		{
			// exchage them;
			temp = m_pStringEntries[i];
			m_pStringEntries[i] = m_pStringEntries[nBestIndex];
			m_pStringEntries[nBestIndex] = temp;
		}
	}

	m_bHasSorted = true;
	return true;
}

int AStringTable::CompareTwoEntry(int nEntry1, int nEntry2)
{
	char * szName1 = m_pCharBuffer + m_pStringEntries[nEntry1].pEntryName;
	char * szName2 = m_pCharBuffer + m_pStringEntries[nEntry2].pEntryName;

	return _stricmp(szName1, szName2);
}
