/*
 * FILE: AFileImage.cpp
 *
 * DESCRIPTION: The class which operating the file images in memory for Angelica Engine
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AFPI.h"
#include "AFileImage.h"
#include "AFilePackage.h"
#include "AFI.h"

AFileImage::AFileImage() : AFile()
{
	m_pFileImage	= NULL;
	m_nCurPtr		= 0;
	m_nFileLength	= NULL;
}

AFileImage::~AFileImage()
{
	Close();
}

bool AFileImage::Init(char * szFullName)
{
	strncpy(m_szFileName, szFullName, MAX_PATH);
	AFileMod_GetRelativePath(szFullName, m_szRelativeName);
	
	// If g_pAFilePackage is set, this means that we are using a file package, 
	// instead of seperate files;
	if( !g_pAFilePackage )
	{
		FILE * pFile;

		pFile = fopen(szFullName, "rb");
		if( NULL == pFile )
		{
			AFERRLOG(("AFileImage::Init Can't open file [%s] to create image in memory", szFullName));		
			return false;
		}

		fseek(pFile, 0, SEEK_END);
		m_nFileLength = ftell(pFile);
		if( 0 == m_nFileLength )
		{
			AFERRLOG(("AFileImage::Init The file [%s] is zero length!"));
			return false;
		}

		fseek(pFile, 0, SEEK_SET);
		m_pFileImage = (LPBYTE) malloc(m_nFileLength * sizeof(BYTE));
		if( NULL == m_pFileImage )
		{
			AFERRLOG(("AFileImage::Init Not enough memory!"));
			return false;
		}
		fread(m_pFileImage, m_nFileLength, 1, pFile);

		fclose(pFile);
	}
	else
	{
		// Init from a package;
		AFPCK_FILEENTRY fileEntry;
		if( !g_pAFilePackage->GetFileEntry(m_szRelativeName, &fileEntry) )
		{
			AFERRLOG(("AFileImage::Can not locate file [%s] in the package!", m_szRelativeName));
			return false;
		}
		m_nFileLength = fileEntry.dwLength;
		m_pFileImage = (LPBYTE) malloc(m_nFileLength * sizeof(BYTE));
		if( NULL == m_pFileImage )
		{
			AFERRLOG(("AFileImage::Init(), Not enough memory!"));
			return false;
		}
		if( !g_pAFilePackage->ReadFile(fileEntry, m_pFileImage, 0, (DWORD *)&m_nFileLength) )
		{
			AFERRLOG(("AFileImage::Init(), Error Reading file [%s] from package!", m_szRelativeName));
			return false;
		}
	}

	return true;
}

bool AFileImage::Release()
{
	if( m_pFileImage )
	{
		free(m_pFileImage);
		m_pFileImage = NULL;
	}

	return true;
}

bool AFileImage::Open(char * szFolderName, char * szFileName, DWORD dwFlags)
{
	char		szFullPath[MAX_PATH];

	AFileMod_GetFullPath(szFullPath, szFolderName, szFileName);

	return Open(szFullPath, dwFlags);
}

bool AFileImage::Open(char * szFullPath, DWORD dwFlags)
{
	// If we have opened it already, we must close it;
	if( m_bHasOpened )
		Close();
	
	if( !Init(szFullPath) )
	{
		AFERRLOG(("AFileImage::Open(), Can not init the file image!"));
		return false;
	}

	if( dwFlags & AFILE_OPENEXIST )
	{
	}
	else
	{
		AFERRLOG(("AFileImage::Open Current we only support read flag to operate a file image"));
		return false;
	}

	DWORD dwFOURCC;
	int   nRead;

	m_dwFlags = dwFlags;
	m_dwFlags = dwFlags & (~(AFILE_BINARY | AFILE_TEXT));
	fimg_read((LPBYTE)&dwFOURCC, 4, &nRead);

	if( dwFOURCC == 0x42584f4d )
		m_dwFlags |= AFILE_BINARY;
	else if( dwFOURCC == 0x54584f4d )
		m_dwFlags |= AFILE_TEXT;
	else
	{
		//Default we use text mode, for we can edit it by hand, and we will not add 
		//the shitting FOURCC at the beginning of the file
		m_dwFlags |= AFILE_TEXT;
		fimg_seek(0, AFILE_SEEK_SET);
	}
	
	m_bHasOpened = true;
	return true;
}

bool AFileImage::Close()
{
	m_nCurPtr = 0;
	m_szFileName[0] = '\0';

	Release();
	return true;
}

bool AFileImage::ResetPointer()
{
	fimg_seek(0, AFILE_SEEK_SET);
	return true;
}

bool AFileImage::fimg_read(LPBYTE pBuffer, int nSize, int * pReadSize)
{
	*pReadSize = 0;
	int nSizeToRead = nSize;
	
	if( m_nCurPtr + nSizeToRead > m_nFileLength )
		nSizeToRead = m_nFileLength - m_nCurPtr;
	
	if( nSizeToRead <= 0 )
		return true;

	memcpy(pBuffer, m_pFileImage + m_nCurPtr, nSizeToRead);
	m_nCurPtr += nSizeToRead;
	*pReadSize = nSizeToRead;
	return true;
}

bool AFileImage::fimg_read_line(char * szLineBuffer, int nMaxLength, int * pReadSize)
{
	int nSizeRead = 0;

	ZeroMemory(szLineBuffer, nMaxLength);
	while( m_nCurPtr < m_nFileLength )
	{
		BYTE byteThis = m_pFileImage[m_nCurPtr];

		if( byteThis != 0x0d && byteThis != 0x0a )
		{
			// Not \n or \r, so copy it into the buffer;
			szLineBuffer[nSizeRead ++] = m_pFileImage[m_nCurPtr ++];
		}
		else
		{
			// We also need to copy \n into the buffer;
			szLineBuffer[nSizeRead ++] = m_pFileImage[m_nCurPtr ++];
			szLineBuffer[nSizeRead] = '\0';
			if( byteThis == 0x0d )
			{
				// We need to check if next byte is \r, if so, just remove it;
				if( m_nCurPtr < m_nFileLength )
				{
					if( m_pFileImage[m_nCurPtr] == 0x0a )
					{
						m_nCurPtr ++;
						nSizeRead ++;
					}
				}
			}
			
			break;
		}
	}

	*pReadSize = nSizeRead;

	if( nSizeRead <= 0 )
		return false;
	return true;
}

bool AFileImage::fimg_seek(int nOffset, int startPos)
{
	switch(startPos)
	{
	case AFILE_SEEK_SET:
		m_nCurPtr = nOffset;
		break;
	case AFILE_SEEK_CUR:
		m_nCurPtr += nOffset;
		break;
	case AFILE_SEEK_END:
		m_nCurPtr = m_nFileLength + nOffset;
		break;
	default:
		return false;
	}
	if( m_nCurPtr < 0 )
		m_nCurPtr = 0;
	else if( m_nCurPtr > m_nFileLength ) // To be compatible with fseek, we have to let the file pointer beyond the last character;
		m_nCurPtr = m_nFileLength;
	return true;
}

bool AFileImage::ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	int nReadSize;

	if( !fimg_read_line(szLineBuffer, dwBufferLength, &nReadSize) )
		return false;

	//chop the \n\r
	if( szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r' )
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	if( szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r' )
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	*pdwReadLength = strlen(szLineBuffer) + 1;
	return true;
}

bool AFileImage::Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength)
{
	int nReadSize;
	fimg_read((LPBYTE)pBuffer, dwBufferLength, &nReadSize);

	*pReadLength = nReadSize;
	return true;
}

bool AFileImage::Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength)
{
	return false;
}

DWORD AFileImage::GetPos()
{
	return (DWORD) m_nCurPtr;
}

bool AFileImage::Seek(DWORD dwBytes, int iOrigin)
{
	return fimg_seek(dwBytes, iOrigin);
}

bool AFileImage::WriteLine(char * szLineBuffer)
{
	return false;
}

bool AFileImage::ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	char ch;
	DWORD dwReadLength;
	DWORD nStrLen = 0;

	Read(&ch, 1, &dwReadLength);
	while( ch )
	{
		szLineBuffer[nStrLen] = ch;
		nStrLen ++;

		if( nStrLen >= dwBufferLength )
			return false;

		Read(&ch, 1, &dwReadLength);
	}

	szLineBuffer[nStrLen] = '\0';

	*pdwReadLength = nStrLen + 1;
	return false;
}
