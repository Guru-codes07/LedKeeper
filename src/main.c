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
typedef enum 
{
    MODE_DAEMON = 0, /* default when no mode flag is given */
    MODE_ONCE,
    MODE_STATUS,
    MODE_LIST_LEDS,
    MODE_VERSION,
    MODE_HELP,
} run_mode_t;

//  cli_options_t - parsed command-line state, passed around instead of using globals.
typedef struct 
{
    run_mode_t mode;
    const char *led_path_override; /* NULL unless --led-path was given */
    int verbose;
} cli_options_t;
static void print_usage(const char *prog_name) 
{
    printf(
        "Usage:\n"
        "  %s [OPTIONS]\n"
        "\n"
        "Options:\n"
        "  --daemon         Run continuously in the foreground (default if no options given)\n"
        "  --once           Turn the LED on once and exit\n"
        "  --status         Show the detected LED and its current state, then exit\n"
        "  --list-leds      List every LED device under /sys/class/leds and exit\n"
        "  --led-path PATH  Use PATH instead of auto-detecting the Scroll Lock LED\n"
        "  -v, --verbose    Enable verbose logging\n"
        "  --version        Show version and exit\n"
        "  --help           Show this help and exit\n",prog_name);
}
static void print_version(void) 
{
    printf("ledkeeper %s\n", LEDKEEPER_VERSION);
}

/* parse_args - fill *opts from argv. Returns 0 on success, -1 on an
   unrecognized option or missing required argument (getopt_long
   already printed a message to stderr in that case).
*/
static int parse_args(int argc, char **argv, cli_options_t *opts) 
{
    static const struct option long_opts[] = {
        {"daemon",    no_argument,       NULL, 'd'},
        {"once",      no_argument,       NULL, 'o'},
        {"status",    no_argument,       NULL, 's'},
        {"list-leds", no_argument,       NULL, 'l'},
        {"led-path",  required_argument, NULL, 'p'},
        {"verbose",   no_argument,       NULL, 'v'},
        {"version",   no_argument,       NULL, 'V'},
        {"help",      no_argument,       NULL, 'h'},
        {NULL, 0, NULL, 0},
    };
    
    opts->mode = MODE_DAEMON;
    opts->led_path_override = NULL;
    opts->verbose = 0;

    int c;
    while ((c = getopt_long(argc, argv, "vh", long_opts, NULL)) != -1) {
        switch (c) {
            case 'd':
                opts->mode = MODE_DAEMON;
                break;
            case 'o':
                opts->mode = MODE_ONCE;
                break;
            case 's':
                opts->mode = MODE_STATUS;
                break;
            case 'l':
                opts->mode = MODE_LIST_LEDS;
                break;
             case 'p':
                opts->led_path_override = optarg;
                break;
            case 'v':
                opts->verbose = 1;
                break;
            case 'V':
                opts->mode = MODE_VERSION;
                break;
            case 'h':
                opts->mode = MODE_HELP;
                break;
            default:
                /* getopt_long already printed "invalid option" to stderr */
                return -1;
        }
    }

    return 0;
}

// resolve_led - locate the LED to operate.
static int resolve_led(const cli_options_t *opts, led_t *out) 
{
    if (opts->led_path_override != NULL) 
    {
        return led_load_from_path(opts->led_path_override, out);
    }
   
    return led_find_scrolllock(out);
}

static int run_list_leds(void) 
{
    led_t leds[MAX_LISTED_LEDS];
    size_t count = 0;

    if (led_list_all(leds, MAX_LISTED_LEDS, &count) != 0) 
    {
        fprintf(stderr, "ledkeeper: failed to scan %s: %s\n",
                LED_SYFS_ROOT, strerror(errno));
        return 1;
    }
    
    if (count == 0) 
     {
        printf("No LED devices found under %s\n", LED_SYFS_ROOT);
        return 0;
    }

    for (size_t i = 0; i < count; i++) 
    {
    int brightness = led_read_brightness(&leds[i]);
    printf("%-32s  max=%-3d  current=%d\n  %s\n",leds[i].name, leds[i].max_brightness,brightness, leds[i].dir_path);
    }

    return 0;
}
static int run_status(const cli_options_t *opts) 
{
    led_t led;

    if (resolve_led(opts, &led) != 0) 
    {
    fprintf(stderr,"ledkeeper: could not find a Scroll Lock LED: %s\n""Try --list-leds to see what's available, or --led-path to specify one directly.\n",strerror(errno));
    return 1;
    }
     int brightness = led_read_brightness(&led);
    if (brightness < 0) 
    {
        fprintf(stderr, "ledkeeper: failed to read LED brightness: %s\n", strerror(errno));
        return 1;
    }

    printf("LED:        %s\n", led.name);
    printf("Path:       %s\n", led.dir_path);
    printf("Brightness: %d (max %d)\n", brightness, led.max_brightness);
    printf("State:      %s\n", brightness > 0 ? "ON" : "OFF");

    return 0;
}



