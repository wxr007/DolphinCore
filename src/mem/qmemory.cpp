
//tcmalloc性能比jemalloc差一点,但是优点是有相关的辅助工具,帮助定位内存泄露/CPU性能分析.
//另外,je存在不稳定因素,已经碰到过好几次，在free的时候会core，无从定位.


#ifdef _USE_TCMALLOC_
//用google/tcmalloc;
#include <google/tcmalloc.h>
#include "mem/qmemory.h"

void *qmalloc(size_t size)
{
    return tc_malloc(size);
}
void qfree(void *ptr)
{
    tc_free(ptr);
}

//void *qmalloc4msg(size_t size);
//void qfree4msg(void *ptr);

//void *qmalloc4net(size_t size);
//void qfree4net(void *ptr);

#elif defined _USE_JEMALLOC_
//用jemalloc;
#include <jemalloc/jemalloc.h>
#include "mem/qmemory.h"

void *qmalloc(size_t size)
{
    return je_malloc(size);
}
void qfree(void *ptr)
{
    je_free(ptr);
}

//void *qmalloc4msg(size_t size);
//void qfree4msg(void *ptr);

//void *qmalloc4net(size_t size);
//void qfree4net(void *ptr);

#elif defined _USE_MPOOL_
//暂时的简单测试来看,CPU占用,并不比glibc好.
#include "mem/lookaside_alloc.h"
void *qmalloc(size_t size)
{
    size = ROUNDNUM2(size + 8 + 1, 8);
    return __mt_char_alloc.allocate(size);
}
void qfree(void *ptr)
{
    __mt_char_alloc.deallocate(ptr);
}


#else
//用glibc;
#include <stdlib.h>
#include "mem/qmemory.h"

void *qmalloc(size_t size)
{
    return malloc(size);
}
void qfree(void *ptr)
{
    free(ptr);
}

//void *qmalloc4msg(size_t size);
//void qfree4msg(void *ptr);

//void *qmalloc4net(size_t size);
//void qfree4net(void *ptr);

#endif











