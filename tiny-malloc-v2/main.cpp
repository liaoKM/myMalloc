#include"tinyMalloc.h"
#include<iostream>
#include<vector>
#include<omp.h>

int main()
{
    std::vector<void*> vec;
    //test malloc tiny chunk once
    void* mem61B=TinyMalloc::malloc(61);//64B
    //test malloc chunk once
    void* mem100KB=TinyMalloc::malloc(100*1024);
    //test malloc large chunk once
    void* mem2G=TinyMalloc::malloc(2LL*1024*1024L*1024);

    ((char*)mem61B)[60]='a';
    ((char*)mem100KB)[2000-1]='a';
    ((char*)mem2G)[2*1024*1024*1024LL-1]='a';
    
    int TIMES=1024*1024*64;
    void** pMem=new void*[TIMES];
    double start,end;
    start=omp_get_wtime();
    //#pragma omp parallel for schedule(static,1024*1024) num_threads(6)
    for(int i=0;i<TIMES;i++)
    {
        pMem[i]=TinyMalloc::malloc(16);
    }
    end=omp_get_wtime();
    std::cout<<"TinyMalloc::malloc Time:"<<end-start<<std::endl;

    //test mem 
    /*std::cout<<"Testing memory: start"<<std::endl;
    std::cout<<"..."<<std::endl;
    for(int i=0;i<TIMES;i++)
    {
        ((char*)pMem[i])[0]=0;
    }
    std::cout<<"Testing memory: end"<<std::endl;*/


    start=omp_get_wtime();
    //#pragma omp parallel for schedule(static,1024*1024) num_threads(6)
    for(int i=0;i<TIMES;i++)
    {
        TinyMalloc::free(pMem[i]);
    }
    end=omp_get_wtime();
    std::cout<<"TinyMalloc::free Time:  "<<end-start<<std::endl;

    start=omp_get_wtime();
    //#pragma omp parallel for schedule(static,1024*1024) num_threads(6)
    for(int i=0;i<TIMES;i++)
    {
        pMem[i]=malloc(16);
    }
    end=omp_get_wtime();
    std::cout<<"malloc Time:            "<<end-start<<std::endl;

    start=omp_get_wtime();
    //#pragma omp parallel for schedule(static,1024*1024) num_threads(6)
    for(int i=0;i<TIMES;i++)
    {
        free(pMem[i]);
    }
    end=omp_get_wtime();
    std::cout<<"free Time:              "<<end-start<<std::endl;

    return 0;
}