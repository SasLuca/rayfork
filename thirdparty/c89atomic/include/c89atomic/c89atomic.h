/*
C89 compatible atomics. Choice of public domain or MIT-0. See license statements at the end of this file.

David Reid - mackron@gmail.com
*/

/*
Introduction
============
This library aims to implement an equivalent to the C11 atomics library. It's intended to be used as a way to
enable the use of atomics in a mostly consistent manner to modern C, while still enabling compatibility with
older compilers. This is *not* a drop-in replacement for C11 atomics, but is very similar. Only limited testing
has been done so use at your own risk. I'm happy to accept feedback and pull requests with bug fixes.

When compiling with GCC and Clang, this library will translate to a one-to-one wrapper around the __atomic_*
intrinsics provided by the compiler.

When compiling with Visual C++ things are a bit more complicated because it does not have support for C11 style
atomic intrinsics. This library will try to use the _Interlocked* intrinsics instead, and if unavailable will
use inlined assembly (x86 only).

Supported compilers are Visual Studio back to VC6, GCC and Clang. If you need support for a different compiler
I'm happy to add support (pull requests appreciated). This library currently assumes the `int` data type is
32 bits.


Differences With C11
--------------------
For praticality, this is not a drop-in replacement for C11's `stdthread.h`. Below are the main differences
between c89thread and stdthread.

    * All operations require an explicit size which is specified by the name of the function, and only 8-,
      16-, 32- and 64-bit operations are supported. Objects of an arbitrary sizes are not supported.
    * Some extra APIs are included:
      - `c89atomic_compare_and_swap_*()`
      - `c89atomic_test_and_set_*()`
      - `c89atomic_clear_*()`
    * All APIs are namespaced with `c89`.
    * `c89atomic_*` data types are undecorated.


Compare Exchange
----------------
This library implements a simple compare-and-swap function called `c89atomic_compare_and_swap_*()` which is
slightly different to C11's `atomic_compare_exchange_*()`. This is named differently to distinguish between
the two. `c89atomic_compare_and_swap_*()` returns the old value as the return value, whereas
`atomic_compare_exchange_*()` will return it through a parameter and supports explicit memory orders for
success and failure cases.

With Visual Studio and versions of GCC earlier than 4.7, an implementation of `atomic_compare_exchange_*()`
is included which is implemented in terms of `c89atomic_compare_and_swap_*()`, but there's subtle details to be
aware of with this implementation. Note that the following only applies for Visual Studio and versions of GCC
earlier than 4.7. Later versions of GCC and Clang use the `__atomic_compare_exchange_n()` intrinsic directly
and are not subject to the following.

Below is the 32-bit implementation of `c89atomic_compare_exchange_strong_explicit_32()` which is implemented
the same for the weak versions and other sizes, and only for Visual Studio and old versions of GCC (prior to
4.7):

```c
c89atomic_bool c89atomic_compare_exchange_strong_explicit_32(volatile c89atomic_uint32* dst,
                                                             volatile c89atomic_uint32* expected,
                                                             c89atomic_uint32 desired,
                                                             c89atomic_memory_order successOrder,
                                                             c89atomic_memory_order failureOrder)
{
    c89atomic_uint32 expectedValue;
    c89atomic_uint32 result;

    expectedValue = c89atomic_load_explicit_32(expected, c89atomic_memory_order_seq_cst);
    result = c89atomic_compare_and_swap_32(dst, expectedValue, desired);
    if (result == expectedValue) {
        return 1;
    } else {
        c89atomic_store_explicit_32(expected, result, failureOrder);
        return 0;
    }
}
```

The call to `c89atomic_store_explicit_32()` is not atomic with respect to the main compare-and-swap operation
which may cause problems when `expected` points to memory that is shared between threads. This only becomes an
issue if `expected` can be accessed from multiple threads at the same time which for the most part will never
happen because a compare-and-swap will almost always be used in a loop with a local variable being used for the
expected value.

If the above is a concern, you should consider reworking your code to use `c89atomic_compare_and_swap_*()`
directly, which is atomic and more efficient. Alternatively you'll need to use a lock to synchronize access
to `expected`, upgrade your compiler, or use a different library.


Types and Functions
-------------------
The following types and functions are implemented:

+-----------------------------------------+-----------------------------------------------+
| C11 Atomics                             | C89 Atomics                                   |
+-----------------------------------------+-----------------------------------------------+
| #include <stdatomic.h>                  | #include "c89atomic.h"                        |
+-----------------------------------------+-----------------------------------------------+
| memory_order                            | c89atomic_memory_order                        |
|     memory_order_relaxed                |     c89atomic_memory_order_relaxed            |
|     memory_order_consume                |     c89atomic_memory_order_consume            |
|     memory_order_acquire                |     c89atomic_memory_order_acquire            |
|     memory_order_release                |     c89atomic_memory_order_release            |
|     memory_order_acq_rel                |     c89atomic_memory_order_acq_rel            |
|     memory_order_seq_cst                |     c89atomic_memory_order_seq_cst            |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag                             | c89atomic_flag                                |
| atomic_bool                             | c89atomic_bool                                |
| atomic_int8                             | c89atomic_int8                                |
| atomic_uint8                            | c89atomic_uint8                               |
| atomic_int16                            | c89atomic_int16                               |
| atomic_uint16                           | c89atomic_uint16                              |
| atomic_int32                            | c89atomic_int32                               |
| atomic_uint32                           | c89atomic_uint32                              |
| atomic_int64                            | c89atomic_int64                               |
| atomic_uint64                           | c89atomic_uint64                              |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag_test_and_set                | c89atomic_flag_test_and_set                   |
| atomic_flag_test_and_set_explicit       | c89atomic_flag_test_and_set_explicit          |
|                                         | c89atomic_test_and_set_8                      |
|                                         | c89atomic_test_and_set_16                     |
|                                         | c89atomic_test_and_set_32                     |
|                                         | c89atomic_test_and_set_64                     |
|                                         | c89atomic_test_and_set_explicit_8             |
|                                         | c89atomic_test_and_set_explicit_16            |
|                                         | c89atomic_test_and_set_explicit_32            |
|                                         | c89atomic_test_and_set_explicit_64            |
+-----------------------------------------+-----------------------------------------------+
| atomic_flag_clear                       | c89atomic_flag_clear                          |
| atomic_flag_clear_explicit              | c89atomic_flag_clear_explicit                 |
|                                         | c89atomic_clear_8                             |
|                                         | c89atomic_clear_16                            |
|                                         | c89atomic_clear_32                            |
|                                         | c89atomic_clear_64                            |
|                                         | c89atomic_clear_explicit_8                    |
|                                         | c89atomic_clear_explicit_16                   |
|                                         | c89atomic_clear_explicit_32                   |
|                                         | c89atomic_clear_explicit_64                   |
+-----------------------------------------+-----------------------------------------------+
| atomic_store                            | c89atomic_store_8                             |
| atomic_store_explicit                   | c89atomic_store_16                            |
|                                         | c89atomic_store_32                            |
|                                         | c89atomic_store_64                            |
|                                         | c89atomic_store_explicit_8                    |
|                                         | c89atomic_store_explicit_16                   |
|                                         | c89atomic_store_explicit_32                   |
|                                         | c89atomic_store_explicit_64                   |
+-----------------------------------------+-----------------------------------------------+
| atomic_load                             | c89atomic_load_8                              |
| atomic_load_explicit                    | c89atomic_load_16                             |
|                                         | c89atomic_load_32                             |
|                                         | c89atomic_load_64                             |
|                                         | c89atomic_load_explicit_8                     |
|                                         | c89atomic_load_explicit_16                    |
|                                         | c89atomic_load_explicit_32                    |
|                                         | c89atomic_load_explicit_64                    |
+-----------------------------------------+-----------------------------------------------+
| atomic_exchange                         | c89atomic_exchange_8                          |
| atomic_exchange_explicit                | c89atomic_exchange_16                         |
|                                         | c89atomic_exchange_32                         |
|                                         | c89atomic_exchange_64                         |
|                                         | c89atomic_exchange_explicit_8                 |
|                                         | c89atomic_exchange_explicit_16                |
|                                         | c89atomic_exchange_explicit_32                |
|                                         | c89atomic_exchange_explicit_64                |
+-----------------------------------------+-----------------------------------------------+
| atomic_compare_exchange_weak            | c89atomic_compare_exchange_weak_8             |
| atomic_compare_exchange_weak_explicit   | c89atomic_compare_exchange_weak_16            |
| atomic_compare_exchange_strong          | c89atomic_compare_exchange_weak_32            |
| atomic_compare_exchange_strong_explicit | c89atomic_compare_exchange_weak_64            |
|                                         | c89atomic_compare_exchange_weak_explicit_8    |
|                                         | c89atomic_compare_exchange_weak_explicit_16   |
|                                         | c89atomic_compare_exchange_weak_explicit_32   |
|                                         | c89atomic_compare_exchange_weak_explicit_64   |
|                                         | c89atomic_compare_exchange_strong_8           |
|                                         | c89atomic_compare_exchange_strong_16          |
|                                         | c89atomic_compare_exchange_strong_32          |
|                                         | c89atomic_compare_exchange_strong_64          |
|                                         | c89atomic_compare_exchange_strong_explicit_8  |
|                                         | c89atomic_compare_exchange_strong_explicit_16 |
|                                         | c89atomic_compare_exchange_strong_explicit_32 |
|                                         | c89atomic_compare_exchange_strong_explicit_64 |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_add                        | c89atomic_fetch_add_8                         |
| atomic_fetch_add_explicit               | c89atomic_fetch_add_16                        |
|                                         | c89atomic_fetch_add_32                        |
|                                         | c89atomic_fetch_add_64                        |
|                                         | c89atomic_fetch_add_explicit_8                |
|                                         | c89atomic_fetch_add_explicit_16               |
|                                         | c89atomic_fetch_add_explicit_32               |
|                                         | c89atomic_fetch_add_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_sub                        | c89atomic_fetch_sub_8                         |
| atomic_fetch_sub_explicit               | c89atomic_fetch_sub_16                        |
|                                         | c89atomic_fetch_sub_32                        |
|                                         | c89atomic_fetch_sub_64                        |
|                                         | c89atomic_fetch_sub_explicit_8                |
|                                         | c89atomic_fetch_sub_explicit_16               |
|                                         | c89atomic_fetch_sub_explicit_32               |
|                                         | c89atomic_fetch_sub_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_or                         | c89atomic_fetch_or_8                          |
| atomic_fetch_or_explicit                | c89atomic_fetch_or_16                         |
|                                         | c89atomic_fetch_or_32                         |
|                                         | c89atomic_fetch_or_64                         |
|                                         | c89atomic_fetch_or_explicit_8                 |
|                                         | c89atomic_fetch_or_explicit_16                |
|                                         | c89atomic_fetch_or_explicit_32                |
|                                         | c89atomic_fetch_or_explicit_64                |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_xor                        | c89atomic_fetch_xor_8                         |
| atomic_fetch_xor_explicit               | c89atomic_fetch_xor_16                        |
|                                         | c89atomic_fetch_xor_32                        |
|                                         | c89atomic_fetch_xor_64                        |
|                                         | c89atomic_fetch_xor_explicit_8                |
|                                         | c89atomic_fetch_xor_explicit_16               |
|                                         | c89atomic_fetch_xor_explicit_32               |
|                                         | c89atomic_fetch_xor_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_fetch_and                        | c89atomic_fetch_and_8                         |
| atomic_fetch_and_explicit               | c89atomic_fetch_and_16                        |
|                                         | c89atomic_fetch_and_32                        |
|                                         | c89atomic_fetch_and_64                        |
|                                         | c89atomic_fetch_and_explicit_8                |
|                                         | c89atomic_fetch_and_explicit_16               |
|                                         | c89atomic_fetch_and_explicit_32               |
|                                         | c89atomic_fetch_and_explicit_64               |
+-----------------------------------------+-----------------------------------------------+
| atomic_thread_fence()                   | c89atomic_thread_fence                        |
| atomic_signal_fence()                   | c89atomic_signal_fence                        |
+-----------------------------------------+-----------------------------------------------+
| atomic_is_lock_free                     | c89atomic_is_lock_free_8                      |
|                                         | c89atomic_is_lock_free_16                     |
|                                         | c89atomic_is_lock_free_32                     |
|                                         | c89atomic_is_lock_free_64                     |
+-----------------------------------------+-----------------------------------------------+
| (Not Defined)                           | c89atomic_compare_and_swap_8                  |
|                                         | c89atomic_compare_and_swap_16                 |
|                                         | c89atomic_compare_and_swap_32                 |
|                                         | c89atomic_compare_and_swap_64                 |
|                                         | c89atomic_compare_and_swap_ptr                |
|                                         | c89atomic_compiler_fence                      |
+-----------------------------------------+-----------------------------------------------+
*/

