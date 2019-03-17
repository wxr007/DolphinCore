
#ifndef _QMEMORY_H__
#define _QMEMORY_H__


//1-通用内存分配/释放;
void *qmalloc(size_t size);
void qfree(void *ptr);

//2-message内存分配/释放;
void *qmalloc4msg(size_t size);
void qfree4msg(void *ptr);

//3-network内存分配/释放;
void *qmalloc4net(size_t size);
void qfree4net(void *ptr);




#endif //_QMEMORY_H__


