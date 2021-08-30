#include"pageCache.h"
#include<assert.h>
RadixTree<3,PAGESHIFT> PageCache::radixtree_;

PageCache::PageCache()
{
    for (int i = 1; i <= MAXPAGENUMINSPAN; i++)
    {
        spanlist_[i].set_page_num(i);
    }
}

Span* PageCache::mallocSpan(int numOfPage)
{
    assert(numOfPage <= MAXPAGENUMINSPAN);
    Span* newSpan=(Span*)sysMallocPage(numOfPage);//todo 重载 Span::operator new!!
    new(newSpan) Span(numOfPage);
    for(int i=0;i<numOfPage;i++)//在前缀树中注册每一页
    {
        radixtree_.insert((((long long)newSpan)>>PAGESHIFT)+i,newSpan);
    }
    return newSpan;
}
bool PageCache::deleteSpan(Span* p)
{
    sysFreePage(p,p->page_num());//todo 前缀树中删除
    return true;
}


void PageCache::retakeSpan(Span* span)
{
    int pageNum=span->page_num();
    std::lock_guard<std::mutex> gurad(spanlist_[pageNum].get_mutex());
    if(spanlist_[pageNum].size()< kReleaseThreshhold)
    {
        spanlist_[pageNum].push_front(span);
    }
    else
    {
        deleteSpan(span);
    }
    return;
}
Span* PageCache::newSpan(int numOfPage)
{
    //todo 优先从更大的Span分裂！！！ 对pageCache 枷锁
    Span* ret=mallocSpan(numOfPage);
    std::lock_guard<std::mutex> gurad(spanlist_[numOfPage].get_mutex());
    spanlist_[numOfPage].push_front(ret);
    return ret;
}

void PageCache::releaseFreeMem(int threadhold)
{
    for(int i=0;i< MAXPAGENUMINSPAN;i++)
    {
        std::lock_guard<std::mutex> gurad(spanlist_[i].get_mutex());
        while(spanlist_[i].size()>threadhold)
        {
            deleteSpan(spanlist_[i].pop_front());
        }
    }
    return;
}

Span* PageCache::distribute(int numOfPage)//分发一个numOfPage大小的span，并从链中unlink
{
    assert(numOfPage <= MAXPAGENUMINSPAN);
    Span* ret=NULL;
    std::lock_guard<std::mutex> gurad(spanlist_[numOfPage].get_mutex());
    if(spanlist_[numOfPage].empty())
    {
        ret=mallocSpan(numOfPage);
    }
    else
    {
        ret= spanlist_[numOfPage].pop_front();
    }
    
    return ret;
}