#ifndef c89atomic_h
#define c89atomic_h

#if defined(__cplusplus)
extern "C" {
#endif

typedef   signed char           c89atomic_int8;
typedef unsigned char           c89atomic_uint8;
typedef   signed short          c89atomic_int16;
typedef unsigned short          c89atomic_uint16;
typedef   signed int            c89atomic_int32;
typedef unsigned int            c89atomic_uint32;
#if defined(_MSC_VER)
    typedef   signed __int64    c89atomic_int64;
    typedef unsigned __int64    c89atomic_uint64;
#else
    #if defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wlong-long"
        #if defined(__clang__)
            #pragma GCC diagnostic ignored "-Wc++11-long-long"
        #endif
    #endif
    typedef   signed long long  c89atomic_int64;
    typedef unsigned long long  c89atomic_uint64;
    #if defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif
#endif

typedef int                     c89atomic_memory_order;
typedef unsigned char           c89atomic_bool;
typedef unsigned char           c89atomic_flag;

/* Architecture Detection */
#if !defined(C89ATOMIC_64BIT) && !defined(C89ATOMIC_32BIT)
#ifdef _WIN32
#ifdef _WIN64
#define C89ATOMIC_64BIT
#else
#define C89ATOMIC_32BIT
#endif
#endif
#endif

#if !defined(C89ATOMIC_64BIT) && !defined(C89ATOMIC_32BIT)
#ifdef __GNUC__
#ifdef __LP64__
#define C89ATOMIC_64BIT
#else
#define C89ATOMIC_32BIT
#endif
#endif
#endif

#if !defined(C89ATOMIC_64BIT) && !defined(C89ATOMIC_32BIT)
#include <stdint.h>
#if INTPTR_MAX == INT64_MAX
#define C89ATOMIC_64BIT
#else
#define C89ATOMIC_32BIT
#endif
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define C89ATOMIC_X64
#elif defined(__i386) || defined(_M_IX86)
#define C89ATOMIC_X86
#elif defined(__arm__) || defined(_M_ARM)
#define C89ATOMIC_ARM
#endif

#ifdef _MSC_VER
    #define C89ATOMIC_INLINE __forceinline
#elif defined(__GNUC__)
    /*
    I've had a bug report where GCC is emitting warnings about functions possibly not being inlineable. This warning happens when
    the __attribute__((always_inline)) attribute is defined without an "inline" statement. I think therefore there must be some
    case where "__inline__" is not always defined, thus the compiler emitting these warnings. When using -std=c89 or -ansi on the
    command line, we cannot use the "inline" keyword and instead need to use "__inline__". In an attempt to work around this issue
    I am using "__inline__" only when we're compiling in strict ANSI mode.
    */
    #if defined(__STRICT_ANSI__)
        #define C89ATOMIC_INLINE __inline__ __attribute__((always_inline))
    #else
        #define C89ATOMIC_INLINE inline __attribute__((always_inline))
    #endif
#else
    #define C89ATOMIC_INLINE
#endif

#if defined(_MSC_VER) /*&& !defined(__clang__)*/
    /* Visual C++. */
    #define c89atomic_memory_order_relaxed  0
    #define c89atomic_memory_order_consume  1
    #define c89atomic_memory_order_acquire  2
    #define c89atomic_memory_order_release  3
    #define c89atomic_memory_order_acq_rel  4
    #define c89atomic_memory_order_seq_cst  5

    /* Visual Studio 2003 and earlier have no support for sized atomic operations. We'll need to use inlined assembly for these compilers. */
    #if _MSC_VER >= 1400    /* 1400 = Visual Studio 2005 */
        /* New Visual C++. */
        #include <intrin.h>

        #define c89atomic_exchange_explicit_8( dst, src, order) (c89atomic_uint8 )_InterlockedExchange8 ((volatile char* )dst, (char )src)
        #define c89atomic_exchange_explicit_16(dst, src, order) (c89atomic_uint16)_InterlockedExchange16((volatile short*)dst, (short)src)
        #define c89atomic_exchange_explicit_32(dst, src, order) (c89atomic_uint32)_InterlockedExchange  ((volatile long* )dst, (long )src)
    #if defined(C89ATOMIC_64BIT)
        #define c89atomic_exchange_explicit_64(dst, src, order) (c89atomic_uint64)_InterlockedExchange64((volatile long long*)dst, (long long)src)
    #endif

        #define c89atomic_fetch_add_explicit_8( dst, src, order) (c89atomic_uint8 )_InterlockedExchangeAdd8 ((volatile char* )dst, (char )src)
        #define c89atomic_fetch_add_explicit_16(dst, src, order) (c89atomic_uint16)_InterlockedExchangeAdd16((volatile short*)dst, (short)src)
        #define c89atomic_fetch_add_explicit_32(dst, src, order) (c89atomic_uint32)_InterlockedExchangeAdd  ((volatile long* )dst, (long )src)
    #if defined(C89ATOMIC_64BIT)
        #define c89atomic_fetch_add_explicit_64(dst, src, order) (c89atomic_uint64)_InterlockedExchangeAdd64((volatile long long*)dst, (long long)src)
    #endif

        #define c89atomic_compare_and_swap_8( dst, expected, desired) (c89atomic_uint8 )_InterlockedCompareExchange8 ((volatile char*     )dst, (char     )desired, (char     )expected)
        #define c89atomic_compare_and_swap_16(dst, expected, desired) (c89atomic_uint16)_InterlockedCompareExchange16((volatile short*    )dst, (short    )desired, (short    )expected)
        #define c89atomic_compare_and_swap_32(dst, expected, desired) (c89atomic_uint32)_InterlockedCompareExchange  ((volatile long*     )dst, (long     )desired, (long     )expected)
        #define c89atomic_compare_and_swap_64(dst, expected, desired) (c89atomic_uint64)_InterlockedCompareExchange64((volatile long long*)dst, (long long)desired, (long long)expected)

        /* Can't use MemoryBarrier() for this as it require Windows headers which we want to avoid in the header. */
    #if defined(C89ATOMIC_X64)
        #define c89atomic_thread_fence(order)   __faststorefence()
    #else
        static C89ATOMIC_INLINE void c89atomic_thread_fence(c89atomic_memory_order order)
        {
            volatile c89atomic_uint32 barrier = 0;
            (void)order;
            c89atomic_fetch_add_explicit_32(&barrier, 0, order);
        }
    #endif  /* C89ATOMIC_X64 */
    #else
        /* Old Visual C++. */
        #if defined(__i386) || defined(_M_IX86)
            /* x86. Implemented via inlined assembly. */

            /* thread_fence() */
            static C89ATOMIC_INLINE void __stdcall c89atomic_thread_fence(int order)
            {
                volatile c89atomic_uint32 barrier;

                (void)order;
                __asm {
                    xchg barrier, eax
                }
            }


            /* exchange() */
            static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_exchange_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, int order)
            {
                (void)order;
                __asm {
                    mov ecx, dst
                    mov al,  src
                    lock xchg [ecx], al
                }
            }

            static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_exchange_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, int order)
            {
                (void)order;
                __asm {
                    mov ecx, dst
                    mov ax,  src
                    lock xchg [ecx], ax
                }
            }

            static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_exchange_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, int order)
            {
                (void)order;
                __asm {
                    mov ecx, dst
                    mov eax, src
                    lock xchg [ecx], eax
                }
            }


            /* fetch_add() */
            static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_add_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, int order)
            {
                (void)order;
                __asm {
                    mov ecx, dst
                    mov al,  src
                    lock xadd [ecx], al
                }
            }

            static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_add_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, int order)
            {
                (void)order;
                __asm {
                    mov ecx, dst
                    mov ax,  src
                    lock xadd [ecx], ax
                }
            }

            static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_add_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, int order)
            {
                (void)order;
                __asm {
                    mov ecx, dst
                    mov eax, src
                    lock xadd [ecx], eax
                }
            }


            /* compare_and_swap() */
            static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_compare_and_swap_8(volatile c89atomic_uint8* dst, c89atomic_uint8 expected, c89atomic_uint8 desired)
            {
                __asm {
                    mov ecx, dst
                    mov al,  expected
                    mov dl,  desired
                    lock cmpxchg [ecx], dl  /* Writes to EAX which MSVC will treat as the return value. */
                }
            }

            static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_compare_and_swap_16(volatile c89atomic_uint16* dst, c89atomic_uint16 expected, c89atomic_uint16 desired)
            {
                __asm {
                    mov ecx, dst
                    mov ax,  expected
                    mov dx,  desired
                    lock cmpxchg [ecx], dx  /* Writes to EAX which MSVC will treat as the return value. */
                }
            }

            static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_compare_and_swap_32(volatile c89atomic_uint32* dst, c89atomic_uint32 expected, c89atomic_uint32 desired)
            {
                __asm {
                    mov ecx, dst
                    mov eax, expected
                    mov edx, desired
                    lock cmpxchg [ecx], edx /* Writes to EAX which MSVC will treat as the return value. */
                }
            }

            static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_compare_and_swap_64(volatile c89atomic_uint64* dst, c89atomic_uint64 expected, c89atomic_uint64 desired)
            {
                __asm {
                    mov esi, dst    /* From Microsoft documentation: "... you don't need to preserve the EAX, EBX, ECX, EDX, ESI, or EDI registers." Choosing ESI since it's the next available one in their list. */
                    mov eax, dword ptr expected
                    mov edx, dword ptr expected + 4
                    mov ebx, dword ptr desired
                    mov ecx, dword ptr desired + 4
                    lock cmpxchg8b qword ptr [esi]  /* Writes to EAX:EDX which MSVC will treat as the return value. */
                }
            }
        #else
            /* x64 or ARM. Should never get here because these are not valid targets for older versions of Visual Studio. */
            error "Unsupported architecture."
        #endif
    #endif

    /*
    I'm not sure how to implement a compiler barrier for old MSVC so I'm just making it a thread_fence() and hopefull the compiler will see the volatile and not do
    any reshuffling. If anybody has a better idea on this please let me know! Cannot use _ReadWriteBarrier() as it has been marked as deprecated.
    */
    #define c89atomic_compiler_fence()      c89atomic_thread_fence(c89atomic_memory_order_seq_cst)

    /* I'm not sure how to implement this for MSVC. For now just using thread_fence(). */
    #define c89atomic_signal_fence(order)   c89atomic_thread_fence(order)

    /* Atomic loads can be implemented in terms of a compare-and-swap. */
    #define c89atomic_load_explicit_8( ptr, order) c89atomic_compare_and_swap_8 (ptr, 0, 0)
    #define c89atomic_load_explicit_16(ptr, order) c89atomic_compare_and_swap_16(ptr, 0, 0)
    #define c89atomic_load_explicit_32(ptr, order) c89atomic_compare_and_swap_32(ptr, 0, 0)
    #define c89atomic_load_explicit_64(ptr, order) c89atomic_compare_and_swap_64(ptr, 0, 0)

    /* atomic_store() is the same as atomic_exchange() but returns void. */
    #define c89atomic_store_explicit_8( dst, src, order) (void)c89atomic_exchange_explicit_8 (dst, src, order)
    #define c89atomic_store_explicit_16(dst, src, order) (void)c89atomic_exchange_explicit_16(dst, src, order)
    #define c89atomic_store_explicit_32(dst, src, order) (void)c89atomic_exchange_explicit_32(dst, src, order)
    #define c89atomic_store_explicit_64(dst, src, order) (void)c89atomic_exchange_explicit_64(dst, src, order)

    /* Some 64-bit atomic operations are not supported by MSVC when compiling in 32-bit mode.*/
