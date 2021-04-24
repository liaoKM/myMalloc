#include"tinyMalloc.h"
#include"pageCache.h"
#include"util.h"
#include"centralCache.h"
#include"threadCache.h"
static __thread ThreadCache* pThreadCache=nullptr;
void newThreadCacheBySys()
{
    int pageNum=(sizeof(ThreadCache)+64+PAGESIZE-1)/PAGESIZE;
    void* mem=sysMallocPage(pageNum);
    ((int*)mem)[0]=pageNum;
    pThreadCache=(ThreadCache*)(mem+64);
    new(pThreadCache) ThreadCache;
}
void* TinyMalloc::malloc(long long size)
{
    if(pThreadCache==nullptr)
    {
        newThreadCacheBySys();
    }

    long long realSize=ceilMemSize(size);
    void* ret;
    if(size<64*1024)//向threadCache申请
    {
        ret=pThreadCache->allocate(realSize);
    }
    else if(size<(PAGESIZE*(MAXSPANSIZE-1)-SPANHEADSIZE))//向centralCache申请
    {
        ret=(void*)CentralCache::getCCInst().getObj(realSize);
    }
    else//系统申请
    {
        //增加64B(512b)头部，用于记录大小
        int pageNum=(realSize+512+PAGESIZE-1)/PAGESIZE;
        ret=sysMallocPage(pageNum);
        ((int*)ret)[0]=pageNum;
        ret=ret+64;
    }
    return ret;
}

#include<iostream>
void TinyMalloc::free(void* p)
{
    Span* span=PageCache::pageid2Span(((long long)p)>>PAGESHIFT);
    if(span==NULL)//没有对应span，内存由系统分配
    {
        int pageNum=*((int*)(p-64));
        sysFreePage(p-64,pageNum);
    }
    else
    {
        int size=span->objSize;
        if(size<64*1024)
        {
            pThreadCache->retakeMem(p,span);
        }
        else if(size<(PAGESIZE*(MAXSPANSIZE-1)-SPANHEADSIZE))
        {
            CentralCache::getCCInst().retakeObj((MemObj*)p,span);
        }
    }
}