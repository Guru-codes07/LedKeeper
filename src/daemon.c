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
  int daemon_setup_signal_handlers(void) 
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_shutdown_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) != 0) 
    {
     return -1;
    }
     if (sigaction(SIGTERM, &sa, NULL) != 0) 
    {
     return -1;
    }

    return 0;
}