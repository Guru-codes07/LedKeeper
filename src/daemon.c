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