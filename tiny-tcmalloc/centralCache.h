#ifndef INCLUDECC
#define INCLUDECC
#include"span.h"
#define CENTRALLISTNUM ((PAGESIZE*(MAXSPANSIZE-1)-SPANHEADSIZE)/64+60/4+1)//112
class CentralCache
{
private:
    SpanList spanList[CENTRALLISTNUM];
    //向spancache申请span，并分割为若干obj
    void allocateSpan(int pageNum,int objsize);
    //将Span释放给spancache
    void releaseSpan(Span* span);
    int size2slot(int size);
    
public:
    //向centralCache申请一个obj
    //size:上层调用保证size==32b*N && size>64B
    MemObj* getObj(int size);
    //向centralCache申请多个obj，至少minNum个，所有obj来自同一span（避免threshCache中出现cache一致性问题）
    //一般用于分配给threadCache
    //size:上层调用保证 size==32b*N && size<64B
    //isEntire:返回的一整个span的所有obj(直接向PageCache申请)
    //返回值：返回Obj的个数
    int getMultiObj(int size,int minNum,MemObj*& oStart,MemObj*& oEnd,bool isEntire=true);
    //回收obj
    //如果回收后span空闲，释放给pageCache
    void retakeObj(MemObj* p,int size);
    //回收num个obj
    //objlist 包含end块
    //上层调用确保所有obj来自同一个span！！
    void retakeMultiObj(MemObj* start,MemObj* end,int num,int size);
    //返回静态centralCache对象
    static CentralCache& getCCInst()
    {
        static CentralCache cc;
        return cc;
    }
    //debug
    //确认objsize与span的objSize对应
    bool checkSize(MemObj* p,int objSize);
    //debug
    //确认objsize对应Span在centralcache中
    Span* checkSpan(MemObj* p);
    //bool
    //确认obj在ObjList中,即obj未分配
    //只能在单线程下检测
    bool checkObjInObjList(MemObj* p);
};
#endif