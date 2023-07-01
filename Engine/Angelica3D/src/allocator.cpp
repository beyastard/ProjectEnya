#include <assert.h>
#include <stdlib.h>

#include "amemory.h"
#include "allocator.h"

namespace abase{
allocator::allocator(size_t __size, size_t __n, size_t __grow)
{
	_total_size	= 0;
	_buf_chain	= NULL;
	_grow		= __grow;
	_block_size	= __size>0?__size:1;
	_fbuf_head 	= NULL;
	//修正_block_size
	
	assert(_block_size >= sizeof(fbuf_node));
	assert(__size);
	if(__n > 0){
		_M_allocate(__n,_block_size);
	}	
}

allocator::~allocator()
{
	if(_buf_chain)
	{
		amem_write_log(("<abase::allocator> ERROR: release() must be invoked before abase::allocator::~allocator\n"));		
		assert(false && "<abase::allocator> ERROR: release() must be invoked before abase::allocator::~allocator");
	}
}

void allocator::_M_allocate(size_t __n,size_t __block_size)
{
	void  *p;
	if(!_buf_chain){
		_buf_chain = new abuf_node();
		if(!_buf_chain) return;
		_buf_chain->_buffer	= malloc(__n * __block_size);
		if(!_buf_chain->_buffer) return ;
		_buf_chain->_next	= NULL;
		_buf_chain->_size	= __n * __block_size;
		p = _buf_chain->_buffer;
	}
	else
	{
		struct abuf_node *nnode;
		nnode = new abuf_node();
		if(!nnode) return;
		nnode->_buffer = malloc(__n * __block_size);
		if(!nnode->_buffer) {
			delete nnode; 
			return;
		}
		nnode->_size = __n * __block_size;
		nnode->_next = _buf_chain;
		_buf_chain = nnode;
		p = nnode->_buffer;
	}
	_total_size += __n * __block_size;
	_M_insert_block_to_table(p,__block_size,__n);
}

void allocator::_M_insert_block_to_table(void *p, size_t __block_size,size_t __n)
{
	for(size_t i = 0; i < __n; i++)
	{		
		if(_fbuf_head){
			((fbuf_node *)p)->_next  = _fbuf_head;
			_fbuf_head = (fbuf_node*)p;
		}
		else
		{
			_fbuf_head = (fbuf_node*) p;
			_fbuf_head->_next = NULL;
		}
		p =(char*)p +  __block_size;
	}
}

int	allocator::check(int type)
{
	if(type == CHECK_MEM_LEAK){
		fbuf_node *tmpnode = _fbuf_head;
		int counter = 0;
		//调用每块可能的内存释放函数
		while(tmpnode)
		{
			counter += _block_size;
			tmpnode =  tmpnode->_next;
			
		}
		return _total_size - counter;
	}
	return -1;
}

void *  allocator::alloc()
{
	if(_fbuf_head){
		void *rst = _fbuf_head;
		_fbuf_head = _fbuf_head->_next;
		return rst;
	}
	else
	{
		if(_grow > 0){
			_M_allocate(_grow,_block_size);
			if(!_fbuf_head) return NULL;
			void *rst = _fbuf_head;
			_fbuf_head = _fbuf_head->_next;
			return rst;
		}
	}
	return NULL;
}

void	allocator::free(void * data)
{
	assert(data);
	if(data) {
		if(_fbuf_head){
			((fbuf_node *)data)->_next  = _fbuf_head;
			_fbuf_head = (fbuf_node *)data;
		}
		else
		{
			_fbuf_head = (fbuf_node*) data;
			_fbuf_head->_next = NULL;
		}
	}
}

void	allocator::release()
{
	if(int rst = check(CHECK_MEM_LEAK)) 
	{
		amem_write_log(("<abase::allocator> ERROR: Memory leak detected in abase::allocator::release, maybe cause unexpected result,%d\n",rst));
		assert(false && "<abase::allocator> ERROR: Memory leak detected in abase::allocator::release, maybe cause unexpected result\n");
		return;
	}

	//将链表中的数据区释放
	struct abuf_node *nnode = _buf_chain;
	while(nnode)
	{
		struct abuf_node * nextnode;
		nextnode = nnode->_next;
		free(nnode->_buffer);
		delete nnode;
		nnode = nextnode;
	}
	_buf_chain = NULL;
	_total_size = 0;
	_fbuf_head = NULL;
}
}
