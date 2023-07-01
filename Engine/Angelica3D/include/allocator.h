/*
 * FILE: allocator.h
 *
 * DESCRIPTION: Buffered allocator
 *
 * CREATED BY: Cui Ming 2002-1-24
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/

#ifndef __ABASE_ALLOCATOR_H__
#define __ABASE_ALLOCATOR_H__


#include <stdio.h>
namespace abase{

class allocator
{
private:
	typedef	struct abuf_node{
		void		*_buffer;
		size_t		_size;
		abuf_node	*_next;
	};	
	typedef struct fbuf_node{ //这是保存临时空间的结点
		fbuf_node * _next;
	};

	void _M_allocate(size_t __n,size_t __block_size);
	void _M_insert_block_to_table(void *p, size_t __block_size,size_t __n);
	
	fbuf_node		*_fbuf_head;	
	struct abuf_node	*_buf_chain;
	size_t			_block_size;
	size_t			_align;
	size_t			_grow;
	size_t			_total_size;

public:
	enum{
		CHECK_MEM_LEAK
	};
	int  check(int type);		//0 表示没有任何错误

public:
	allocator(size_t __block_size, size_t __default_n, size_t __grow);
	~allocator();

	void *  alloc();
	void	free(void *);
	void	release();	//release with custom support
};
}

#endif
