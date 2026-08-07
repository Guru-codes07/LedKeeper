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

