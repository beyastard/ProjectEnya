/*
 * FILE: hashtab.h
 *
 * DESCRIPTION: hash table
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/

#ifndef __ABASE_HASH_TABLE_H__
#define __ABASE_HASH_TABLE_H__
#include "vector.h"

namespace abase{
enum { __abase_num_primes = 28};	//prime list from stl :)
static const unsigned long __abase_prime_list[__abase_num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
};
inline size_t __abase_next_prime(size_t __n)
{
	for(int i = 0;i< __abase_num_primes; i++)
	{
		if(__abase_prime_list[i] > __n)
		{
			return __abase_prime_list[i];
		}
	}
	return size_t(-1);
}


template<class T, class U>
struct pair {
	typedef T first_type;
	typedef U second_type;
	T first;
	U second;
	pair();
	pair(const T& x, const U& y){ first = x;second = y;}
template<class V, class W>
	pair(const pair<V, W>& pr) 
	{
		first = pr.first;
		second = pr.second;
	}
};

template <class _Val,class _Key>
struct Hashtable_node
{
  Hashtable_node* _next;
  _Val _val;
  _Key _key;
  inline Hashtable_node(const _Key & key, const _Val & val, Hashtable_node* next):_val(val),_key(key)
  {
	_next = next;
  }
};

struct _hash_function
{
	_hash_function() {}
	_hash_function(const _hash_function & hf) {}

	inline unsigned long operator()(int data) const {return data;}
	inline unsigned long operator()(long data) const {return data;}
	inline unsigned long operator()(void *data) const {return (unsigned long)data;}
	inline unsigned long operator()(char *s) const
	{
		unsigned long h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *(unsigned char *)s;
		}
		return h;
	}
	inline unsigned long operator()(const char *s) const
	{
		unsigned long h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *(unsigned char *)s;
		}
		return h;
	}

};
/*class definition*/
template <class _Value, class _Key, class _HashFunc>
class hashtab
{
private:
typedef _Key	key_type;
typedef _Value	value_type;
typedef Hashtable_node<_Value,_Key> _Node;

private:
	_HashFunc	_hash;
	size_t		_num_elements;
	vector<_Node *> _buckets;

public:
	hashtab(size_t __n, 
		const _HashFunc & __hf)
	:_hash(__hf),
	_num_elements(0),
	_buckets(_M_next_size(__n),(_Node *) 0)
	{
	}

	hashtab(size_t __n):_hash(),
	_num_elements(0),
	_buckets(_M_next_size(__n),(_Node *) 0)
	{
	}

	~hashtab()
	{
		clear();
	}

	size_t size() const { return _num_elements; }
	size_t max_size() const { return size_t(-1); }
	bool empty() const { return size() == 0; }
	void clear();
	void resize(size_t __num_elements_hint);
	inline bool put(const key_type & __key , const value_type & __val){
		resize(_num_elements + 1);		
		return put_noresize(__key,__val);
	}
	bool put_noresize(const key_type & __key , const value_type & __val);
	pair<value_type *, bool> get(const key_type &__key) const;
	bool erase(const key_type &__key);

	template<class _EnumFunc>
	void enum_element(_EnumFunc & __func)
	{	
		for (_Node **fp = _buckets.begin();fp != _buckets.end(); fp ++) {
			_Node* __cur = *fp;
			while (__cur != NULL) {
				__func(__cur->_val);
				__cur = __cur->_next;
			}
		}
	}

	template<class _EnumFunc>
	void enum_buckets(_EnumFunc & __func)
	{	
		for (_Node **fp = _buckets.begin();fp != _buckets.end(); fp ++) {
			__func(*fp);
		}
	}

private:
	size_t _M_next_size(size_t __n) const 
	{
		return __abase_next_prime(__n);
	}

};

template <class _Value, class _Key, class _HashFunc>
bool hashtab<_Value,_Key,_HashFunc>::
	put_noresize(const key_type & __key , const value_type & __val)
{
	size_t __n = _hash(__key) % _buckets.size();
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
		if (__cur->_key == __key)
			return false;

	_Node* __tmp = new _Node(__key,__val,__first);
	_buckets[__n] = __tmp;
	++_num_elements;
	return true;
}

template <class _Value, class _Key, class _HashFunc>
void hashtab<_Value,_Key,_HashFunc>::
	resize(size_t __num_elements_hint)
{
	const size_t __old_n = _buckets.size();
	if( __num_elements_hint > __old_n)
	{
		const size_t __n = _M_next_size(__num_elements_hint);
		if(__n > __old_n)
		{
			vector<_Node*> __tmp(__n, (_Node*)(0));
			for (size_t __bucket = 0; __bucket < __old_n; ++__bucket) {
				_Node* __first = _buckets[__bucket];
				while (__first) {
					size_t __new_bucket = _hash(__first->_key) % __n;
					_buckets[__bucket] = __first->_next;
					__first->_next = __tmp[__new_bucket];
					__tmp[__new_bucket] = __first;
					__first = _buckets[__bucket];
				}
			}
			_buckets.swap(__tmp);
		}
	}
}

template <class _Value, class _Key, class _HashFunc>
void hashtab<_Value,_Key,_HashFunc>::
	clear()
{
	for (size_t __i = 0; __i < _buckets.size(); ++__i) {
		_Node* __cur = _buckets[__i];
		while (__cur != NULL) {
			_Node * __next = __cur->_next;
			delete __cur;
			__cur = __next;
		}
		_buckets[__i] = 0;
	}
	_num_elements = 0;
}

template <class _Value, class _Key, class _HashFunc>
pair<_Value *, bool> hashtab<_Value,_Key,_HashFunc>::
	get(const key_type &__key) const
{
	size_t __n = _hash(__key) % _buckets.size();
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
		if (__cur->_key == __key)
			return pair<_Value *,bool>(&(__cur->_val),true);
	return pair<_Value *,bool>((_Value *)NULL,false);
}

template <class _Value, class _Key, class _HashFunc>
bool hashtab<_Value,_Key,_HashFunc>::
	erase(const key_type &__key)
{
	size_t __n = _hash(__key) % _buckets.size();
	_Node* __first = _buckets[__n];
	_Node *__prev;

	for (_Node* __cur = __first; __cur; __prev = __cur,__cur = __cur->_next) 
		if (__cur->_key == __key)
		{			
			if(__cur == __first)
			{
				_buckets[__n] = __cur->_next;
			}
			else
			{
				__prev->_next = __cur->_next;
			}
			delete __cur;
			_num_elements --;
			return true;
		}
	return false;
}
}
#endif
