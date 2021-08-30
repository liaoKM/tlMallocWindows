#include"core/util.h"
//#include"centralCache.h"
#include"core/span.h"
#include"core/spanlist.h"
#define TCTHRESHHOLD 64*1024
#define TCLISTNUM (1024+60/4)+1
class ThreadCache
{
private:
    static const int kListNum = MAXSMALLCHUNKSIZE / SMALLCHUNKINCREASE + LOCALCHUNKTHRESHHOLD / LARGECHUNKINCREASE + 1;
    Spanlist spanlist_[kListNum];
    //从pageCache获取一个span，该span大小至少minNum*size
    void get_span_from_pagecache(int size,int minNum);
    //将空闲的span返回给PC
    void release_span_to_pagecache(Span* span);
    int size2slot(int size);
public:
    __declspec(noinline) void* distribute(int size);
    //回收内存
    //span：内存对应的span，由外层调用查找得到
    __declspec(noinline) void retake(Chunk* mem,Span* span);

};
//__thread static ThreadCache* pThreadCache=nullptr;//move to calss TinyMalloc