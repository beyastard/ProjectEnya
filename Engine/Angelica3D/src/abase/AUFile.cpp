#include "AUFile.h"
#include "ADebug.h"
#include "AMemLeak.h"

// class AUFile

AUFile::AUFile()
{
	m_hFile		= INVALID_HANDLE_VALUE;
}

AUFile::~AUFile()
{
	Close();
}

void AUFile::Close()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile	= INVALID_HANDLE_VALUE;
	}
}

bool AUFile::Create(const char* pszPathName, DWORD dwCreationDisposition)
{
	Close();

	if(::SetFileAttributes(pszPathName,FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_NORMAL)==FALSE)
	{
		ADebug::Msg(0,"Change Attribute Error <%s>",pszPathName);
		return false;
	}
	m_hFile	= CreateFile(
		pszPathName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	return true;
}

bool AUFile::Write(const void * buff, int size)
{
	AASSERT(m_hFile != INVALID_HANDLE_VALUE);

	if(size <= 0)
	{
		return true;
	}

	AASSERT(buff != NULL);

	DWORD dwReadLen	= 0;
	if(!WriteFile(m_hFile, (LPCVOID)buff, size, &dwReadLen, NULL))
	{
		return FALSE;
	}
	return true;
}

bool AUFile::Read(void * buff, int size)
{
	AASSERT(m_hFile != INVALID_HANDLE_VALUE);

	if(size <= 0)
	{
		return true;
	}

	AASSERT(buff != NULL);

	DWORD	dwReadLen	= 0;
	if(!ReadFile(m_hFile, buff, size, &dwReadLen, NULL))
	{
		size	= 0;
		return FALSE;
	}
	size	= (int)dwReadLen;

	return true;
}

bool AUFile::SetPointer(int nDistanceToMove, DWORD dwMoveMethod)
{
	if(-1 == SetFilePointer(
		m_hFile,                // handle to file
		nDistanceToMove,        // bytes to move pointer
		NULL,					// bytes to move pointer
		dwMoveMethod            // starting point
		))
	{
		return false;
	}
	return true;
}

int  AUFile::GetSize()
{
	return GetFileSize(m_hFile, NULL);
}