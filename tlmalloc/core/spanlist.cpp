#include"core/spanlist.h"
#include<assert.h>
#include<mutex>
#ifndef NDEBUG
	#define checkList \
	{\
		Span * p = head_.next;\
		Span* end = &head_;\
		int count = 0;\
		for (Span* p = head_.next; p != end; p = p->next)\
		{\
			count++;\
		}\
		assert(count == size_);\
		count = 0;\
		for (Span* p = head_.prev; p != end; p = p->prev)\
		{\
			count++;\
		}\
		assert(count == size_);\
	}
#else
	#define checkList 
#endif

Spanlist::Spanlist():head_(),mutex_()
{
	assert(((uint64_t)this & 0b11) == 0);//make sure this is aligned!!
	head_.next = &head_;
	head_.prev = &head_;
	size_ = 0;
}
void Spanlist::set_page_num(uint64_t pageNum)
{
	head_.set_page_num(pageNum);
}
void Spanlist::unlink(Span* span)
{
	checkList
	assert(span->prev);//assert span is in list
	assert(span->next);
	//assert(head_.page_num() == span->page_num());
	size_--;
	span->prev->next = span->next;
	span->next->prev = span->prev;
	checkList
}
void Spanlist::insert(Span* span, Span* pos)
{
	checkList
	size_++;
	span->prev = pos;
	span->next = pos->next;
	span->next->prev = span;
	pos->next = span;
	checkList

}
void Spanlist::push_front(Span* span)
{
	insert(span, &head_);
}
void Spanlist::push_back(Span* span)
{
	insert(span, head_.prev);
}
Span* Spanlist::pop_front()
{
	Span* top = head_.next;
	unlink(top);
	return top;
}
Span* Spanlist::pop_back()
{
	Span* bott = head_.prev;
	unlink(bott);
	return bott;
}
bool Spanlist::empty() const
{
	assert((size_ == 0) == (head_.next == &head_));
	return size_ == 0;
}
Span* Spanlist::clear()
{
	Span* temp = head_.next;
	temp->prev = head_.prev;
	temp->prev->next = temp;

	head_.next = &head_;
	head_.prev = &head_;
	size_ = 0;
	return temp;
}
Chunk* Spanlist::distributeChunk()
{ 
	Chunk* ret = head_.next->distribute_chunk();
	return ret;
}
void Spanlist::retakeChunk(Chunk* chunk,Span* span)
{
	//assert(span->page_num() == head_.page_num());
	span->retake(chunk);
	return;
}
void Spanlist::sort_partial(Span* span)
{
	assert(span->prev == &head_ || span->chunk_size() == span->prev->chunk_size());
	assert(span->next == &head_ || span->chunk_size() == span->next->chunk_size());
	Span* pos = span->next;
	assert(span->prev==&head_|| span->next == &head_ ||span->prev->freechunk_num() <= span->next->freechunk_num());
	bool unorder=false;
	while(pos !=&head_&&span->freechunk_num()> pos->freechunk_num())
	{
		unorder = true;
		pos = pos->next;
	}
	if(unorder)
	{
		unlink(span);
		insert(span, pos->prev);
	}
	return;
}
