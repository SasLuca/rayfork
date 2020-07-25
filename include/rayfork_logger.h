#ifndef RAYFORK_LOGGER_H
#define RAYFORK_LOGGER_H

#include "rayfork_common.h"
#include "rayfork_error.h"

#define RF_DEFAULT_LOGGER (RF_LIT(rf_logger) { NULL, rf_libc_printf_logger })
#define RF_NULL_LOGGER    (RF_LIT(rf_logger) { NULL, NULL })

typedef enum rf_log_type
{
    RF_LOG_TYPE_NONE    = 0,
    RF_LOG_TYPE_DEBUG   = 0x1, // Useful mostly to rayfork devs
    RF_LOG_TYPE_INFO    = 0x2, // Information
    RF_LOG_TYPE_WARNING = 0x4, // Warnings about things to be careful about
    RF_LOG_TYPE_ERROR   = 0x8, // Errors that prevented functions from doing everything they advertised
    RF_LOG_TYPE_ALL     = RF_LOG_TYPE_DEBUG | RF_LOG_TYPE_INFO | RF_LOG_TYPE_WARNING | RF_LOG_TYPE_ERROR,
} rf_log_type;

struct rf_logger;

typedef void (*rf_log_proc)(struct rf_logger* logger, const char* file, int line, const char* proc_name, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);

typedef struct rf_logger
{
    void* user_data;
    rf_log_proc log_proc;
} rf_logger;

RF_API const char* rf_log_type_string(rf_log_type);

RF_API void rf_set_logger(rf_logger logger);
RF_API void rf_set_logger_filter(rf_log_type);

RF_API void rf_libc_printf_logger(struct rf_logger* logger, const char* file, int line, const char* proc_name, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args);

#endif // RAYFORK_LOGGER_H