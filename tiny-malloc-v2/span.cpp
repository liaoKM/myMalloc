#include"span.h"

Span::Span(int pNum):pageNum(pNum),objSize(-1),objNum(0)
{
    pageId=((long long)this)>>PAGESHIFT;
}
Span::Span():pageNum(-1),objSize(-1),objNum(0)
{
    pageId=-1;
}
bool Span::split(int objSize)//大块的span分为小块的memobj并放入链表
{
    if(isSplited())//已经被分配
    {
        return false;
    }

    //todo 对齐
    //objSize=ceilMemSize(objSize);
    this->objSize=objSize;


    void* userSpace=(void*)this+SPANHEADSIZE;//512位对齐
    int num=(pageNum*PAGESIZE-SPANHEADSIZE)/objSize;
    this->objNum=num;
    void* end=userSpace+(num-1)*objSize;
    //插入链表
    for(int i=0;i<num;i++)
    {
        MemObj* p=(MemObj*)(userSpace+i*objSize);
        p->next=(MemObj*)(userSpace+(i+1)*objSize);
        p->prev=(MemObj*)(userSpace+(i-1)*objSize);
    }
    objList.multiPushFront((MemObj*)userSpace,(MemObj*)end,num);
    /*for(int i=0;i<num;i++)
    {
        objList.pushFront((MemObj*)(userSpace+objSize*i));
    }*/
    return true;
}
bool Span::merge()//空闲的小块memobj合并
{
    if(isFree())
    {
        objList.clear();
        objNum=0;
        objSize=-1;
        return true;
    }
    else
    {
        return false;
    }
}
bool Span::isFree()
{
    return objNum==objList.size();
}
bool Span::isSplited()
{
    return objSize!=-1;
}
void SpanList::Lock()
{
	_mtx.lock();
}
void SpanList::Unlock()
{
    _mtx.unlock();
}