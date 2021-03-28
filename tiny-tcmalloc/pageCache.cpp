#include"pageCache.h"
RadixTree<3,PAGESHIFT> PageCache::radixtree;

Span* PageCache::mallocSpan(int numOfPage)
{
    if(numOfPage>MAXSPANSIZE)
        return nullptr;
    Span* newSpan=(Span*)sysMallocPage(numOfPage);//todo 重载 Span::operator new!!
    new(newSpan) Span(numOfPage);
    for(int i=0;i<numOfPage;i++)//在前缀树中注册每一页
    {
        radixtree.insert((((long long)newSpan)>>PAGESHIFT)+i,newSpan);
    }
    return newSpan;
}
bool PageCache::deleteSpan(Span* p)
{
    sysFreePage(p,p->pageNum);//todo 前缀树中删除
    return true;
}


void PageCache::retakeSpan(Span* span)
{
    int temp=span->pageNum;
    spanlist[span->pageNum].Lock();
    if(spanlist[span->pageNum].size()<THRESHLISTSIZE)
    {
        spanlist[span->pageNum].pushFront(span);
    }
    else
    {
        deleteSpan(span);
    }
    spanlist[temp].Unlock();
    return;
}
Span* PageCache::newSpan(int numOfPage)
{
    //todo 优先从更大的Span分裂！！！ 对pageCache 枷锁
    _mtx.lock();
    Span* ret=mallocSpan(numOfPage);
    _mtx.unlock();
    spanlist[numOfPage].Lock();
    spanlist[numOfPage].pushFront(ret);
    spanlist[numOfPage].Unlock();
    return ret;
}

void PageCache::releaseFreeMem(int threadhold)
{
    _mtx.lock();
    for(int i=0;i<MAXSPANSIZE;i++)
    {
        spanlist[i].Lock();
        while(spanlist[i].size()>threadhold)
        {
            Span* tobeFree=spanlist[i].getFrontChunk();
            deleteSpan(tobeFree);
        }
        spanlist[i].Unlock();
    }
    _mtx.unlock();
    return;
}

Span* PageCache::distribute(int numOfPage)//分发一个numOfPage大小的span，并从链中unlink
{
    if(numOfPage>MAXSPANSIZE)
    {
        return nullptr;
    }
    Span* ret=NULL;
    spanlist[numOfPage].Lock();
    if(spanlist[numOfPage].isEmpty())
    {
        spanlist[numOfPage].Unlock();
        _mtx.lock();
        ret=mallocSpan(numOfPage);
        _mtx.unlock();
    }
    else
    {
        ret=spanlist[numOfPage].getFrontChunk();
        spanlist[numOfPage].Unlock();
    }
    
    return ret;
}

