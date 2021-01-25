#ifndef RAYFORK_FOUNDATION_LOGGER_H
#define RAYFORK_FOUNDATION_LOGGER_H

#include "rayfork/foundation/basicdef.h"
#include "rayfork/foundation/error.h"

/*
 Note(LucaSas): MSVC, clang and gcc all deal with __VA_ARGS__ differently.
 Normally you would expect that __VA_ARGS__ consume a trailing comma but it doesn't, this is why we must ##__VA_ARGS__.
 ##__VA_ARGS__ is a preprocessor black magic which achieves this goal, it's not standard but every compiler supports it, if
 this causes issues on some compiler just disable logs with RF_DISABLE_LOGGER.
 Also bear in mind that ##__VA_ARGS__ still works differently between compilers but this code seems to work on all major compilers.
*/
#define rf_default_logger                  (rf_lit(rf_logger) { 0, rf_libc_printf_logger_wrapper })
#define rf_log(log_type, msg, ...)         (rf_internal_log((log_type), (msg), ##__VA_ARGS__))
#define rf_log_error(error_type, msg, ...) (rf_log(rf_log_type_error, (msg), (error_type), ##__VA_ARGS__), rf_global_last_recorded_error = error_type)

typedef enum rf_log_type
{
    rf_log_type_none    = 0,
    rf_log_type_debug   = 0x1, // Useful mostly to rayfork devs
    rf_log_type_info    = 0x2, // Information
    rf_log_type_warning = 0x4, // Warnings about things to be careful about
    rf_log_type_error   = 0x8, // Errors that prevented functions from doing everything they advertised
    rf_log_type_all     = rf_log_type_debug | rf_log_type_info | rf_log_type_warning | rf_log_type_error,
} rf_log_type;

struct rf_logger;

typedef void (*rf_log_proc)(struct rf_logger* logger, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);

typedef struct rf_logger
{
    void* user_data;
    rf_log_proc log_proc;
} rf_logger;

rf_extern rf_log_type rf_global_log_filter;
rf_extern rf_logger   rf_global_logger;

rf_extern const char* rf_log_type_string(rf_log_type);

rf_extern void rf_libc_printf_logger_wrapper(struct rf_logger* logger, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);

rf_extern void rf_internal_log(rf_log_type log_type, const char* msg, ...);

#endif // RAYFORK_FOUNDATION_LOGGER_H