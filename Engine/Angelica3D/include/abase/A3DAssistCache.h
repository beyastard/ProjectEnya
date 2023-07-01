// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistCache.h
// Creator: Wei Hua (魏华)
// 通用缓存

#ifndef	__A3DASSISTCACHE_H__
#define	__A3DASSISTCACHE_H__

#include "APlatform.h"
#include "AStringArray.h"
#include "ABinString.h"
#include <hashtab.h>

class	A3DASSISTCACHEUNIT
{
public:
	bool	m_bTaken;						// 该单元已被占用
	int		m_nUseCount;					// 总共使用的次数
	int		m_nLastUsedTick;				// 追后一次使用的时刻
	void	*m_pExtra;						// 附加数据
	int		m_nProperty;					// 属性数据
	inline int		Cmp(A3DASSISTCACHEUNIT &Unit)
	{
		int	rst;
		rst	= m_nLastUsedTick - Unit.m_nLastUsedTick;
		if( rst == 0 )
		{
			return	m_nUseCount - Unit.m_nUseCount;
		}
		return	rst;
	}
};

class	A3DAssistCache
{
protected:
	int		m_nTotal;
	A3DASSISTCACHEUNIT	*m_pUnits;
protected:
	virtual bool	StillInUse(int nIdx)		{return false;}	// 由子类实现表示这个单元是否还在使用中
public:
	static unsigned int	m_nCurTick;								// 可以由外界填写
public:
	A3DAssistCache();
	virtual	~A3DAssistCache();
	bool	Init(int nTotal);
	bool	Release();
	void	SetLastUsedTick(int nIdx);
	void	SetLastUsedTick(int nIdx, int nTick);
	void	Reset(int nIdx);
	void	ResetAll();
	int		GetMEUU(bool bForce = true);	// 获得一个最不长使用的单元索引(Most Earlist Unused Unit)
											// 或者空闲没有被使用的单元索引
};

// 对字符串相关资源的缓冲
////////////////////////////////////////////////////////////////////
// A3DAssistCache_StrNoDup
////////////////////////////////////////////////////////////////////
template <class _HashStrTab, class _StrArray>
class	A3DAssistCache_StrNoDup	: public A3DAssistCache
{
protected:
	_HashStrTab				* m_pTab;
	abase::pair<int *,bool>	m_srst;
	_StrArray				m_asa;
public:
	A3DAssistCache_StrNoDup() : m_srst(NULL, false)
	{
		m_pTab	= NULL;
	}
	~A3DAssistCache_StrNoDup()
	{
	}
	bool	Init(int nTotal, int nStrLen)
	{
		bool	bval;

		if( !m_asa.Init(nTotal, nStrLen) )
		{
			return	false;
		}
		
		bval	= A3DAssistCache::Init(nTotal);
		if( !bval )
		{
			return	bval;
		}
		m_pTab	= new _HashStrTab(nTotal);
		if( !m_pTab )
		{
			return	false;
		}

		return	true;
	}
	bool	Release()
	{
		m_asa.Release();

		if( m_pTab )
		{
			delete	m_pTab;
			m_pTab	= NULL;
		}
		A3DAssistCache::Release();

		return	true;
	}
	void	Reset(int nIdx)
	{
		bool	bval;
		if( m_pUnits[nIdx].m_pExtra )
		{
			bval	= m_pTab->erase((char *)m_pUnits[nIdx].m_pExtra);
		}
		A3DAssistCache::Reset(nIdx);
	}
	void	ResetAll()
	{
		m_pTab->clear();
		A3DAssistCache::ResetAll();
	}
	// 返回pszStr对应的索引，如果没有返回-1。然后上层调用GetMEUU得到一个可以使用的索引号
	int		SearchString(char *pszStr)
	{
		m_srst	= m_pTab->get(pszStr);
		if( m_srst.second )
		{
			return	*m_srst.first;
		}
		return	-1;
	}
	// 如果原来nIdx中有东西就要清除它相关的所有东西(如：hash中的字串)
	int		RegisterString(int nIdx, char *pszStr)
	{
		if( nIdx<0 )
		{
			return	-1;
		}
		if( m_pUnits[nIdx].m_bTaken )
		{
			Reset(nIdx);
		}
		if( !m_asa.StrCpy(nIdx, pszStr) )
		{
			return	-1;
		}
		if( !m_pTab->put(m_asa.GetUnit(nIdx), nIdx) )
		{
			return	-1;
		}
		m_pUnits[nIdx].m_pExtra	= m_asa.GetUnit(nIdx);
		m_pUnits[nIdx].m_bTaken	= true;

		return	0;
	}
};
// 普通字串
struct HASHSTR
{
	typedef const char * LPCSTR;
	const char * _reference;
	HASHSTR()						{}
	HASHSTR(const char * _ref)		{ _reference = _ref; }
	HASHSTR(char * _ref)			{ _reference = _ref; }
	inline operator LPCSTR() const	{ return _reference; }
	inline bool operator ==(const HASHSTR &rhs) const
	{ return (!strcmp(_reference, rhs)); }
};
typedef	abase::hashtab<int, HASHSTR, abase::_hash_function>	AAC_StrTab;
// 含二进制数据的字串
struct HASHBINSTR
{
	typedef const char * LPCSTR;
	const char * _reference;
	HASHBINSTR()					{}
	HASHBINSTR(const char * _ref)	{ _reference = _ref; }
	HASHBINSTR(char * _ref)			{ _reference = _ref; }
	inline operator LPCSTR() const	{ return _reference; }
	inline bool operator ==(const HASHBINSTR &rhs) const
	{
		return !BinStrCmp(_reference, rhs._reference);
	}
};
struct _binstr_hash_function
{
	_binstr_hash_function() {}
	_binstr_hash_function(const _binstr_hash_function & hf)	{}
	inline unsigned long operator()(const char *s) const
	{
		return	BinStrHash(s);
	}
};
typedef	abase::hashtab<int, HASHBINSTR, _binstr_hash_function>	AAC_BinStrTab;
// 真正的应用类
typedef		A3DAssistCache_StrNoDup<AAC_StrTab, AStringArray>		A3DAssistCache_String;
typedef		A3DAssistCache_StrNoDup<AAC_BinStrTab, ABinStringArray>	A3DAssistCache_BinString;

////////////////////////////////////////////////////////////////////
// A3DAssistCache_StrDup
////////////////////////////////////////////////////////////////////
class	A3DAssistCache_StrDup		: public A3DAssistCache
{
protected:
	AStringArray	m_asa;
protected:
	// 返回true表示已经有了被初始化的单位，*pnMEUU中返回的是单位的序号
	// 返回false表示需要利用*pnMEUU重新初始化单位再使用
	bool	SearchAndAct(char *szStr, int *pnMEUU, bool bFindOld);	// 里面调用StillInUse尽可能找到一个不用重新初始化的单位对象
	void	Reset(int nIdx);
	// 即所有都默认为只依靠MEUU决定使用顺序
	virtual bool	StillInUse(int nIdx)		{return true;}
public:
	A3DAssistCache_StrDup();
	~A3DAssistCache_StrDup();
	bool	Init(int nTotal, int nStrLen);
	bool	Release();
	bool	ResetAll();
};
#endif