#if defined(C89ATOMIC_32BIT)
    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_exchange_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, int order)
    {
        volatile c89atomic_uint64 oldValue;

        do {
            oldValue = *dst;
        } while (c89atomic_compare_and_swap_64(dst, oldValue, src) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_add_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, int order)
    {
        volatile c89atomic_uint64 oldValue;
        volatile c89atomic_uint64 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue + src;
        } while (c89atomic_compare_and_swap_64(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }
#endif

    /* fetch_sub() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_sub_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, int order)
    {
        volatile c89atomic_uint8 oldValue;
        volatile c89atomic_uint8 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint8)(oldValue - src);
        } while (c89atomic_compare_and_swap_8(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_sub_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, int order)
    {
        volatile c89atomic_uint16 oldValue;
        volatile c89atomic_uint16 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint16)(oldValue - src);
        } while (c89atomic_compare_and_swap_16(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_sub_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, int order)
    {
        volatile c89atomic_uint32 oldValue;
        volatile c89atomic_uint32 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue - src;
        } while (c89atomic_compare_and_swap_32(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_sub_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, int order)
    {
        volatile c89atomic_uint64 oldValue;
        volatile c89atomic_uint64 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue - src;
        } while (c89atomic_compare_and_swap_64(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }


    /* fetch_and() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_and_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, int order)
    {
        volatile c89atomic_uint8 oldValue;
        volatile c89atomic_uint8 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint8)(oldValue & src);
        } while (c89atomic_compare_and_swap_8(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_and_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, int order)
    {
        volatile c89atomic_uint16 oldValue;
        volatile c89atomic_uint16 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint16)(oldValue & src);
        } while (c89atomic_compare_and_swap_16(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_and_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, int order)
    {
        volatile c89atomic_uint32 oldValue;
        volatile c89atomic_uint32 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue & src;
        } while (c89atomic_compare_and_swap_32(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_and_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, int order)
    {
        volatile c89atomic_uint64 oldValue;
        volatile c89atomic_uint64 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue & src;
        } while (c89atomic_compare_and_swap_64(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }


    /* fetch_xor() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_xor_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, int order)
    {
        volatile c89atomic_uint8 oldValue;
        volatile c89atomic_uint8 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint8)(oldValue ^ src);
        } while (c89atomic_compare_and_swap_8(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_xor_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, int order)
    {
        volatile c89atomic_uint16 oldValue;
        volatile c89atomic_uint16 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint16)(oldValue ^ src);
        } while (c89atomic_compare_and_swap_16(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_xor_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, int order)
    {
        volatile c89atomic_uint32 oldValue;
        volatile c89atomic_uint32 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue ^ src;
        } while (c89atomic_compare_and_swap_32(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_xor_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, int order)
    {
        volatile c89atomic_uint64 oldValue;
        volatile c89atomic_uint64 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue ^ src;
        } while (c89atomic_compare_and_swap_64(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }


    /* fetch_or() */
    static C89ATOMIC_INLINE c89atomic_uint8 __stdcall c89atomic_fetch_or_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, int order)
    {
        volatile c89atomic_uint8 oldValue;
        volatile c89atomic_uint8 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint8)(oldValue | src);
        } while (c89atomic_compare_and_swap_8(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint16 __stdcall c89atomic_fetch_or_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, int order)
    {
        volatile c89atomic_uint16 oldValue;
        volatile c89atomic_uint16 newValue;

        do {
            oldValue = *dst;
            newValue = (c89atomic_uint16)(oldValue | src);
        } while (c89atomic_compare_and_swap_16(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint32 __stdcall c89atomic_fetch_or_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, int order)
    {
        volatile c89atomic_uint32 oldValue;
        volatile c89atomic_uint32 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue | src;
        } while (c89atomic_compare_and_swap_32(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint64 __stdcall c89atomic_fetch_or_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, int order)
    {
        volatile c89atomic_uint64 oldValue;
        volatile c89atomic_uint64 newValue;

        do {
            oldValue = *dst;
            newValue = oldValue | src;
        } while (c89atomic_compare_and_swap_64(dst, oldValue, newValue) != oldValue);

        (void)order;
        return oldValue;
    }

    #define c89atomic_test_and_set_explicit_8( dst, order) c89atomic_exchange_explicit_8 (dst, 1, order)
    #define c89atomic_test_and_set_explicit_16(dst, order) c89atomic_exchange_explicit_16(dst, 1, order)
    #define c89atomic_test_and_set_explicit_32(dst, order) c89atomic_exchange_explicit_32(dst, 1, order)
    #define c89atomic_test_and_set_explicit_64(dst, order) c89atomic_exchange_explicit_64(dst, 1, order)

    #define c89atomic_clear_explicit_8( dst, order) c89atomic_store_explicit_8 (dst, 0, order)
    #define c89atomic_clear_explicit_16(dst, order) c89atomic_store_explicit_16(dst, 0, order)
    #define c89atomic_clear_explicit_32(dst, order) c89atomic_store_explicit_32(dst, 0, order)
    #define c89atomic_clear_explicit_64(dst, order) c89atomic_store_explicit_64(dst, 0, order)

    #define c89atomic_flag_test_and_set_explicit(ptr, order)    (c89atomic_flag)c89atomic_test_and_set_explicit_8(ptr, order)
    #define c89atomic_flag_clear_explicit(ptr, order)           c89atomic_clear_explicit_8(ptr, order)
#elif defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC__ >= 7)))
    /* Modern GCC atomic built-ins. */
    #define C89ATOMIC_HAS_NATIVE_COMPARE_EXCHANGE
    #define C89ATOMIC_HAS_NATIVE_IS_LOCK_FREE

    #define c89atomic_memory_order_relaxed  __ATOMIC_RELAXED
    #define c89atomic_memory_order_consume  __ATOMIC_CONSUME
    #define c89atomic_memory_order_acquire  __ATOMIC_ACQUIRE
    #define c89atomic_memory_order_release  __ATOMIC_RELEASE
    #define c89atomic_memory_order_acq_rel  __ATOMIC_ACQ_REL
    #define c89atomic_memory_order_seq_cst  __ATOMIC_SEQ_CST

    #define c89atomic_compiler_fence()      __asm__ __volatile__("":::"memory")
    #define c89atomic_thread_fence(order)   __atomic_thread_fence(order)
    #define c89atomic_signal_fence(order)   __atomic_signal_fence(order)

    #define c89atomic_is_lock_free_8(ptr)   __atomic_is_lock_free(1, ptr)
    #define c89atomic_is_lock_free_16(ptr)  __atomic_is_lock_free(2, ptr)
    #define c89atomic_is_lock_free_32(ptr)  __atomic_is_lock_free(4, ptr)
    #define c89atomic_is_lock_free_64(ptr)  __atomic_is_lock_free(8, ptr)

    #define c89atomic_flag_test_and_set_explicit(dst, order)        (c89atomic_flag)__atomic_test_and_set(dst, order)
    #define c89atomic_flag_clear_explicit(dst, order)               __atomic_clear(dst, order)

    #define c89atomic_test_and_set_explicit_8( dst, order)          __atomic_exchange_n(dst, 1, order)
    #define c89atomic_test_and_set_explicit_16(dst, order)          __atomic_exchange_n(dst, 1, order)
    #define c89atomic_test_and_set_explicit_32(dst, order)          __atomic_exchange_n(dst, 1, order)
    #define c89atomic_test_and_set_explicit_64(dst, order)          __atomic_exchange_n(dst, 1, order)

    #define c89atomic_clear_explicit_8( dst, order)                 __atomic_store_n(dst, 0, order)
    #define c89atomic_clear_explicit_16(dst, order)                 __atomic_store_n(dst, 0, order)
    #define c89atomic_clear_explicit_32(dst, order)                 __atomic_store_n(dst, 0, order)
    #define c89atomic_clear_explicit_64(dst, order)                 __atomic_store_n(dst, 0, order)

    #define c89atomic_store_explicit_8( dst, src, order)            __atomic_store_n(dst, src, order)
    #define c89atomic_store_explicit_16(dst, src, order)            __atomic_store_n(dst, src, order)
    #define c89atomic_store_explicit_32(dst, src, order)            __atomic_store_n(dst, src, order)
    #define c89atomic_store_explicit_64(dst, src, order)            __atomic_store_n(dst, src, order)

    #define c89atomic_load_explicit_8( dst, order)                  __atomic_load_n(dst, order)
    #define c89atomic_load_explicit_16(dst, order)                  __atomic_load_n(dst, order)
    #define c89atomic_load_explicit_32(dst, order)                  __atomic_load_n(dst, order)
    #define c89atomic_load_explicit_64(dst, order)                  __atomic_load_n(dst, order)

    #define c89atomic_exchange_explicit_8( dst, src, order)         __atomic_exchange_n(dst, src, order)
    #define c89atomic_exchange_explicit_16(dst, src, order)         __atomic_exchange_n(dst, src, order)
    #define c89atomic_exchange_explicit_32(dst, src, order)         __atomic_exchange_n(dst, src, order)
    #define c89atomic_exchange_explicit_64(dst, src, order)         __atomic_exchange_n(dst, src, order)

    #define c89atomic_compare_exchange_strong_explicit_8( dst, expected, desired, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, desired, 0, successOrder, failureOrder)
    #define c89atomic_compare_exchange_strong_explicit_16(dst, expected, desired, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, desired, 0, successOrder, failureOrder)
    #define c89atomic_compare_exchange_strong_explicit_32(dst, expected, desired, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, desired, 0, successOrder, failureOrder)
    #define c89atomic_compare_exchange_strong_explicit_64(dst, expected, desired, successOrder, failureOrder)   __atomic_compare_exchange_n(dst, expected, desired, 0, successOrder, failureOrder)

    #define c89atomic_compare_exchange_weak_explicit_8( dst, expected, desired, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, desired, 1, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_16(dst, expected, desired, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, desired, 1, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_32(dst, expected, desired, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, desired, 1, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_64(dst, expected, desired, successOrder, failureOrder)     __atomic_compare_exchange_n(dst, expected, desired, 1, successOrder, failureOrder)

    #define c89atomic_fetch_add_explicit_8( dst, src, order)        __atomic_fetch_add(dst, src, order)
    #define c89atomic_fetch_add_explicit_16(dst, src, order)        __atomic_fetch_add(dst, src, order)
    #define c89atomic_fetch_add_explicit_32(dst, src, order)        __atomic_fetch_add(dst, src, order)
    #define c89atomic_fetch_add_explicit_64(dst, src, order)        __atomic_fetch_add(dst, src, order)

    #define c89atomic_fetch_sub_explicit_8( dst, src, order)        __atomic_fetch_sub(dst, src, order)
    #define c89atomic_fetch_sub_explicit_16(dst, src, order)        __atomic_fetch_sub(dst, src, order)
    #define c89atomic_fetch_sub_explicit_32(dst, src, order)        __atomic_fetch_sub(dst, src, order)
    #define c89atomic_fetch_sub_explicit_64(dst, src, order)        __atomic_fetch_sub(dst, src, order)

    #define c89atomic_fetch_or_explicit_8( dst, src, order)         __atomic_fetch_or(dst, src, order)
    #define c89atomic_fetch_or_explicit_16(dst, src, order)         __atomic_fetch_or(dst, src, order)
    #define c89atomic_fetch_or_explicit_32(dst, src, order)         __atomic_fetch_or(dst, src, order)
    #define c89atomic_fetch_or_explicit_64(dst, src, order)         __atomic_fetch_or(dst, src, order)

    #define c89atomic_fetch_xor_explicit_8( dst, src, order)        __atomic_fetch_xor(dst, src, order)
    #define c89atomic_fetch_xor_explicit_16(dst, src, order)        __atomic_fetch_xor(dst, src, order)
    #define c89atomic_fetch_xor_explicit_32(dst, src, order)        __atomic_fetch_xor(dst, src, order)
    #define c89atomic_fetch_xor_explicit_64(dst, src, order)        __atomic_fetch_xor(dst, src, order)

    #define c89atomic_fetch_and_explicit_8( dst, src, order)        __atomic_fetch_and(dst, src, order)
    #define c89atomic_fetch_and_explicit_16(dst, src, order)        __atomic_fetch_and(dst, src, order)
    #define c89atomic_fetch_and_explicit_32(dst, src, order)        __atomic_fetch_and(dst, src, order)
    #define c89atomic_fetch_and_explicit_64(dst, src, order)        __atomic_fetch_and(dst, src, order)

    #define c89atomic_compare_and_swap_8 (dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
    #define c89atomic_compare_and_swap_16(dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
    #define c89atomic_compare_and_swap_32(dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
    #define c89atomic_compare_and_swap_64(dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
#else
    /* Legacy GCC atomic built-ins. Everything is a full memory barrier. */
    #define c89atomic_memory_order_relaxed  1
    #define c89atomic_memory_order_consume  2
    #define c89atomic_memory_order_acquire  3
    #define c89atomic_memory_order_release  4
    #define c89atomic_memory_order_acq_rel  5
    #define c89atomic_memory_order_seq_cst  6

    #define c89atomic_compiler_fence()      __asm__ __volatile__("":::"memory")
    #define c89atomic_thread_fence(order)   __sync_synchronize()
    #define c89atomic_signal_fence(order)   c89atomic_thread_fence(order)

    static C89ATOMIC_INLINE c89atomic_uint8 c89atomic_exchange_explicit_8(volatile c89atomic_uint8* dst, c89atomic_uint8 src, c89atomic_memory_order order)
    {
        if (order > c89atomic_memory_order_acquire) {
            __sync_synchronize();
        }

        return __sync_lock_test_and_set(dst, src);
    }

    static C89ATOMIC_INLINE c89atomic_uint16 c89atomic_exchange_explicit_16(volatile c89atomic_uint16* dst, c89atomic_uint16 src, c89atomic_memory_order order)
    {
        volatile c89atomic_uint16 oldValue;

        do {
            oldValue = *dst;
        } while (__sync_val_compare_and_swap(dst, oldValue, src) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint32 c89atomic_exchange_explicit_32(volatile c89atomic_uint32* dst, c89atomic_uint32 src, c89atomic_memory_order order)
    {
        volatile c89atomic_uint32 oldValue;

        do {
            oldValue = *dst;
        } while (__sync_val_compare_and_swap(dst, oldValue, src) != oldValue);

        (void)order;
        return oldValue;
    }

    static C89ATOMIC_INLINE c89atomic_uint64 c89atomic_exchange_explicit_64(volatile c89atomic_uint64* dst, c89atomic_uint64 src, c89atomic_memory_order order)
    {
        volatile c89atomic_uint64 oldValue;

        do {
            oldValue = *dst;
        } while (__sync_val_compare_and_swap(dst, oldValue, src) != oldValue);

        (void)order;
        return oldValue;
    }

    #define c89atomic_fetch_add_explicit_8( dst, src, order)        __sync_fetch_and_add(dst, src)
    #define c89atomic_fetch_add_explicit_16(dst, src, order)        __sync_fetch_and_add(dst, src)
    #define c89atomic_fetch_add_explicit_32(dst, src, order)        __sync_fetch_and_add(dst, src)
    #define c89atomic_fetch_add_explicit_64(dst, src, order)        __sync_fetch_and_add(dst, src)

    #define c89atomic_fetch_sub_explicit_8( dst, src, order)        __sync_fetch_and_sub(dst, src)
    #define c89atomic_fetch_sub_explicit_16(dst, src, order)        __sync_fetch_and_sub(dst, src)
    #define c89atomic_fetch_sub_explicit_32(dst, src, order)        __sync_fetch_and_sub(dst, src)
    #define c89atomic_fetch_sub_explicit_64(dst, src, order)        __sync_fetch_and_sub(dst, src)

    #define c89atomic_fetch_or_explicit_8( dst, src, order)         __sync_fetch_and_or(dst, src)
    #define c89atomic_fetch_or_explicit_16(dst, src, order)         __sync_fetch_and_or(dst, src)
    #define c89atomic_fetch_or_explicit_32(dst, src, order)         __sync_fetch_and_or(dst, src)
    #define c89atomic_fetch_or_explicit_64(dst, src, order)         __sync_fetch_and_or(dst, src)

    #define c89atomic_fetch_xor_explicit_8( dst, src, order)        __sync_fetch_and_xor(dst, src)
    #define c89atomic_fetch_xor_explicit_16(dst, src, order)        __sync_fetch_and_xor(dst, src)
    #define c89atomic_fetch_xor_explicit_32(dst, src, order)        __sync_fetch_and_xor(dst, src)
    #define c89atomic_fetch_xor_explicit_64(dst, src, order)        __sync_fetch_and_xor(dst, src)

    #define c89atomic_fetch_and_explicit_8( dst, src, order)        __sync_fetch_and_and(dst, src)
    #define c89atomic_fetch_and_explicit_16(dst, src, order)        __sync_fetch_and_and(dst, src)
    #define c89atomic_fetch_and_explicit_32(dst, src, order)        __sync_fetch_and_and(dst, src)
    #define c89atomic_fetch_and_explicit_64(dst, src, order)        __sync_fetch_and_and(dst, src)

    #define c89atomic_compare_and_swap_8( dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
    #define c89atomic_compare_and_swap_16(dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
    #define c89atomic_compare_and_swap_32(dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
    #define c89atomic_compare_and_swap_64(dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)

    #define c89atomic_load_explicit_8( ptr, order)                  c89atomic_compare_and_swap_8 (ptr, 0, 0)
    #define c89atomic_load_explicit_16(ptr, order)                  c89atomic_compare_and_swap_16(ptr, 0, 0)
    #define c89atomic_load_explicit_32(ptr, order)                  c89atomic_compare_and_swap_32(ptr, 0, 0)
    #define c89atomic_load_explicit_64(ptr, order)                  c89atomic_compare_and_swap_64(ptr, 0, 0)

    #define c89atomic_store_explicit_8( dst, src, order)            (void)c89atomic_exchange_explicit_8 (dst, src, order)
    #define c89atomic_store_explicit_16(dst, src, order)            (void)c89atomic_exchange_explicit_16(dst, src, order)
    #define c89atomic_store_explicit_32(dst, src, order)            (void)c89atomic_exchange_explicit_32(dst, src, order)
    #define c89atomic_store_explicit_64(dst, src, order)            (void)c89atomic_exchange_explicit_64(dst, src, order)

    #define c89atomic_test_and_set_explicit_8( dst, order)          c89atomic_exchange_explicit_8 (dst, 1, order)
    #define c89atomic_test_and_set_explicit_16(dst, order)          c89atomic_exchange_explicit_16(dst, 1, order)
    #define c89atomic_test_and_set_explicit_32(dst, order)          c89atomic_exchange_explicit_32(dst, 1, order)
    #define c89atomic_test_and_set_explicit_64(dst, order)          c89atomic_exchange_explicit_64(dst, 1, order)

    #define c89atomic_clear_explicit_8( dst, order)                 c89atomic_store_explicit_8 (dst, 0, order)
    #define c89atomic_clear_explicit_16(dst, order)                 c89atomic_store_explicit_16(dst, 0, order)
    #define c89atomic_clear_explicit_32(dst, order)                 c89atomic_store_explicit_32(dst, 0, order)
    #define c89atomic_clear_explicit_64(dst, order)                 c89atomic_store_explicit_64(dst, 0, order)

    #define c89atomic_flag_test_and_set_explicit(ptr, order)        (c89atomic_flag)c89atomic_test_and_set_explicit_8(ptr, order)
    #define c89atomic_flag_clear_explicit(ptr, order)               c89atomic_clear_explicit_8(ptr, order)
#endif

/* compare_exchange() */
#if !defined(C89ATOMIC_HAS_NATIVE_COMPARE_EXCHANGE)
c89atomic_bool c89atomic_compare_exchange_strong_explicit_8(volatile c89atomic_uint8* dst, volatile c89atomic_uint8* expected, c89atomic_uint8 desired, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_uint8 expectedValue;
    c89atomic_uint8 result;

    (void)successOrder;
    (void)failureOrder;

    expectedValue = c89atomic_load_explicit_8(expected, c89atomic_memory_order_seq_cst);
    result = c89atomic_compare_and_swap_8(dst, expectedValue, desired);
    if (result == expectedValue) {
        return 1;
    } else {
        c89atomic_store_explicit_8(expected, result, failureOrder);
        return 0;
    }
}

c89atomic_bool c89atomic_compare_exchange_strong_explicit_16(volatile c89atomic_uint16* dst, volatile c89atomic_uint16* expected, c89atomic_uint16 desired, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_uint16 expectedValue;
    c89atomic_uint16 result;

    (void)successOrder;
    (void)failureOrder;

    expectedValue = c89atomic_load_explicit_16(expected, c89atomic_memory_order_seq_cst);
    result = c89atomic_compare_and_swap_16(dst, expectedValue, desired);
    if (result == expectedValue) {
        return 1;
    } else {
        c89atomic_store_explicit_16(expected, result, failureOrder);
        return 0;
    }
}

c89atomic_bool c89atomic_compare_exchange_strong_explicit_32(volatile c89atomic_uint32* dst, volatile c89atomic_uint32* expected, c89atomic_uint32 desired, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_uint32 expectedValue;
    c89atomic_uint32 result;

    (void)successOrder;
    (void)failureOrder;

    expectedValue = c89atomic_load_explicit_32(expected, c89atomic_memory_order_seq_cst);
    result = c89atomic_compare_and_swap_32(dst, expectedValue, desired);
    if (result == expectedValue) {
        return 1;
    } else {
        c89atomic_store_explicit_32(expected, result, failureOrder);
        return 0;
    }
}

c89atomic_bool c89atomic_compare_exchange_strong_explicit_64(volatile c89atomic_uint64* dst, volatile c89atomic_uint64* expected, c89atomic_uint64 desired, c89atomic_memory_order successOrder, c89atomic_memory_order failureOrder)
{
    c89atomic_uint64 expectedValue;
    c89atomic_uint64 result;

    (void)successOrder;
    (void)failureOrder;

    expectedValue = c89atomic_load_explicit_64(expected, c89atomic_memory_order_seq_cst);
    result = c89atomic_compare_and_swap_64(dst, expectedValue, desired);
    if (result == expectedValue) {
        return 1;
    } else {
        c89atomic_store_explicit_64(expected, result, failureOrder);
        return 0;
    }
}

#define c89atomic_compare_exchange_weak_explicit_8( dst, expected, desired, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_8 (dst, expected, desired, successOrder, failureOrder)
#define c89atomic_compare_exchange_weak_explicit_16(dst, expected, desired, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_16(dst, expected, desired, successOrder, failureOrder)
#define c89atomic_compare_exchange_weak_explicit_32(dst, expected, desired, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_32(dst, expected, desired, successOrder, failureOrder)
#define c89atomic_compare_exchange_weak_explicit_64(dst, expected, desired, successOrder, failureOrder) c89atomic_compare_exchange_strong_explicit_64(dst, expected, desired, successOrder, failureOrder)
#endif  /* C89ATOMIC_HAS_NATIVE_COMPARE_EXCHANGE */

#if !defined(C89ATOMIC_HAS_NATIVE_IS_LOCK_FREE)
    #define c89atomic_is_lock_free_8( ptr)  1
    #define c89atomic_is_lock_free_16(ptr)  1
    #define c89atomic_is_lock_free_32(ptr)  1

    /* For 64-bit atomics, we can only safely say atomics are lock free on 64-bit architectures or x86. Otherwise we need to be conservative and assume not lock free. */
    #if defined(C89ATOMIC_64BIT)
        #define c89atomic_is_lock_free_64(ptr)  1
    #else
        #if defined(C89ATOMIC_X86) || defined(C89ATOMIC_X64)
            #define c89atomic_is_lock_free_64(ptr)  1
        #else
            #define c89atomic_is_lock_free_64(ptr)  0
        #endif
    #endif
#endif  /* C89ATOMIC_HAS_NATIVE_IS_LOCK_FREE */

/* Pointer versions of relevant operations. */
#if defined(C89ATOMIC_64BIT)
    #define c89atomic_is_lock_free_ptr(ptr)                                                                     c89atomic_is_lock_free_64((volatile c89atomic_uint64*)ptr)
    #define c89atomic_load_explicit_ptr(ptr, order)                                                             (void*)c89atomic_load_explicit_64((volatile c89atomic_uint64*)ptr, order)
    #define c89atomic_store_explicit_ptr(dst, src, order)                                                       (void*)c89atomic_store_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64)src, order)
    #define c89atomic_exchange_explicit_ptr(dst, src, order)                                                    (void*)c89atomic_exchange_explicit_64((volatile c89atomic_uint64*)dst, (c89atomic_uint64)src, order)
    #define c89atomic_compare_exchange_strong_explicit_ptr(dst, expected, desired, successOrder, failureOrder)  c89atomic_compare_exchange_strong_explicit_64((volatile c89atomic_uint64*)dst, (volatile c89atomic_uint64*)expected, (c89atomic_uint64)desired, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_ptr(dst, expected, desired, successOrder, failureOrder)    c89atomic_compare_exchange_weak_explicit_64((volatile c89atomic_uint64*)dst, (volatile c89atomic_uint64*)expected, (c89atomic_uint64)desired, successOrder, failureOrder)
    #define c89atomic_compare_and_swap_ptr(dst, expected, desired)                                              (void*)c89atomic_compare_and_swap_64 ((volatile c89atomic_uint64*)dst, (c89atomic_uint64)expected, (c89atomic_uint64)desired)
#elif defined(C89ATOMIC_32BIT)
    #define c89atomic_is_lock_free_ptr(ptr)                                                                     c89atomic_is_lock_free_32((volatile c89atomic_uint32*)ptr)
    #define c89atomic_load_explicit_ptr(ptr, order)                                                             (void*)c89atomic_load_explicit_32((volatile c89atomic_uint32*)ptr, order)
    #define c89atomic_store_explicit_ptr(dst, src, order)                                                       (void*)c89atomic_store_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
    #define c89atomic_exchange_explicit_ptr(dst, src, order)                                                    (void*)c89atomic_exchange_explicit_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32)src, order)
    #define c89atomic_compare_exchange_strong_explicit_ptr(dst, expected, desired, successOrder, failureOrder)  c89atomic_compare_exchange_strong_explicit_32((volatile c89atomic_uint32*)dst, (volatile c89atomic_uint32*)expected, (c89atomic_uint32)desired, successOrder, failureOrder)
    #define c89atomic_compare_exchange_weak_explicit_ptr(dst, expected, desired, successOrder, failureOrder)    c89atomic_compare_exchange_weak_explicit_32((volatile c89atomic_uint32*)dst, (volatile c89atomic_uint32*)expected, (c89atomic_uint32)desired, successOrder, failureOrder)
    #define c89atomic_compare_and_swap_ptr(dst, expected, desired)                                              (void*)c89atomic_compare_and_swap_32((volatile c89atomic_uint32*)dst, (c89atomic_uint32)expected, (c89atomic_uint32)desired)
#else
    error "Unsupported architecture."
#endif

#define c89atomic_flag_test_and_set(ptr)                                c89atomic_flag_test_and_set_explicit(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_flag_clear(ptr)                                       c89atomic_flag_clear_explicit(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_test_and_set_8( ptr)                                  c89atomic_test_and_set_explicit_8 (ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_test_and_set_16(ptr)                                  c89atomic_test_and_set_explicit_16(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_test_and_set_32(ptr)                                  c89atomic_test_and_set_explicit_32(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_test_and_set_64(ptr)                                  c89atomic_test_and_set_explicit_64(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_clear_8( ptr)                                         c89atomic_clear_explicit_8 (ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_clear_16(ptr)                                         c89atomic_clear_explicit_16(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_clear_32(ptr)                                         c89atomic_clear_explicit_32(ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_clear_64(ptr)                                         c89atomic_clear_explicit_64(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_store_8(  dst, src)                                   c89atomic_store_explicit_8 ( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_16( dst, src)                                   c89atomic_store_explicit_16( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_32( dst, src)                                   c89atomic_store_explicit_32( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_64( dst, src)                                   c89atomic_store_explicit_64( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_store_ptr(dst, src)                                   c89atomic_store_explicit_ptr(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_load_8(  ptr)                                         c89atomic_load_explicit_8 ( ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_16( ptr)                                         c89atomic_load_explicit_16( ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_32( ptr)                                         c89atomic_load_explicit_32( ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_64( ptr)                                         c89atomic_load_explicit_64( ptr, c89atomic_memory_order_seq_cst)
#define c89atomic_load_ptr(ptr)                                         c89atomic_load_explicit_ptr(ptr, c89atomic_memory_order_seq_cst)

#define c89atomic_exchange_8(  dst, src)                                c89atomic_exchange_explicit_8 ( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_16( dst, src)                                c89atomic_exchange_explicit_16( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_32( dst, src)                                c89atomic_exchange_explicit_32( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_64( dst, src)                                c89atomic_exchange_explicit_64( dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_exchange_ptr(dst, src)                                c89atomic_exchange_explicit_ptr(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_strong_8(  dst, expected, desired)   c89atomic_compare_exchange_strong_explicit_8 ( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_16( dst, expected, desired)   c89atomic_compare_exchange_strong_explicit_16( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_32( dst, expected, desired)   c89atomic_compare_exchange_strong_explicit_32( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_64( dst, expected, desired)   c89atomic_compare_exchange_strong_explicit_64( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_strong_ptr(dst, expected, desired)   c89atomic_compare_exchange_strong_explicit_ptr(dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_compare_exchange_weak_8(  dst, expected, desired)     c89atomic_compare_exchange_weak_explicit_8 ( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_16( dst, expected, desired)     c89atomic_compare_exchange_weak_explicit_16( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_32( dst, expected, desired)     c89atomic_compare_exchange_weak_explicit_32( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_64( dst, expected, desired)     c89atomic_compare_exchange_weak_explicit_64( dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)
#define c89atomic_compare_exchange_weak_ptr(dst, expected, desired)     c89atomic_compare_exchange_weak_explicit_ptr(dst, expected, desired, c89atomic_memory_order_seq_cst, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_add_8( dst, src)                                c89atomic_fetch_add_explicit_8 (dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_16(dst, src)                                c89atomic_fetch_add_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_32(dst, src)                                c89atomic_fetch_add_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_add_64(dst, src)                                c89atomic_fetch_add_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_sub_8( dst, src)                                c89atomic_fetch_sub_explicit_8 (dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_16(dst, src)                                c89atomic_fetch_sub_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_32(dst, src)                                c89atomic_fetch_sub_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_sub_64(dst, src)                                c89atomic_fetch_sub_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_or_8( dst, src)                                 c89atomic_fetch_or_explicit_8 (dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_16(dst, src)                                 c89atomic_fetch_or_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_32(dst, src)                                 c89atomic_fetch_or_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_or_64(dst, src)                                 c89atomic_fetch_or_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_xor_8( dst, src)                                c89atomic_fetch_xor_explicit_8 (dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_16(dst, src)                                c89atomic_fetch_xor_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_32(dst, src)                                c89atomic_fetch_xor_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_xor_64(dst, src)                                c89atomic_fetch_xor_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#define c89atomic_fetch_and_8( dst, src)                                c89atomic_fetch_and_explicit_8 (dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_16(dst, src)                                c89atomic_fetch_and_explicit_16(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_32(dst, src)                                c89atomic_fetch_and_explicit_32(dst, src, c89atomic_memory_order_seq_cst)
#define c89atomic_fetch_and_64(dst, src)                                c89atomic_fetch_and_explicit_64(dst, src, c89atomic_memory_order_seq_cst)

#if defined(__cplusplus)
}
#endif
#endif  /* c89atomic_h */

/*
This software is available as a choice of the following licenses. Choose
whichever you prefer.

===============================================================================
ALTERNATIVE 1 - Public Domain (www.unlicense.org)
===============================================================================
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

===============================================================================
ALTERNATIVE 2 - MIT No Attribution
===============================================================================
Copyright 2020 David Reid

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/