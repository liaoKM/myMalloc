#include"centralCache.h"
#include"pageCache.h"
#include<cmath>
#include <assert.h>
//#define CCGETSLOT(x) ((x)/4)
int CentralCache::size2slot(int size)
{
    int slot=0;
    if(size<64)
    {
        slot=size/4;
    }
    else
    {
        slot=size/64+60/4;
    }
    return slot;
}
void CentralCache::allocateSpan(int pageNum,int objsize)//向spancache申请span，并分割为若干obj
{
    Span* span=PageCache::getPageCacheInst().distribute(pageNum);
    span->split(objsize);
    int slot=size2slot(objsize);
    spanList[slot].Lock();
    spanList[slot].pushFront(span);
    spanList[slot].Unlock();
    return;
}
void CentralCache::releaseSpan(Span* span)//将Span释放给spancache
{
    span->merge();
    PageCache::getPageCacheInst().retakeSpan(span);
}

MemObj* CentralCache::getObj(int size)//返回一个obj，大内存申请
{
    assert(size%4==0&&size>64*1024);
    int slot=size2slot(size);
    spanList[slot].Lock();
    Span* head=spanList[slot].getHeadPointer();
    Span* p=head->next;
    while(p!=head&&p->objList.size()==0)
    {
        p=p->next;
    }
    if(p==head)//没有空闲的span，申请后分配
    {
        int pageNum=ceilf32((size+SPANHEADSIZE)/(float)PAGESIZE);
        p=PageCache::getPageCacheInst().distribute(pageNum);
        p->split(size);
        spanList[slot].pushFront(p);
    }
    MemObj* ret=p->objList.getFrontChunk();
    spanList[slot].Unlock();
    return ret;
}
int CentralCache::getMultiObj(int size,int minNum,MemObj*& oStart,MemObj*& oEnd,bool isEntire)
{
    assert(size%4==0&&size<=64*1024);
    int slot=size2slot(size);
    spanList[slot].Lock();
    Span* head=spanList[slot].getHeadPointer();
    Span* p=head->next;
    if(!isEntire)
    {
        while(p!=head&&p->objList.size()<minNum)
        {
            p=p->next;
        }
    }
    if(isEntire||p==head)//没有空闲的span，申请后分配
    {
        int pageNum=ceilf32((size*minNum+SPANHEADSIZE)/(float)PAGESIZE);
        p=PageCache::getPageCacheInst().distribute(pageNum);
        p->split(size);
        spanList[slot].pushFront(p);
    }
    MemObj* objHead=p->objList.getHeadPointer();
    oStart=objHead->next;
    oEnd=objHead->prev;
    int retObjNum=p->objNum;
    p->objList.clear();
    spanList[slot].Unlock();
    return retObjNum;
}
void CentralCache::retakeObj(MemObj* p,Span* span)
{
    int slot=size2slot(span->objSize);
    spanList[slot].Lock();
    span->objList.pushFront(p);
    if(span->isFree())
    {
        spanList[slot].unlink(span);
        releaseSpan(span);
    }
    spanList[slot].Unlock();
    return;
}
void CentralCache::retakeMultiObj(MemObj* start,MemObj* end,int num,int size)//参数：objlist 包含end块
{
    int slot=size2slot(size);
    spanList[slot].Lock();
    long long pageId=(long long)start>>PAGESHIFT;
    Span* span=PageCache::pageid2Span(pageId);
    span->objList.multiPushFront(start,end,num);
    if(span->isFree())
    {
        spanList[slot].unlink(span);
        releaseSpan(span);
    }
    spanList[slot].Unlock();
    return;
}
bool CentralCache::checkSize(MemObj* p,int objSize)
{
    Span* span=PageCache::getPageCacheInst().pageid2Span((long long)p>>PAGESHIFT);
    if(span->objSize!=objSize)
    {
        return false;
    }
    return true;
}

Span* CentralCache::checkSpan(MemObj* p)
{
    Span* ret=nullptr;
    Span* span=PageCache::getPageCacheInst().pageid2Span((long long)p>>PAGESHIFT);
    
    int slot=size2slot(span->objSize);
    spanList[slot].Lock();
    Span* head=spanList[slot].getHeadPointer();
    for(Span* p=head->next;p!=head;p=p->next)
    {
        if(p==span)
        {
            ret=p;
        }
    }
    spanList[slot].Unlock();
    return ret;
}
bool CentralCache::checkObjInObjList(MemObj* Obj)
{
    bool inList=false;
    Span* span=PageCache::getPageCacheInst().pageid2Span((long long)Obj>>PAGESHIFT);
    if(!span->isSplited())
    {
        inList=true;
    }
    else
    {
        MemObj* head=span->objList.getHeadPointer();
        for(MemObj* p=head->next;p!=head;p=p->next)
        {
            if(p==Obj)
            {
                inList=true;
            }
        }
    }
    
    return inList;
}