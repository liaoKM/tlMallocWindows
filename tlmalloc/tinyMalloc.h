#include<memory.h>
class TinyMalloc
{
public:
    static void* malloc(size_t size);
    static void free(void* p);
};