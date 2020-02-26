/*
 * @Author: power
 * @Date: 2020-01-28 10:41:36
 * @LastEditTime: 2020-02-19 15:24:46
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /test/Alloctor.cpp
 */
// #include "Alloctor.hpp"
#ifndef __Alloctor_h_
#define __Alloctor_h_
#include <stdlib.h>
#include "MemoryPoll.hpp"
void* operator new(size_t nSize)
{
    return MemoryMgr::Instance().allocMem(nSize);
}
void operator delete(void* p)
{
    MemoryMgr::Instance().freeMem(p);
}
void* operator new[](size_t nSize)
{
    return MemoryMgr::Instance().allocMem(nSize);
}
void operator delete[](void* p)
{
    MemoryMgr::Instance().freeMem(p);
}
void* mem_alloc(size_t size)
{
    return malloc(size);
}
void mem_free(void *p)
{
    free(p);
}
#endif // !__Alloctor_h_
