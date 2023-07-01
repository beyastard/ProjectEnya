#include "AArray.h"

// template class AArrayT<char*>;
template class AArrayT<int>;

AArrayString::AArrayString()
{
}

AArrayString::~AArrayString()
{
	Release();
}

void AArrayString::Release()
{
	for(int i = m_aData.GetLength() - 1; i >= 0; i --)
	{
		if(m_aData[i] != NULL)
		{
			delete m_aData[i];
		}
	}
	m_aData.Release();
}

bool AArrayString::RemoveAt(int nIndex)
{
	if(nIndex < 0 || nIndex >= m_aData.GetLength())
	{
		return false;
	}
	if(m_aData[nIndex] != NULL)
	{
		delete m_aData[nIndex];
	}
	return RemoveAt(nIndex);
}

bool AArrayString::InsertAt(int nIndex, const char* pszStr)
{
	int nLength	= strlen(pszStr);
	char* p;
	if(nLength == 0)
	{
		p = NULL;
	}
	else
	{
		p = new char[nLength + 1];
		strncpy(p, pszStr, nLength+1);
	}
	if(!m_aData.InsertAt(nIndex, p))
	{
		if(p != NULL)
		{
			delete p;
		}
		return false;
	}
	return true;
}

bool AArrayString::AddString(const char* pszStr)
{
	return InsertAt(m_aData.GetLength(), pszStr);
}

bool AArrayString::RemoveString(const char* pszStr)
{
	int nIndex	= Find(pszStr);
	
	return RemoveAt(nIndex);
}

bool AArrayString::ReplaceString(int nIndex, const char* pszNewStr)
{
	if(nIndex < 0 || nIndex >= m_aData.GetLength())
	{
		return false;
	}
	if(m_aData[nIndex] != NULL)
	{
		delete m_aData[nIndex];
	}

	int nLength	= strlen(pszNewStr);
	char* p;
	if(nLength == 0)
	{
		p = NULL;
	}
	else
	{
		p = new char[nLength + 1];
		strncpy(p, pszNewStr, nLength+1);
	}

	m_aData[nIndex]	= p;
	return true;
}

int AArrayString::Find(const char* pszStr)
{
	int nIndex;
	
	for(nIndex = m_aData.GetLength(); nIndex >= 0; nIndex --)
	{
		if(strcmp(pszStr, m_aData[nIndex]) == 0)
		{
			break;
		}
	}

	return nIndex;
}

bool AArrayString::Move(int nFrom, int nTo)
{
	return m_aData.Move(nFrom, nTo);
}

const char* AArrayString::GetString(int nIndex)
{
	if(nIndex < 0 || nIndex >= m_aData.GetLength())
	{
		return NULL;
	}
	return m_aData[nIndex];
}