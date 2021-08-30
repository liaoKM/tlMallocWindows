#pragma once
#include <cstdint>
struct Chunk
{
	Chunk* next;
};

class ChunkList
{
public:
    ChunkList();
    bool empty() const {return head_.next == nullptr;}
    uint64_t size() const { return size_; }
    Chunk* top() const { return head_.next; }
    Chunk* clear();
    void push_front_multi(Chunk* start, Chunk* end, uint64_t num);
    void push_front(Chunk* p);
    Chunk* pop_front();
private:
    Chunk head_;
    uint64_t size_;
};
