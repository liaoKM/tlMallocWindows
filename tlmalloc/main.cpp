#include"tinyMalloc.h"
#include<iostream>
#include<vector>
#include<omp.h>
#include<random>
#include<memory.h>


static void random_new_delete(bool isPT=true) 
{
    const int kMaxOnHeap = 5000;
    const int kMaxRequestSize = 128;

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(0, kMaxOnHeap-1);
    std::uniform_int_distribution<int> distrSize(0, kMaxRequestSize-1);
    // We don't want random number generation to be a large part of
    // what we measure, so create a table of numbers now.
    const int kRandomTableSize = 98765;
    std::vector<int> random_index(kRandomTableSize);
    std::vector<int> random_request_size(kRandomTableSize);
    for (int i = 0; i < kRandomTableSize; i++) {
        random_index[i] = distr(eng);
        random_request_size[i] =distrSize(eng) + 1;
    }
    void* v[kMaxOnHeap];
    memset(v, 0, sizeof(v));

    void (*pFree) (void *);
    void* (*pMalloc) (size_t);
    if(isPT)
    {
        pFree=free;
        pMalloc=malloc;
    }
    else
    {
        pFree=TinyMalloc::free;
        pMalloc=TinyMalloc::malloc;
    }

    int r = 0;
    for (int i = 0; i < 1024*1024*128; i++) {
        int index = random_index[r];
        pFree(v[index]);
        v[index] = pMalloc(random_request_size[r]);
        if (++r == kRandomTableSize) {
         r = 0;
        }
    }
    for (int j = 0; j < kMaxOnHeap; j++) {
        pFree(v[j]);
    }
}

int main()
{
    std::vector<void*> vec;
    //test malloc tiny chunk once
    void* mem61B=TinyMalloc::malloc(61);//64B
    //test malloc chunk once
    void* mem100KB=TinyMalloc::malloc(100*1024);
    //test malloc large chunk once
    void* mem2G=TinyMalloc::malloc(2LL*1024*1024L*1024);

    ((char*)mem61B)[60]='a';
    ((char*)mem100KB)[2000-1]='a';
    ((char*)mem2G)[2*1024*1024*1024LL-1]='a';
    
    int TIMES=1024*1024*64;
    void** pMem=new void*[TIMES];
    double start,end;

    
    start = omp_get_wtime();
    #pragma omp parallel for schedule(static,1) num_threads(4)
    for(int i=0;i<4;i++)
    {
        random_new_delete(true);
    }
    end=omp_get_wtime();
    std::cout<<"random ptmallc Time:              "<<end-start<<std::endl;

    start=omp_get_wtime();
    #pragma omp parallel for schedule(static,1) num_threads(4)
    for(int i=0;i<4;i++)
    {
        random_new_delete(false);
    }
    end=omp_get_wtime();
    std::cout<<"random tinyMalloc Time:              "<<end-start<<std::endl;


    return 0;
}