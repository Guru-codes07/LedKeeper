// LEDKEEPER - A C project for controlling keyboard LEDs
// LEDKEEPER V1.0
// daemon.h - daemon lifecycle: signal handling and the monitor loop.
// This module owns the "keep running until asked to stop" behavior.
#ifndef DAEMON_H
#define DAEMON_H
#include "led.h"
#define DAEMON_DEFAULT_POLL_INTERVAL_MS 200

// daemon_config_t - runtime behavior knobs for the monitor loop.
typedef struct {
    int poll_interval_ms;
    int verbose;         
} daemon_config_t;

//daemon_config_set_defaults - populate *config with DAEMON_DEFAULT_POLL_INTERVAL_MS and verbose = 0.
void daemon_config_set_defaults(daemon_config_t *config);

/*  daemon_setup_signal_handlers - install handlers for SIGINT and
    SIGTERM that flag a pending shutdown.
*/
int daemon_setup_signal_handlers(void);

/* daemon_should_exit - returns nonzero if SIGINT or SIGTERM has been
   received since daemon_setup_signal_handlers() was called.
*/
int daemon_should_exit(void);

/* daemon_run - run the monitor loop: repeatedly check led's
 brightness, restore it via led_turn_on() whenever it reads as off.
*/
int daemon_run(const led_info_t *led, const daemon_config_t *config);

#endif
