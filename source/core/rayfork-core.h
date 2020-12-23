#ifndef RAYFORK_CORE_H
#define RAYFORK_CORE_H

#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"

#define rf_invalid_index (-1)

typedef ptrdiff_t rf_int;
typedef int rf_bool;

#pragma region internal flags and macros
#if defined(_MSC_VER)
    #define rayfork_msvc
#elif defined(__clang__)
    #define rayfork_clang
#elif defined(__GNUC__)
    #define rayfork_gnuc
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define rayfork_platform_windows
#endif

#if defined(__linux__) && !defined(__ANDROID__)
    #define rayfork_platform_linux
#endif

#if defined(__linux__) && defined(__ANDROID__)
    #define rayfork_platform_android
#endif

#if defined(__APPLE__) && !TARGET_OS_IPHONE
    #define rayfork_platform_macos
#endif

#if defined(__APPLE__) && TARGET_OS_IPHONE
    #define rayfork_platform_ios
#endif

#ifndef rf_extern
    #ifdef __cplusplus
        #define rf_extern extern "C"
    #else
        #define rf_extern extern
    #endif
#endif

#if defined(rayfork_msvc)
    #define rf_dll_import  __declspec(dllimport)
    #define rf_dll_export  __declspec(dllexport)
    #define rf_dll_private static
#else
    #if defined(rayfork_gnuc) || defined(rayfork_clang)
        #define rf_dll_import  __attribute__((visibility("default")))
        #define rf_dll_export  __attribute__((visibility("default")))
        #define rf_dll_private __attribute__((visibility("hidden")))
    #else
        #define rf_dll_import
        #define rf_dll_export
        #define rf_dll_private static
    #endif
#endif

#ifndef rf_public
    #if defined(rayfork_dll)
        #ifdef rayfork_implementation_flag
            #define rf_public rf_extern rf_dll_export
        #else
            #define rf_public rf_extern rf_dll_import
        #endif
    #else
        #define rf_public rf_extern
    #endif
#endif

#ifndef rf_internal
    #if defined(rayfork_dll)
        #define rf_internal rf_dll_private
    #else
        #define rf_internal static
    #endif
#endif

// Used to make constant literals work even in C++ mode
#ifdef __cplusplus
    #define rf_lit(type) type
#else
    #define rf_lit(type) (type)
#endif

#ifndef rf_thread_local
    #if __cplusplus >= 201103L
        #define rf_thread_local thread_local
    #elif __STDC_VERSION_ >= 201112L
        #define rf_thread_local _Thread_local
    #elif defined(rayfork_gnuc) || defined(rayfork_clang)
        #define rf_thread_local __thread
    #elif defined(rayfork_msvc)
        #define rf_thread_local __declspec(thread)
    #endif
#endif

#define rf_concat_impl(a, b) a##b
#define rf_concat(a, b)      rf_concat_impl(a, b)

/* This macro is used to name variables in custom macros such as custom for-each-loops */
#define rf_macro_var(X) rf_concat(rf_macro_gen_var, rf_concat(X, __LINE__))
#pragma endregion

#pragma region source location
typedef struct rf_source_location
{
    const char* file_name;
    const char* proc_name;
    rf_int line_in_file;
} rf_source_location;

#define rf_current_source_location (rf_lit(rf_source_location) { __FILE__, __FUNCTION__, __LINE__ })
#pragma endregion

#pragma region allocator

#define rf_old_allocation_size_unknown                 (-1)
#define rf_default_allocator                           (rf_lit(rf_allocator) { 0, rf_libc_allocator_wrapper })
#define rf_calloc(allocator, size)                     (rf_calloc_wrapper((allocator), 1, size))
#define rf_alloc(allocator, size)                      ((allocator).allocator_proc(&(allocator), rf_current_source_location, rf_allocator_mode_alloc,   (rf_lit(rf_allocator_args) { 0, (size), 0 })))
#define rf_free(allocator, ptr)                        ((allocator).allocator_proc(&(allocator), rf_current_source_location, rf_allocator_mode_free,    (rf_lit(rf_allocator_args) { (ptr), 0, 0 })))
#define rf_realloc(allocator, ptr, new_size, old_size) ((allocator).allocator_proc(&(allocator), rf_current_source_location, rf_allocator_mode_realloc, (rf_lit(rf_allocator_args) { (ptr), (new_size), (old_size) })))

typedef enum rf_allocator_mode
{
    rf_allocator_mode_unknown = 0,
    rf_allocator_mode_alloc,
    rf_allocator_mode_free,
    rf_allocator_mode_realloc,
} rf_allocator_mode;

typedef struct rf_allocator_args
{
    /*
     * In case of rf_allocator_mode_alloc this argument can be ignored.
     * In case of rf_allocator_mode_realloc this argument is the pointer to the buffer that must be reallocated.
     * In case of rf_allocator_mode_free this argument is the pointer that needs to be freed.
     */
    void* pointer_to_free_or_realloc;

    /*
     * In case of rf_allocator_mode_alloc this is the new size that needs to be allocated.
     * In case of rf_allocator_mode_realloc this is the new size that the buffer should have.
     * In case of rf_allocator_mode_free this argument can be ignored.
     */
    rf_int size_to_allocate_or_reallocate;

    /*
     * In case of rf_allocator_mode_alloc this argument can be ignored.
     * In case of rf_allocator_mode_realloc this is the old size of the buffer.
     * In case of rf_allocator_mode_free this argument can be ignored.
     */
    rf_int old_size;
} rf_allocator_args;

struct rf_allocator;

