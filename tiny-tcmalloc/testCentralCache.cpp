#include"centralCache.h"
//#include"pageCache.h"
#include<iostream>
int main()
{
    //std::cout<<sizeof(CentralCache)<<std::endl;
    //std::cout<<sizeof(PageCache)<<std::endl;
    CentralCache* cc=new CentralCache;
    //#pragma omp paralle for
    for(int i=0;i<10;i++)
    {
        bool success=true;
        ///////////getObj/////////////////////
        int objSize=ceilMemSize(32*sizeof(int));
        MemObj* p=cc->getObj(objSize);
        success=cc->checkSize(p,objSize)&&(cc->checkSpan(p)!=nullptr)&&(cc->checkObjInObjList(p)==false);
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.getObj"<<std::endl;
        }
        ///////////////retakeObj//////////
        cc->retakeObj(p,objSize);
        success=(cc->checkObjInObjList(p)==true);
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.retakeObj"<<std::endl;
        }
        ///////////////getMulitObj//////////
        success=true;
        MemObj* start,* end;
        objSize=sizeof(void*)*2;
        cc->getMultiObj(objSize,32,start,end);
        
        
        int count=1;
        for(MemObj* p=start;p!=end;p=p->next)
        {
            success=success&&(cc->checkSize(start,objSize)&&(cc->checkSpan(p)!=nullptr)&&(cc->checkObjInObjList(p)==false));
            count++;
        }
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.getMultiObj"<<std::endl;
        }
        ///////////////retakeMultiObj//////////
        success=true;
        cc->retakeMultiObj(start,end,count,objSize);
        for(MemObj* p=start;p!=end;p=p->next)
        {
            success=success&&(cc->checkObjInObjList(p)==true);
        }
        success=success&&(cc->checkObjInObjList(end)==true);
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.retakeMultiObj"<<std::endl;
        }
    }



    #pragma omp paralle for
    for(int i=0;i<1000;i++)
    {
        bool success=true;
        ///////////getObj/////////////////////
        int objSize=ceilMemSize(32*sizeof(int));
        MemObj* p=cc->getObj(objSize);
        success=cc->checkSize(p,objSize)&&(cc->checkSpan(p)!=nullptr);
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.getObj"<<std::endl;
        }
        ///////////////retakeObj//////////
        cc->retakeObj(p,objSize);
        /*success=(cc->checkObjInObjList(p)==true);
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.retakeObj"<<std::endl;
        }*/
        ///////////////getMulitObj//////////
        success=true;
        MemObj* start,* end;
        objSize=sizeof(void*)*2;
        cc->getMultiObj(objSize,32,start,end);
        
        
        int count=1;
        for(MemObj* p=start;p!=end;p=p->next)
        {
            success=success&&(cc->checkSize(start,objSize)&&(cc->checkSpan(p)!=nullptr));
            count++;
        }
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.getMultiObj"<<std::endl;
        }
        ///////////////retakeMultiObj//////////
        success=true;
        cc->retakeMultiObj(start,end,count,objSize);
        /*for(MemObj* p=start;p!=end;p=p->next)
        {
            success=success&&(cc->checkObjInObjList(p)==true);
        }
        success=success&&(cc->checkObjInObjList(end)==true);
        if(!success)
        {
            std::cout<<"[fail!!!]:CentralCache.retakeMultiObj"<<std::endl;
        }*/
    }
    return 0;
}