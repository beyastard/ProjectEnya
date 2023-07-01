#include "ABinary.h"

IMPLEMENT_ASERIAL(ABinary, AObject, 1)

ABinary::ABinary()
{
	m_guidType	= BT_EMPTY;
	m_iLength	= 0;
	m_pData		= NULL;
}

ABinary::ABinary(GUID guidType, int iLength, const void * pInitData)
{
	assert(iLength > 0);
	m_guidType = guidType;
	m_iLength = 0;
	m_pData = NULL;

	void * buffer = GetBufferSetLength(iLength);
	if(pInitData)
	{
		memcpy(buffer, pInitData, iLength);
	}
}

ABinary::ABinary(const ABinary &bin)
{
	m_guidType = bin.m_guidType;
	m_iLength = bin.m_iLength;
	m_pData = NULL;
	if(bin.m_pData)
	{
		void * buffer = GetBufferSetLength(bin.m_iLength);
		memcpy(buffer, bin.m_pData, bin.m_iLength);
	}
	else
	{
		m_pData = NULL;
	}
}

ABinary::~ABinary()
{
	if(m_pData)
	{
		free(m_pData);
	}
}

GUID ABinary::GetType()
{
	return m_guidType;
}

void ABinary::SetType(GUID guidType)
{
	m_guidType = guidType;
}

void * ABinary::GetBufferSetLength(int length)
{
	assert(length >= 0);

	if(length > 0)		// 分配非零长度
	{
		if(!m_pData)	// 分配
		{
			m_pData = malloc(length);
		}
		else			// 重分配
		{
			m_pData = realloc(m_pData, length);
		}

		if(length > m_iLength) // 内存增长或被分配
		{
			memset((char *)m_pData + m_iLength, 0, length - m_iLength);
		}
	}
	else				// 分配零长度（释放动作）
	{
		free(m_pData);
		m_pData = NULL;
	}

	if(m_pData) m_iLength = length; else m_iLength = 0;
	return m_pData;
}

int ABinary::GetLength()
{
	return m_iLength;
}

bool ABinary::Load(AArchive &ar)
{
	if(AObject::Load(ar)==false)
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	int nVersion;
	if(ar.Read(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	//利用这个版本号进行版本控制

	ar.Read(&m_guidType, sizeof(m_guidType));
	int length = 0;
	ar.Read(&length, sizeof(length));
	if(length > 0)
	{
		void * buffer = GetBufferSetLength(length);
		ar.Read(buffer, length);
	}
	else
	{
		GetBufferSetLength(0);
	}

	return true;
}

bool ABinary::Save(AArchive &ar)
{
	if(AObject::Save(ar)==false)
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	int nVersion;
	nVersion = ABinary::GetClassInfo()->GetVersion();
	if(ar.Write(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}

	ar.Write(&m_guidType, sizeof(m_guidType));
	int length = m_iLength;
	ar.Write(&length, sizeof(length));
	if(m_pData)
	{
		ar.Write(m_pData, length);
	}

	return true;
}

const ABinary & ABinary::operator =(const ABinary &bin)
{
	m_guidType = bin.m_guidType;
	m_iLength = bin.m_iLength;
	m_pData = NULL;
	if(bin.m_pData)
	{
		void * buffer = GetBufferSetLength(bin.m_iLength);
		memcpy(buffer, bin.m_pData, bin.m_iLength);
	}
	else
	{
		m_pData = NULL;
	}
	
	return bin;
}
