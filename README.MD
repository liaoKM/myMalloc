tiny-malloc 
===================
v1 版本
-------
参考tcmalloc，实现内存池。各线程内存池独立，申请释放五锁。小内存下速度比glibc malloc快  

存在问题：  
1.mem chunk使用双向链表不合理。  
    将span分成若干块chunk时，创建链表阶段内存写入密集。(一次加法运算，两次64b写入。)    
2.threadCache 内存回收部分，回收逻辑过于复杂。慢。    

v2 版本
---------
与常归内存库不同。v2版本内存申请释放 __前提__：__由线程a申请的内存必由线程a释放__。  
优化：  
    threadCache直接向spanCache申请span（tcmalloc：threadCache向centerCache申请objlist）,简化 __分配回收流程__ ，确保多线程申请释放小块内存并写入时 __不会出现cache一致性__ 问题。  

时间比较：  
    cpu: i7 8700k (6c12t)  

    TinyMalloc::malloc Time:0.135389
    TinyMalloc::free Time:  0.104028
    malloc Time:            0.497358
    free Time:              0.16263
    threads num:12

    TinyMalloc::malloc Time:0.15894
    TinyMalloc::free Time:  0.115987
    malloc Time:            0.471873
    free Time:              0.156196
    threads num:6

    TinyMalloc::malloc Time:0.671254
    TinyMalloc::free Time:  0.32608
    malloc Time:            1.26734
    free Time:              0.470851
    threads num:1

todo:  
    1.mem chunk部分改为单链表。  
    2.延后链表建立时间，当chunk被分配时才从span中割出。内存写入大小不变，但有利于cpu乱序执行掩盖内存io。  
    3.spanCache建立动态阈值，可以参考tcp拥塞控制。  
    4.乱入template，spanlist与chunklist单独实现。  