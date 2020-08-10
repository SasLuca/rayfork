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