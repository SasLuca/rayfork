#ifndef RAYFORK_RAND_H
#define RAYFORK_RAND_H

#include "rayfork_common.h"

#define RF_DEFAULT_RAND_PROC (rf_libc_rand_wrapper)

typedef int (*rf_rand_proc)(int min, int max);

RF_API int rf_libc_rand_wrapper(int min, int max);

#endif // RAYFORK_RAND_H