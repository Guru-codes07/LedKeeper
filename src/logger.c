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
