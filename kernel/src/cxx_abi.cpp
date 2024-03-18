#include "include/cxx_abi.h"


// Default empty pure virtual function

extern "C"
{


void __cxa_pure_virtual()
{

}


atexit_func_entry_t __atexit_funcs[AtExitMaxFunctions];
uarch_t __atexit_func_count = 0;

void* __DsoHandle = nullptr;


int __cxa_at_exit(void(*fn)(void*), void* objPtr, void* dsoHandle)
{
    if (__atexit_func_count >= AtExitMaxFunctions) return -1;
    __atexit_funcs[__atexit_func_count].DestructorFn = fn;
    __atexit_funcs[__atexit_func_count].ObjPtr = objPtr;
    __atexit_funcs[__atexit_func_count].DsoHandle = dsoHandle;
    __atexit_func_count++;
    return 0;
}

void __cxa_finalize(void *fn)
{
    uarch_t i = __atexit_func_count;

    if (!fn)
    {
        while (i--)
        {
            if (__atexit_funcs[i].DestructorFn)
            {
                (*__atexit_funcs[i].DestructorFn)(__atexit_funcs[i].ObjPtr);
            }
        }

        return;
    }

    while (i--)
    {
        if (__atexit_funcs[i].DestructorFn == fn)
        {
            (*__atexit_funcs[i].DestructorFn)(__atexit_funcs[i].ObjPtr);
            __atexit_funcs[i].DestructorFn = nullptr;
        }
    }
}


}
