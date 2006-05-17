/*
 * miscellaneous glibc extending functions
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2002, 2003 Roland Stigge
 *
 * ANT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ANT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ANT; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "config.h"

/* regular GNU system includes */
#include <stdio.h>
#include <unistd.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#ifdef HAVE_SYS_TIME_H
  #include <sys/time.h>
#endif
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <sys/stat.h>
#include <errno.h>

/* own header files */
#include "globals.h"
#include "util.h"

/* declared in globals.h, defined here */
int debug = 0;

/* Subtract the `struct timeval' values X and Y,
 *    storing the result in RESULT.
 *    Return 1 if the difference is negative, otherwise 0.
 */
int timeval_subtract (struct timeval *result,
		      struct timeval *x,
		      struct timeval *y) {
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
      int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
      y->tv_usec -= 1000000 * nsec;
      y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
      int nsec = (x->tv_usec - y->tv_usec) / 1000000;
      y->tv_usec += 1000000 * nsec;
      y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

/* sleeps usecs microseconds (will be interrupted by signals) */
void ant_sleep(int usecs) {
  struct timespec requested;
  struct timespec remaining;

  requested.tv_sec = 0;
  requested.tv_nsec = usecs * 1000;
  nanosleep(&requested, &remaining);
}

/*
 * convert vanity number (case insensitive: 'b','B' -> '2') in-place
 */
char *un_vanity(char *s) {
  char *temp = s;

  while (*temp) {
    switch (*temp) {
    case 'a': case 'b': case 'c': case 'A': case 'B': case 'C':
      *temp = '2';
      break;
    case 'd': case 'e': case 'f': case 'D': case 'E': case 'F':
      *temp = '3';
      break;
    case 'g': case 'h': case 'i': case 'G': case 'H': case 'I':
      *temp = '4';
      break;
    case 'j': case 'k': case 'l': case 'J': case 'K': case 'L':
      *temp = '5';
      break;
    case 'm': case 'n': case 'o': case 'M': case 'N': case 'O':
      *temp = '6';
      break;
    case 'p': case 'q': case 'r': case 's':
    case 'P': case 'Q': case 'R': case 'S':
      *temp = '7';
      break;
    case 't': case 'u': case 'v': case 'T': case 'U': case 'V':
      *temp = '8';
      break;
    case 'w': case 'x': case 'y': case 'z':
    case 'W': case 'X': case 'Y': case 'Z':
      *temp = '9';
      break;
    }
    temp++;
  }

  return s;
}

/*
 * returns a string representing the time difference between the
 * specified times
 *
 * NOTE: caller has to free() the returned string itself.
 */
char *timediff_str(time_t time1, time_t time0) {
  int duration;
  char *buf;
  int hours;
  int minutes;
  int seconds;
  
  buf = (char *)malloc(9);

  duration = (int)difftime(time1, time0);
  seconds = duration % 60;
  minutes = (duration / 60) % 60;
  hours = duration / (60 * 60);
  if (snprintf(buf, 9, "%02d:%02d:%02d", hours, minutes, seconds) >= 9)
    buf[8] = '\0';

  return buf;
}

/*
 * returns a string with the specified long int as content
 *
 * NOTE: caller has to free() the returned memory
 */
char *ltostr(long int i) {
  char *s;

  if (asprintf(&s, "%ld", i) < 0)
    return NULL;

  return s;
}

/*
 * forks and executes given command (with arguments) via sh
 */
void execute(char *command) {
  pid_t result = fork();
  char *argv[] = {"sh", "-c", command, NULL};

  if (result == -1) { /* error */
    fprintf(stderr, "Fork error.\n");
  } else if (result == 0) { /* we are the child */
    if (execvp("sh", argv)) {
      fprintf(stderr, "Exec error.\n");
      exit(1);
    }
  }
}

/*
 * return home dir, returned buffer must not be freed but will be overwritten
 * by subsequent calls (or calls to getpwuid())
 * returns NULL on error
 */
char *get_homedir(void) {
  struct passwd *user;

  if (!(user = getpwuid(getuid()))) {
    return NULL;
  }
  
  if (user->pw_dir == NULL || !strcmp(user->pw_dir, "") ||
      user->pw_dir[strlen(user->pw_dir) - 1] == '/') {
    return NULL;
  }

  return user->pw_dir;
}

/*
 * When the specified directory doesn't exist, create it (755)
 * (used by options and history write functions)
 * returns 0 on success, -1 otherwise
 */
int touch_dir(char *dirname) {

  if (!mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
    return 0;
  
  if (errno == EEXIST) return 0;

  return -1;
}

/*
 * Creates dotdir (in homedir) if it doesn't exist
 *
 * returns 0 on success, -1 otherwise
 */
int touch_dotdir(void) {
  char *homedir;
  char *filename;

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return -1;
  }

  if (asprintf(&filename, "%s/." PACKAGE, homedir) < 0) {
    fprintf(stderr,
           "Warning: Couldn't allocate memory for configuration directory.\n");
    return -1;
  }
  if (touch_dir(filename) < 0) {
    if (debug)
      fprintf(stderr, "Warning: Can't reach configuration directory.\n");
    return -1;
  }
  free(filename);

  return 0;
}

/*
 * in the string s, substitute x by y
 * NOTE: s is dynamically allocated and will be re-allocated by this function
 *
 * returns the number of substitutions
 */
int substitute(char **s, char *x, char *y) {
  int result = 0;
  char *pos;
  char *temp;
  
  while ((pos = strstr(*s, x))) { /* while we found something: substitute */
    *pos = '\0';
    asprintf(&temp, "%s%s%s", *s, y, &pos[strlen(x)]);
    free(*s);
    *s = temp;

    result ++;
  }
 
  return result;
}

/*
 * returns the date in the form of only digits, e.g. YYYYMMDDhhmmss
 * the resulting string is dynamically allocated and should be deallocated
 * with free().
 *
 * returns NULL on error
 */
char *util_digitstime(time_t *time) {
  char *s = (char *)malloc(15);
  int len;
  
  s[0] = '\1';
  len = strftime(s, 15, "%Y%m%d%H%M%S", localtime(time));

  if (len == 0 && s[0] != '\0') {
    return NULL;
  } else {
    return s;
  }
}

/*
 * returns a string where all occurences of the character c are removed
 * from the input string s
 * NOTE: caller has to free() the result
 */
char* stripchr(char* s, char c) {
  int count = 1; /* trailing space */
  char* temp;
  char* result;

  for (temp = s; *temp; temp++) {
    if (*temp != c)
      count++;
  }
  if ((result = (char*) malloc (count))) {
    temp = result;
    while (*s) {
      if (*s != c) {
	*temp = *s;
	temp ++;
      }
      s++;
    }
    *temp = '\0';
  }

  return result;
}

/*
 * returns the filename extension for the specified file / path
 * or NULL on problems
 */
char* filename_extension(char* fn) {
  char* result = strrchr(fn, '.');
  if (result) {
    result++;
    if (strchr(result, '/')) {
      result = NULL;
    }
  }
  return result;
}

/*
 * saves the current codeset globally for further calls to output_codeset_set()
 * -> returns 0 on success, -1 otherwise (default: sets codeset to "UTF-8")
 */
static char* output_codeset = NULL; /* the saved codeset */
int output_codeset_save(void) {
  if (!(output_codeset = bind_textdomain_codeset(PACKAGE, NULL))) {
    output_codeset = strdup("UTF-8");
    return -1;
  } else {
    output_codeset = strdup(output_codeset);
    return 0;
  }
}

/*
 * sets the output codeset GNU gettext produces while translating messages
 * if codeset == 0, a saved values will be used.
 * (save with: output_codeset_save())
 * -> returns 0 on success, -1 otherwise
 */
int output_codeset_set(char* codeset) {
  if (!codeset)
    codeset = output_codeset;
  if (!bind_textdomain_codeset(PACKAGE, codeset)) {
    fprintf(stderr, "Error setting gettext output codeset to %s.\n", codeset);
    return -1;
  }
  return 0;
}

