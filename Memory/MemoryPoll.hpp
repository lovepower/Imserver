/*
 * @Author: power
 * @Date: 2020-01-28 11:05:02
 * @LastEditTime: 2020-02-22 17:17:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /test/MemoryPoll.hpp
 */
#ifndef _MEMORYPOLL_H_
#define _MEMORYPOLL_H_
#define MAX_MEMORY_SZIE 256
#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <iostream>
class MemoryPoll;
class MemoryBlock
{
private:
    /* data */
    //预留字段
    char c1;
    char c2;
    char c3;
public:
    MemoryPoll* pAlloc;//所属的内存池
    MemoryBlock* pNext;//下一块位置
    int nID;//内存块编号
    int nRef;//内存块编号
    bool bPool;//是否在内存池中
};
class MemoryPoll
{
private:
    /* data */
public:
    MemoryPoll()
    {
        _pbuf = nullptr;
        _pHeader = nullptr;
        _nSize = 0;
        _nBlockSize = 0;
    }

    ~MemoryPoll()
    {
        if(_pbuf)
        free(_pbuf);
    }
    //内存池申请
    void* allocMemory(size_t nSize)
    {
        std::lock_guard<std::mutex> lg(_mutex);
        if(!_pbuf)
        {
            initMemory();
        }
        MemoryBlock* pReturn = nullptr;
        if(nullptr==_pHeader)
        {
            pReturn = (MemoryBlock*)malloc(nSize+sizeof(MemoryBlock));
            pReturn->bPool = false;
            pReturn->nID = -1;
            pReturn->nRef = 1;
            pReturn->pAlloc = nullptr;
            pReturn->pNext = nullptr;
        }
        else
        {
            pReturn = _pHeader;
            _pHeader= _pHeader->pNext;
            assert(0 == pReturn->nRef);
            pReturn->nRef = 1;
        }
        return ((char*)pReturn + sizeof(MemoryBlock));
        
    }
    void freeMemory(void* pMem)
    {
        MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem-sizeof(MemoryBlock));
        assert(1 == pBlock->nRef);
        if(pBlock->bPool)
        {
            std::lock_guard<std::mutex> lg(_mutex);
            if(--pBlock->nRef !=0)
            {
                return;//被占用
            }
            pBlock->pNext = _pHeader;
            _pHeader = pBlock;
        }
        else
        {
            if(--pBlock->nRef!=0)
            {
                return;
            }
            free(pBlock);
        }
        
    }
    //初始化内存池
    void initMemory()
    {
        //断言
        assert(nullptr==_pbuf);
        if(_pbuf)
        return;
        //计算内存池的大小
        size_t realSize = _nSize + sizeof(MemoryBlock);//真实每块内存的大小
        size_t bufSize = realSize*_nBlockSize;//内存池的总大小
        _pbuf =(char*)malloc(bufSize);
        //初始化内存池
        _pHeader = (MemoryBlock*)_pbuf;
        _pHeader->bPool = true;
        _pHeader->nID = 0;
        _pHeader->nRef = 0;
        _pHeader->pAlloc = this;
        _pHeader->pNext = nullptr;
        //遍历内存块进行初始化
        MemoryBlock* pTemp1 = _pHeader;
        for (size_t i = 1; i < _nBlockSize; i++)
        {
            MemoryBlock* pTemp2 = (MemoryBlock*) (_pbuf+i*realSize);
            pTemp2->nID = i;
            pTemp2->bPool = true;
            pTemp2->nRef = 0;
            pTemp2->pAlloc = this;
            pTemp2->pNext = nullptr;
            pTemp1->pNext = pTemp2;
            pTemp1 = pTemp2;

        }
        
    }
protected:
    //内存池地址即维护一块大内存
    char* _pbuf;
    //首部位置
    MemoryBlock* _pHeader;
    //内存池的大小
    size_t _nSize;
    //内存单元的数量
    size_t _nBlockSize;
    std::mutex _mutex;
};

template<size_t nSize,size_t nBlockSize>
class MemoryAlloctor:public MemoryPoll
{
private:
    /* data */
public:
    MemoryAlloctor()
    {
        const size_t n = sizeof(void*);
        _nSize = (nSize/n)*n+(nSize % n ? n : 0);//规定以4的倍数分配内存空间
        _nBlockSize = nBlockSize;
    }
};
//内存池管理工具
class MemoryMgr
{
private:
    /* data */
    MemoryAlloctor<64,100> _mem64;
    MemoryAlloctor<128,100> _mem128;
    MemoryAlloctor<256,100> _mem256;
    MemoryPoll* _szAlloc[MAX_MEMORY_SZIE+1];
public:
    MemoryMgr()
    {
        init_szAlloc(0,64,&_mem64);
        init_szAlloc(65,128,&_mem128);
        init_szAlloc(129,256,&_mem256);
    }
    ~MemoryMgr()
    {
        
    }
    static MemoryMgr& Instance()
    {
        static MemoryMgr mgr;
        // std::cout<<"开始进行内存操作"<<std::endl;
        return mgr;
    }
    void init_szAlloc(int nBegin,int nEnd,MemoryPoll* pMemA)
    {
        for (int i = nBegin; i <= nEnd; i++)
        {
            /* code */
            _szAlloc[i] = pMemA;
        }
        
    }
    void* allocMem(size_t nSize)
    {
        if(nSize <= MAX_MEMORY_SZIE)
        {
            return _szAlloc[nSize]->allocMemory(nSize);
        }
        else
        {
            MemoryBlock* pReturn = (MemoryBlock*)malloc(nSize+sizeof(MemoryBlock));
            pReturn->bPool = false;
            pReturn->nID = -1;
            pReturn->nRef = 1;
            pReturn->pAlloc = nullptr;
            pReturn->pNext = nullptr;
            return ((char*)pReturn+sizeof(MemoryBlock));
        }
        
    }
    //释放内存
    void freeMem(void*pMem)
    {
        MemoryBlock * pBlock = (MemoryBlock*)((char*)pMem-sizeof(MemoryBlock));
        if (pBlock->bPool)
        {
            pBlock->pAlloc->freeMemory(pMem);
        }
        else
        {
            if(--pBlock->nRef == 0)
            {
                free(pBlock);
            }
        }
        
        
    }
};





#endif // !_MEMORYPOLL_H_