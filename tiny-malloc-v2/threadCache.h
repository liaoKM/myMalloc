#include"util.h"
#include"centralCache.h"
#include"span.h"
#define TCTHRESHHOLD 64*1024
#define TCLISTNUM (1024+60/4)+1
class ThreadCache
{
private:
    SpanList memList[TCLISTNUM];
    //从pageCache获取一个span，该span大小至少minNum*size
    void getSpanFromPC(int size,int minNum);
    //将空闲的span返回给PC
    void releaseSpanToPC(Span* span);
    int size2slot(int size);
public:
    void* allocate(int size);
    //回收内存
    //span：内存对应的span，由外层调用查找得到
    void retakeMem(void* mem,Span* span);

};
//__thread static ThreadCache* pThreadCache=nullptr;//move to calss TinyMalloc