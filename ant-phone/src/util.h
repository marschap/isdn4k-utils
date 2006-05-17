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

#ifndef _ANT_UTIL_H
#define _ANT_UTIL_H

#include "config.h"

/* regular GNU system includes */
#ifdef HAVE_SYS_TIME_H
  #include <sys/time.h>
#endif
#include <time.h>

#define SHORT_INTERVAL 10000
/* 10 milliseconds */

int timeval_subtract (struct timeval *result,
		      struct timeval *x,
		      struct timeval *y);
void ant_sleep(int usecs);
char *un_vanity(char *s);
char *timediff_str(time_t time1, time_t time0);
char *ltostr(long int i);
void execute(char *command);
char *get_homedir(void);
int touch_dir(char *dirname);
int touch_dotdir(void);
int substitute(char **s, char *x, char *y);
char *util_digitstime(time_t *time);
char* stripchr(char* s, char c);
char* filename_extension(char* fn);
int output_codeset_save(void);
int output_codeset_set(char* codeset);

#endif /* util.h */
