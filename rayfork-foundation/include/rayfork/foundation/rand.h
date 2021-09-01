#ifndef RAYFORK_FOUNDATION_RAND_H
#define RAYFORK_FOUNDATION_RAND_H

#include "rayfork/foundation/basicdef.h"

#define rf_default_rand_proc ((rf_rand_proc*)rf_libc_rand_wrapper)
#define rf_default_rand_in_range(min, max) rf_rand_in_range(min, max, rf_default_rand_proc)

typedef rf_int (rf_rand_proc)();

rf_extern rf_int rf_libc_rand_wrapper();

rf_extern rf_int rf_rand_in_range(rf_int min, rf_int max, rf_rand_proc* rand);

#endif // RAYFORK_FOUNDATION_RAND_H