#ifndef RAYFORK_COMMON_H
#define RAYFORK_COMMON_H

// Libc includes
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "math.h"
#include "float.h"

#if !defined(_MSC_VER)
    #include "stdbool.h"
#else
    #if _MSC_VER >= 1800
        #include <stdbool.h>
    #else // stdbool.h missing prior to MSVC++ 12.0 (VS2013)
        #define bool int
        #define true 1
        #define false 0
    #endif
#endif

#ifndef RF_API
    #ifdef RAYFORK_DLL
        #ifdef RAYFORK_IMPLEMENTATION
            #ifdef __cplusplus
                #define RF_API extern "C" __declspec(dllexport)
            #else
                #define RF_API __declspec(dllexport)
            #endif
        #else
            #ifdef __cplusplus
                #define RF_API extern "C" __declspec(dllimport)
            #else
                #define RF_API __declspec(dllimport)
            #endif
        #endif
    #else
        #ifdef __cplusplus
            #define RF_API extern "C"
        #else
            #define RF_API extern
        #endif
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
    #elif defined(__GNUC__) /* note that ICC (linux) and Clang are covered by __GNUC__ */
        #define RF_THREAD_LOCAL __thread
    #elif defined(_MSC_VER)
        #define RF_THREAD_LOCAL __declspec(thread)
    #endif
#endif

#define RF_CONCAT2(a, b) a##b
#define RF_CONCAT(a, b) RF_CONCAT2(a, b)

#endif // RAYFORK_COMMON_H