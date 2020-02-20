
#ifndef RAYFORK_COMMON_INTERNAL_H
#define RAYFORK_COMMON_INTERNAL_H

#include "rayfork_common.h"
#include "string.h"

#ifndef RF_ASSERT
    #include "assert.h"
    #define RF_ASSERT(condition) assert(condition)
#endif

#ifndef RF_NO_THREAD_LOCALS
    #if __cplusplus >= 201103L
        #define RF_THREAD_LOCAL thread_local
    #elif __STDC_VERSION_ >= 201112L
        #define RF_THREAD_LOCAL _Thread_local
    #elif defined(__GNUC__)
        #define RF_THREAD_LOCAL __thread
    #elif defined(_MSC_VER)
        #define RF_THREAD_LOCAL __declspec(thread)
    #endif
#endif

#ifndef RF_LOG
    #define RF_LOG(type, msg)
#endif

#ifndef RF_LOG_V
    #define RF_LOG_V(type, msg, ...)
#endif

extern rf_context* _rf_ctx;

RF_INTERNAL void* _rf_realloc_wrapper(rf_allocator allocator, const void* source, int old_size, int new_size)
{
    void* new_alloc = RF_ALLOC(allocator, new_size);
    memcpy(new_alloc, source, old_size);
    return new_alloc;
}

RF_INTERNAL bool _rf_strings_match(const char* a, int a_len, const char* b, int b_len)
{
    if (a_len != b_len)
    {
        return false;
    }

    const char* a_end  = a + a_len;

    while (a != a_end)
    {
        if (*a != *b)
        {
            return false;
        }

        a++;
        b++;
    }

    return true;
}

RF_INTERNAL bool _rf_is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return false;
    }

    return _rf_strings_match(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

#endif //#ifdef RAYFORK_COMMON_INTERNAL_H

