#include"core/chunklist.h"
ChunkList::ChunkList():size_(0)
{
    head_.next = nullptr;
}
Chunk* ChunkList::clear()
{
    Chunk* temp = head_.next;
    head_.next = nullptr;
    size_ = 0;
    return temp;
}
void ChunkList::push_front_multi(Chunk* start, Chunk* end,uint64_t num)
{
    end->next = head_.next;
    head_.next = start;
    size_ += num;
}
void ChunkList::push_front(Chunk* p)
{
    size_++;
    p->next = head_.next;
    head_.next = p;
}
Chunk* ChunkList::pop_front()
{
    size_--;
    Chunk* temp = head_.next;
    head_.next = temp->next;
    return temp;
}