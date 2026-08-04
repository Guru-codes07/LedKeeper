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
static const char *SCROLLLOCK_NAME_VARIANTS[] = { "scrolllock","scroll_lock","scroll-lock","scrl"};
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

