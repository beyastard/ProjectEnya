/*
 * FILE: amemory.h
 *
 * DESCRIPTION: memory management.
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/

#ifndef __AMEMORY_H__
#define __AMEMORY_H__

//function declaration
#ifdef __cplusplus
#include <new>
extern "C" {
#endif

extern int amemory_vebose_level;

void	*a_malloc(size_t size, const char *file, int line);
void	a_free(void *data, const char *file, int line);
void	*a_realloc(void *data, int size,const char *file,int line);

void	* align_malloc_db(size_t size, int align,const char *file,int line);
void	align_free_db(void *pdata, const char *file, int line);

void	* align_malloc_rl(size_t size, int align);
void	align_free_rl(void *pdata);
void	amemory_write_log(const char * fmt, ...);

#ifdef __cplusplus
}
#endif

//macro definition
#if defined(_DEBUG) || defined(_FORCE_USE_AMEMORY)
//Debug mode
#define amalloc(x) a_malloc(x,__FILE__,__LINE__)
#define afree(x) a_free(x,__FILE__,__LINE__)
#define arealloc(x,y) a_realloc(x,y,__FILE__,__LINE__)
#define align_malloc(x,y) align_malloc_db(x,y,__FILE__,__LINE__)
#define align_free(x) align_free_db(x,__FILE__,__LINE__)
#define amem_write_log(x) amemory_write_log x
#else
//release mode
#define amalloc(x) malloc(x)
#define afree(x)   free(x)
#define arealloc(x,y) arealloc(x,y)
#define align_malloc(x,y) align_malloc_rl(x,y)
#define align_free(x) align_free_rl(x)
#define amem_write_log(x) NULL

#endif

#endif
