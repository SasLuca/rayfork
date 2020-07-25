#ifndef RAYFORK_ALLOCATOR_H
#define RAYFORK_ALLOCATOR_H

#include "rayfork_common.h"

#define RF_DEFAULT_ALLOCATOR (RF_LIT(rf_allocator) { NULL, rf_libc_malloc_wrapper, rf_libc_free_wrapper })
#define RF_NULL_ALLOCATOR           (RF_LIT(rf_allocator) {0})
#define RF_ALLOC(allocator, size)   ((allocator).alloc_proc((allocator).user_data, (size)))
#define RF_CALLOC(allocator, size)  rf_calloc_wrapper((allocator), 1, size)
#define RF_FREE(allocator, pointer) ((allocator).free_proc((allocator).user_data, (pointer)))

typedef struct rf_allocator
{
    void* user_data;
    void* (*alloc_proc) (void* user_data, int size_to_alloc);
    void  (*free_proc) (void* user_data, void* ptr_to_free);
} rf_allocator;

RF_API void* rf_calloc_wrapper(rf_allocator allocator, int amount, int size);

RF_API void* rf_libc_malloc_wrapper(void* user_data, int size_to_alloc);
RF_API void  rf_libc_free_wrapper(void* user_data, void* ptr_to_free);

#endif // RAYFORK_ALLOCATOR_H