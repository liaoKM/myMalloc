#include"util.h"
#include"centralCache.h"
#define TCTHRESHHOLD 64*1024
#define TCLISTNUM (1024+60/4)+1
class ThreadCache
{
private:
    ChunkList<MemObj> memList[TCLISTNUM];
    //从centralCache获取至少minNum个size大小的obj
    //minNum个obj来自同一span 
    void getObjListFromCC(int size,int minNum);
    //减小list内obj的数量
    //遍历chunk，获取pageId。空闲的span返回给PageCache
    void releaseObjToCC(int size);
    int size2slot(int size);
public:
    void* allocate(int size);
    void retakeMem(void* mem,int size);

};
//__thread static ThreadCache* pThreadCache=nullptr;//move to calss TinyMalloc