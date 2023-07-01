/*
 * FILE: AProperty.h
 *
 * DESCRIPTION: 
 *	实现属性定义所需的类和接口定义
 *		ARange;
 *		ASet;
 *		AProperty;
 *		ARangeTemplate;
 *		ASetTemplate;
 *		ASetElement;
 *		APropertyTemplate;
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */


#ifndef _APROPERTY_H__
#define _APROPERTY_H__
#pragma warning ( disable : 4786 )
#include <assert.h>
#include "AVariant.h"
#include "AUString.h"
#include "stdarg.h"

// 为实现代码简化而定义宏
#define EXTERN_DEFINESET(type, name) extern ASetTemplate<type> name;
#define	DEFINE_SETBEGIN(type, name) ASetTemplate<type> name(
#define DEFINE_SETELEMENT(type, name, value) ASetTemplate<type>::ASetElement<type>(name, value),
#define DEFINE_SETEND(type) ASetTemplate<type>::ASetElement<type>());

// 编辑器对待属性的推荐方式
enum 
{
	WAY_DEFAULT,		// 缺省方式，由编辑器自动检测AVariant的类型并处理
	WAY_BOOLEAN,		// 布尔量处理方式
	WAY_INTEGER,		// 整型处理方式
	WAY_FLOAT,			// 浮点处理方式
	WAY_STRING,			// 字串处理方式
	WAY_FILENAME,		// 文件名
	WAY_COLOR,			// 颜色值
	WAY_VECTOR,			// 向量值
	WAY_BINARY,			// 二进制块
	WAY_OBJECT,			// 对象
	WAY_UNITID,			// 对象ID
	WAY_PATHID,			// 路径ID
	WAY_STRID,			// (文档内)字符串资源ID
	WAY_SFXID,			// (文档内)音效资源ID
	// 处理方式说明
	WAY_READONLY	= (1 << 31),// 属性只读访问
};

/////ARange接口
class ARange
{
public:

	virtual AVariant GetMaxValue() = 0;
	virtual AVariant GetMinValue() = 0;
	virtual bool IsInRange(AVariant value) = 0;
};

/////ASet接口
class ASet
{
public:

	virtual int GetCount() = 0;
	virtual AUString GetNameByIndex(int index) = 0;
	virtual AVariant GetValueByIndex(int index) = 0;
	virtual AVariant GetValueByName(AUString szName) = 0;
	virtual AUString GetNameByValue(AVariant value) = 0;
	virtual int FindValue(AVariant value) = 0;
	virtual int FindName(AUString szName) = 0;

};

/////AProperty接口
class AProperty
{
public:

	virtual const char * GetName() = 0;
	virtual const char * GetPath() = 0;
	virtual ASet * GetPropertySet() = 0;
	virtual ARange * GetPropertyRange() = 0;
	virtual int GetWay() = 0;
	virtual void SetWay(int way) = 0;
	virtual AVariant GetValue(void * pBase) = 0;
	virtual void SetValue(void * pBase, AVariant value) = 0;
	virtual ~AProperty() {};
};

/////ARangeTemplate类
template <class TYPE> class ARangeTemplate : public ARange
{
public:

	ARangeTemplate(TYPE min, TYPE max) :m_min(min), m_max(max) {}
	AVariant GetMaxValue() {return AVariant(m_max);}
	AVariant GetMinValue() {return AVariant(m_min);}

	bool IsInRange(AVariant value)
	{
		if(TYPE(value) < m_min && TYPE(value) > m_max)
		{
			return false;
		}

		return true;
	}

private:
	
	TYPE m_min;
	TYPE m_max;
};

