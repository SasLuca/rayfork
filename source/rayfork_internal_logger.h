#ifndef RAYFORK_LOGGER_INTERNALS_H
#define RAYFORK_LOGGER_INTERNALS_H

/*
 Note(LucaSas): MSVC, clang and gcc all deal with __VA_ARGS__ differently.
 Normally you would expect that __VA_ARGS__ consume a trailing comma but it doesn't, this is why we must ##__VA_ARGS__.
 ##__VA_ARGS__ is a preprocessor black magic which achieves this goal, it's not standard but every compiler supports it, if
 this causes issues on some compiler just disable logs with RF_DISABLE_LOGGER.
 Also bear in mind that ##__VA_ARGS__ still works differently between compilers but this code seems to work on all major compilers.
*/
#define RF_LOG(log_type, msg, ...) rf_log_impl(__FILE__, __LINE__, __FUNCTION__, (log_type), (msg), ##__VA_ARGS__)
#define RF_LOG_ERROR(error_type, msg, ...) RF_LOG(RF_LOG_TYPE_ERROR, (msg), (error_type), ##__VA_ARGS__)

RF_INTERNAL void rf_log_impl(const char* file, int line, const char* proc_name, rf_log_type log_type, const char* msg, ...);

#endif // RAYFORK_LOGGER_INTERNALS_H