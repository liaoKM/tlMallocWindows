#ifndef UTILHEAD
#define UTILHEAD


#include<cmath>
#include<algorithm>
#include<intrin.h>
#include<assert.h>


#define PAGESIZE 4096
#define PAGESHIFT 12
#define MAXPAGENUMINSPAN 128
//#define SPANHEADSIZE 64 sizeof(Span)

#define MINCHUNKSIZE 8
#define SMALLCHUNKINCREASE 4//smallchunk在thread cache中增长步长
#define LARGECHUNKINCREASE 64//largechunk在增长步长
#define MAXSMALLCHUNKSIZE LARGECHUNKINCREASE-SMALLCHUNKINCREASE//smallchunk 最大尺寸

#define LOCALCHUNKTHRESHHOLD (64*1024)//thtreadCache阈值，超过则在centercache中分配
#define SIZEOFSPANMAX (PAGESIZE*(MAXPAGENUMINSPAN))//一个span的最大尺寸，包含头部


//sbrk申请堆内存，程序结束前不释放
void* systemMalloc(int size);
//mmap申请内存，用于创建span
void* sysMallocPage(int nPagesize);
//mmap释放内存
bool sysFreePage(void* p,int nPagesize);

//内存大小取整
//8B以内 取8B
//64B以 4B取整
//64B到pageSize*(MAXSPANSIZE-1)-SPANHEADSIZE 64B对齐
//大于pageSize*(MAXSPANSIZE-1)-SPANHEADSIZE的，操作系统分配
long long ceilMemSize(long long memSize);

uint64_t ceilInt(uint64_t a, uint64_t b);
template<typename T>
T CAS64(T* source, T expected, T val)
{
	assert(sizeof(T) == 8);
#ifdef _MSC_VER
	return (T)_InterlockedCompareExchange64((__int64*)source, (__int64)val, (__int64)expected);
#elif __GNUC__

#endif
}

#endif