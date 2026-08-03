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
















#endif
