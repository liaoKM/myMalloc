#define CHUNKNEXT(x) (((void**)(x))[0])
#define CHUNKPREV(x) (((void**)((x)+64))[0])
struct MemObj
{
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
    }

    void mulitInsert(Chunk* start,Chunk* end,Chunk* pos)
    {
        
        Chunk* temp=pos->next;

        pos->next->prev=end;
        pos->next=start;
        end->next=temp;
        start->prev=pos;
    }

    Chunk* unlink(Chunk* target)
    {
        target->prev->next=target->next;
        target->next->prev=target->prev;
        return target
    }
public:
    ChunkList()
    {
        chunkNum=0;
        head->next=&head;
        head->prev=&head;
    }
    void pushFront(Chunk* chunk)
    {
        insert(chunk,&head);
    }
    void pushBack(Chunk* chunk)
    {
        insert(chunk,head->prev);
    }
    void multiPushFront(chunk* start,chunk* end)
    {
        mulitInsert(start,end,&head);
    }
    void multiPushBack(chunk* start,chunk* end)
    {
        mulitInsert(start,end,head->prev);
    }
    Chunk* getFrontChunk()
    {
        return unlink(head->next);
    }
    Chunk* getBackChunk()
    {
        return unlink(head->prev);
    }

};

template<int n,class slotType>
struct radixNode
{
    int depth;
    int slotNum;
    slotType* slot[1<<n];
};

template<int depth,int n>
class RadixTree
{
    int radixDepth;
    radixNode<n,>* root;
public:
    RadixTree():radixDepth(depth)
    {

    }
};