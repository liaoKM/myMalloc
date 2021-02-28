#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <error.h>


using namespace std;

int main()
{
   int pagesize=sysconf(_SC_PAGESIZE);

   int a=0;
   cout<<"------------stack---------------"<<endl;
   cout<<&a<<endl;
   //sbrk
   cout<<"----------sbrk---------------"<<endl;
   void* startBrk=sbrk(0);
   cout<<startBrk<<endl;
   void* sBrkRet=sbrk(64);
   cout<<sBrkRet<<endl;
   void* sbrkEnd=sbrk(0);
   cout<<sbrkEnd<<endl;
   ((int*)startBrk)[0]=10;//读写测试
   cout<<((int*)startBrk)[0]<<endl;//读写测试
   //mmap
   cout<<"----------mmap---------------"<<endl;
   int* mmap1=(int*)mmap(0, pagesize , PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE,-1, 0); 
   cout<<mmap1<<endl;
   int* mmap2=(int*)mmap(0, pagesize , PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE,-1, 0); 
   cout<<mmap2<<endl;
   int* mmap3=(int*)mmap(0, pagesize , PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE,-1, 0); 
   cout<<mmap3<<endl;

   ///////////////brk/////////////////
   cout<<"----------brk---------------"<<endl;
   if(brk(startBrk)!=-1)
   {
      cout<<"sbrk success"<<endl;
      //cout<<((int*)sBrkRet)[0]<<endl;//segmentation fault
   }
   else
   {
      cout<<"sbrk fail"<<endl;
      //cout<<errno<<endl;
      perror("");
   }

   if(brk(sbrkEnd)!=-1)
   {
      cout<<"sbrk success"<<endl;
      cout<<((int*)sBrkRet)[0]<<endl;//测试
   }
   else
   {
      cout<<"sbrk fail"<<endl;
      //cout<<errno<<endl;
      perror("");
   }
   
   cout<<"----------munmap---------------"<<endl;
   if(munmap(mmap3,pagesize)!=-1)
   {
      cout<<"munmap success"<<endl;
      //cout<<mmap3[0]<<endl;//segmentation fault
   }
   else
   {
      cout<<"munmap fail"<<endl;
      //cout<<errno<<endl;
      perror("");
   }
   if(munmap(mmap3+16,pagesize)!=-1)//地址偏移导致错误
   {
      cout<<"munmap success"<<endl;
   }
   else
   {
      cout<<"munmap fail"<<endl;
      //cout<<errno<<endl;
      perror("");
   }
   return 0;
}