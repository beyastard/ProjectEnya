/*
 * FILE: AMemFile.cpp
 *
 * DESCRIPTION: 内存文件
 *
 * CREATED BY: 杨智盈, 2002/10/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */


#include "AMemFile.h"
#include "memory.h"
#include "assert.h"

AMemFile::AMemFile(DWORD buffersize)
: m_dw_length(0)
, m_dw_position(0)
{
	m_handle_memory = ::GlobalAlloc(GMEM_MOVEABLE, buffersize);
	assert(m_handle_memory);
}

AMemFile::~AMemFile()
{
	if(m_handle_memory)
	{
		::GlobalFree(m_handle_memory);
	}
}

bool AMemFile::Write(const void * buff, int size)
{
	assert(size > 0);
	assert(buff);
	assert(m_handle_memory);

	if(m_dw_position + size >= ::GlobalSize(m_handle_memory))
	{
		// 扩充内存区
		m_handle_memory = ::GlobalReAlloc(
			m_handle_memory, 
			(((m_dw_length + size) / 1024) + 1) * 1024,
			GMEM_MOVEABLE);
		assert(m_handle_memory);
	}

	char * data = (char *)::GlobalLock(m_handle_memory);
	if(!data) return false;
	memcpy(data + m_dw_position, buff, size);
	::GlobalUnlock(m_handle_memory);

	m_dw_position += size;
	if(m_dw_position > m_dw_length)
	{
		m_dw_length = m_dw_position;
	}

	return true;
}

bool AMemFile::Read(void * buff, int size)
{
	assert(size > 0 && (unsigned int)size <= m_dw_length);
	assert(buff);
	assert(m_handle_memory);

	if(m_dw_position + size > m_dw_length)
	{
		return false;
	}

	char * data = (char *)::GlobalLock(m_handle_memory);
	if(!data) return false;
	memcpy(buff, data + m_dw_position, size);
	::GlobalUnlock(m_handle_memory);
	m_dw_position += size;
	return true;
}

bool AMemFile::IsBOF()
{
	return m_dw_position == 0;
}

bool AMemFile::IsEOF()
{
	return m_dw_position == m_dw_length;
}

DWORD AMemFile::GetLength()
{
	return m_dw_length;
}

DWORD AMemFile::GetPosition()
{
	return m_dw_position;
}

void AMemFile::SeekToBegin()
{
	m_dw_position = 0;
}

void AMemFile::SeekToEnd()
{
	m_dw_position = m_dw_length;
}

AObject * AMemFile::Dup(AObject *pSrc)
{
	assert(pSrc);
	AClassInfo * pInfo = pSrc->GetClassInfo();
	assert(pInfo);
	AObject * pObject = pInfo->GetCreateMethod()();
	assert(pObject);

	if(!DupData(pSrc, pObject))
	{
		delete pObject;
		return NULL;
	}

	return pObject;
}

bool AMemFile::DupData(AObject *pSrc, AObject *pDst)
{
	assert(pSrc);
	assert(pDst);

	AMemFile memfile;
	{
		// 保存数据
		AArchive ar(&memfile, true);
		if(!pSrc->Save(ar)) return false;
	}

	memfile.SeekToBegin();

	{
		// 装载数据
		AArchive ar(&memfile, false);
		if(!pDst->Load(ar)) return false;
	}

	return true;
}
