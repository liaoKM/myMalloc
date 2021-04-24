#include"util.h"
#include"span.h"
#define THRESHLISTSIZE 8
class PageCache
{
private:
    SpanList spanlist[MAXSPANSIZE];
    static RadixTree<3,PAGESHIFT> radixtree;//放到tinyMalloc中
    std::mutex _mtx;
    Span* mallocSpan(int numOfPage);//申请新的span，并挂到链表,无锁
    bool deleteSpan(Span* p);//释放span内存到操作系统,无锁
public:
    Span* newSpan(int numOfPage);//大Span分裂为满足要求的Span--pageCache锁；如位申请内存，则list锁
    void retakeSpan(Span* span);//回收空闲的span,list锁
    Span* distribute(int numOfPage);//分发一个numOfPage大小的span，并从链中unlink；list锁
    static Span* pageid2Span(long long key)
    {
        return (Span*)radixtree.getVal(key);
    }
    void releaseFreeMem(int threshhold=THRESHLISTSIZE);//减少spanlist大小;threshhold=0:释放pageCache内所有span!!!;pageCache锁
    static PageCache& getPageCacheInst()
    {
        static PageCache pagecache;
        return pagecache;
    }

};
//RadixTree<3,PAGESHIFT> PageCache::radixtree;