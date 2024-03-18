#pragma once

#include "include/types.h"

extern "C"
{

static constexpr uint64_t AtExitMaxFunctions = 128;

struct atexit_func_entry_t
{
    void (*DestructorFn)(void*);
    void* ObjPtr;
    void* DsoHandle;
};

int __cxa_atexit(void (*fn)(void*), void* objPtr, void* dsoHandle);
void __cxa_finalize(void *fn);


}
