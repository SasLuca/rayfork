#ifndef RAYFORK_INTERNAL_BASE_H
#define RAYFORK_INTERNAL_BASE_H

#include "rayfork_base.h"
#include "string.h"
#include "math.h"

#pragma region internals

#ifndef RF_ASSERT
    #include "assert.h"
    #define RF_ASSERT(condition) assert(condition)
#endif

#if !defined(RF_NO_DEFAULT_ALLOCATOR)
RF_API void* _rf_libc_malloc_wrapper(void* user_data, int size_to_alloc)
{
    ((void)user_data);

    return malloc(size_to_alloc);
}

RF_API void _rf_libc_free_wrapper(void* user_data, void* ptr_to_free)
{
    ((void)user_data);

    free(ptr_to_free);
}
#endif

#if !defined(RF_NO_DEFAULT_IO)

RF_API int _rf_get_file_size(void* user_data, const char* filename)
{
    FILE* file = fopen(filename, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);

    return size;
}

RF_API bool _rf_load_file_into_buffer(void* user_data, const char* filename, void* dst, int dst_size)
{
    bool result = false;

    FILE* file = fopen(filename, "rb");
    if (file != NULL)
    {
        fseek(file, 0L, SEEK_END);
        int file_size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        if (dst_size >= file_size)
        {
            int read_size = fread(dst, 1, file_size, file);
            if (ferror(file) && read_size == file_size)
            {
                result = true;
            }
        }
        // else log_error buffer is not big enough
    }
    // else log error could not open file

    fclose(file);

    return true;
}

#endif // !defined(RF_NO_DEFAULT_IO)

RF_INTERNAL bool _rf_match_str_n(const char* a, int a_len, const char* b, int b_len)
{
    return a_len == b_len && strncmp(a, b, a_len) == 0;
}

RF_INTERNAL bool _rf_match_str_cstr(const char* a, int a_len, const char* b)
{
    return _rf_match_str_n(a, a_len, b, strlen(b));
}

RF_INTERNAL void* _rf_realloc_wrapper(rf_allocator allocator, void* source, int old_size, int new_size)
{
    void* new_alloc = RF_ALLOC(allocator, new_size);
    if (new_alloc && source && old_size) { memcpy(new_alloc, source, old_size); }
    if (source) { RF_FREE(allocator, source); }
    return new_alloc;
}

RF_INTERNAL void* _rf_calloc_wrapper(rf_allocator allocator, int amount, int size)
{
    void* ptr = RF_ALLOC(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

RF_INTERNAL int _rf_default_get_random_value(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

RF_INTERNAL bool _rf_is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return false;
    }

    return _rf_match_str_n(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

// String pointer reverse break: returns right-most occurrence of charset in s
RF_INTERNAL const char* _rf_strprbrk(const char* s, const char* charset)
{
    const char* latest_match = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latest_match = s++) { }
    return latest_match;
}

// Get directory for a given file_path
RF_INTERNAL const char* _rf_get_directory_path_from_file_path(const char* file_path)
{
    static RF_THREAD_LOCAL char _rf_global_dir_path[RF_MAX_FILEPATH_LEN];

    const char* last_slash = NULL;
    memset(_rf_global_dir_path, 0, RF_MAX_FILEPATH_LEN);

    last_slash = _rf_strprbrk(file_path, "\\/");
    if (!last_slash) { return NULL; }

    // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
    strncpy(_rf_global_dir_path, file_path, strlen(file_path) - (strlen(last_slash) - 1));
    _rf_global_dir_path[strlen(file_path) - strlen(last_slash)] = '\0'; // Add '\0' manually

    return _rf_global_dir_path;
}

#pragma endregion

#endif // RAYFORK_INTERNAL_BASE_H