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

int led_find_scrolllock(led_t *out) 
{
 if (out == NULL)
 {
   errno = EINVAL;
   return -1;
 }
 DIR *dir = opendir(LED_SYFS_ROOT);
    if (dir == NULL) 
    {
     return -1;
    }

    struct dirent *entry;
    int found = 0;

    while ((entry = readdir(dir)) != NULL) 
    {
      if (entry->d_name[0] == '.') 
      {
        continue;                               /* skip "." and ".." */
      }
      
      if (!name_matches_scrolllock(entry->d_name)) 
      {
        continue;
      }

      char full_path[PATH_MAX];
      int n = snprintf(full_path, PATH_MAX, "%s/%s", LED_SYFS_ROOT, entry->d_name);
      if (n < 0 || n >= PATH_MAX) 
     {
       continue;
     }
     if (led_load_from_path(full_path, out) == 0) 
     {
        found = 1;
        break;
      }
          /* Name matched but the directory turned out not to be a
           * valid LED (missing brightness file) — keep scanning in
           * case another entry matches too. */
    }

   closedir(dir);
   if (!found) 
   {
    errno = ENODEV;
    return -1;
   }

    return 0;
}

int led_list_all(led_t *out_arr, size_t max_entries, size_t *out_count) 
{
    if (out_arr == NULL || out_count == NULL) 
    {
      errno = EINVAL;
      return -1;
    }

    *out_count = 0;

    DIR *dir = opendir(LED_SYFS_ROOT);
    if (dir == NULL) 
    {
      return -1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL && *out_count < max_entries) 
    {
      if (entry->d_name[0] == '.') 
      {
        continue;
      }

      char full_path[PATH_MAX];
      int n = snprintf(full_path, PATH_MAX, "%s/%s", LED_SYFS_ROOT, entry->d_name);
      if (n < 0 || n >= PATH_MAX) 
      {
        continue;
      }

        if (led_load_from_path(full_path, &out_arr[*out_count]) == 0) 
      {
        (*out_count)++;
      }
    }

    closedir(dir);
    return 0;
}

int led_read_brightness(const led_t *led) 
{
    if (led == NULL) 
    {
      errno = EINVAL;
      return -1;
    }

    int value = 0;
    if (read_int_file(led->brightness_path, &value) != 0) 
    {
      return -1;
    }

    return value;
}

int led_set_brightness(const led_t *led, int value) 
{
    if (led == NULL) 
    {
      errno = EINVAL;
      return -1;
    }

  return write_int_file(led->brightness_path, value);
}

int led_turn_on(const led_t *led) 
{
    if (led == NULL) 
    {
      errno = EINVAL;
      return -1;
    }

    return led_set_brightness(led, led->max_brightness);
}

int led_turn_off(const led_t *led) 
{
return led_set_brightness(led, 0);
}






