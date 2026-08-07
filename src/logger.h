// LEDKEEPER - A C project for controlling keyboard LEDs
// LEDKEEPER V1.0
// logger.h - Header file for the logger module.
/* Provides a small logging facade so the rest of the codebase never
   calls fprintf()/syslog() directly. Two backends are supported.
     * stderr - logs to stderr, with a timestamp and log level prefix.
     * syslog - logs to syslog, with a timestamp and log level prefix.
   Both can be active at once; logger_init() decides which are used.
*/

#ifndef LOGGER_H
#define LOGGER_H
typedef enum 
{
 LOG_LEVEL_INFO = 0,
 LOG_LEVEL_WARN,
 LOG_LEVEL_ERROR,
} log_level_t;

// logger_init - configure the logging backend(s).
void logger_init(const char *ident, int use_syslog, int verbose);

/* logger_shutdown - release any resources logger_init() acquired
   (closes the syslog connection if one was opened). Safe to call
   even if logger_init() was never called.
*/
void logger_shutdown(void);

/* log_info / log_warn / log_error - emit a printf-style formatted
   message at the given severity. log_info() is suppressed unless
   logger_init() was called with verbose != 0; log_warn()/log_error()
   are always emitted.
*/
void log_info(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void log_warn(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void log_error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif