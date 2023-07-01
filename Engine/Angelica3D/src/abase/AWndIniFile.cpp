#include "AWndIniFile.h"
#include <math.h>

__declspec(naked) void __cdecl IniDecode(BYTE* pBuf, int nLength)
{
	__asm
	{
		mov		edx,	[esp+4]	// pBuf
		mov		ecx,	[esp+8]	// nLength

		xor		eax,	eax
Label_top:
		mov		al,		[edx+1]
		rol		al,		3
		mov		[edx],	al
		inc		edx
		dec		ecx
		jg		Label_top
		ret
	}
}

__declspec(naked) void __cdecl IniEncode(BYTE* pBuf, int nLength)
{
	__asm
	{
		mov		edx,	[esp+4]	// pBuf
		mov		ecx,	[esp+8]	// nLength

		xor		eax,	eax
Label_top:
		mov		al,		[edx+ecx-1]
		ror		al,		3
		mov		[edx+ecx],	al
		dec		ecx
		jg		Label_top
		mov		byte ptr[edx],	0
		ret
	}
}

AWndIniFile::AIniSection::AIniSection()
{
}

AWndIniFile::AIniSection::~AIniSection()
{
	int	nIndex;
	for(nIndex = m_Keys.GetLength() - 1; nIndex >= 0; nIndex --)
	{
		AIniKey* pKey = m_Keys[nIndex];
		if(pKey != NULL)
		{
			delete pKey;
		}
	}
	if(m_pszName != NULL)
	{
		delete m_pszName;
	}
	m_Keys.Release();
}

AWndIniFile::AWndIniFile():
m_pszFile		(NULL),
m_nCurSection	(-1)
{
}

AWndIniFile::~AWndIniFile()
{
	Release();
}

void AWndIniFile::Release()
{
	int nIndex;
	for(nIndex = m_Sections.GetLength() - 1; nIndex >= 0; nIndex --)
	{
		AIniSection* pSec = m_Sections[nIndex];
		if(pSec != NULL)
		{
			delete pSec;
		}
	}
	m_Sections.Release();

	if(m_pszFile != NULL)
	{
		delete m_pszFile;
		m_pszFile	= NULL;
	}

	m_nCurSection	= -1;

	m_File.Close();
}

bool AWndIniFile::Init(const char* pszFileName)
{
	if(pszFileName == NULL)
	{
		ADEBUG_TRACE("Invalid parameter.");
		return false;
	}

	INT nFileSize	= 0;
	BYTE*	pBuf	= NULL;

	if(!m_File.Create(pszFileName, OPEN_ALWAYS))
	{
		ADEBUG_TRACE1("Cannot open the IniFile <%s>", pszFileName);
		goto Lable_False_Exit;
	}

	nFileSize	= m_File.GetSize();
	pBuf		= new BYTE[nFileSize + 16];

	if(!m_File.Read(pBuf, nFileSize))
	{
		ADEBUG_TRACE1("Cannot read the IniFile <%s>", pszFileName);
		goto Lable_False_Exit;
	}
	pBuf[nFileSize]	= '\0';

	if(!Init(pBuf, nFileSize))
	{
		goto Lable_False_Exit;
	}

	if(pBuf != NULL)
	{
		delete[] pBuf;
	}

	m_pszFile	= new char[strlen(pszFileName) + 8];
	strcpy(m_pszFile, pszFileName);

	return true;

Lable_False_Exit:

	if(pBuf != NULL)
	{
		delete[] pBuf;
	}
	Release();
	return false;
}

bool AWndIniFile::Init(const BYTE* pBuf, int nBufLength)
{
	if(pBuf == NULL || nBufLength < 0)
	{
		ADEBUG_TRACE("Invalid parameter.");
		return false;
	}

	if(nBufLength == 0)
	{
		return true;
	}

	BYTE* p	= NULL;
	if(pBuf[0] == 0)
	{
		p = new BYTE[nBufLength];
		memcpy(p, pBuf, nBufLength);
		nBufLength -= 1;
		if(nBufLength <= 0)
		{
			return true;
		}
		IniDecode(p, nBufLength);
		p[nBufLength]	= 0;
	}
	else 
	{
		p = const_cast<BYTE*>(pBuf);
	}
	BYTE *pFirst	= p;
	int	nLength		= nBufLength;

	while(nLength > 0)
	{
		p	= const_cast<BYTE*>(IniReadLine(p, nLength));
		nLength	= nBufLength - (p - pFirst);
	}

	if(pBuf[0] == 0)
	{
		if(pFirst != NULL)
		{
			delete pFirst;
		}
	}

	return true;
}

bool AWndIniFile::Save(BYTE*& pBuf, int& nLength)
{
	int nBufLength	= 0;
	int nSecIndex	= 0;
	int nKeyIndex	= 0;
	AArrayT<AIniSection*>& Secs	= m_Sections;

	for(nSecIndex = 0; nSecIndex < Secs.GetLength(); nSecIndex ++)
	{
		nBufLength += Secs[nSecIndex]->m_nNameLength + 6; // include "[]\r\n" + "\r\n"
		AArrayT<AIniKey*>& Keys	= Secs[nSecIndex]->m_Keys;
		for(nKeyIndex = 0; nKeyIndex < Keys.GetLength(); nKeyIndex ++)
		{
			nBufLength += Keys[nKeyIndex]->m_nNameLength + 
				Keys[nKeyIndex]->m_nValueLength + 3;	// include "=\r\n"
		}
	}
	if(nBufLength <= 0)
	{
		pBuf	= NULL;
		nLength	= 0;
		return true;
	}

#define WND_INI_ENCODE

#ifdef	WND_INI_ENCODE
	pBuf	= new BYTE[nBufLength+1+1]; // include the last '\0' and first "0".
#else
	pBuf	= new BYTE[nBufLength+1]; // include the last '\0'
#endif	// WND_INI_ENCODE
	nLength	= nBufLength;

	int	nCurrent	= 0;

	for(nSecIndex = 0; nSecIndex < Secs.GetLength(); nSecIndex ++)
	{
		pBuf[nCurrent]	= '[';
		nCurrent ++;
		strncpy((char*)&pBuf[nCurrent], Secs[nSecIndex]->m_pszName, Secs[nSecIndex]->m_nNameLength);
		nCurrent += Secs[nSecIndex]->m_nNameLength;
		strncpy((char*)&pBuf[nCurrent], "]\r\n", 3);
		nCurrent += 3;
		AArrayT<AIniKey*>& Keys	= Secs[nSecIndex]->m_Keys;
		for(nKeyIndex = 0; nKeyIndex < Keys.GetLength(); nKeyIndex ++)
		{
			strncpy((char*)&pBuf[nCurrent], Keys[nKeyIndex]->m_pszName, Keys[nKeyIndex]->m_nNameLength);
			nCurrent	+= Keys[nKeyIndex]->m_nNameLength;
			pBuf[nCurrent]	= '=';
			nCurrent	++;
			strncpy((char*)&pBuf[nCurrent], Keys[nKeyIndex]->m_pszValue, Keys[nKeyIndex]->m_nValueLength);
			nCurrent	+= Keys[nKeyIndex]->m_nValueLength;
			pBuf[nCurrent]		= '\r';
			pBuf[nCurrent+1]	= '\n';
			nCurrent	+= 2;
		}
		pBuf[nCurrent]		= '\r';
		pBuf[nCurrent+1]	= '\n';
		nCurrent	+= 2;
	}

#ifdef	WND_INI_ENCODE
	IniEncode(pBuf, nBufLength);
	nBufLength ++;
#endif	// WND_INI_ENCODE

	pBuf[nBufLength]	= '\0';
	nLength	= nBufLength;

	return true;
}

bool AWndIniFile::Save()
{
	if(m_pszFile == NULL)
	{
		ADEBUG_TRACE("Cannot save the ini file : null file name");
		return false;
	}
	m_File.Close();
	if(!m_File.Create(m_pszFile, CREATE_ALWAYS))
	{
		delete m_pszFile;
		m_pszFile	= NULL;
		ADEBUG_TRACE("Cannot save the ini file : failed to recreate file.");
		return false;
	}

	BYTE* pBuf		= NULL;
	INT	  nLength	= 0;
	if(!Save(pBuf, nLength))
	{
		if(pBuf != NULL) delete[] pBuf;
		ADEBUG_TRACE("Cannot save the ini file : failed save to buf.");
		return false;
	}

	bool bRet	= true;
	m_File.SetPointer(0, FILE_BEGIN);
	if(nLength > 0)
	{
		bRet	= m_File.Write(pBuf, nLength); 
	}

	if(pBuf != NULL)
	{
		delete[] pBuf;
	}

	return true;
}

bool AWndIniFile::Save(const char* pszFileName)
{
	if(pszFileName == NULL)
	{
		ADEBUG_TRACE("Bad parameter.");
		return false;
	}
	int nLength	= strlen(pszFileName);
	m_pszFile	= new char[nLength + 1];
	strcpy(m_pszFile, pszFileName);

	Save();
	return true;
}

const BYTE*	AWndIniFile::IniReadBlank(const BYTE* pBuf, int nBufLength)
{
	const BYTE* p = pBuf;
	while(isspace(*p) && p - pBuf < nBufLength) p ++;
	return p;
}

const BYTE*	AWndIniFile::IniReadLine(const BYTE* pBuf, int nBufLength)
{
	pBuf	= IniReadBlank(pBuf, nBufLength);
	if(*pBuf == '[')
	{
		return IniReadSection(pBuf, nBufLength);
	}
	else if(*pBuf == ';')
	{
		return IniSkipLine(pBuf, nBufLength);
	}
	else if(*pBuf == '\0')
	{
		return pBuf + 1;
	}
	else
	{
		return IniReadKey(pBuf, nBufLength);
	}
	return pBuf;
}

const BYTE*	AWndIniFile::IniReadSection(const BYTE* pBuf, int nBufLength)
{
	const BYTE* pStart	= pBuf + 1;
	const BYTE* p		= pStart;
	while(*p != ']')
	{
		if(p - pBuf >= (nBufLength-1) || *p == '\r' || *p == '\n')
		{
			ADEBUG_TRACE("Bad section format.");
			return p;
		}
		p ++;
	}
	const BYTE* pEnd	= p - 1;
	while(isspace(*pStart) && pStart - pBuf < nBufLength) pStart ++;
	while(isspace(*pEnd) && pEnd >= pStart) pEnd --;

	if(pEnd < pStart)
	{
		ADEBUG_TRACE("Bad section format. no valid string.");
		return p;
	}

	int nStrLength			= (pEnd - pStart) + 1;

	IniAddSection((const char*)pStart, nStrLength);

	return p + 1;
}

const BYTE*	AWndIniFile::IniSkipLine(const BYTE* pBuf, int nBufLength)
{
	int nIndex = 0;

	for(nIndex = 0; nIndex < nBufLength; nIndex ++)
	{
		if(pBuf[nIndex] == '\n' || pBuf[nIndex] == '\r')
		{
			break;
		}
	}

	return &pBuf[nIndex];
}

const BYTE*	AWndIniFile::IniReadKey(const BYTE* pBuf, int nBufLength)
{
	if(nBufLength <= 0)
	{
		ADEBUG_TRACE("Bad length parameter.");
		return NULL;
	}

	const char* pszKeyName	= NULL;
	const char* pszKeyValue	= NULL;
	int		nIndex			= 0;
	int		nStart			= 0;
	int		nEnd			= 0;
	int		nVStart			= 0;
	int		nVEnd			= 0;
	int		nNameLength		= 0;
	int		nValueLength	= 0;

	for(nIndex = 0; nIndex < nBufLength; nIndex ++)
	{
		if(pBuf[nIndex] == '=')
		{
			break;
		}
		if(pBuf[nIndex] == '\n' || pBuf[nIndex] == '\r')
		{
			ADEBUG_TRACE("Bad key name format");
			return &pBuf[nIndex];
		}
	}
	nStart	= 0;
	nEnd	= nIndex - 1;
	while(isspace(pBuf[nStart]) && nStart < nBufLength) nStart ++;
	while(isspace(pBuf[nEnd]) && nEnd >= nStart) nEnd --;
	
	nNameLength	= nEnd - nStart + 1;
	if(nNameLength <= 0)
	{
		ADEBUG_TRACE("Bad key value format.");
		return &pBuf[nIndex]; 
	}

	nIndex ++;
	if(nIndex >= nBufLength)
	{
		ADEBUG_TRACE("Bad key value format.");
		return &pBuf[nIndex]; 
	}
	nVStart	= nIndex;

	for(; nIndex < nBufLength; nIndex ++)
	{
		BYTE b = pBuf[nIndex];
		if(b == '\n' || b == '\r' || b == '\0')
		{
			
			break;
		}
	}
	if(nIndex < nBufLength)
	{
		nVEnd	= nIndex - 1;
	}
	else
	{
		nVEnd	= nIndex;
	}

	while(isspace(pBuf[nVStart]) && nVStart < nBufLength) nVStart ++;
	while(isspace(pBuf[nVEnd]) && nVEnd >= nVStart) nVEnd --;

	nValueLength	= nVEnd - nVStart + 1;
	if(nValueLength <= 0)
	{
		ADEBUG_TRACE("Bad key value format.");
		return &pBuf[nIndex]; 
	}

	if(m_nCurSection >= 0)
	{
		IniAddKey(m_nCurSection, (const char*)&pBuf[nStart], nNameLength, (const char*)&pBuf[nVStart], nValueLength);
	}
	else
	{
		ADEBUG_TRACE("The key must in a section");
	}

	return &pBuf[nIndex];
}

bool AWndIniFile::IniFindSection(const char* pszName, int nLength, int& nIndex)
{
	int nLow	= 0;
	int nHigh	= m_Sections.GetLength() - 1;
	bool bFind	= false;

	while(nLow <= nHigh)
	{
		int nHalf	= (nHigh + nLow) >> 1;
		char* pszSecName	= m_Sections[nHalf]->m_pszName;
		int nLen	= m_Sections[nHalf]->m_nNameLength;
		int nCmp;
		if(nLen == nLength)
		{
			nCmp	= strncmp(pszName, pszSecName, nLength);
		}
		else
		{
			nCmp	= nLength - nLen;
		}
		if(nCmp == 0)
		{
			bFind	= true;
			nLow	= nHalf;
			break;
		}
		else if(nCmp > 0)
		{
			nLow	= nHalf + 1;
		}
		else
		{
			nHigh	= nHalf - 1;
		}
	}

	nIndex	= nLow;
	return bFind;
}

bool AWndIniFile::IniAddSection(const char* pszName, int nLength)
{
	if(nLength <= 0)
	{
		ADEBUG_TRACE("Bad length parameter.");
		return false;
	}

	int	nIndex	= 0;
	if(!IniFindSection(pszName, nLength, nIndex))
	{
		//return true;
		AIniSection*	pSec	= new AIniSection;
		pSec->m_nNameLength		= nLength;
		pSec->m_pszName			= new char[nLength + 1];
		strncpy(pSec->m_pszName, (const char*)pszName, nLength);
		pSec->m_pszName[nLength]	= '\0';
		m_Sections.InsertAt(nIndex, pSec);
	}
	
	m_nCurSection	= nIndex;

	return true;
}

bool AWndIniFile::IniFindKey(int nSection, const char* pszKeyName, int nNameLength, int& nIndex)
{
	AArrayT<AIniKey*>&	Keys	= m_Sections[nSection]->m_Keys;
	int nLow	= 0;
	int nHigh	= Keys.GetLength() - 1;
	bool bFind	= false;

	while(nLow <= nHigh)
	{
		int nHalf	= (nHigh + nLow) >> 1;
		char* pszKName	= Keys[nHalf]->m_pszName;
		int nLen	= Keys[nHalf]->m_nNameLength;
		int nCmp;
		if(nLen == nNameLength)
		{
			nCmp	= strncmp(pszKeyName, pszKName, nNameLength);
		}
		else
		{
			nCmp	= nNameLength - nLen;
		}
		if(nCmp == 0)
		{
			bFind	= true;
			nLow	= nHalf;
			break;
		}
		else if(nCmp > 0)
		{
			nLow	= nHalf + 1;
		}
		else
		{
			nHigh	= nHalf - 1;
		}
	}

	nIndex	= nLow;
	return bFind;
}

bool AWndIniFile::IniAddKey(int nSection, const char* pszName, int nNameLength, const char* pszValue, int nValueLength)
{
	if(nNameLength <= 0, nValueLength <= 0)
	{
		ADEBUG_TRACE("Bad length parameter.");
		return false;
	}

	int nIndex	= 0;
	if(!IniFindKey(nSection, pszName, nNameLength, nIndex))
	{
		//return true;
		AIniKey*	pKey	= new AIniKey;
		pKey->m_pszName		= new char[nNameLength + 1];
		pKey->m_nNameLength	= nNameLength;
		pKey->m_pszValue	= new char[nValueLength + 1];
		pKey->m_nValueLength= nValueLength;
		strncpy(pKey->m_pszName, pszName, nNameLength);
		pKey->m_pszName[nNameLength]	= '\0';
		strncpy(pKey->m_pszValue, pszValue, nValueLength);
		pKey->m_pszValue[nValueLength]	= '\0';
		m_Sections[nSection]->m_Keys.InsertAt(nIndex, pKey);
	}
	else
	{
		//return true;
		AIniKey*	pKey	= m_Sections[nSection]->m_Keys[nIndex];
		if(pKey->m_pszName) delete pKey->m_pszName;
		if(pKey->m_pszValue)delete pKey->m_pszValue;
		pKey->m_pszName		= new char[nNameLength + 1];
		pKey->m_nNameLength	= nNameLength;
		pKey->m_pszValue	= new char[nValueLength + 1];
		pKey->m_nValueLength= nValueLength;
		strncpy(pKey->m_pszName, pszName, nNameLength);
		pKey->m_pszName[nNameLength]	= '\0';
		strncpy(pKey->m_pszValue, pszValue, nValueLength);
		pKey->m_pszValue[nValueLength]	= '\0';
	}

	return true;
}

bool AWndIniFile::IniFindKey(const char* pszSection, const char* pszKey, int& nSecIndex, int& nKeyIndex)
{
	if(!IniFindSection(pszSection, strlen(pszSection), nSecIndex))
	{
		nSecIndex	= -1;
		nKeyIndex	= -1;
		return false;
	}
	if(!IniFindKey(nSecIndex, pszKey, strlen(pszKey), nKeyIndex))
	{
		nSecIndex	= -1;
		nKeyIndex	= -1;
		return false;
	}
	return true;
}

const char*	AWndIniFile::IniGetValue(int nSecIndex, int nKeyIndex)
{
	return m_Sections[nSecIndex]->m_Keys[nKeyIndex]->m_pszValue;
}


bool AWndIniFile::IniWriteValue(const char* pszSection, const char* pszKey, const char* pszValue, int nValueLength)
{
	if(!IniAddSection(pszSection, strlen(pszSection)))
	{
		ADEBUG_TRACE1("Cannot add section [%s]", pszSection);
		return false;
	}
	if(!IniAddKey(m_nCurSection, pszKey, strlen(pszKey), pszValue, nValueLength))
	{
		ADEBUG_TRACE1("Cannot add key [%s]", pszKey);
		return false;
	}
	return true;
}

const char* AWndIniFile::IniGetValue(const char* pszSection, const char* pszKey)
{
	if(pszSection == NULL || pszKey == NULL)
	{
		return NULL;
	}
	int nSecIndex, nKeyIndex;
	if(!IniFindKey(pszSection, pszKey, nSecIndex, nKeyIndex))
	{
		return NULL;
	}
	return IniGetValue(nSecIndex, nKeyIndex);
}

bool AWndIniFile::ReadInt(const char* pszSection, const char* pszKey, int& nValue)
{
	const char* pszValue	= IniGetValue(pszSection, pszKey);
	if(pszValue == NULL)
	{
		return false;
	}
	if(sscanf(pszValue, "%d", &nValue) <= 0)
	{
		return false;
	}
	return true;
}

bool AWndIniFile::ReadInt(const char* pszSection, const char* pszKey, long&	nValue)
{
	const char* pszValue	= IniGetValue(pszSection, pszKey);
	if(pszValue == NULL)
	{
		return false;
	}
	if(sscanf(pszValue, "%d", &nValue) <= 0)
	{
		return false;
	}
	return true;
}

bool AWndIniFile::ReadString(const char* pszSection, const char* pszKey, const char*& pszValue)
{
	const char* pszKeyValue	= IniGetValue(pszSection, pszKey);
	if(pszKeyValue == NULL)
	{
		pszValue	= NULL;
		return false;
	}

	pszValue	= pszKeyValue;
	return true;
}

bool AWndIniFile::ReadFloat(const char* pszSection, const char* pszKey, float& fValue)
{
	const char* pszValue	= IniGetValue(pszSection, pszKey);
	if(pszValue == NULL)
	{
		return false;
	}
	if(sscanf(pszValue, "%f", &fValue) <= 0)
	{
		return false;
	}
	return true;
}

bool AWndIniFile::ReadRect(const char* pszSection, const char* pszKey, ARECT& rRect)
{
	const char* pszValue	= IniGetValue(pszSection, pszKey);
	if(pszValue == NULL)
	{
		return false;
	}
	if(sscanf(pszValue, "%d,%d,%d,%d", &rRect.left, &rRect.top, &rRect.right, &rRect.bottom) >= 4)
	{
		return true;
	}
	int nSection = GetSectionIndex(pszValue);
	if(nSection < 0)
	{
		return false;
	}
	int nKey = GetKeyIndex(nSection, "value");
	if(nKey >= 0)
	{
		const char* pszVal	= GetKeyValue(nSection, nKey);
		if(pszVal == NULL)
		{
			return false;
		}
		return sscanf(pszVal, "%d,%d,%d,%d", &rRect.left, &rRect.top, &rRect.right, &rRect.bottom) >= 4;
	}
	if(!ReadInt(pszValue, "left", rRect.left))
	{
		return false;
	}
	if(!ReadInt(pszValue, "top", rRect.top))
	{
		return false;
	}
	if(!ReadInt(pszValue, "right", rRect.right))
	{
		return false;
	}
	if(!ReadInt(pszValue, "bottom", rRect.bottom))
	{
		return false;
	}
	return true;
}

bool AWndIniFile::ReadPoint(const char* pszSection, const char* pszKey, APOINT& ptPoint)
{
	const char* pszValue	= IniGetValue(pszSection, pszKey);
	if(pszValue == NULL)
	{
		return false;
	}
	if(sscanf(pszValue, "%d,%d", &ptPoint.x, &ptPoint.y) >= 2)
	{
		return true;
	}
	int nSection = GetSectionIndex(pszValue);
	if(nSection < 0)
	{
		return false;
	}
	int nKey = GetKeyIndex(nSection, "value");
	if(nKey >= 0)
	{
		const char* pszVal	= GetKeyValue(nSection, nKey);
		if(pszVal == NULL)
		{
			return false;
		}
		return sscanf(pszVal, "%d,%d", &ptPoint.x, &ptPoint.y) >= 2;
	}
	if(!ReadInt(pszValue, "x", ptPoint.x))
	{
		return false;
	}
	if(!ReadInt(pszValue, "y", ptPoint.y))
	{
		return false;
	}
	return true;
}

/*bool AWndIniFile::ReadDouble(const char* pszSection, const char* pszKey, double& fValue)
{
	const char* pszValue	= IniGetValue(pszSection, pszKey);
	if(pszValue == NULL)
	{
		return false;
	}
	char* pszRet;
	fValue	= strtod(pszValue, &pszRet);
	if(errno == ERANGE)
	{
		return false;
	}
	return true;
}*/

bool AWndIniFile::WriteInt(const char* pszSection, const char* pszKey, int nValue)
{
	char pszBuf[64] = {0};
	sprintf(pszBuf, "%d", nValue);
	return IniWriteValue(pszSection, pszKey, pszBuf, strlen(pszBuf));
}

bool AWndIniFile::WriteFloat(const char* pszSection, const char* pszKey, float fValue)
{
	char pszBuf[64] = {0};
	sprintf(pszBuf, "%f", fValue);
	return IniWriteValue(pszSection, pszKey, pszBuf, strlen(pszBuf));
}

bool AWndIniFile::WriteString(const char* pszSection, const char* pszKey, const char* pszValue)
{
	return IniWriteValue(pszSection, pszKey, pszValue, strlen(pszValue));
}

int	 AWndIniFile::GetSectionCount()
{
	return m_Sections.GetLength();
}

int  AWndIniFile::GetKeyCount(int nSection)
{
	if(nSection >= 0 && nSection < m_Sections.GetLength())
	{
		return m_Sections[nSection]->m_Keys.GetLength();
	}
	return -1;
}

int  AWndIniFile::GetKeyCount()
{
	int nSecCount	= m_Sections.GetLength();
	int nSecIndex	= 0;
	int nKeyCount	= 0;
	for(nSecIndex = 0; nSecIndex < nSecCount; nSecIndex ++)
	{
		nKeyCount += m_Sections[nSecIndex]->m_Keys.GetLength();
	}
	return nKeyCount;
}

const char* AWndIniFile::GetSectionName(int nSection)
{
	if(nSection >= 0 && nSection < m_Sections.GetLength())
	{
		return m_Sections[nSection]->m_pszName;
	}
	return NULL;
}

const char* AWndIniFile::GetKeyName(int nSection, int nKey)
{
	if(nSection >= 0 && nSection < m_Sections.GetLength())
	{
		if(nKey >= 0 && nKey < m_Sections[nSection]->m_Keys.GetLength())
		{
			return m_Sections[nSection]->m_Keys[nKey]->m_pszName;
		}
	}
	return NULL;
}

const char* AWndIniFile::GetKeyValue(int nSection, int nKey)
{
	if(nSection >= 0 && nSection < m_Sections.GetLength())
	{
		if(nKey >= 0 && nKey < m_Sections[nSection]->m_Keys.GetLength())
		{
			return m_Sections[nSection]->m_Keys[nKey]->m_pszValue;
		}
	}
	return NULL;
}

int  AWndIniFile::GetSectionIndex(const char* pszSection)
{
	int nIndex = 0;
	if(!IniFindSection(pszSection, strlen(pszSection), nIndex))
	{
		return -1;
	}
	return nIndex;
}

int  AWndIniFile::GetKeyIndex(int nSection, const char* pszKey)
{
	if(nSection >= 0 && nSection < m_Sections.GetLength())
	{
		int nIndex;
		if(IniFindKey(nSection, pszKey, strlen(pszKey), nIndex))
		{
			return nIndex;
		}
	}
	return -1;
}

#pragma comment(lib, "Rpcrt4.lib")

bool AWndIniFile::ReadGUID(char *szSection, char * key,GUID &guid)
{
	const  char* str	= NULL;
	if(!ReadString(szSection,key,str))
		return false;
	if(::UuidFromString((unsigned char *)str,&guid) == RPC_S_OK)
		return true;
	return false;
}
