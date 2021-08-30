#pragma once
//64位内存虚址空间  0x0000,0000,0000,0000----0x0000,7fff,ffff,f000
//64位：高16位为0，低12位为页内地址。因此，页号有效范围：36位
//N=2^12=4096（4K pagesize）
//sizeof(radixNode<12>)=64b*4K=8B*4K=32KB
#include"core/util.h"
#include<atomic>
template<int N>
struct radixNode
{
    void* slot[1 << N];
};

template<int DEPTH, int N>
class RadixTree
{
    int radixDepth;
    int slotNum;
    void** root;
    void* logHeapTop;
    inline void** getSlot(void** crrRoot, int slot)
    {
        return (void**)crrRoot[slot];

    }
    void** newSlot(void** crrRoot, int slot)
    {
        void* newNode = sysMallocPage((sizeof(void*) * slotNum + PAGESIZE - 1) / PAGESIZE);
        void* oldVal = CAS64<void*>((crrRoot + slot), nullptr, newNode);
        assert(crrRoot[slot] != nullptr);
        if (oldVal != nullptr)
        {
            sysFreePage(newNode, (sizeof(void*) * slotNum + PAGESIZE - 1) / PAGESIZE);
        }
        //crrRoot[slot] = sysMallocPage((sizeof(void*) * slotNum + PAGESIZE - 1) / PAGESIZE);
        return (void**)crrRoot[slot];
    }
    void freeNode(void** p, int d)
    {
        if (p != NULL && d < DEPTH)
        {
            for (int i = 0; i < slotNum; i++)
            {
                if (p[i] != nullptr)
                {
                    freeNode((void**)p[i], d + 1);
                }
            }
            sysFreePage(p, (sizeof(void*) * slotNum + PAGESIZE - 1) / PAGESIZE);
        }
        else
        {
            return;
        }
    }
public:
    RadixTree() :radixDepth(DEPTH), slotNum(1 << N)
    {
        root = (void**)systemMalloc(sizeof(void*) * slotNum);
        //logHeapTop=sbrk(0);
        for (int i = 0; i < slotNum; i++)
        {
            root[i] = nullptr;
        }
        return;
    }
    //key:pageId
    void* getVal(long long key)
    {
        void** pNode = root;
        for (int i = 0; i < DEPTH - 1; i++)
        {
            int shift = N * (DEPTH - 1 - i);
            int nodeKey = 0xfff & (key >> shift);
            pNode = getSlot(pNode, nodeKey);
        }
        int shift = 0;
        int nodeKey = 0xfff & (key >> shift);
        return pNode[nodeKey];
    }
    //key:pageId
    //value:span pointer
    void insert(long long key, void* value)
    {
        void** pNode = root;
        void** temp;
        for (int i = 0; i < DEPTH - 1; i++)
        {
            int shift = N * (DEPTH - 1 - i);
            int nodeKey = 0xfff & (key >> shift);
            temp = getSlot(pNode, nodeKey);
            if (temp == nullptr)
            {
                pNode = newSlot(pNode, nodeKey);
            }
            else
            {
                pNode = temp;
            }
        }
        int shift = 0;
        int nodeKey = 0xfff & (key >> shift);
        pNode[nodeKey] = value;
    }
    ~RadixTree()
    {
        freeNode(root, 0);
        root = nullptr;
    }
};