// LEDKEEPER - A C project for controlling keyboard LEDs
// LEDKEEPER V1.0
// main.c
/*  * main.c - CLI entry point for LedKeeper
  Part of LedKeeper.
   Wires together led.c (detection/control), daemon.c (signal
   handling + monitor loop), and logger.c (leveled logging) behind a
   small set of CLI flags. This file owns argument parsing and mode
   dispatch only - it does not itself touch sysfs, signals, or
   logging internals directly.
*/
#define _POSIX_C_SOURCE 200809L

#include "daemon.h"
#include "led.h"
#include "logger.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LEDKEEPER_VERSION "1.0.0"
#define MAX_LISTED_LEDS   64

// run_mode_t - which action the user asked for via CLI flags.
