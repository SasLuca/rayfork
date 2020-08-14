#define RF_RECORDED_ERROR(error_type) (RF_LIT(rf_recorded_error) { RF_SOURCE_LOCATION, error_type })

/*
 Note(LucaSas): MSVC, clang and gcc all deal with __VA_ARGS__ differently.
 Normally you would expect that __VA_ARGS__ consume a trailing comma but it doesn't, this is why we must ##__VA_ARGS__.
 ##__VA_ARGS__ is a preprocessor black magic which achieves this goal, it's not standard but every compiler supports it, if
 this causes issues on some compiler just disable logs with RF_DISABLE_LOGGER.
 Also bear in mind that ##__VA_ARGS__ still works differently between compilers but this code seems to work on all major compilers.
*/
#define RF_LOG(log_type, msg, ...)         rf_log_impl(RF_SOURCE_LOCATION, (log_type), (msg), ##__VA_ARGS__)
#define RF_LOG_ERROR(error_type, msg, ...) (RF_LOG(RF_LOG_TYPE_ERROR, (msg), (error_type), ##__VA_ARGS__), rf__last_error = RF_RECORDED_ERROR(error_type))

RF_INTERNAL void rf_log_impl(rf_source_location source_location, rf_log_type log_type, const char* msg, ...)
{
    if (!(log_type & rf_ctx.logger_filter)) return;

    va_list args;

    va_start(args, msg);

    rf_error_type error_type = RF_NO_ERROR;

    // If the log type is an error then the error type must be the first arg
    if (log_type == RF_LOG_TYPE_ERROR)
    {
        error_type = va_arg(args, rf_error_type);
    }

    if (rf_ctx.logger.log_proc)
    {
        rf_ctx.logger.log_proc(&rf_ctx.logger, source_location, log_type, msg, error_type, args);
    }

    va_end(args);
}

RF_API const char* rf_log_type_string(rf_log_type log_type)
{
    switch (log_type)
    {
        case RF_LOG_TYPE_NONE:    return "NONE";
        case RF_LOG_TYPE_DEBUG:   return "DEBUG";
        case RF_LOG_TYPE_INFO:    return "INFO";
        case RF_LOG_TYPE_WARNING: return "WARNING";
        case RF_LOG_TYPE_ERROR:   return "ERROR";
        default:                  return "RAYFORK_LOG_TYPE_UNKNOWN";
    }
}

RF_API void rf_libc_printf_logger(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args)
{
    ((void)logger); // unused
    printf("[RAYFORK %s]: ", rf_log_type_string(log_type));
    vprintf(msg, args);
    printf("\n");
}