#ifndef RAYFORK_COMMON_H
#define RAYFORK_COMMON_H

// Libc includes
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "float.h"
#include "math.h"

#if defined(_MSC_VER)
    #define RAYFORK_MSVC
#elif defined(__clang__)
    #define RAYFORK_CLANG
#elif defined(__GNUC__)
    #define RAYFORK_GNUC
#endif

#if !defined(RAYFORK_MSVC)
    #include "stdbool.h"
#elif !defined(__cplusplus)
    #if _MSC_VER >= 1800
        #include <stdbool.h>
    #else // stdbool.h missing prior to MSVC++ 12.0 (VS2013)
        #define bool char
        #define true 1
        #define false 0
    #endif
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
    #ifdef RAYFORK_DLL
        #ifdef RAYFORK__IMPLEMENTATION_FLAG
            #define RF_API RF_EXTERN RF_DLL_EXPORT
        #else
            #define RF_API RF_EXTERN RF_DLL_IMPORT
        #endif
    #else
        #define RF_API RF_EXTERN
    #endif
#endif

#ifndef RF_INTERNAL
    #ifdef RAYFORK_DLL
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

typedef struct rf_source_location
{
    const char* file_name;
    const char* proc_name;
    int line_in_file;
} rf_source_location;

#define RF_SOURCE_LOCATION (RF_LIT(rf_source_location) { __FILE__, __FUNCTION__, __LINE__ })

#endif // RAYFORK_COMMON_H