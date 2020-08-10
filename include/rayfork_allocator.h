#ifndef RAYFORK_ALLOCATOR_H
#define RAYFORK_ALLOCATOR_H

#include "rayfork_common.h"

#define RF_DEFAULT_ALLOCATOR                           (RF_LIT(rf_allocator) { NULL, rf_libc_allocator_wrapper })
#define RF_NULL_ALLOCATOR                              (RF_LIT(rf_allocator) {0})

#define RF_CALLOC(allocator, size)                     rf_calloc_wrapper((allocator), 1, size)

#define RF_ALLOC(allocator, size)                      ((allocator).allocator_proc(&(allocator), RF_SOURCE_LOCATION, RF_AM_ALLOC,   (RF_LIT(rf_allocator_args) { 0, (size), 0 })))
#define RF_FREE(allocator, ptr)                        ((allocator).allocator_proc(&(allocator), RF_SOURCE_LOCATION, RF_AM_FREE,    (RF_LIT(rf_allocator_args) { (ptr), 0, 0 })))
#define RF_REALLOC(allocator, ptr, new_size, old_size) ((allocator).allocator_proc(&(allocator), RF_SOURCE_LOCATION, RF_AM_REALLOC, (RF_LIT(rf_allocator_args) { (ptr), (new_size), (old_size) })))

typedef enum rf_allocator_mode
{
    RF_AM_ALLOC,
    RF_AM_REALLOC,
    RF_AM_FREE,
} rf_allocator_mode;

typedef struct rf_allocator_args
{
    /*
     * In case of RF_AM_ALLOC this argument can be ignored.
     * In case of RF_AM_REALLOC this argument is the pointer to the buffer that must be reallocated.
     * In case of RF_AM_FREE this argument is the pointer that needs to be freed.
     */
    void* pointer_to_free_or_realloc;

    /*
     * In case of RF_AM_ALLOC this is the new size that needs to be allocated.
     * In case of RF_AM_REALLOC this is the new size that the buffer should have.
     * In case of RF_AM_FREE this argument can be ignored.
     */
    int size_to_allocate_or_reallocate;

    /*
     * In case of RF_AM_ALLOC this argument can be ignored.
     * In case of RF_AM_REALLOC this is the old size of the buffer.
     * In case of RF_AM_FREE this argument can be ignored.
     */
    int old_size;
} rf_allocator_args;

struct rf_allocator;

typedef void* (rf_allocator_proc)(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args);

typedef struct rf_allocator
{
    void* user_data;
    rf_allocator_proc* allocator_proc;
} rf_allocator;

RF_API void* rf_calloc_wrapper(rf_allocator allocator, int amount, int size);

RF_API void* rf_libc_allocator_wrapper(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args);

#endif // RAYFORK_ALLOCATOR_H