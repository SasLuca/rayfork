#ifndef RAYFORK_CORE_H
#define RAYFORK_CORE_H

#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"

#define RF_INVALID_INDEX (-1)

typedef ptrdiff_t rf_int;
typedef int rf_bool;

#pragma region internal flags and macros
#if defined(_MSC_VER)
    #define RAYFORK_MSVC
#elif defined(__clang__)
    #define RAYFORK_CLANG
#elif defined(__GNUC__)
    #define RAYFORK_GNUC
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define RAYFORK_PLATFORM_WINDOWS
#endif

#if defined(__linux__) && !defined(__ANDROID__)
    #define RAYFORK_PLATFORM_LINUX
#endif

#if defined(__linux__) && defined(__ANDROID__)
    #define RAYFORK_PLATFORM_ANDROID
#endif

#if defined(__APPLE__) && !TARGET_OS_IPHONE
    #define RAYFORK_PLATFORM_MACOS
#endif

#if defined(__APPLE__) && TARGET_OS_IPHONE
    #define RAYFORK_PLATFORM_IOS
#endif

#if (defined(RAYFORK_PLATFORM_WINDOWS) + defined(RAYFORK_PLATFORM_LINUX) + defined(RAYFORK_PLATFORM_MACOS) + defined(RAYFORK_PLATFORM_ANDROID) + defined(RAYFORK_PLATFORM_IOS)) != 1
    #error rayfork: none or more than one platforms defined.
#endif

#ifndef RF_EXTERN
    #ifdef __cplusplus
        #define RF_EXTERN extern "C"
    #else
        #define RF_EXTERN extern
    #endif
#endif

#if defined(RAYFORK_MSVC)
    #define RF_DLL_IMPORT  __declspec(dllimport)
    #define RF_DLL_EXPORT  __declspec(dllexport)
    #define RF_DLL_PRIVATE static
#else
    #if defined(RAYFORK_GNUC) || defined(RAYFORK_CLANG)
        #define RF_DLL_IMPORT  __attribute__((visibility("default")))
        #define RF_DLL_EXPORT  __attribute__((visibility("default")))
        #define RF_DLL_PRIVATE __attribute__((visibility("hidden")))
    #else
        #define RF_DLL_IMPORT
        #define RF_DLL_EXPORT
        #define RF_DLL_PRIVATE static
    #endif
#endif

#ifndef RF_API
    #if defined(RAYFORK_DLL)
        #ifdef RAYFORK_IMPLEMENTATION_FLAG
            #define RF_API RF_EXTERN RF_DLL_EXPORT
        #else
            #define RF_API RF_EXTERN RF_DLL_IMPORT
        #endif
    #else
        #define RF_API RF_EXTERN
    #endif
#endif

#ifndef RF_INTERNAL
    #if defined(RAYFORK_DLL)
        #define RF_INTERNAL RF_DLL_PRIVATE
    #else
        #define RF_INTERNAL static
    #endif
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
    #elif defined(RAYFORK_GNUC) || defined(RAYFORK_CLANG)
        #define RF_THREAD_LOCAL __thread
    #elif defined(RAYFORK_MSVC)
        #define RF_THREAD_LOCAL __declspec(thread)
    #endif
#endif

#define RF_CONCAT_IMPL(a, b) a##b
#define RF_CONCAT(a, b)      RF_CONCAT_IMPL(a, b)

/* This macro is used to name variables in custom macros such as custom for-each-loops */
#define RF_MACRO_VAR(X) RF_CONCAT(rf_macro_gen_var, RF_CONCAT(X, __LINE__))
#pragma endregion

#pragma region source location
typedef struct rf_source_location
{
    const char* file_name;
    const char* proc_name;
    rf_int line_in_file;
} rf_source_location;

#define RF_SOURCE_LOCATION (RF_LIT(rf_source_location) { __FILE__, __FUNCTION__, __LINE__ })
#pragma endregion

#pragma region allocator

#define RF_DEFAULT_ALLOCATOR                           (RF_LIT(rf_allocator) { 0, rf_libc_allocator_wrapper })
#define RF_CALLOC(allocator, size)                     rf_calloc_wrapper((allocator), 1, size)
#define RF_ALLOC(allocator, size)                      ((allocator).allocator_proc(&(allocator), RF_SOURCE_LOCATION, RF_AM_ALLOC,   (RF_LIT(rf_allocator_args) { 0, (size), 0 })))
#define RF_FREE(allocator, ptr)                        ((allocator).allocator_proc(&(allocator), RF_SOURCE_LOCATION, RF_AM_FREE,    (RF_LIT(rf_allocator_args) { (ptr), 0, 0 })))
#define RF_REALLOC(allocator, ptr, new_size, old_size) ((allocator).allocator_proc(&(allocator), RF_SOURCE_LOCATION, RF_AM_REALLOC, (RF_LIT(rf_allocator_args) { (ptr), (new_size), (old_size) })))

