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

int daemon_should_exit(void) 

{
    return g_shutdown_requested != 0;
}

static void sleep_for_ms(int ms) 
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
int daemon_run(const led_t *led, const daemon_config_t *config) 
{
    if (led == NULL || config == NULL) 
    {
        errno = EINVAL;
        return -1;
    }

    log_info("monitoring \"%s\" every %dms", led->name, config->poll_interval_ms);

    while (!daemon_should_exit()) 
    {
        int brightness = led_read_brightness(led);

        if (brightness < 0) 
        {
            log_error("failed to read LED brightness: %s", strerror(errno));
            return -1;
        }
         if (brightness == 0) 
         {
            if (led_turn_on(led) != 0) 
            {
                log_error("failed to restore LED: %s", strerror(errno));
                return -1;
            }

            log_info("restored LED \"%s\"", led->name);
        }

        sleep_for_ms(config->poll_interval_ms);
    }
     
    log_info("shutdown signal received, exiting");

    return 0;
}
