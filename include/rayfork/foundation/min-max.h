#ifndef RAYFORK_FOUNDATION_MIN_MAX_H
#define RAYFORK_FOUNDATION_MIN_MAX_H

#include "basicdef.h"

rf_inline int rf_min_i(rf_int a, rf_int b) { return ((a) < (b) ? (a) : (b)); }
rf_inline int rf_max_i(rf_int a, rf_int b) { return ((a) > (b) ? (a) : (b)); }

rf_inline float rf_min_f(float a, float b) { return ((a) < (b) ? (a) : (b)); }
rf_inline float rf_max_f(float a, float b) { return ((a) > (b) ? (a) : (b)); }

#endif // RAYFORK_FOUNDATION_MIN_MAX_H