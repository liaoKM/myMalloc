#include"threadCache.h"
#include"pageCache.h"
#include<algorithm>
void ThreadCache::getSpanFromPC(int size,int minNum)
{
    int slot=size2slot(size);
    Span* span=PageCache::getPageCacheInst().distribute((size*minNum+PAGESIZE-1)/PAGESIZE);
    span->split(size);
    memList[slot].pushBack(span);
}
void ThreadCache::releaseSpanToPC(Span* span)
{
    int slot=size2slot(span->objSize);
    SpanList& list=memList[slot];
    list.unlink(span);
    span->merge();
    PageCache::getPageCacheInst().retakeSpan(span);
}
int ThreadCache::size2slot(int size)
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
void* ThreadCache::allocate(int size)
{
    int slot=size2slot(size);
    SpanList& spanList=memList[slot];
    if(spanList.isEmpty())
    {
        int minNum=std::max(128*PAGESIZE/size,8);//4页或16个对象大小
        getSpanFromPC(size,minNum);//todo向CC申请
    }
    Span* sp=memList[slot].frontChunk();
    void* mem=sp->objList.getFrontChunk();
    if(sp->objList.size()==0)//使用完的span从链表unlink
    {
        spanList.unlink(sp);
    }
    return mem;
}
void ThreadCache::retakeMem(void* mem,Span* span)
{
    span->objList.pushFront((MemObj*)mem);
    if(span->objList.size()==1)//插入之前span是用完的
    {
        int slot=size2slot(span->objSize);
        memList[slot].pushFront(span);
    }
    else if(span->isFree())
    {
        //触发回收
        releaseSpanToPC(span);
    }
    else
    {
        //对spanList排序，保证生于块最少的span位于list头部
        /*int slot=size2slot(span->objSize);
        SpanList& list=memList[slot];
        Span* endPtr=list.getHeadPointer();
        int objNum=span->objList.size();
        Span* p;
        for(p=span->next;p!=endPtr&&
        objNum>p->objList.size();)
        {
            ;
        }
        //将span移动到p前
        Span* spPrev=p->prev;
        span->prev=spPrev;
        span->next=p;
        p->prev=span;
        spPrev=span;*/
    }
}