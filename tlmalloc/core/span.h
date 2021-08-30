#ifndef INCLUDESPAN
#define INCLUDESPAN
#include <cstdint>
#include "core/chunklist.h"
#include "core/util.h"
class Span
{
public:
    Span(int pNum);
    Span();
    bool split(int objSize);//大块的span分为小块的memobj并放入链表
    bool merge();//空闲的小块memobj合并
    bool isFree() const;
    bool freelist_empty() const { return freelist_.empty(); }
    void set_page_num(int pNum);
    uint64_t chunk_size() const { return chunk_size_; }
    uint64_t pageId() const { return pageId_; }
    uint64_t page_num() const { return page_num_; }
    uint64_t freechunk_num() const { return freelist_.size(); }
    //todo add retake
    Chunk* distribute_chunk();
    void retake(Chunk* chunk);
    //ChunkList& freelist() { return freelist_; }
    unsigned char* memspace() { return placeholder+sizeof(placeholder); }
    Span* next=nullptr;
    Span* prev=nullptr;
private:
    ChunkList freelist_;
    uint64_t chunk_size_ = 0;
    uint64_t pageId_ = 0;//span起始页号
    uint64_t page_num_ = 0;
    unsigned char placeholder[8];

    uint64_t chunk_num() const;

};

#endif