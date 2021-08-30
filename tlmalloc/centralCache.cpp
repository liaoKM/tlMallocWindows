#include"centralCache.h"
#include"pageCache.h"
#include<cmath>
#include <assert.h>
//#define CCGETSLOT(x) ((x)/4)
int CentralCache::size2slot(int size)
{
    int slot=0;
    if(size <= MAXSMALLCHUNKSIZE)
    {
        slot=size/ SMALLCHUNKINCREASE;
    }
    else
    {
        slot= kSmallSpanNum+size/ LARGECHUNKINCREASE;
    }
    return slot;
}
void CentralCache::allocateSpan(int pageNum,int objsize)//向spancache申请span，并分割为若干obj
{
    Span* span=PageCache::getPageCacheInst().distribute(pageNum);
    span->split(objsize);
    int slot=size2slot(objsize);
    assert(slot <= kSmallSpanNum + kLargeSpanNum);
    std::lock_guard<std::mutex> guard(spanList[slot].get_mutex());
    spanList[slot].push_front(span);
    return;
}
void CentralCache::releaseSpan(Span* span)//将Span释放给spancache
{
    span->merge();
    PageCache::getPageCacheInst().retakeSpan(span);
}

Chunk* CentralCache::distribute_obj(int size)//返回一个obj，大内存申请
{
    assert(size % LARGECHUNKINCREASE == 0);
    assert(size > LOCALCHUNKTHRESHHOLD);
    assert(size < SIZEOFSPANMAX-sizeof(Span));
    int slot=size2slot(size);
    Spanlist& splist = spanList[slot];
    Chunk* ret = nullptr;
    std::lock_guard<std::mutex> guard(splist.get_mutex());
    if (splist.empty())
    {
        int pageNum = ceilInt((size + sizeof(Span)) , PAGESIZE);
        Span* p = PageCache::getPageCacheInst().distribute(pageNum);
        p->split(size);
        spanList[slot].push_front(p);
    }
    assert(!splist.empty());
    ret = splist.distributeChunk();

    return ret;
}

void CentralCache::retake_obj(Chunk* p,Span* span)
{
    assert(span->chunk_size() <= SIZEOFSPANMAX - sizeof(Span));
    int slot=size2slot(span->chunk_size());
    std::lock_guard<std::mutex> guard(spanList[slot].get_mutex());
    span->retake(p);
    if(span->isFree())
    {
        spanList[slot].unlink(span);
        releaseSpan(span);
    }
    return;
}
