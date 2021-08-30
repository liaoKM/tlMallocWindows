#ifndef INCLUDECC
#define INCLUDECC
#include"core/spanlist.h"
#include"core/span.h"

class CentralCache
{
    
public:
    static const int kSmallSpanNum = (MAXSMALLCHUNKSIZE / SMALLCHUNKINCREASE);
    static const int kLargeSpanNum = (SIZEOFSPANMAX - sizeof(Span)) / LARGECHUNKINCREASE;


    Chunk* distribute_obj(int size);
    //向centralCache申请多个obj，至少minNum个，所有obj来自同一span（避免threshCache中出现cache一致性问题）
    //一般用于分配给threadCache
    //size:上层调用保证 size==32b*N && size<64B
    //isEntire:返回的一整个span的所有obj(直接向PageCache申请)
    //返回值：返回Obj的个数
    //int getMultiObj(int size,int minNum,MemObj*& oStart,MemObj*& oEnd,bool isEntire=true);
    //回收obj
    //如果回收后span空闲，释放给pageCache
    void retake_obj(Chunk* p,Span* span);
    //返回静态centralCache对象
    static CentralCache& getCCInst()
    {
        static CentralCache cc;
        return cc;
    }

private:
    Spanlist spanList[kSmallSpanNum + kLargeSpanNum+1];//add 1 for [0]
    //向spancache申请span，并分割为若干obj
    void allocateSpan(int pageNum, int objsize);
    //将Span释放给spancache
    void releaseSpan(Span* span);
    int size2slot(int size);
};
#endif