/////ASetTemplate类
template <class TYPE> class ASetTemplate : public ASet
{
public:

	// 内建元素类
	template <class TYPE> class ASetElement
	{
	public:

		ASetElement()
		{
			m_szName.Empty();
		}

		ASetElement(AUString szName, TYPE data)
		{
			m_szName	= szName;
			m_data		= data;
		}

		ASetElement(ASetElement & e)
		{
			m_szName	= e.m_szName;
			m_data		= e.m_data;
		}

		AUString GetName() {return m_szName;}
		TYPE GetData() {return m_data;}

	private:

		AUString m_szName;
		TYPE		 m_data;
	};

protected:

	int					m_iCount;
	ASetElement<TYPE> * m_pElements;

public:

	// 构造函数
	ASetTemplate(ASetElement<TYPE> first, ...)
	{
		va_list elements, elements2;
		int iCount = 0;
		ASetElement<TYPE> element = first;
		va_start(elements, first);
		while(!element.GetName().IsEmpty())
		{
			iCount ++;
			element = va_arg(elements, ASetElement<TYPE>);
		}
		va_end(elements);

		assert(iCount > 0);
		m_pElements = new ASetElement<TYPE>[iCount];
		assert(m_pElements);

		int i = 0;
		element = first;
		va_start(elements2, first);
		while(!element.GetName().IsEmpty())
		{
			m_pElements[i] = element;
			element = va_arg(elements2, ASetElement<TYPE>);
			i ++;
		}
		va_end(elements2);
		m_iCount = iCount;
	}

	~ASetTemplate()
	{
		if(m_pElements)
		{
			delete []m_pElements;
		}
	}

	//元素个数
	int GetCount()
	{
		return m_iCount;
	}

	// 根据索引获取名称
	AUString GetNameByIndex(int index)
	{
		assert(m_pElements);
		assert(index >= 0 && index < m_iCount);
		return m_pElements[index].GetName();
	}

	// 根据索引获取值
	AVariant GetValueByIndex(int index)
	{
		assert(m_pElements);
		assert(index >= 0 && index < m_iCount);
		return AVariant(m_pElements[index].GetData());
	}

	// 根据名称查值
	AVariant GetValueByName(AUString szName)
	{
		assert(m_pElements);
		assert(szName);

		for(int index = 0;index < m_iCount;index ++)
		{
			if(strcmp(szName, m_pElements[index].GetName()) == 0)
			{
				return AVariant(m_pElements[index].GetData());
			}
		}

		return AVariant();	//未找到元素的情况下，返回一个空量，其类型为AVT_INVALIDTYPE。
	}

	// 根据值查名称
	AUString GetNameByValue(AVariant value)
	{
		assert(m_pElements);

		for(int index = 0;index < m_iCount;index ++)
		{
			if(m_pElements[index].GetData() == (TYPE)value)
			{
				return m_pElements[index].GetName();
			}
		}

		return AUString();
	}

	// 查值索引
	int FindValue(AVariant value)
	{
		assert(m_pElements);
		for(int index = 0;index < m_iCount;index ++)
		{
			if(m_pElements[index].GetData() == (TYPE)value)
			{
				return index;
			}
		}

		return -1;
	}

	// 查名称索引
	int FindName(AUString szName)
	{
		assert(m_pElements);
		for(int index = 0;index < m_iCount;index ++)
		{
			if(m_pElements[index].GetName() == szName)
			{
				return index;
			}
		}

		return -1;
	}
};

template <class TYPE> class APropertyTemplate : public AProperty  
{
public:
	
	APropertyTemplate(
		const char * szName,			// 属性名
		int iOffset,					// 数据区偏移量
		ASet * pSet = NULL,				// 集合
		ARange * pRange = NULL,			// 范围
		int way = WAY_DEFAULT,			// 属性的处理方式
		const char * szPath = NULL)		// 路径（文件链接的搜寻位置，为NULL时限制在执行程序开始的路径以下）
	{
		assert(szName);
		m_szName	= szName;
		m_iOffset	= iOffset;
		m_pRange	= pRange;
		m_pSet		= pSet;
		m_iWay		= way;
		if(szPath) m_szPath = szPath;
	}

	AVariant GetValue(void * pBase)
	{
		assert(pBase);
		char * pData = (char *)pBase;
		return AVariant(*(TYPE*)(pData + m_iOffset));
	}

	void SetValue(void * pBase, AVariant value)
	{
		assert(pBase);
		char * pData = (char *)pBase;
		*(TYPE*)(pData + m_iOffset) = (TYPE)value;
	}

	const char * GetName()
	{
		return m_szName;
	}

	const char * GetPath()
	{
		return m_szPath;
	}

	ASet * GetPropertySet()
	{
		return m_pSet;
	}

	ARange * GetPropertyRange()
	{
		return m_pRange;
	}

	int GetWay()
	{
		return m_iWay;
	}

	void SetWay(int way)
	{
		m_iWay = way;
	}

private:

	AUString			m_szName;		// 属性名
	AUString			m_szPath;		// 路径(当属性为文件链接时，限制文件的搜寻 位置)
	int					m_iOffset;		// 数据偏移量
	int					m_iWay;			// 属性的处理方式
	ARange *			m_pRange;		// 属性值的范围
	ASet *				m_pSet;			// 属性值的集合
};
#endif
