RF_INTERNAL RF_THREAD_LOCAL rf_allocator rf__global_allocator_for_dependencies;
#define RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(allocator) rf__global_allocator_for_dependencies = (allocator)

RF_API void* rf_calloc_wrapper(rf_allocator allocator, int amount, int size)
{
    void* ptr = RF_ALLOC(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

RF_API void* rf_libc_allocator_wrapper(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args)
{
    RF_ASSERT(this_allocator);

    void* result = 0;

    switch (mode)
    {
        case RF_AM_ALLOC:
            result = malloc(args.size_to_allocate_or_reallocate);
            break;

        case RF_AM_FREE:
            free(args.pointer_to_free_or_realloc);
            break;

        case RF_AM_REALLOC:
            result = realloc(args.pointer_to_free_or_realloc, args.size_to_allocate_or_reallocate);
            break;

        default: break;
    }

    return result;
}