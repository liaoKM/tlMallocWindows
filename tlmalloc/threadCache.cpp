#include"threadCache.h"
#include"pageCache.h"
#include<algorithm>
#include<assert.h> 

void ThreadCache::get_span_from_pagecache(int size,int minNum)//try to get a span with size*minNum space.not guarantee
{
    assert(size< LOCALCHUNKTHRESHHOLD);
    int pageNum = minNum < 63 * 64 ? ceilInt(size * minNum , PAGESIZE)
        : std::round(size * 63 * 64 / (float)PAGESIZE);
    //int pageNum = ceilInt(size * minNum , PAGESIZE);
    int slot=size2slot(size);
    Span* span=PageCache::getPageCacheInst().distribute(pageNum);
    span->split(size);
    spanlist_[slot].push_back(span);//free spanlist put into back.
}
void ThreadCache::release_span_to_pagecache(Span* span)
{
    assert(span->isFree());
    int slot=size2slot(span->chunk_size());
    Spanlist& list= spanlist_[slot];
    list.unlink(span);
    span->merge();
    PageCache::getPageCacheInst().retakeSpan(span);
}
int ThreadCache::size2slot(int size)
{
    int slot=0;
    if(size <= MAXSMALLCHUNKSIZE)
    {
        slot=size/ SMALLCHUNKINCREASE;
    }
    else
    {
        slot=size/ LARGECHUNKINCREASE + MAXSMALLCHUNKSIZE / SMALLCHUNKINCREASE;
    }
    return slot;
}
void* ThreadCache::distribute(int size)
{
    int slot=size2slot(size);
    Spanlist& spl=spanlist_[slot];
    if(spl.empty())
    {
        int minNum=std::max(16*PAGESIZE/size,8);//16页(64K)或8个对象大小
        get_span_from_pagecache(size,minNum);//todo向CC申请
    }
    Span* sp= spl.front();
    void* mem = sp->distribute_chunk();

    if(sp->freelist_empty())//使用完的span从链表unlink
    {
        spl.unlink(sp);
    }

    return mem;
}
void ThreadCache::retake(Chunk* mem,Span* span)
{
    assert(PageCache::pageid2Span(((uint64_t)mem) >> PAGESHIFT) == span);
    span->retake(mem);

    
    if(span->freechunk_num()==1)//插入之前span是用完的
    {
        int slot=size2slot(span->chunk_size());
        spanlist_[slot].push_front(span);//剩余数量少，放于列表头，优先分配
    }
    else if(span->isFree())//触发回收
    {
        release_span_to_pagecache(span);
    }
    else
    {
        //对spanList排序，保证剩余块最少的span位于list头部
        int slot=size2slot(span->chunk_size());
        spanlist_[slot].sort_partial(span);
    }
}