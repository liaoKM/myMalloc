#include"util.h"

//sbrk申请堆内存，程序结束前不释放
void* systemMalloc(int size)
{
    //todo !!!
    return sbrk(size);
}

//mmap申请内存，用于创建span
void* sysMallocPage(int nPage)
{
    return mmap(0, (long long)nPage*PAGESIZE , PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE,-1, 0);
}
//mmap释放内存
bool sysFreePage(void* p,int nPage)
{
    return munmap(p,(long long)nPage*PAGESIZE)!=-1;
}
//内存大小取整
long long ceilMemSize(long long memSize)
{
    if(memSize<16)
    {
        memSize=16;//最小16B,保证chunkList的双向链表（64b*2）
    }
    else if(memSize<64)
    {
        memSize=((memSize+4-1)/4)*4;
    }
    else//最大512b对齐 即64B
    {
        memSize=ceil(memSize/64.0f)*64;
    }
    return memSize;
}