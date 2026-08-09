// LEDKEEPER - A C project for controlling keyboard LEDs
// LEDKEEPER V1.0
// logger.c - Logging functionality for LEDKEEPER.
// part of LEDKEEPER.

#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
static const char *g_ident = "ledkeeper";
static int g_use_syslog = 0;
static int g_verbose = 0;

void logger_init(const char *ident, int use_syslog, int verbose) 
{
    g_ident = (ident != NULL) ? ident : "ledkeeper";
    g_use_syslog = use_syslog;
    g_verbose = verbose;

    if (g_use_syslog) 
    {
        openlog(g_ident, LOG_PID, LOG_DAEMON);
    }
}
void logger_shutdown(void) 
{
    if (g_use_syslog) 
    {
        closelog();
    }
}

// level_to_label - short uppercase tag for stderr output, e.g. "INFO".
static const char *level_to_label(log_level_t level) 
{
    switch (level) 
    {
        case LOG_LEVEL_INFO:
            return "INFO";
        case LOG_LEVEL_WARN:
            return "WARN";
        case LOG_LEVEL_ERROR:
            return "ERROR";
        default:
            return "LOG";
    }
}

/* level_to_syslog_priority - map our small log_level_t enum onto the
   standard syslog(3) priority levels. */
   static int level_to_syslog_priority(log_level_t level) 
   {
    switch (level) 
    {
        case LOG_LEVEL_INFO:
            return LOG_INFO;
        case LOG_LEVEL_WARN:
            return LOG_WARNING;
        case LOG_LEVEL_ERROR:
            return LOG_ERR;
        default:
            return LOG_INFO;
    }
}
/* log_emit - shared implementation behind log_info()/log_warn()/
   log_error(). Writes to stderr unconditionally (aside from the
   verbose gate on INFO) and additionally to syslog if enabled.
*/
static void log_emit(log_level_t level, const char *fmt, va_list args) 
{
    if (level == LOG_LEVEL_INFO && !g_verbose) 
    {
        return;
    }

/* va_list can only be safely traversed once; since we may need
   to use it for both stderr and syslog output, copy it for each
   use rather than passing the same va_list to two vfprintf/
    vsyslog calls. 
*/
    
 va_list stderr_args;
 va_copy(stderr_args, args);
 fprintf(stderr, "%s: [%s] ", g_ident, level_to_label(level));
 vfprintf(stderr, fmt, stderr_args);
 fprintf(stderr, "\n");
 va_end(stderr_args);
 
 if (g_use_syslog) 
  {
        va_list syslog_args;
        va_copy(syslog_args, args);
        vsyslog(level_to_syslog_priority(level), fmt, syslog_args);
        va_end(syslog_args);
    }
}
void log_info(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    log_emit(LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

void log_warn(const char *fmt, ...)
 {
    va_list args;
    va_start(args, fmt);
    log_emit(LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

