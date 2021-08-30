#include"core/span.h"
#include<assert.h>
Span::Span(int pNum) :page_num_(pNum), freelist_(), placeholder{ 0 }
{
    pageId_ = ((uint64_t)this) >> PAGESHIFT;
}
Span::Span() : page_num_(0), freelist_(), placeholder{ 0 }
{
    pageId_ = 0;
}
void Span::set_page_num(int pNum)
{
    page_num_ = pNum;
}

uint64_t Span::chunk_num() const
{
    uint64_t chunknum=(page_num_ * PAGESIZE - sizeof(Span)) / chunk_size_;
    return chunknum;
}
bool Span::split(int chunksize)//大块的span分为小块的memobj并放入链表
{
    assert(chunk_size_ == 0);

    //todo 对齐
    chunk_size_= chunksize;

    unsigned char* userSpace= memspace();
    uint64_t num = chunk_num();
    //init mem_
    for(uint64_t i=0;i<num-1;i++)
    {
        ((Chunk*)(userSpace + i * chunk_size_))->next = (Chunk*)(userSpace + (i + 1) * chunk_size_);
    }
    //push to free list
    freelist_.push_front_multi((Chunk*)userSpace, (Chunk*)(userSpace + (num - 1) * chunk_size_), num);
    return true;
}
bool Span::merge()//空闲的小块memobj合并
{
    if(isFree())
    {
        freelist_.clear();
        chunk_size_ = 0;
        return true;
    }
    else
    {
        return false;
    }
}
bool Span::isFree() const
{
    return freelist_.size() == chunk_num();
}
Chunk* Span::distribute_chunk()
{
    return freelist_.pop_front();
}
void Span::retake(Chunk* chunk)
{
    assert(((uint64_t)chunk >> PAGESHIFT) >= pageId_);
    assert(((uint64_t)chunk >> PAGESHIFT) < pageId_+page_num_);
    freelist_.push_front(chunk);
}
