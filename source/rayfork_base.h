#ifndef RAYFORK_BASE_H
#define RAYFORK_BASE_H

#include "stdbool.h"

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

#ifndef RF_LOG_IMPL_V
    #define RF_LOG_IMPL_V(type, msg, file, line, proc_name, ...)
#endif

#define RF_LOG_V(type, msg, ...)             RF_LOG_IMPL_V(type, "[file: " __FILE__ "][line " __LINE__ "][proc: " __FUNCTION__ "]" msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define RF_LOG(type, msg)                    RF_LOG_IMPL_V(type, "[file: " __FILE__ "][line " __LINE__ "][proc: " __FUNCTION__ "]" msg, __FILE__, __LINE__, __FUNCTION__)
#define RF_LOG_ERROR(error_type, msg)        RF_LOG(RF_LOG_TYPE_ERROR, "[ERROR]" msg)
#define RF_LOG_ERROR_V(error_type, msg, ...) RF_LOG_V(RF_LOG_TYPE_ERROR, "[ERROR]" msg, __VA_ARGS__)

#define RF_NULL_ALLOCATOR           (RF_LIT(rf_allocator) {0})
#define RF_ALLOC(allocator, size)   ((allocator).alloc_proc((allocator).user_data, (size)))
#define RF_CALLOC(allocator, size)  _rf_calloc_wrapper((allocator), 1, size)
#define RF_FREE(allocator, pointer) ((allocator).free_proc((allocator).user_data, (pointer)))

#define RF_NULL_IO                                (RF_LIT(rf_io_callbacks) {0})
#define RF_FILE_SIZE(io, filename)                ((io).file_size_proc((io).user_data, filename))
#define RF_READ_FILE(io, filename, dst, dst_size) ((io).read_file_proc((io).user_data, filename, dst, dst_size))

#define RF_MIN(a, b) ((a) < (b) ? (a) : (b))
#define RF_MAX(a, b) ((a) > (b) ? (a) : (b))

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

#if !defined(RF_NO_DEFAULT_ALLOCATOR)

#include "stdlib.h"

#define RF_DEFAULT_ALLOCATOR (RF_LIT(rf_allocator) { NULL, _rf_libc_malloc_wrapper, _rf_libc_free_wrapper })

RF_API void* _rf_libc_malloc_wrapper(void* user_data, int size_to_alloc);
RF_API void  _rf_libc_free_wrapper(void* user_data, void* ptr_to_free);

#endif // !defined(RF_NO_DEFAULT_ALLOCATOR)

#if !defined(RF_NO_DEFAULT_IO)

#include "stdio.h"

#define RF_DEFAULT_IO (RF_LIT(rf_io_callbacks) { NULL, _rf_get_file_size, _rf_load_file_into_buffer })

RF_API int _rf_get_file_size(void* user_data, const char* filename);

RF_API bool _rf_load_file_into_buffer(void* user_data, const char* filename, void* dst, int dst_size);

#endif // !defined(RF_NO_DEFAULT_IO)

#pragma endregion

#endif // RAYFORK_BASE_H