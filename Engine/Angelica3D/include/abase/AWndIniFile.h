/*
 * FILE: AWndIniFile.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-11
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AWNDINIFILE_H__
#define	__AWNDINIFILE_H__

#include "ADebug.h"
#include "AArray.h"
#include "AUFile.h"
#include "AWndTypes.h"
//#include "FIniFile.h"


class AWndIniFile
{
protected:
	class AIniKey
	{
	public:
		char*	m_pszName;
		char*	m_pszValue;
		int		m_nNameLength;
		int		m_nValueLength;
		AIniKey():
		m_pszName		(NULL),
		m_pszValue		(NULL),
		m_nNameLength	(0),
		m_nValueLength	(0)
		{
		}
		~AIniKey()
		{
			if(m_pszName)	delete m_pszName;
			if(m_pszValue)	delete m_pszValue;
		}
	};

	class AIniSection
	{
	public:
		char*	m_pszName;
		int		m_nNameLength;
		AArrayT<AIniKey*>	m_Keys;

		AIniSection();
		~AIniSection();
	};

	AArrayT<AIniSection*>	m_Sections;
	char*	m_pszFile;
	AUFile	m_File;
	int		m_nCurSection;

	const BYTE*	IniReadBlank	(const BYTE* pBuf, int nBufLength);
	const BYTE*	IniReadLine		(const BYTE* pBuf, int nBufLength);
	const BYTE*	IniReadSection	(const BYTE* pBuf, int nBufLength);
	const BYTE*	IniSkipLine		(const BYTE* pBuf, int nBufLength);
	const BYTE*	IniReadKey		(const BYTE* pBuf, int nBufLength);
	const char*	IniGetValue		(int nSecIndex,	int nKeyIndex);
	const char* IniGetValue		(const char* pszSection, const char* pszKey);

	bool	IniAddSection	(const char* pszName, int nLength);
	bool	IniAddKey		(int nSection, const char* pszName, int nNameLength, const char* pszValue, int nValueLength);
	bool	IniFindSection	(const char* pszName, int nLength, int& nIndex);
	bool	IniFindKey		(int nSection, const char* pszKeyName, int nNameLength, int& nIndex);

	bool	IniWriteValue	(const char* pszSection, const char* pszKey, const char* pszValue, int nValueLength);
	bool	IniFindKey		(const char* pszSection, const char* pszKey, int& nSecIndex, int& nKeyIndex);
public:
	bool ReadGUID(char * szSection,char * key,GUID &guid);
	AWndIniFile();
	virtual ~AWndIniFile();

	virtual void Release();

	virtual bool Init(const char* pszFileName);
	virtual bool Init(const BYTE* pBuf, int nBufLength);
	virtual bool Save(BYTE*& pBuf, int& nLength);
	virtual bool Save();
	virtual bool Save(const char* pszFileName);

	bool ReadInt	(const char* pszSection, const char* pszKey, int&	nValue);
	bool ReadInt	(const char* pszSection, const char* pszKey, long&	nValue);
	bool ReadFloat	(const char* pszSection, const char* pszKey, float& fValue);
	bool ReadString	(const char* pszSection, const char* pszKey, const char*& pszValue);
	bool ReadRect	(const char* pszSection, const char* pszKey, ARECT& rRect);
	bool ReadPoint	(const char* pszSection, const char* pszKey, APOINT& ptPoint);

	bool WriteInt	(const char* pszSection, const char* pszKey, int   nValue);
	bool WriteFloat	(const char* pszSection, const char* pszKey, float fValue);
	bool WriteString(const char* pszSection, const char* pszKey, const char* pszValue);

	int	 GetSectionCount();
	int  GetKeyCount(int nSection);
	int  GetKeyCount();
	int  GetSectionIndex(const char* pszSection);
	int  GetKeyIndex(int nSection, const char* pszKey);
	const char* GetSectionName(int nSection);
	const char* GetKeyName(int nSection, int nKey);
	const char* GetKeyValue(int nSection, int nKey);
};

#endif	// __AWNDINIFILE_H__