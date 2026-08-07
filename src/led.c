// LEDKEEPER - A C project for controlling keyboard LEDs
// LEDKEEPER V1.0

#define _GNU_SOURCE 

#include"led.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<dirent.h>
#include<unistd.h>
#include<fcntl.h>

// Name variants for "scroll lock" LED across kernel drivers.
static const char *SCROLLLOCK_NAME_PATTERNS[] = { "scrolllock","scroll_lock","scroll-lock","scrl"};
#define SCROLLLOCK_PATTERN_COUNT \
    (sizeof(SCROLLLOCK_NAME_PATTERNS) / sizeof(SCROLLLOCK_NAME_PATTERNS[0]))  // C idiom for an array length operator.

// read_int_file - read a small integer out of a one-line sysfs file.    
static int read_int_file (const char *path , int *out)
{
    int fd = open(path,O_RDONLY);
    if(fd<=0)
    {
        return -1;
    }
 
    // declaring a buffer
 char buff[32];
 ssize_t n = read(fd, buff, sizeof(buff) - 1);
 int saved_errno = errno;
 close(fd);
 if (n < 0) 
 {
    errno = saved_errno;
    return -1;
 }
 
  buff[n] = '\0';
  errno = 0;
    char *endptr = NULL;
    long value = strtol(buff, &endptr, 10);
    if (endptr == buff) 
    {
        /* No digits parsed at all. */
        errno = EINVAL;
        return -1;
    }

    *out = (int)value;
    return 0;
}

// write_int_file - write a small integer to a sysfs file as ASCII
static int write_int_file(const char *path, int value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0)
    {
      return -1;
    }
 
    char buff[32];
 int len = snprintf(buff, sizeof(buff), "%d", value);
 ssize_t written = write(fd, buff, (size_t)len);
 int saved_errno = errno;
 close(fd);

  if (written < 0 || written != len) 
  {
    errno = (written < 0) ? saved_errno : EIO;
    return -1;
  } 
 
  return 0;
}

/* name_matches_scrolllock - case-insensitive substring match of
  `name` against the known Scroll Lock naming variants.
*/
static int name_matches_scrolllock(const char *name) 
{
  for (size_t i = 0; i < SCROLLLOCK_PATTERN_COUNT; i++)  // contains errors
    {
     if (strcasestr(name, SCROLLLOCK_NAME_PATTERNS[i]) != NULL) 
    {
      return 1;
    }
    }
    return 0;
}

int led_load_from_path(const char *path, led_t *out) 
{
  if (path == NULL || out == NULL) 
  {
    errno = EINVAL;
    return -1;
  }

  // Have to clear errors from this part onwards
 memset(out, 0, sizeof(*out));

 /* Derive the display name from the final path component. */
 const char *slash = strrchr(path, '/');
 const char *base = (slash != NULL) ? slash + 1 : path;
 strncpy(out->name, base, LED_NAME_MAX - 1);
 strncpy(out->dir_path, path, PATH_MAX - 1);

  int n = snprintf(out->brightness_path, PATH_MAX, "%s/brightness", path);
  if (n < 0 || n >= PATH_MAX) 
  {
    errno = ENAMETOOLONG;
    return -1;
  }
   
  /* Confirm the brightness file actually exists and is accessible. */
    if (access(out->brightness_path, F_OK) != 0) 
  {
    return -1;
  }

  /* max_brightness is informational; default to 1 if unreadable. */
  char max_path[PATH_MAX];
  n = snprintf(max_path, PATH_MAX, "%s/max_brightness", path);
   if (n > 0 && n < PATH_MAX) 
  {
     int max_val = 0;
     if (read_int_file(max_path, &max_val) == 0 && max_val > 0) 
      {
        out->max_brightness = max_val;
      } 
    else 
    {
      out->max_brightness = 1;
    }
  } 
    else
   {
    out->max_brightness = 1;
   }

   return 0;
}



