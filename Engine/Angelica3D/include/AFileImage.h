/*
 * FILE: AFileImage.h
 *
 * DESCRIPTION: The class which operating the file images in memory for Angelica Engine
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFILEIMAGE_H_
#define _AFILEIMAGE_H_

#include "AFPlatform.h"
#include "AFile.h"

class AFileImage : public AFile
{
private:
	LPBYTE			m_pFileImage;	// Memory pointer of the file image in memory;
	int				m_nCurPtr;		// In index into the file image buffer;
	int				m_nFileLength;	// File length;

	bool fimg_read(LPBYTE pBuffer, int nSize, int * pReadSize); // read some size of data into a buffer;
	bool fimg_read_line(char * szLineBuffer, int nMaxLength, int * pReadSize); // read a line into a buffer;
	bool fimg_seek(int nOffset, int startPos); // offset current pointer

protected:
	bool Init(char * szFullPath);
	bool Release();

public:
	AFileImage();
	~AFileImage();

	bool Open(char * szFullPath, DWORD dwFlags);
	bool Open(char * szFolderName, char * szFileName, DWORD dwFlags);
	bool ResetPointer();
	bool Close();

	bool Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength);
	bool Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength);

	bool ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);
	bool ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);

	bool WriteLine(char * szLineBuffer);
	
	DWORD GetPos();
	bool Seek(DWORD dwBytes, int iOrigin);

	inline LPBYTE GetFileBuffer() { return m_pFileImage; }
	inline int GetFileLength() { return m_nFileLength; }
};

typedef AFileImage * PAFileImage;

#endif //_AFILEIMAGE_H_
