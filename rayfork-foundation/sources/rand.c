#include "rayfork/foundation/rand.h"

#include "stdlib.h"
#include "time.h"

rf_internal bool rf_is_libc_rand_initialized;

rf_extern rf_int rf_libc_rand_wrapper()
{
    if (!rf_is_libc_rand_initialized)
    {
        srand(time(NULL));
        rf_is_libc_rand_initialized = true;
    }
    return rand();
}

rf_extern rf_int rf_rand_in_range(rf_int min, rf_int max, rf_rand_proc* rand)
{
    rf_int result = rand() % (max + 1 - min) + min;
    return result;
}