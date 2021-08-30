#include"util.h"

#ifdef __linux__ 
#include <unistd.h>
#include <sys/mman.h>
#include <error.h>
#elif _WIN32
#include <windows.h>
#endif

//sbrk申请堆内存，程序结束前不释放
void* systemMalloc(int size)
{
    //todo !!!
    return VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
}

//mmap申请内存，用于创建span
void* sysMallocPage(int nPage)
{
#ifdef __linux__ 
    return mmap(0, (long long)nPage*PAGESIZE , PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE,-1, 0);
#elif _WIN32
    return VirtualAlloc(nullptr, (long long)nPage * PAGESIZE, MEM_COMMIT, PAGE_READWRITE);

#endif
}
//mmap释放内存
bool sysFreePage(void* p,int nPage)
{
#ifdef __linux__ 
    return munmap(p,(long long)nPage*PAGESIZE)!=-1;
#elif _WIN32

    return VirtualFree(p, 0, MEM_RELEASE);
#endif
}
//内存大小取整
long long ceilMemSize(long long memSize)
{
    if(memSize < MINCHUNKSIZE)
    {
        memSize= MINCHUNKSIZE;//最小8B,单链表指针最少8B
    }
    else if(memSize< LARGECHUNKINCREASE)
    {
        memSize=(ceilInt(memSize , (float)SMALLCHUNKINCREASE))* SMALLCHUNKINCREASE;//memsize对齐4B
    }
    else//最大512b对齐 即64B
    {
        memSize= ceilInt(memSize,(float)LARGECHUNKINCREASE)* LARGECHUNKINCREASE;
    }
    return memSize;
}

uint64_t ceilInt(uint64_t a, uint64_t b)
{
    return ((a + b - 1) / b);
}