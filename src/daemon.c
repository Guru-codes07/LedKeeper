// LEDKEEPER - A C project for controlling keyboard LEDs
// LEDKEEPER V1.0
// daemon.c - daemon lifecycle: signal handling and the monitor loop
#include "daemon.h"
#include "logger.h"
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>

static volatile sig_atomic_t g_shutdown_requested = 0;
static void handle_shutdown_signal(int signum) 
{
    (void)signum;
    g_shutdown_requested = 1;
}

void daemon_config_set_defaults(daemon_config_t *config) 
{
    if (config == NULL) 
    {
        return;
    }
    config->poll_interval_ms = DAEMON_DEFAULT_POLL_INTERVAL_MS;
    config->verbose = 0;
}