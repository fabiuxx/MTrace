/* 
 * File:   mtrace.h
 * Author: Acer
 *
 * Created on February 11, 2014, 10:08 AM
 */

#ifndef __MTRACE_H__
#define	__MTRACE_H__

#include "base.h"

#define TRACE_MEMORY_ALLOCATIONS   1

#if TRACE_MEMORY_ALLOCATIONS
    #define x_malloc(size)          __malloc(size, __FILE__, __LINE__)
    #define x_realloc(ptr, size)    __realloc(ptr, size, __FILE__, __LINE__)
    #define x_free(ptr)             __free(ptr)

    void* __malloc(size_t, char*, uint16_t);
    void* __realloc(void*, size_t, char*, uint16_t);
    void  __free(void*);
    void  memory_state();
    void  memory_clean();
#else
    #define x_malloc(size)          __malloc(size)
    #define x_realloc(ptr, size)    __realloc(ptr, size)
    #define x_free(ptr)             __free(ptr)

    void* __malloc(size_t);
    void* __realloc(void*, size_t);
    void  __free(void*);
    void  memory_state();
#endif
    
#endif	/* __MTRACE_H__ */

