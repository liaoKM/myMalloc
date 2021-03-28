#include"threadCache.h"
#include"pageCache.h"
#include<algorithm>
void ThreadCache::getObjListFromCC(int size,int minNum)
{
    int slot=size2slot(size);
    MemObj* start,*end;
    int objNum=CentralCache::getCCInst().getMultiObj(size,minNum,start,end,true);
    memList[slot].multiPushFront(start,end,objNum);
}
void ThreadCache::releaseObjToCC(int size)
{
    int slot=size2slot(size);
    ChunkList<MemObj>& list=memList[slot];
    int freeNum=list.size()/2;
    int TotalCount=0;
    //循环取list尾的Obj
    while (TotalCount<freeNum)
    {
        int count=1;
        MemObj* delListHead=list.getBackChunk();
        MemObj* delListTail=delListHead;
        Span* span=PageCache::pageid2Span(((long long)delListHead)>>PAGESHIFT);
        long long pageId=span->pageId;
        int pageNum=span->pageNum;

        MemObj* head=list.getHeadPointer();
        for(MemObj* p=head->next;p!=head;)
        {
            long long tempId=((long long)p)>>PAGESHIFT;
            if(tempId>=pageId&&tempId<pageId+pageNum)
            {
                MemObj* delObj=p;
                p=p->next;
                list.unlink(delObj);
                delListTail->next=delObj;
                delListTail=delObj;
                count++;
            }
            else
            {
                p=p->next;
            }
        }
        TotalCount+=count;
        CentralCache::getCCInst().retakeMultiObj(delListHead,delListTail,count,size);
    }
    
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
    if(memList[slot].isEmpty())
    {
        int minNum=std::max(64*4*1024/size,16);
        getObjListFromCC(size,minNum);//todo向CC申请
    }
    return memList[slot].getFrontChunk();
}
void ThreadCache::retakeMem(void* mem,int size)
{
    int slot=size2slot(size);
    memList[slot].pushFront((MemObj*)mem);
    if(memList[slot].size()>std::max(TCTHRESHHOLD/size,16*2))
    {
        //触发回收
        releaseObjToCC(size);
    }
}