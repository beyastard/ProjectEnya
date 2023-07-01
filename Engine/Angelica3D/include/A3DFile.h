/*
 * FILE: A3DFile.h
 *
 * DESCRIPTION: The class which operating the files in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DFILE_H_
#define _A3DFILE_H_

#include "A3DPlatform.h"
#include "A3DData.h"

#define A3DFILE_TYPE_BINARY				0x42584f4d
#define A3DFILE_TYPE_TEXT				0x54584f4d

#define A3DFILE_OPENEXIST				0x00000001
#define A3DFILE_CREATENEW				0x00000002
#define A3DFILE_OPENAPPEND				0x00000004
#define A3DFILE_TEXT					0x00000008
#define A3DFILE_BINARY					0x00000010

#define A3DFILE_LINEMAXLEN				2048

#define A3DFILE_SEEK_SET				SEEK_SET
#define A3DFILE_SEEK_CUR				SEEK_CUR
#define A3DFILE_SEEK_END				SEEK_END

class A3DFile : public A3DData
{
private:
	FILE *	m_pFile;

protected:
	char	m_szFileName[MAX_PATH];
	DWORD	m_dwFlags;

public:
	A3DFile();
	~A3DFile();
	virtual bool Open(char * szFileName, DWORD dwFlags);
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
	inline DWORD IsText() { return m_dwFlags & A3DFILE_TEXT; }
	inline char * GetFileName() { return m_szFileName; }
};

typedef A3DFile * PA3DFile;
#endif