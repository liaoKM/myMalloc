#ifndef UTILHEAD
#define UTILHEAD

#include <unistd.h>
#include <sys/mman.h>
#include <error.h>
#include<cmath>
#include<algorithm>

#define MAXSPANSIZE 129
#define CHUNKNEXT(x) (((void**)(x))[0])
#define CHUNKPREV(x) (((void**)((x)+64))[0])
#define PAGESIZE 4096
#define PAGESHIFT 12
#define SPANHEADSIZE 64

#define MAXTHREADMAL (64*1024)//64KB
#define MAXCACHESIZE ((MAXSPANSIZE-1)*(PAGESIZE))
class MemObj
{
public:
    MemObj* next;
    MemObj* prev;
};

template<class Chunk>
class ChunkList
{
private:
    int chunkNum;
    Chunk head;
    void insert(Chunk* target,Chunk* pos)
    {
        
        Chunk* temp=pos->next;

        pos->next->prev=target;
        pos->next=target;
        target->next=temp;
        target->prev=pos;

        chunkNum++;
    }

    void mulitInsert(Chunk* start,Chunk* end,int num,Chunk* pos)
    {
        
        Chunk* temp=pos->next;

        pos->next->prev=end;
        pos->next=start;
        end->next=temp;
        start->prev=pos;
        chunkNum+=num;
    }

public:
    ChunkList()
    {
        chunkNum=0;
        head.next=&head;
        head.prev=&head;
    }
    Chunk* unlink(Chunk* target)
    {
        target->prev->next=target->next;
        target->next->prev=target->prev;
        chunkNum--;
        return target;
    }
    void pushFront(Chunk* chunk)
    {
        insert(chunk,&head);
    }
    void pushBack(Chunk* chunk)
    {
        insert(chunk,head.prev);
    }
    void multiPushFront(Chunk* start,Chunk* end,int num)
    {
        mulitInsert(start,end,num,&head);
    }
    void multiPushBack(Chunk* start,Chunk* end,int num)
    {
        mulitInsert(start,end,num,head.prev);
    }
    Chunk* getFrontChunk()
    {
        return unlink(head.next);
    }
    Chunk* getBackChunk()
    {
        return unlink(head.prev);
    }
    Chunk* getHeadPointer()
    {
        return &head;
    }
    void clear()
    {
        head.next=&head;
        head.prev=&head;
        chunkNum=0;
    }
    int size()
    {
        return chunkNum;
    }
    bool isEmpty()
    {
        return chunkNum==0;
    }
    bool _checkListAndNum()
    {
        if(chunkNum==0)
        {
            if(head.next==head.prev&&head.next==&head)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            int countForward=0;
            for(Chunk* p=head.next;p!=&head;p=p->next)
            {
                countForward++;
            }
            int countBackward=0;
            for(Chunk* p=head.prev;p!=&head;p=p->prev)
            {
                countBackward++;
            }
            return countForward==chunkNum&&countBackward==chunkNum;
        }
    }

};

template<int N>
struct radixNode
{
    void* slot[1<<N];
};

//sbrk申请堆内存，程序结束前不释放
void* systemMalloc(int size);
//mmap申请内存，用于创建span
void* sysMallocPage(int nPagesize);
//mmap释放内存
bool sysFreePage(void* p,int nPagesize);

//内存大小取整
//16B以内 取16B
//64B以 4B取整
//64B到pageSize*(MAXSPANSIZE-1)-SPANHEADSIZE 64B对齐
//大于pageSize*(MAXSPANSIZE-1)-SPANHEADSIZE的，操作系统分配
long long ceilMemSize(long long memSize);

//64位内存虚址空间  0x0000,0000,0000,0000----0x0000,7fff,ffff,f000
//64位：高16位为0，低12位为页内地址。因此，页号有效范围：36位
//N=2^12=4096（4K pagesize）
//sizeof(radixNode<12>)=64b*4K=8B*4K=32KB
template<int DEPTH,int N>
class RadixTree
{
    int radixDepth;
    int slotNum;
    void** root;
    void* logHeapTop;
    inline void** getSlot(void** crrRoot,int slot)
    {
        return (void**)crrRoot[slot];

    }
    void** newSlot(void** crrRoot,int slot)
    {
        crrRoot[slot]=sysMallocPage((sizeof(void*)*slotNum+PAGESIZE-1)/PAGESIZE);
        return (void**)crrRoot[slot];
    }
    void freeNode(void** p,int d)
    {
        if(p!=NULL&&d<DEPTH)
        {
            for(int i=0;i<slotNum;i++)
            {
                if(p[i]!=nullptr)
                {
                    freeNode((void**)p[i],d+1);
                }
            }
            sysFreePage(p,(sizeof(void*)*slotNum+PAGESIZE-1)/PAGESIZE);
        }
        else
        {
            return;
        }
    }
public:
    RadixTree():radixDepth(DEPTH),slotNum(1<<N)
    {
        root=(void**)systemMalloc(sizeof(void*)*slotNum);
        logHeapTop=sbrk(0);
        for(int i=0;i<slotNum;i++)
        {
            root[i]=nullptr;
        }
        return;
    }
    //key:pageId
    void* getVal(long long key)
    {
        void** pNode=root;
        for(int i=0;i<DEPTH-1;i++)
        {
            int shift=N*(DEPTH-1-i);
            int nodeKey=0xfff&(key>>shift);
            pNode=getSlot(pNode,nodeKey);
        }
        int shift=0;
        int nodeKey=0xfff&(key>>shift);
        return pNode[nodeKey];
    }
    //key:pageId
    //value:span pointer
    void insert(long long key,void* value)
    {
        void** pNode=root;
        void** temp;
        for(int i=0;i<DEPTH-1;i++)
        {
            int shift=N*(DEPTH-1-i);
            int nodeKey=0xfff&(key>>shift);
            temp=getSlot(pNode,nodeKey);
            if(temp==nullptr)
            {
                pNode=newSlot(pNode,nodeKey);
            }
            else
            {
                pNode=temp;
            }
        }
        int shift=0;
        int nodeKey=0xfff&(key>>shift);
        pNode[nodeKey]=value;
    }
    ~RadixTree()
    {
        freeNode(root,0);
        root=nullptr;
    }
};

#endif