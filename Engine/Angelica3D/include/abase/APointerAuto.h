/*
 * FILE: APointerAuto.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-8-23
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__APOINTERAUTO_H__
#define	__APOINTERAUTO_H__

template<typename POINTER_TYPE>
class APointerAutoT
{
public:
	POINTER_TYPE	m_pData;

	APointerAutoT(POINTER_TYPE pData):
	m_pData(pData)
	{
	}

	APointerAutoT():
	m_pData(NULL)
	{
	}

	~APointerAutoT()
	{
		if(m_pData != NULL)
		{
			m_pData->Release();
			delete m_pData;
		}
	}

	POINTER_TYPE operator = (POINTER_TYPE pData)
	{
		if(m_pData != NULL)
		{
			delete m_pData;
		}
		m_pData	= pData;
		return pData;
	}

	bool operator == (POINTER_TYPE pData)
	{
		return m_pData == pData;
	}

	bool operator != (POINTER_TYPE pData)
	{
		return m_pData != pData;
	}

	operator POINTER_TYPE ()
	{
		return m_pData;
	}

	POINTER_TYPE operator -> ()
	{
		return m_pData;
	}
};

#endif	// __APOINTERAUTO_H__