#ifndef RAYFORK_INTERNAL_BASE_H
#define RAYFORK_INTERNAL_BASE_H

RF_INTERNAL bool rf__match_str_n(const char* a, int a_len, const char* b, int b_len)
{
    return a_len == b_len && strncmp(a, b, a_len) == 0;
}

RF_INTERNAL bool rf__match_str_cstr(const char* a, int a_len, const char* b)
{
    return rf__match_str_n(a, a_len, b, strlen(b));
}

RF_INTERNAL void* rf__realloc_wrapper(rf_allocator allocator, void* source, int old_size, int new_size)
{
    void* new_alloc = RF_ALLOC(allocator, new_size);
    if (new_alloc && source && old_size) { memcpy(new_alloc, source, old_size); }
    if (source) { RF_FREE(allocator, source); }
    return new_alloc;
}

RF_INTERNAL void* rf__calloc_wrapper(rf_allocator allocator, int amount, int size)
{
    void* ptr = RF_ALLOC(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

RF_INTERNAL int rf__default_get_random_value(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

RF_INTERNAL bool rf__is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return false;
    }

    return rf__match_str_n(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

// String pointer reverse break: returns right-most occurrence of charset in s
RF_INTERNAL const char* rf__strprbrk(const char* s, const char* charset)
{
    const char* latest_match = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latest_match = s++) { }
    return latest_match;
}

// Get directory for a given file_path
RF_INTERNAL const char* rf__get_directory_path_from_file_path(const char* file_path)
{
    static RF_THREAD_LOCAL char rf__global_dir_path[RF_MAX_FILEPATH_LEN];

    const char* last_slash = NULL;
    memset(rf__global_dir_path, 0, RF_MAX_FILEPATH_LEN);

    last_slash = rf__strprbrk(file_path, "\\/");
    if (!last_slash) { return NULL; }

    // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
    strncpy(rf__global_dir_path, file_path, strlen(file_path) - (strlen(last_slash) - 1));
    rf__global_dir_path[strlen(file_path) - strlen(last_slash)] = '\0'; // Add '\0' manually

    return rf__global_dir_path;
}

#endif // RAYFORK_INTERNAL_BASE_H