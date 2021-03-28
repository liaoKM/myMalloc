#ifndef INCLUDESPAN
#define INCLUDESPAN
#include"util.h"
#include <mutex>
class Span
{
public:
    long long pageId;//span起始页号
    long long pageNum;
    ChunkList<MemObj> objList;
    int objSize;
    int objNum;//span分裂obj的个数，包含已分配和未分配的obj

    Span* next=nullptr;
    Span* prev=nullptr;

    Span();
    Span(int pNum);
    bool split(int objSize);//大块的span分为小块的memobj并放入链表
    bool merge();//空闲的小块memobj合并
    bool isFree();
    bool isSplited();
};

class SpanList: public ChunkList<Span>
{
private:
    std::mutex _mtx;
public:
    void Lock();
	void Unlock();
};
#endif