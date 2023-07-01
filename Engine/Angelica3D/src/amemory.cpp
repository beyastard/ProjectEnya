#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "amemory.h"
#include "hashtab.h"

/*internal data structure and variable*/

class amemory_log
{
private:
	FILE * file;
public:
	amemory_log()
	{
		file = NULL;
	}
	amemory_log(char *filename)
	{
		file = fopen(filename,"w");
	}
	~amemory_log()
	{
		if(file) fclose(file);
	}

public:
	void open(const char *filename)
	{
		if(file) fclose(file);
		file = fopen(filename,"w");
	}
	void write_log(const char * fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		FILE * output;
		output = file?file:stdout;
		(void) vfprintf(output, fmt, ap);
		va_end(ap);
		fflush(output);
	}
};//memory log end

struct amemory_node
{
	const char *_file;
	int  _line;
	void *_block;
	size_t _size;
};//memory node end



static const char * AMEMLOGFILE="amemory.log";
static const int overflow_block = 32;
static const int band_magic_number = 0xdd;

int amemory_vebose_level = 0;
static amemory_log amem_log;

class amemory_func
{
public:
	inline void operator()(const amemory_node & __node) const{
		amem_log.write_log("Memory leak detected in file %s, line %d , a %d bytes block at address %p\n",__node._file,__node._line,__node._size,__node._block);
	}
};// hashfunc end


static abase::hashtab<amemory_node, void *, abase::_hash_function> amem_tab(10);

/* function definition here*/
static void amem_exitproc()
{
	if(!amem_tab.empty())
	{
		amem_log.write_log("\n-----------------------------------------------------------\n");
		amem_log.write_log("%d blocks memory leak found.\n",amem_tab.size());
		amemory_func enum_obj;
		amem_tab.enum_element(enum_obj);
	}
}

static void amem_initialize()
{
	static int initonce = 0;
	if(!initonce)
	{
		initonce = 1;
		amem_log.open(AMEMLOGFILE);
		atexit(amem_exitproc);
	}
}

void	amemory_write_log(const char * fmt, ...)
{
	char buffer[1024];
	va_list ap;
	va_start(ap, fmt);
	(void) _vsnprintf(buffer,1024, fmt, ap);
	va_end(ap);
	amem_log.write_log("%s",buffer);
}

void	*a_malloc(size_t size, const char *file, int line)
{
	void * buf;
	amemory_node node;
	amem_initialize();
	buf = malloc(size + overflow_block*2); //overflow and underflow
	if(!buf) return NULL;
	memset(buf,band_magic_number,size + overflow_block*2);
	node._file = file;
	node._line = line;
	node._block = buf;
	node._size = size;
	amem_tab.put(node._block,node);
	buf = (char*)buf + overflow_block;
	if(amemory_vebose_level>0)
	{
		amem_log.write_log("Alloc successfully, in file:%s,\tline:%d\t, alloc memory %d bytes at %p\n",file,line,size,buf);
	}
	return buf;
}
void	a_free(void *data, const char *file, int line)
{
	void *p;
	p = (char*)data -overflow_block;
	abase::pair<amemory_node *,bool> node = amem_tab.get(p);
	if(node.second)
	{
		if(amemory_vebose_level>0)
		{
			amem_log.write_log("Free successfully, in file:%s,\tline:%d\t, free memory at %p\n",file,line,data);
		}
		
		unsigned char *tmp1,*tmp2;
		tmp1 = (unsigned char*)p;
		tmp2 = tmp1 + node.first->_size + overflow_block;
		for(int i =0; i < overflow_block;i++)
		{
			if(*tmp1++ != band_magic_number || *tmp2++ !=band_magic_number )
			{
				amem_log.write_log("Overflow Occur, in file:%s,\tline:%d\t, free memory at %p\n",file,line,data);
				assert(false);
			}
		}

		free(node.first->_block);
		amem_tab.erase(p);
	}
	else
	{
		amem_log.write_log("Free unmatched buffer,in file:%s,\tline:%d\t, memory pointer is %p\n",file,line,data);
		free(data);
	}	
}

void	*a_realloc(void *data, int size,const char *file,int line)
{
	if(NULL == data)
	{
		return a_malloc(size,file,line);
	}

	if(0 == size)
	{
		a_free(data,file,line);
		return NULL;
	}
	void *p = (char*)data - overflow_block;
	abase::pair<amemory_node *,bool> node = amem_tab.get(p);
	if(node.second)
	{
		void * tmp;
		tmp = realloc(node.first->_block,size + overflow_block*2);
		if(tmp!= NULL)
		{
			node.first->_block	= tmp;
			node.first->_file	= file;
			node.first->_line	= line;
			node.first->_size	= size;
			tmp = (char*)tmp + overflow_block;
			if(amemory_vebose_level>0)
			{
				amem_log.write_log("Realloc successfully, in file:%s,\tline:%d\t, alloc memory %d bytes at %p\n",file,line,size,tmp);
			}

		}
		return tmp;
	}
	else	
	{
		amem_log.write_log("Realloc unmatched buffer,in file:%s,\tline:%d\t, memory pointer is %p\n",file,line,data);
		return NULL;
	}
}

void	* align_malloc_rl(size_t size, int align)
{
	int tsize;
	char * data;
	char * data2;
	tsize = size + align + sizeof(int);
	data = (char *)malloc(tsize);
	if(data){
		if((unsigned int)(data) % align){
			data2 = (char *)(((unsigned int)data & (~(align -1))) + align);
		}
		else
		{
			data2 = data + ((align > sizeof(int))?align:sizeof(int));
		}
		if(data2 - data < sizeof(int)) data2 += align;
		*(int*)(data2 - sizeof(int)) = data2 - data;
		return data2;
	}
	return NULL;
}

void	 align_free_rl(void *data)
{
	char * data2;
	data2 = (char *)data;
	data2 -= *(int *)(data2 - sizeof(int));
	free(data2);
}

void	* align_malloc_db(size_t size, int align,const char *file,int line)
{
	int tsize;
	char * data;
	char * data2;
	tsize = size + align + sizeof(int);
	data = (char *)a_malloc(tsize,file,line);
	if(data){
		if((unsigned int)(data) % align){
			data2 = (char *)(((unsigned int)data & (~(align -1))) + align);
		}
		else
		{
			data2 = data + ((align > sizeof(int))?align:sizeof(int));
		}
		if(data2 - data < sizeof(int)) data2 += align;
		*(int*)(data2 - sizeof(int)) = data2 - data;
		return data2;
	}
	return NULL;
}

void	 align_free_db(void *pdata, const char *file, int line)
{
	char * data2;
	data2 = (char *)pdata;
	data2 -= *(int *)(data2 - sizeof(int));
	a_free(data2,file,line);
}


