#ifndef RAYFORK_FOUNDATION_ALLOCATOR_H
#define RAYFORK_FOUNDATION_ALLOCATOR_H

#include "basicdef.h"
#include "analysis-annotations.h"

/// A struct literal for an rf_allocator which wraps over libc's malloc/realloc/free
#define rf_default_allocator (rf_lit(rf_allocator) { 0, rf_libc_allocator_wrapper })

/// The rf_allocator procedure decides what it needs to do based on a variable of this type.
typedef enum rf_allocator_mode
{
    rf_allocator_mode_unknown = 0,
    rf_allocator_mode_alloc,
    rf_allocator_mode_free,
    rf_allocator_mode_realloc,
} rf_allocator_mode;

/// The arguments for the rf_allocator procedure.
typedef struct rf_allocator_args
{
    /// In case of rf_allocator_mode_alloc this argument can be ignored.
    /// In case of rf_allocator_mode_realloc this argument is the pointer to the buffer that must be reallocated.
    /// In case of rf_allocator_mode_free this argument is the pointer that needs to be freed.
    void* pointer_to_free_or_realloc;


    /// In case of rf_allocator_mode_alloc this is the new size that needs to be allocated.
    /// In case of rf_allocator_mode_realloc this is the new size that the buffer should have.
    /// In case of rf_allocator_mode_free this argument can be ignored.
    rf_int size_to_allocate_or_reallocate;

    /// In case of rf_allocator_mode_alloc this argument can be ignored.
    /// In case of rf_allocator_mode_realloc this is the current size of the buffer being reallocated. If the pointer is not null and the current_allocation_size is 0 then the allocator does not provide this information.
    /// In case of rf_allocator_mode_free this argument can be ignored.
    rf_int current_allocation_size;
} rf_allocator_args;

/// The result type of the rf_allocator procedure.
typedef struct rf_allocator_result
{
    void* pointer_to_new_buffer;
    rf_success_status success;
} rf_allocator_result;

// Forwards declaration
struct rf_allocator;

/// The rf_allocator procedure
typedef rf_allocator_result (rf_allocator_proc)(struct rf_allocator* this_allocator, rf_allocator_mode mode, rf_allocator_args args);

/// An allocator interface with a pointer to the allocator procedure and a pointer to custom user data for the allocator.
typedef struct rf_allocator
{
    void* user_data;
    rf_allocator_proc* proc;
} rf_allocator;

/// An implementation of the rf_libc_allocator_wrapper using libc's malloc/realloc/free. Use rf_default_allocator for a struct literal that provides an instance of this allocator.
rf_extern rf_allocator_result rf_libc_allocator_wrapper(rf_inout rf_allocator* this_allocator, rf_allocator_mode mode, rf_allocator_args args);

/// Simple wrapper for the alloc mode of the allocator.
rf_extern void* rf_alloc(rf_inout rf_allocator allocator, rf_int amount);

/// Simple wrapper for the free mode of the allocator.
rf_extern void  rf_free(rf_inout rf_allocator allocator, rf_inout void* ptr);

/// Simple implementation of calloc that works with any allocator.
rf_extern void* rf_calloc(rf_inout rf_allocator allocator, rf_int amount, rf_int size);

/// Simple wrapper for the realloc mode of the allocator that does *not* provide the currrent size of the allocation.
rf_extern void* rf_realloc_ez(rf_inout rf_allocator allocator, rf_inout void* source, rf_int new_size);

/// Simple wrapper for the realloc mode of the allocator that does provide the current size of the allocation.
rf_extern void* rf_realloc_ex(rf_inout rf_allocator allocator, rf_inout void* source, rf_int current_size, rf_int new_size);

/// Global thread-local variable used to set the allocator for a library running on the current thread.
/// This is set to the provided allocator before calling a thirdparty library and set to zero afterwards.
/// This is only used for single header libraries which implement custom allocators using global macro defines. (eg: stb_image)
rf_extern rf_thread_local rf_allocator rf_global_allocator_for_thirdparty_libraries;

#endif // RAYFORK_FOUNDATION_ALLOCATOR_H
