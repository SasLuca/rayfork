#ifndef RAYFORK_FOUNDATION_BASICDEF_H
#define RAYFORK_FOUNDATION_BASICDEF_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef ptrdiff_t rf_int;
typedef size_t    rf_usize;

typedef enum rf_success_status
{
    rf_unsuccessful = 0,
    rf_successful   = 1,
} rf_success_status;

typedef enum rf_valid_t
{
    rf_invalid = 0,
    rf_valid   = 1,
} rf_valid_t;

#define rf_invalid_index (-1)

#ifndef rf_unused
    #define rf_unused(variable) ((void)(variable))
#endif

#ifndef rf_inline
    #define rf_inline static inline
#endif

#if __STDC_VERSION__ >= 201112L
    #define rf_c_version 11
    #define rf_c11
#else
    #define rf_c_version 99
    #define rf_c99
#endif

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

#ifndef rf_extern
    #if defined(rayfork_dll)
        #ifdef rayfork_implementation_flag
            #define rf_extern rf_extern rf_dll_export
        #else
            #define rf_extern rf_extern rf_dll_import
        #endif
    #else
        #define rf_extern rf_extern
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
#define rf_macro_var(x) rf_concat(rf_macro_gen_var, rf_concat(x, __LINE__))

#endif // RAYFORK_FOUNDATION_BASICDEF_H