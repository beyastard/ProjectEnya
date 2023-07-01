/*
 * FILE: AFile.h
 *
 * DESCRIPTION: The class which operating the files in Angelica Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFILE_H_
#define _AFILE_H_

#include "AFPlatform.h"

#define AFILE_TYPE_BINARY			0x42584f4d
#define AFILE_TYPE_TEXT				0x54584f4d

#define AFILE_OPENEXIST				0x00000001
#define AFILE_CREATENEW				0x00000002
#define AFILE_OPENAPPEND			0x00000004
#define AFILE_TEXT					0x00000008
#define AFILE_BINARY				0x00000010

#define AFILE_LINEMAXLEN			2048

#define AFILE_SEEK_SET				SEEK_SET
#define AFILE_SEEK_CUR				SEEK_CUR
#define AFILE_SEEK_END				SEEK_END

class AFile
{
private:
	FILE *	m_pFile;

protected:
	// An fullpath file name;
	char	m_szFileName[MAX_PATH];

	// An relative file name that relative to the work dir;
	char	m_szRelativeName[MAX_PATH];

	DWORD	m_dwFlags;

	bool	m_bHasOpened;

public:
	AFile();
	virtual ~AFile();

	virtual bool Open(char * szFullPath, DWORD dwFlags);
	virtual bool Open(char * szFolderName, char * szFileName, DWORD dwFlags);
	virtual bool ResetPointer(); // Reset the file pointer;
	virtual bool Close();

	virtual bool Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength);
	virtual bool Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength);
	virtual bool ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);
	virtual bool ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);
	virtual bool WriteLine(char * szLineBuffer);
	virtual bool GetStringAfter(char * szBuffer, char * szTag, char * szResult);
	virtual DWORD GetPos();
	virtual bool Seek(DWORD dwBytes, int iOrigin);

	inline DWORD GetFlags() { return m_dwFlags; }
	//Binary first, so if there is no binary or text, it is a binary file;
	inline DWORD IsBinary() { return !IsText(); }
	inline DWORD IsText() { return m_dwFlags & AFILE_TEXT; }

	inline char * GetFileName() { return m_szFileName; }
	inline char * GetRelativeName() { return m_szRelativeName; }
};

typedef AFile * PAFile;
#endif