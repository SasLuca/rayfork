RF_API void* rf_calloc_wrapper(rf_allocator allocator, int amount, int size)
{
    void* ptr = RF_ALLOC(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

RF_API void* rf_libc_malloc_wrapper(void* user_data, int size_to_alloc)
{
    ((void)user_data);
    return malloc(size_to_alloc);
}

RF_API void rf_libc_free_wrapper(void* user_data, void* ptr_to_free)
{
    ((void)user_data);
    free(ptr_to_free);
}