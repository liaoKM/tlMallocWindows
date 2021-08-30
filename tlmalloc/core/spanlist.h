#pragma once
#include"core/span.h"
#include<mutex>

class Spanlist
{
public:
	Spanlist();
	void set_page_num(uint64_t pageNum);

	//for threadcache distribute span
	void unlink(Span* span);
	void push_front(Span* span);
	void push_back(Span* span);
	Span* pop_front();
	Span* pop_back();
	Span* front() const { return head_.next; }
	Span* back() const { return head_.prev; }
	//for center cache distribute chunk
	Chunk* distributeChunk();
	void retakeChunk(Chunk* chunk, Span* span);
	std::mutex& get_mutex() { return mutex_; }
	//only call in threadCache::retake
	void sort_partial(Span* span);


	uint64_t size() const { return size_; }
	Span* clear();
	bool empty() const;
private:
	//std::atomic<uint64_t> size_;
	uint64_t size_;//we can make sure Spanlist is aligned,so uint64_t on x64 machine is atomic
	std::mutex mutex_;
	Span head_;
	void insert(Span* span,Span* pos);
};
