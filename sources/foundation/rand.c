#include "rayfork/foundation/rand.h"

#include "stdlib.h"

rf_extern rf_int rf_libc_rand_wrapper()
{
    return rand();
}

rf_extern rf_int rf_rand_in_range(rf_int min, rf_int max, rf_rand_proc* rand)
{
    rf_int result = rand() % (max + 1 - min) + min;
    return result;
}