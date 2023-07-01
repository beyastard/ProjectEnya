/*
 * FILE: AData.h
 *
 * DESCRIPTION: A class which is the base of all the data objects;
 *
 * CREATED BY: JiangLi, 2002/08/08
 *             杨智盈 2002/8/20 properies manager & analyze
 *
 * HISTORY: 
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.	
 */

#ifndef	__ADATA_H__
#define	__ADATA_H__

#include "AIDObject.h"
#include "AProperty.h"
#include "AUString.h"
#include "AUList.h"
#include "stddef.h"

// AObject -> AIDObject -> class AData;

// 初始化静态属性集合（仅限于在构造函数中出现）
#define INITIALIZE_STATICPROPERIES(classname) 

// 定义静态属性集合起始点，参数：类名，基类名
#define STATICPROPERIES_BEGIN(base, classname) \
	virtual int ProperiesCount() \
	{ \
		int count = 0;\
		classname::Properies(&count, NULL, 0);\
		return base::ProperiesCount() + count;\
	}\
	virtual AProperty * Properies(int index)\
	{\
		AProperty * pProp = base::Properies(index);\
		if(pProp) return pProp;\
		index -= base::ProperiesCount();\
		return classname::Properies(NULL, NULL, index);\
	}\
	virtual AProperty * Properies(AUString name)\
	{\
		AProperty * pProp = base::Properies(name);\
		if(pProp) return pProp;\
		return classname::Properies(NULL, name, 0);\
	}\
	struct _staticproperies\
	{\
		AProperty * properies(int * pCount, const char * pName, int index)\
		{\
			static AProperty * pProps[] = \
			{

// 定义静态属性终结点，参数：类名
#define STATICPROPERIES_END(classname) \
			};\
			static int count = sizeof(pProps) / sizeof(AProperty *);\
			if(pCount) {*pCount = count;return NULL;}\
			if(pName)\
			{\
				for(int a = 0;a < count;a ++)\
					if(strcmp(pProps[a]->GetName(), pName) == 0) \
						return pProps[a];\
				return NULL;\
			}\
			if(index >= 0 && index < count) return pProps[index];\
			return NULL;\
		}\
		~_staticproperies()\
		{\
			int count = 0;\
			properies(&count, NULL, 0);\
			for(int a = 0;a < count;a ++)\
			{\
				AProperty * pProp = properies(NULL, NULL, a);\
				delete pProp;\
			}\
		}\
	};\
	friend struct _staticproperies;\
	static AProperty * Properies(int * pCount, const char * pName, int index)\
	{\
		static classname::_staticproperies properies;\
		return properies.properies(pCount, pName, index);\
	}

// 定义属性项目，参数：属性名，变量类型，变量指针，属性值集合，属性值范围，属性的处理方式，文件名的搜寻位置（如果属性值为文件链接的话）
#define STATICPROPERTY(classname, name, prop_type, prop_data, prop_set, prop_range, prop_way, prop_path) \
			new APropertyTemplate<prop_type>(name, offsetof(classname, prop_data), prop_set, prop_range, prop_way, prop_path),


/*
  典型的属性定义有三种：

  简单类型
  STATICPROPERTY("property", int, m_int_value, NULL, NULL, WAY_DEFAULT, NULL)

  简单类型但指定处理模式
  STATICPROPERTY("property", A3DCOLOR, m_a3c_value, NULL, NULL, WAY_COLOR, NULL)

  文件链接
  STATICPROPERTY("property", AUString, m_str_skytexture, NULL, NULL, WAY_FILENAME, "texture\\sky")

  关于处理方式的定义，如下：
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
*/

class AData : public AIDObject
{
public:

	DECLARE_ASERIAL(AData)

public:

	// collection resources;
	virtual void CollectionResourceRec(AListT<AUString> & resources);

	void SetName(AUString name);
	AUString GetName();

	virtual bool Save(AArchive &ar);
	virtual bool Load(AArchive &ar);

	// 属性个数
	int GetPropertyCount()
	{
		return ProperiesCount();
	}

	// 查找属性
	AProperty * GetProperty(int index)			// 使用序号获得属性
	{
		return Properies(index);
	}

	AProperty * GetProperty(AUString name)		// 使用名称获得属性
	{
		return Properies(name);
	}

	AData();
	virtual ~AData();

protected:

	AUString				m_str_name;			// 对象名称

	virtual int ProperiesCount()
	{
		int count = 0;
		AData::Properies(&count, NULL, 0);
		return /* AObject::ProperiesCount() + */ count;
	}

	virtual AProperty * Properies(int index)
	{
		return AData::Properies(NULL, NULL, index);
	}

	virtual AProperty * Properies(AUString name)
	{
		return AData::Properies(NULL, name, 0);
	}

	struct _staticproperies
	{
		AProperty * properies(int * pCount, const char * pName, int index)
		{
			static AProperty * pProps[] = 
			{
				new APropertyTemplate<AUString>("名称", offsetof(AData, m_str_name), NULL, NULL, WAY_DEFAULT, NULL),
			};

			static int count = sizeof(pProps) / sizeof(AProperty *);
			if(pCount) {*pCount = count;return NULL;}
			if(pName)
			{
				for(int a = 0;a < count;a ++)
					if(strcmp(pProps[a]->GetName(), pName) == 0) 
						return pProps[a];
				return NULL;
			}
			if(index >= 0 && index < count) return pProps[index];
			return NULL;
		}

		~_staticproperies()
		{
			int count = 0;
			properies(&count, NULL, 0);
			for(int a = 0;a < count;a ++)
			{
				AProperty * pProp = properies(NULL, NULL, a);
				delete pProp;
			}
		}
	};
	friend struct _staticproperies;
	static AProperty * Properies(int * pCount, const char * pName, int index)
	{
		static AData::_staticproperies properies;
		return properies.properies(pCount, pName, index);
	}
};

#endif