#include "rayfork/foundation/allocator.h"

#include "string.h"
#include "malloc.h"

rf_extern rf_allocator_result rf_libc_allocator_wrapper(rf_inout rf_allocator* this_allocator, rf_allocator_mode mode, rf_allocator_args args)
{
    rf_unused(this_allocator);

    rf_allocator_result result = {0};

    switch (mode)
    {
        case rf_allocator_mode_alloc:
            result.pointer_to_new_buffer = malloc(args.size_to_allocate_or_reallocate);
            result.success = rf_successful;
            break;

        case rf_allocator_mode_free:
            free(args.pointer_to_free_or_realloc);
            result.success = rf_successful;
            break;

        case rf_allocator_mode_realloc:
            result.pointer_to_new_buffer = realloc(args.pointer_to_free_or_realloc, args.size_to_allocate_or_reallocate);
            result.success = rf_successful;
            break;

        default: break;
    }

    return result;
}

rf_extern void* rf_alloc(rf_allocator allocator, rf_int amount)
{
    rf_allocator_result result = allocator.proc(&allocator, rf_allocator_mode_alloc, (rf_allocator_args) { .size_to_allocate_or_reallocate = amount });
    return result.pointer_to_new_buffer;
}

rf_extern void  rf_free(rf_allocator allocator, rf_inout void* ptr)
{
    allocator.proc(&allocator, rf_allocator_mode_free, (rf_allocator_args) { .pointer_to_free_or_realloc = ptr });
}

rf_extern void* rf_calloc(rf_allocator allocator, rf_int amount, rf_int size)
{
    void* ptr = rf_alloc(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

rf_extern void* rf_realloc_ez(rf_allocator allocator, rf_inout void* source, rf_int new_size)
{
    rf_allocator_result result = allocator.proc(&allocator, rf_allocator_mode_realloc, (rf_allocator_args) { 
        .pointer_to_free_or_realloc = source,
        .size_to_allocate_or_reallocate = new_size,
    });
    
    return result.pointer_to_new_buffer;
}

rf_extern void* rf_realloc_ex(rf_allocator allocator, rf_inout void* source, rf_int current_size, rf_int new_size)
{
    rf_allocator_result result = allocator.proc(&allocator, rf_allocator_mode_realloc, (rf_allocator_args) { 
        .pointer_to_free_or_realloc = source,
        .size_to_allocate_or_reallocate = new_size, 
        .current_allocation_size = current_size,
    });
    
    return result.pointer_to_new_buffer;
}

rf_thread_local rf_allocator rf_global_allocator_for_thirdparty_libraries;
