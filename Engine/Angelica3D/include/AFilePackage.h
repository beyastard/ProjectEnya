/*
 * FILE: AFilePackage.h
 *
 * DESCRIPTION: A File Package Class for Angelica Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
   2002/5/30 --- Add compression ability
				struct _AFPCK_FILEENTRY add the 4th element: dwOriginLength
				RemoveFile, ReplaceFile added
				version upgraded to 1.2
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFILEPACKAGE_H_
#define _AFILEPACKAGE_H_

#include "AFPlatform.h"

//#define AFPCK_VERSION			0x00010001
//#define AFPCK_VERSION			0x00010002 // Add compression
#define AFPCK_VERSION			0x00010003 // The final release version on June 2002

typedef struct _AFPCK_FILEENTRY
{
	char		szFileName[MAX_PATH]; // The file name of this entry; this may contain a path;
	DWORD		dwOffset; // The offset from the beginning of the package file;
	DWORD		dwLength; // The length of this file;
	DWORD		dwCompressedLength; // The compressed data length;

} AFPCK_FILEENTRY, * PAFPCK_FILEENTRY;

typedef struct _AFPCK_FILEHEADER
{
	DWORD		dwVersion; // Composed by two word version, major part and minor part;
	DWORD		dwEntryOffset; // The entry list offset from the beginning;
	char		szDescription[256];
} AFPCK_FILEHEADER, * PAFPCK_FILEHEADER;

enum AFPCK_OPENMODE
{
	AFPCK_OPENEXIST = 0,
	AFPCK_CREATENEW = 1
};

class AFilePackage
{
private:
	bool				m_bHasChanged;
	bool				m_bReadOnly;

	AFPCK_FILEHEADER	m_header;
	AFPCK_OPENMODE		m_mode;

	int					m_nNumFiles;
	AFPCK_FILEENTRY *	m_pFileEntries;

	FILE *				m_fpPackageFile;

	LPBYTE				m_pBuffer;		// A buffer for compression and uncompression;
	DWORD				m_dwBufferLen;	// The length of the buffer;

	bool				m_bHasSorted;	// Flag indicating whether the file entry has been sorted;

	// Prepare a compression usage buffer;
	bool PrepareBuffer(DWORD dwBufferLen);

protected:
public:
	AFilePackage();
	~AFilePackage();

	bool Open(char * szPckPath, AFPCK_OPENMODE mode);
	bool Close();

	/*
		Append a file into the package
		parameter:
			IN: szFileName		file name
			IN: pFileBuffer		the buffer containing file content
			IN: dwFileLength	the length of the buffer
	*/
	bool AppendFile(char * szFileName, LPBYTE pFileBuffer, DWORD dwFileLength);

	/*
		Remove a file from the package, we will only remove the file entry from the package;
		the file's data will remain in the package
		parameter:
			IN: szFileName		file name
	*/
	bool RemoveFile(char * szFileName);

	/*
		Replace a file in the package, we will only replace the file entry in the package;
		the old file's data will remain in the package
		parameter:
			IN: szFileName		file name
	*/
	bool ReplaceFile(char * szFileName, LPBYTE pFileBuffer, DWORD dwFileLength);

	/*
		Read the file's content from the package
		parameter: 
			IN: szFileName			file name
			IN: pFileBuffer			buffer to contain the file's content;
			IN: dwOffset			offset of the file to read start;
			IN/OUT: pdwBufferLen	in: the max buffer size of pFileBuffer, out: the actually filled length of pFileBuffer;

	*/

	// Sort the file entry list;
	bool ResortEntries();

	bool ReadFile(char * szFileName, LPBYTE pFileBuffer, DWORD dwOffset, DWORD * pdwBufferLen);
	bool ReadFile(AFPCK_FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD dwOffset, DWORD * pdwBufferLen);

	// Find a file entry;
	// return true if found, false if not found;
	bool GetFileEntry(char * szFileName, AFPCK_FILEENTRY * pFileEntry, int * pnIndex=NULL);
	bool GetFileEntryByIndex(int nIndex, AFPCK_FILEENTRY * pFileEntry);

	inline int GetFileNumber() { return m_nNumFiles; }
	inline AFPCK_FILEHEADER GetFileHeader() { return m_header; }
};

typedef class AFilePackage * PAFilePackage;

bool OpenFilePackage(char * szPackFile);
bool CloseFilePackage();

extern AFilePackage *	g_pAFilePackage;
#endif//AFILEPACKAGE_H_
