#ifndef RAYFORK_FOUNDATION_NUMERIC_LIMITS_H
#define RAYFORK_FOUNDATION_NUMERIC_LIMITS_H

#include "basicdef.h"

#define rf_sizeof_in_bits(type) (sizeof(type) * 8)

#define rf_signed_integer_type_max(type) ((type)(((size_t)(~0)) >> 1))
#define rf_signed_integer_type_min(type) ((type)(((size_t)1) << rf_sizeof_in_bits(type)))

#define rf_unsigned_integer_type_max(type) ((type)((size_t)(~0)))
#define rf_unsigned_integer_type_min(type) ((type)0)

#define rf_int_max  rf_signed_integer_type_max(rf_int)

#define rf_int8_max rf_signed_integer_type_max(int8_t)
#define rf_int8_min rf_signed_integer_type_min(int8_t)

#define rf_int16_max rf_signed_integer_type_max(int16_t)
#define rf_int16_min rf_signed_integer_type_min(int16_t)

#define rf_int32_max rf_signed_integer_type_max(int32_t)
#define rf_int32_min rf_signed_integer_type_min(int32_t)

#define rf_int64_max rf_signed_integer_type_max(int64_t)
#define rf_int64_min rf_signed_integer_type_min(int64_t)

#define rf_uint8_max rf_signed_integer_type_max(uint8_t)
#define rf_uint8_min rf_signed_integer_type_min(uint8_t)

#define rf_uint16_max rf_signed_integer_type_max(uint16_t)
#define rf_uint16_min rf_signed_integer_type_min(uint16_t)

#define rf_uint32_max rf_signed_integer_type_max(uint32_t)
#define rf_uint32_min rf_signed_integer_type_min(uint32_t)

#define rf_uint64_max rf_signed_integer_type_max(uint64_t)
#define rf_uint64_min rf_signed_integer_type_min(uint64_t)

#endif // RAYFORK_FOUNDATION_NUMERIC_LIMITS_H