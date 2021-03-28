#include"util.h"
#include<iostream>
int main()
{
    bool success=true;
    int N=100;
    ChunkList<MemObj> memList;
    MemObj* mem=new MemObj[N];
    MemObj* p;

    //////////////////pushFront////////////////////
    success=true;
    for(int i=0;i<N;i++)
    {
        memList.pushFront(mem+i);
    }
    p= memList.getHeadPointer()->next;
    for(int i=0;i<N;i++)
    {
        if(p!=&mem[N-1-i])
        {
            success=false;
            std::cout<<"[fail!!!]:ChunkList.pushFront"<<std::endl;
            break;
        }
        p=p->next;
    }
    if(success&&memList._checkListAndNum())
    {
        std::cout<<"[success]:ChunkList.pushFront"<<std::endl;
    }

    //////////////////getFront//////////////////////
    success=true;
    for(int i=0;i<N;i++)
    {
        MemObj* crr=memList.getFrontChunk();
        if(crr!=&mem[N-1-i])
        {
            success=false;
            std::cout<<"[fail!!!]:ChunkList.getFrontChunk"<<std::endl;
            break;
        }
    }
    if(success&&memList._checkListAndNum())
    {
        std::cout<<"[success]:ChunkList.getFrontChunk"<<std::endl;
    }
    //////////////////pushBack////////////////////
    success=true;
    for(int i=0;i<N;i++)
    {
        memList.pushBack(mem+i);
    }
    p= memList.getHeadPointer()->next;
    for(int i=0;i<N;i++)
    {
        if(p!=&mem[i])
        {
            success=false;
            std::cout<<"[fail!!!]:ChunkList.pushBack"<<std::endl;
            break;
        }
        p=p->next;
    }
    if(success&&memList._checkListAndNum())
    {
        std::cout<<"[success]:ChunkList.pushBack"<<std::endl;
    }
    //////////////////getBack////////////////////
    success=true;
    for(int i=0;i<N;i++)
    {
        MemObj* crr=memList.getBackChunk();
        if(crr!=&mem[N-1-i])
        {
            success=false;
            std::cout<<"[fail!!!]:ChunkList.getBackChunk"<<std::endl;
            break;
        }
    }
    if(success&&memList._checkListAndNum())
    {
        std::cout<<"[success]:ChunkList.getBackChunk"<<std::endl;
    }
    //////////////////multiPushFront////////////////////
    mem[0].prev=nullptr;
    for(int i=0;i<N-1;i++)
    {
        mem[i].next=&mem[i+1];
        mem[i+1].prev=&mem[i];
    }
    mem[N-1].next=nullptr;
    memList.multiPushFront(mem,mem+N-1,N);
    if(memList._checkListAndNum())
    {
        std::cout<<"[success]:ChunkList.multiPushFront"<<std::endl;
    }
    else
    {
        std::cout<<"[fail!!!]:ChunkList.multiPushFront"<<std::endl;
    }
    ////////////////////unlink//////////////////////
    success=true;
    for(int i=0;i<N;i++)
    {
        memList.unlink(mem+i);
    }
    if(memList._checkListAndNum()&&memList.size()==0)
    {
        std::cout<<"[success]:ChunkList.unlink"<<std::endl;
    }
    else
    {
        std::cout<<"[fail!!!]:ChunkList.unlink"<<std::endl;
    }
    /////////////////multiPushBack/////////////////////
    mem[0].prev=nullptr;
    for(int i=0;i<N-1;i++)
    {
        mem[i].next=&mem[i+1];
        mem[i+1].prev=&mem[i];
    }
    mem[N-1].next=nullptr;
    memList.multiPushBack(mem,mem+N-1,N);
    if(memList._checkListAndNum())
    {
        std::cout<<"[success]:ChunkList.multiPushBack"<<std::endl;
    }
    else
    {
        std::cout<<"[fail!!!]:ChunkList.multiPushBack"<<std::endl;
    }
    //////////////////radixTree//////////////////////////
    {
        RadixTree<3,12> tree;
        void* temp=sysMallocPage(1);
        long long pageId=((long long)temp)>>12;
        tree.insert(pageId,temp);
        void* getAdd=tree.getVal(pageId);
        if(temp==getAdd)
        {
            std::cout<<"[success]:RadixTree.insert and RadixTree.getVal"<<std::endl;
        }
        else
        {
            std::cout<<"[fail!!!]:RadixTree.insert and RadixTree.getVal"<<std::endl;
        }
    }
    std::cout<<"[success]:RadixTree.destructor"<<std::endl;

    return 0;
}