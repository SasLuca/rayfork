#ifndef RAYFORK_BASE_H
#define RAYFORK_BASE_H

#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#pragma region macros

#ifndef RF_API
    #ifdef __cplusplus
        #define RF_API extern "C"
    #else
        #define RF_API extern
    #endif
#endif

#ifndef RF_INTERNAL
    #define RF_INTERNAL static
#endif

// Used to make constant literals work even in C++ mode
#ifdef __cplusplus
    #define RF_LIT(type) type
#else
    #define RF_LIT(type) (type)
#endif

#ifndef RF_THREAD_LOCAL
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

#ifndef RF_ASSERT
    #include "assert.h"
    #define RF_ASSERT(condition) assert(condition)
#endif

#ifndef RF_LOG_IMPL_V
    #define RF_LOG_IMPL_V(type, msg, file, line, proc_name, ...)
#endif

#define RF_LOG_V(type, msg, ...)             RF_LOG_IMPL_V(type, "[file: " __FILE__ "][line " __LINE__ "][proc: " __FUNCTION__ "]" msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define RF_LOG(type, msg)                    RF_LOG_IMPL_V(type, "[file: " __FILE__ "][line " __LINE__ "][proc: " __FUNCTION__ "]" msg, __FILE__, __LINE__, __FUNCTION__)
#define RF_LOG_ERROR(error_type, msg)        RF_LOG(RF_LOG_TYPE_ERROR, "[ERROR]" msg)
#define RF_LOG_ERROR_V(error_type, msg, ...) RF_LOG_V(RF_LOG_TYPE_ERROR, "[ERROR]" msg, __VA_ARGS__)

#define RF_NULL_ALLOCATOR           (RF_LIT(rf_allocator) {0})
#define RF_ALLOC(allocator, size)   ((allocator).alloc_proc((allocator).user_data, (size)))
#define RF_CALLOC(allocator, size)  rf__calloc_wrapper((allocator), 1, size)
#define RF_FREE(allocator, pointer) ((allocator).free_proc((allocator).user_data, (pointer)))

#define RF_NULL_IO                                (RF_LIT(rf_io_callbacks) {0})
#define RF_FILE_SIZE(io, filename)                ((io).file_size_proc((io).user_data, filename))
#define RF_READ_FILE(io, filename, dst, dst_size) ((io).read_file_proc((io).user_data, filename, dst, dst_size))

static inline int rf_min_i(int a, int b) { return ((a) < (b) ? (a) : (b)); }
static inline int rf_max_i(int a, int b) { return ((a) > (b) ? (a) : (b)); }

static inline int rf_min_f(float a, float b) { return ((a) < (b) ? (a) : (b)); }
static inline int rf_max_f(float a, float b) { return ((a) > (b) ? (a) : (b)); }

#ifndef RF_MAX_FILEPATH_LEN
    #define RF_MAX_FILEPATH_LEN (1024)
#endif

#pragma endregion

#pragma region enums

typedef enum rf_error_type
{
    RF_BAD_ARGUMENT,
    RF_BAD_ALLOC,
    RF_BAD_IO,
    RF_BAD_BUFFER_SIZE,
    RF_BAD_FORMAT,
    RF_STBI_FAILED,
    RF_STBTT_FAILED,
    RF_UNSUPPORTED,
} rf_error_type;

typedef enum rf_log_type
{
    RF_LOG_TYPE_DEBUG, // Useful mostly to rayfork devs
    RF_LOG_TYPE_INFO, // Information
    RF_LOG_TYPE_WARNING, // Warnings about things to be careful about
    RF_LOG_TYPE_ERROR, // Errors that prevented functions from doing everything they advertised
} rf_log_type;

#pragma endregion

#pragma region io and allocators

typedef struct rf_io_callbacks
{
    void* user_data;
    int (*file_size_proc) (void* user_data, const char* filename);
    bool (*read_file_proc) (void* user_data, const char* filename, void* dst, int dst_size); // Returns true if operation was successful
} rf_io_callbacks;

typedef struct rf_allocator
{
    void* user_data;
    void* (*alloc_proc) (void* user_data, int size_to_alloc);
    void (*free_proc) (void* user_data, void* ptr_to_free);
} rf_allocator;

typedef int (*rf_rand_proc)(int min, int max);

#define RF_DEFAULT_ALLOCATOR (RF_LIT(rf_allocator) { NULL, rf_libc_malloc_wrapper, rf_libc_free_wrapper })
static inline void* rf_libc_malloc_wrapper(void* user_data, int size_to_alloc)
{
    ((void)user_data);
    return malloc(size_to_alloc);
}

static inline void  rf_libc_free_wrapper(void* user_data, void* ptr_to_free)
{
    ((void)user_data);
    free(ptr_to_free);
}

#define RF_DEFAULT_IO (RF_LIT(rf_io_callbacks) { NULL, rf_get_file_size, rf_load_file_into_buffer })
static inline int rf_get_file_size(void* user_data, const char* filename)
{
    ((void)user_data);

    FILE* file = fopen(filename, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);

    return size;
}

static inline bool rf_load_file_into_buffer(void* user_data, const char* filename, void* dst, int dst_size)
{
    ((void)user_data);
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

    return result;
}

#define RF_DEFAULT_RAND_PROC (rf_default_rand_proc)
static inline int rf_default_rand_proc(int min, int max)
{
    return rand() % (max + 1 - min) + min;
}

#pragma endregion

#endif // RAYFORK_BASE_H