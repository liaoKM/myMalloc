#include"pageCache.h"
#include<iostream>
int main()
{   
    PageCache pageCache;
    #pragma omp parallel for
    for(int i=0;i<100;i++)
    {
        ////////////////////pageCache////////////    
        Span* sp=pageCache.distribute(4);
        int temp=sp->objSize;
        if(temp==-1)
        {
            //std::cout<<"[success]:PageCache.distribute & PageCache.newSpan"<<std::endl;
        }
        else
        {
            std::cout<<"[fail!!!]:PageCache.distribute or PageCache.newSpan"<<std::endl;
        }
        /////////////sapn split/////////////////
        bool success=true;
        //Span* sp=(Span*)sysMallocPage(4);
        //new(sp) Span(4);//4*4096B
        sp->split(512);
        void* start=(void*)sp+64;
        for(int i=0;i<sp->objNum;i++)
        {
            void* chunk=(void*)sp->objList.getFrontChunk();
            if(chunk!=start+512*i)
            {
                success=false;
                break;
            }
        }
        if(success)
        {
            //std::cout<<"[success]:Span.split"<<std::endl;
        }
        else
        {
            std::cout<<"[fail!!!]:Span.split"<<std::endl;
        }
        /////////////sapn merge/////////////////
        for(int i=0;i<sp->objNum;i++)
        {
            sp->objList.pushFront((MemObj*)(start+512*i));
        }
        success=true;
        sp->merge();
        if(sp->objList.size()==0&&sp->objNum==0&&(!sp->isSplited()))
        {
            //std::cout<<"[success]:Span.merge"<<std::endl;
        }
        else
        {
            std::cout<<"[fail!!!]:Span.merge"<<std::endl;
        }
        //////////////pageCache retake//////////////////////
        pageCache.retakeSpan(sp);
        //todo how to check retakeSpan;
        /////////////pageCache delete////////////////////

    }
    return 0;
}