typedef void* (rf_allocator_proc)(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args);

typedef struct rf_allocator
{
    void* user_data;
    rf_allocator_proc* allocator_proc;
} rf_allocator;

rf_public void* rf_calloc_wrapper(rf_allocator allocator, rf_int amount, rf_int size);
rf_public void* rf_default_realloc(rf_allocator allocator, void* source, int old_size, int new_size);
rf_public void* rf_libc_allocator_wrapper(rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args);

rf_public rf_thread_local rf_allocator rf__global_allocator_for_dependencies;
#define rf_set_global_dependencies_allocator(allocator) rf__global_allocator_for_dependencies = (allocator)
#pragma endregion

#pragma region io
#define rf_file_size(io, filename)                ((io).file_size_proc((io).user_data, filename))
#define rf_read_file(io, filename, dst, dst_size) ((io).read_file_proc((io).user_data, filename, dst, dst_size))
#define rf_default_io                             (rf_lit(rf_io_callbacks) { 0, rf_libc_get_file_size, rf_libc_load_file_into_buffer })

typedef struct rf_io_callbacks
{
    void*   user_data;
    rf_int  (*file_size_proc) (void* user_data, const char* filename);
    rf_bool (*read_file_proc) (void* user_data, const char* filename, void* dst, rf_int dst_size); // Returns true if operation was successful
} rf_io_callbacks;

rf_public rf_int  rf_libc_get_file_size(void* user_data, const char* filename);
rf_public rf_bool rf_libc_load_file_into_buffer(void* user_data, const char* filename, void* dst, rf_int dst_size);
#pragma endregion

#pragma region error
#define rf_make_recorded_error(error_type) (rf_lit(rf_recorded_error) { rf_current_source_location, error_type })

typedef enum rf_error_type
{
    rf_no_error,
    rf_bad_argument,
    rf_bad_alloc,
    rf_bad_io,
    rf_bad_buffer_size,
    rf_bad_format,
    rf_limit_reached,
    rf_stbi_failed,
    rf_stbtt_failed,
    rf_unsupported,
} rf_error_type;

typedef struct rf_recorded_error
{
    rf_source_location reported_source_location;
    rf_error_type error_type;
} rf_recorded_error;

rf_public rf_recorded_error rf_get_last_recorded_error();

rf_public rf_thread_local rf_recorded_error rf__last_error;
#pragma endregion

#pragma region logger

/*
 Note(LucaSas): MSVC, clang and gcc all deal with __VA_ARGS__ differently.
 Normally you would expect that __VA_ARGS__ consume a trailing comma but it doesn't, this is why we must ##__VA_ARGS__.
 ##__VA_ARGS__ is a preprocessor black magic which achieves this goal, it's not standard but every compiler supports it, if
 this causes issues on some compiler just disable logs with RF_DISABLE_LOGGER.
 Also bear in mind that ##__VA_ARGS__ still works differently between compilers but this code seems to work on all major compilers.
*/
#define rf_default_logger                  (rf_lit(rf_logger) { 0, rf_libc_printf_logger })
#define rf_log(log_type, msg, ...)         (rf__internal_log(rf_current_source_location, (log_type), (msg), ##__VA_ARGS__))
#define rf_log_error(error_type, msg, ...) (rf_log(rf_log_type_error, (msg), (error_type), ##__VA_ARGS__), rf__last_error = rf_make_recorded_error(error_type))

typedef enum rf_log_type
{
    rf_log_type_none    = 0,
    rf_log_type_debug   = 0x1, // Useful mostly to rayfork devs
    rf_log_type_info    = 0x2, // Information
    rf_log_type_warning = 0x4, // Warnings about things to be careful about
    rf_log_type_error   = 0x8, // Errors that prevented functions from doing everything they advertised
    rf_log_type_all     = rf_log_type_debug | rf_log_type_info | rf_log_type_warning | rf_log_type_error,
} rf_log_type;

struct rf_logger;

typedef void (*rf_log_proc)(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);

typedef struct rf_logger
{
    void* user_data;
    rf_log_proc log_proc;
} rf_logger;

rf_public const char* rf_log_type_string(rf_log_type);

rf_public void rf_set_logger(rf_logger logger);
rf_public void rf_set_logger_filter(rf_log_type);

rf_public rf_logger   rf_get_logger();
rf_public rf_log_type rf_get_log_filter();

rf_public void rf_libc_printf_logger(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);
rf_public void rf__internal_log(rf_source_location source_location, rf_log_type log_type, const char* msg, ...);
#pragma endregion

#pragma region assert
#if !defined(rf_assert) && defined(rayfork_enable_assertions)
    #include "assert.h"
    #define rf_assert(condition) assert(condition)
#else
    #define rf_assert(condition)
#endif
#pragma endregion

#pragma region rng
#define rf_default_rand_proc (rf_libc_rand_wrapper)

typedef rf_int (*rf_rand_proc)(rf_int min, rf_int max);

rf_public rf_int rf_libc_rand_wrapper(rf_int min, rf_int max);
#pragma endregion

#pragma region min max
rf_internal inline int rf_min_i(rf_int a, rf_int b) { return ((a) < (b) ? (a) : (b)); }
rf_internal inline int rf_max_i(rf_int a, rf_int b) { return ((a) > (b) ? (a) : (b)); }

rf_internal inline int rf_min_f(float a, float b) { return ((a) < (b) ? (a) : (b)); }
rf_internal inline int rf_max_f(float a, float b) { return ((a) > (b) ? (a) : (b)); }
#pragma endregion

#endif // RAYFORK_CORE_H