typedef enum rf_allocator_mode
{
    RF_AM_UNKNOWN = 0,
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
    rf_int size_to_allocate_or_reallocate;

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

RF_API void* rf_calloc_wrapper(rf_allocator allocator, rf_int amount, rf_int size);
RF_API void* rf_default_realloc(rf_allocator allocator, void* source, int old_size, int new_size);
RF_API void* rf_libc_allocator_wrapper(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args);

extern RF_THREAD_LOCAL rf_allocator rf__global_allocator_for_dependencies;
#define RF_SET_GLOBAL_DEPENDENCIES_ALLOCATOR(allocator) rf__global_allocator_for_dependencies = (allocator)

#pragma endregion

#pragma region io

#define RF_NULL_IO                                (RF_LIT(rf_io_callbacks) {0})
#define RF_FILE_SIZE(io, filename)                ((io).file_size_proc((io).user_data, filename))
#define RF_READ_FILE(io, filename, dst, dst_size) ((io).read_file_proc((io).user_data, filename, dst, dst_size))
#define RF_DEFAULT_IO (RF_LIT(rf_io_callbacks)    { NULL, rf_libc_get_file_size, rf_libc_load_file_into_buffer })

typedef struct rf_io_callbacks
{
    void*   user_data;
    rf_int  (*file_size_proc) (void* user_data, const char* filename);
    rf_bool (*read_file_proc) (void* user_data, const char* filename, void* dst, rf_int dst_size); // Returns true if operation was successful
} rf_io_callbacks;

RF_API rf_int  rf_libc_get_file_size(void* user_data, const char* filename);
RF_API rf_bool rf_libc_load_file_into_buffer(void* user_data, const char* filename, void* dst, rf_int dst_size);

#pragma endregion

#pragma region error

#define RF_RECORDED_ERROR(error_type) (RF_LIT(rf_recorded_error) { RF_SOURCE_LOCATION, error_type })

typedef enum rf_error_type
{
    RF_NO_ERROR,
    RF_BAD_ARGUMENT,
    RF_BAD_ALLOC,
    RF_BAD_IO,
    RF_BAD_BUFFER_SIZE,
    RF_BAD_FORMAT,
    RF_LIMIT_REACHED,
    RF_STBI_FAILED,
    RF_STBTT_FAILED,
    RF_UNSUPPORTED,
} rf_error_type;

typedef struct rf_recorded_error
{
    rf_source_location reported_source_location;
    rf_error_type error_type;
} rf_recorded_error;

RF_API rf_recorded_error rf_get_last_recorded_error();

extern RF_THREAD_LOCAL rf_recorded_error rf__last_error;

#pragma endregion

#pragma region logger

/*
 Note(LucaSas): MSVC, clang and gcc all deal with __VA_ARGS__ differently.
 Normally you would expect that __VA_ARGS__ consume a trailing comma but it doesn't, this is why we must ##__VA_ARGS__.
 ##__VA_ARGS__ is a preprocessor black magic which achieves this goal, it's not standard but every compiler supports it, if
 this causes issues on some compiler just disable logs with RF_DISABLE_LOGGER.
 Also bear in mind that ##__VA_ARGS__ still works differently between compilers but this code seems to work on all major compilers.
*/
#define RF_LOG(log_type, msg, ...)         rf__internal_log(RF_SOURCE_LOCATION, (log_type), (msg), ##__VA_ARGS__)
#define RF_LOG_ERROR(error_type, msg, ...) (RF_LOG(RF_LOG_TYPE_ERROR, (msg), (error_type), ##__VA_ARGS__), rf__last_error = RF_RECORDED_ERROR(error_type))

#define RF_DEFAULT_LOGGER (RF_LIT(rf_logger) { NULL, rf_libc_printf_logger })
#define RF_NULL_LOGGER    (RF_LIT(rf_logger) { NULL, NULL })

typedef enum rf_log_type
{
    RF_LOG_TYPE_NONE    = 0,
    RF_LOG_TYPE_DEBUG   = 0x1, // Useful mostly to rayfork devs
    RF_LOG_TYPE_INFO    = 0x2, // Information
    RF_LOG_TYPE_WARNING = 0x4, // Warnings about things to be careful about
    RF_LOG_TYPE_ERROR   = 0x8, // Errors that prevented functions from doing everything they advertised
    RF_LOG_TYPE_ALL     = RF_LOG_TYPE_DEBUG | RF_LOG_TYPE_INFO | RF_LOG_TYPE_WARNING | RF_LOG_TYPE_ERROR,
} rf_log_type;

struct rf_logger;

typedef void (*rf_log_proc)(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);

typedef struct rf_logger
{
    void* user_data;
    rf_log_proc log_proc;
} rf_logger;

RF_API const char* rf_log_type_string(rf_log_type);

RF_API void rf_set_logger(rf_logger logger);
RF_API void rf_set_logger_filter(rf_log_type);

RF_API rf_logger   rf_get_logger();
RF_API rf_log_type rf_get_log_filter();

RF_API void rf_libc_printf_logger(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);
RF_API void rf__internal_log(rf_source_location source_location, rf_log_type log_type, const char* msg, ...);

#pragma endregion

#pragma region assert

#if !defined(RF_ASSERT) && defined(RAYFORK_ENABLE_ASSERTIONS)
    #include "assert.h"
    #define RF_ASSERT(condition) assert(condition)
#else
    #define RF_ASSERT(condition)
#endif

#pragma endregion

#pragma region rng

#define RF_DEFAULT_RAND_PROC (rf_libc_rand_wrapper)

typedef rf_int (*rf_rand_proc)(rf_int min, rf_int max);

RF_API rf_int rf_libc_rand_wrapper(rf_int min, rf_int max);

#pragma endregion

#pragma region min max

RF_INTERNAL inline int rf_min_i(rf_int a, rf_int b) { return ((a) < (b) ? (a) : (b)); }
RF_INTERNAL inline int rf_max_i(rf_int a, rf_int b) { return ((a) > (b) ? (a) : (b)); }

RF_INTERNAL inline int rf_min_f(float a, float b) { return ((a) < (b) ? (a) : (b)); }
RF_INTERNAL inline int rf_max_f(float a, float b) { return ((a) > (b) ? (a) : (b)); }

#pragma endregion

#endif // RAYFORK_CORE_H