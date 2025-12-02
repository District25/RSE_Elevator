#if defined(USE_PLATFORM_F7_DISCO_ZEPHYR_SYSMEM)

#include <cstdlib>
#include <cassert>
#include <zephyr/kernel.h>

/*
 * Redirect 'new' and 'delete' operators to Zephr's 'system heap'.
 */

void * operator new(size_t size)
{
   void * p;

   p = k_malloc(size);

   return p;
}

void * operator new[](size_t size)
{
    return operator new(size);
}

void operator delete(void * p)
{
    k_free(p);
}

void operator delete[](void * ptr)
{
    operator delete(ptr);
}

#endif // USE_PLATFORM_F7_DISCO_ZEPHYR_SYSMEM
