// LEDKEEPER - A C project for controlling keyboard LEDs
// LEDKEEPER V1.0
/* led.h - Scroll lock LED discovery and control 
   A custom header file that's a part of the LEDKEEPER.
*/

#ifndef LED_H
#define LED_H

#include<stddef.h>
#include<limits.h>
#define PATH_MAX 4096
#define LED_SYFS_ROOT "/sys/class/leds"
#define LED_NAME_MAX 256

// /sys/class/leds/*scrolllock*/brightness
/* The path to the brightness file for the scroll lock LED in my laptop.
This file can be read to determine the current state of the LED (on or off)
Can be written to in order to change the state of the LED. */

// creating a struct to hold the LED information called as led_info_t
typedef struct 
{
char name[LED_NAME_MAX];         /* sysfs entry name, e.g. "input3::scrolllock" */
char dir_path[PATH_MAX];         /* full path to the LED's sysfs directory */
char brightness_path[PATH_MAX];  /* full path to <dir_path>/brightness */
int  max_brightness;             /* value considered fully "on"; defaults to 1 if unreadable */
} led_info_t;

int led_find_scrolllock(led_info_t *out);
/* The above function finds the scroll lock LED and fills the provided led_info_t structure like:
   led_find_scrolllock - scan LED_SYSFS_ROOT for a device whose name matches a known Scroll Lock LED naming pattern 
   (case-insensitive; handles "scrolllock", "scroll_lock", "scroll-lock", "scrl" variants). 
*/

int led_list_all(led_info_t *out_arr, size_t max_entries, size_t *out_count);
/* led_list_all - enumerate every LED device under LED_SYSFS_ROOT,regardless of name, into out_arr (capacity max_entries).
   The above function lists all available LEDs and fills the provided led_info_t array.
*/









#endif
