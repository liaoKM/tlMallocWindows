#include"core/spanlist.h"
#include"core/util.h"
#include"core/radixTree.h"
class PageCache
{
public:
    //static const int kMaxPageNumInSpan = 129;
    PageCache();
    Span* newSpan(int numOfPage);//大Span分裂为满足要求的Span--pageCache锁；如位申请内存，则list锁
    void retakeSpan(Span* span);//回收空闲的span,list锁
    Span* distribute(int numOfPage);//分发一个numOfPage大小的span，并从链中unlink；list锁
    static Span* pageid2Span(long long key)
    {
        return (Span*)radixtree_.getVal(key);
    }
    void releaseFreeMem(int threshhold= kReleaseThreshhold);//减少spanlist大小;threshhold=0:释放pageCache内所有span!!!;pageCache锁
    static PageCache& getPageCacheInst()
    {
        static PageCache pagecache;
        return pagecache;
    }
private:
    static const int kReleaseThreshhold = 8;
    Spanlist spanlist_[MAXPAGENUMINSPAN+1];
    static RadixTree<3, (48 - PAGESHIFT) / 3> radixtree_;
    std::mutex mutex_;
    Span* mallocSpan(int numOfPage);//申请新的span，并挂到链表,无锁
    bool deleteSpan(Span* p);//释放span内存到操作系统,无锁
};
//RadixTree<3,PAGESHIFT> PageCache::radixtree;