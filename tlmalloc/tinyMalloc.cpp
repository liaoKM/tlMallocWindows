#include"tinyMalloc.h"
#include"pageCache.h"
#include"core/util.h"
#include"centralCache.h"
#include"threadCache.h"
#include<assert.h>
#ifdef __linux__ 
    static __thread ThreadCache* pThreadCache=nullptr;
#elif _WIN32
    static __declspec(thread) ThreadCache* pThreadCache = nullptr;
#endif
void newThreadCacheBySys()
{
    int pageNum=(sizeof(ThreadCache)+64+PAGESIZE-1)/PAGESIZE;
    pThreadCache =(ThreadCache*)sysMallocPage(pageNum);
    assert(pThreadCache);
    //((int*)mem)[0]=pageNum;
   // pThreadCache=(ThreadCache*)((char*)mem+64);
    new(pThreadCache) ThreadCache;
}
void* TinyMalloc::malloc(size_t size)
{
    if(pThreadCache==nullptr)
    {
        newThreadCacheBySys();
    }

    long long realSize=ceilMemSize(size);
    void* ret;
    if(size<64*1024)//向threadCache申请
    {
        ret=pThreadCache->distribute(realSize);
    }
    else if(size<= PAGESIZE * MAXPAGENUMINSPAN - sizeof(Span))//向centralCache申请
    {
        ret=(void*)CentralCache::getCCInst().distribute_obj(realSize);
    }
    else//系统申请
    {
        //增加64B(512b)头部，用于记录大小
        int pageNum=(realSize+512+PAGESIZE-1)/PAGESIZE;
        ret=sysMallocPage(pageNum);
        ((int*)ret)[0]=pageNum;
        ret= (char*)ret+64;
    }
    return ret;
}

#include<iostream>
void TinyMalloc::free(void* p)
{
    if(p==nullptr)
        return;
    Span* span=PageCache::pageid2Span(((long long)p)>>PAGESHIFT);
    if(span==NULL)//没有对应span，内存由系统分配
    {
        int pageNum=*((int*)((char*)p-64));
        sysFreePage((char*)p-64,pageNum);
    }
    else
    {
        int size=span->chunk_size();
        if(size< LOCALCHUNKTHRESHHOLD)
        {
            pThreadCache->retake((Chunk*)p,span);
        }
        else 
        {
            CentralCache::getCCInst().retake_obj((Chunk*)p,span);
        }
    }
}