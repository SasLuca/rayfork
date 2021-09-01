#include "rayfork/foundation/logger.h"
#include "rayfork/foundation/error.h"

#include "stdio.h"

rf_log_type rf_global_log_filter;
rf_logger   rf_global_logger;

rf_extern void rf_internal_log(rf_log_type log_type, const char* msg, ...)
{
    if (!(log_type & rf_global_log_filter)) {
        return;
    }

    va_list args = {0};
    va_start(args, msg);

    rf_error_type error_type = rf_no_error;

    // If the log type is an error then the error type must be the first arg
    if (log_type == rf_log_type_error) {
        error_type = va_arg(args, rf_error_type);
    }

    if (rf_global_logger.log_proc) {
        rf_global_logger.log_proc(&rf_global_logger, log_type, msg, error_type, args);
    }

    va_end(args);
}

rf_extern const char* rf_log_type_string(rf_log_type log_type)
{
    switch (log_type)
    {
        case rf_log_type_none:    return "NONE";
        case rf_log_type_debug:   return "DEBUG";
        case rf_log_type_info:    return "INFO";
        case rf_log_type_warning: return "WARNING";
        case rf_log_type_error:   return "ERROR";
        default:                  return "RAYFORK_LOG_TYPE_UNKNOWN";
    }
}

rf_extern void rf_libc_printf_logger_wrapper(struct rf_logger* logger, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args)
{
    ((void)logger); // unused
    printf("[RAYFORK %s]: ", rf_log_type_string(log_type));
    vprintf(msg, args);
    printf("\n");
}