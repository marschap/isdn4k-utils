/*
 * ISDN handling functions
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

/* GNU headers */
#include <stdio.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#ifdef HAVE_TERMIOS_H
  #include <termios.h>
#endif
#ifdef HAVE_FCNTL_H
  #include <fcntl.h>
#endif
#ifdef HAVE_SYS_TIME_H
  #include <sys/time.h>
#endif
#include <string.h>
#include <signal.h>
#include <errno.h>

/* own header files */
#include "globals.h"
#include "isdn.h"

static char* calls_filenames[] =
{ "/var/lib/isdn/calls", "/var/log/isdn/calls", "/var/log/isdn.log" };
char* isdn_calls_filename_from_config = NULL;

/*
 * locks (via lock file) and opens a (free) ttyI device
 *
 * output:
 *   isdn_device_name, isdn_lockfile_name
 *
 * returns isdn file descriptor on success, -1 otherwise
 */
int open_isdn_device(char **isdn_device_name, char **isdn_lockfile_name) {
  int i = 0; /* try from 0 */
  int found = 0; /* free file name found */
  char buf[64];
  int fd;
  int n;
  int pid;

  while (i < 64 && !found) {
    snprintf(buf, sizeof(buf), "%s/LCK..ttyI%d", LOCK_PATH, i);
    *isdn_lockfile_name = strdup(buf);
    if ((fd = open(*isdn_lockfile_name, O_RDONLY)) >= 0) { /* exists */
      /* stale? -> remove (for re-use) */
      n = read(fd, buf, sizeof(buf) - 1);
      close(fd);
      if (n > 0) {
	if (n == 4) {
	  if (sizeof(int) == 4)
	    pid = *(int*) buf;
	  else
	    pid = 0;
	} else {
	  buf[n] = 0;
	  sscanf(buf, "%d", &pid);
	}
	if (pid > 0) {
	  if (kill((pid_t) pid, 0) < 0 && errno == ESRCH) { /* stale */
	    if (!unlink(*isdn_lockfile_name))
	      found = 1;    /* file removed */
	  }
	}
      }
    } else {
      if (errno == ENOENT)
	found = 1;          /* file doesn't exist */
    }
    if (!found) i++;
  }

  if (found && i < 64) { /* got a valid lock file name */
    /* lock device */
    if ((fd = open(*isdn_lockfile_name, O_WRONLY | O_CREAT, 0666)) < 0) {
      return -1;
    }
    snprintf(buf, sizeof(buf), "%10ld\n", (long)getpid());
    write(fd, buf, strlen(buf));
    close(fd);

    /* finally name and open the device itself */
    snprintf(buf, sizeof(buf), "/dev/ttyI%d", i);
    *isdn_device_name = strdup(buf);

    /* tty device would possibly block on open -> O_NONBLOCK */
    return open(*isdn_device_name, O_RDWR | O_NONBLOCK);
  } else
    return -1;
}

/*
 * tries to read the given string from the specified tty (e.g. a ttyI)
 * (waits for the string if not read immediately)
 *
 * input:
 *   fd       tty file descriptor
 *   s        (0-terminated) string to compare with
 *   timeout  give up after this number of seconds
 *               0: return immediately if no data available
 *              -1: no timeout
 *   got      if got != NULL, store reference to result there
 * 
 * output:
 *   got      pointer to string containing buffer actually read if called
 *              with got != NULL
 *              -> will be NULL on error
 *            NOTE: caller is responsible to free the referenced buffer
 *
 * returns 0 on success, -1 otherwise
 *
 * NOTE: currently uses a fixed size buffer, implying a maximum number
 *       of bytes read
 */
int tty_read(int fd, char *s, int timeout, char **got) {
  int failed = 0;   /* 0 or 1*/
  int buf_last = 0; /* index of end of string ('\0') */
  char buf[256] = "";
  struct timeval tv, *tvp;
  fd_set fds;
  
  if (timeout >= 0) {
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    tvp = &tv;
  } else
    tvp = NULL;
  
  while (!strstr(buf, s) && !failed) {
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    if (buf_last == sizeof(buf) - 1) /* buffer full */
      failed = 1;
    else {
      if (select(FD_SETSIZE, &fds, 0, 0, tvp) == 1) { /* input ready */
	read(fd, &buf[buf_last], 1); /* read 1 more byte */
	buf[++buf_last] = 0;
      } else { /* timeout or signal */
	failed = 1;
      }
    }
  }
  /* printf("%s\n", buf); */
  if (got != NULL) {
    *got = strdup(buf);
  }
  return -failed;
}

/*
 * writes specified 0-terminated string to the specified tty
 *
 * returns 0 on success, -1 otherwise
 */
int tty_write(int fd, char *s) {
  if (write(fd, s, strlen(s)) != (int)strlen(s)) {
    return -1;
  }
  return 0;
}

/*
 * clears input and output queue of specified tty
 *
 * returns 0 on success, -1 otherwise
 */
int tty_clear(int fd) {
  return tcflush(fd, TCIOFLUSH);
}

/*
 * ISDN initialization
 *
 * returns 0 on success, -1 otherwise
 */
int init_isdn_device(int isdn_fd, struct termios *backup) {
  int failed = 0;
  int flags;
  char *(init_commands[]) = {
    "AT&F\n",        "OK\r\n",   /* restore factory settings                 */
    "ATE0\n",        "OK\r\n",   /* echo off                                 */
    /*"AT&B128\n",     "OK\r\n",*/   /* set outgoing packet size */
    "ATI\n",         "Linux ISDN\r\nOK\r\n", /* check for real isdn device   */
    "AT+FCLASS=8\n", "OK\r\n",   /* enable audio mode                        */
    "AT+VSM=6\n",    "OK\r\n",   /* set uLaw format                          */
    "ATS18=1\n",     "OK\r\n",   /* set audio mode (dial out)                */
    "ATS14=4\n",     "OK\r\n",   /* layer-2 protocol = transparent           */
    "ATS13.4=1\n",   "OK\r\n",   /* CALLER NUMBER after first RING           */
    "ATS13.6=1\n",   "OK\r\n",   /* enable RUNG messages */
    "ATS23=1\n",     "OK\r\n"    /* Calling Party Number (CPN) extended RING */
  };
  struct termios settings;
  unsigned int i;

  /* assume:
   *   ttyI speed is 64000 by default
   */
  
  /* switch O_NONBLOCK off (turned on while opening) */
  flags = fcntl(isdn_fd, F_GETFL, 0);
  if (flags != -1) {
    if (fcntl(isdn_fd, F_SETFL, flags & ~O_NONBLOCK) == -1) {
      perror("G_GETFL");
      return -1;
    }
  } else {
    perror("F_GETFL");
    return -1;
  }

  if (tcgetattr(isdn_fd, &settings))
    return -1;

  memcpy(backup, &settings, sizeof(struct termios));

  settings.c_lflag &= ~(ICANON | ECHO | ECHONL | ECHOCTL | ISIG);
  settings.c_iflag &= ~(IXON | IXOFF | IXANY | IGNCR | ICRNL | INLCR );
  settings.c_iflag |= IGNPAR;
  settings.c_cflag |= HUPCL;
  settings.c_oflag &= ~ONLCR;

  settings.c_cc[VMIN] = 1;
  settings.c_cc[VTIME] = 0;
  
  if (tcsetattr(isdn_fd, TCSANOW, &settings))
    return -1;

  i = 0;
  
  while (!failed && i < sizeof(init_commands) / sizeof(char*)) {
    tty_clear(isdn_fd);
    if (tty_write(isdn_fd, init_commands[i++]))
      failed = 1;
    else
      if (tty_read(isdn_fd, init_commands[i++], ISDN_COMMAND_TIMEOUT, NULL))
	failed = 1;
  }

  return -failed;
}

/*
 * ISDN de-initialization, restores termios settings
 *
 * returns 0 on success, -1 otherwise
 */
int deinit_isdn_device(int isdn_fd, struct termios *backup) {
  return tcsetattr(isdn_fd, TCSANOW, backup);
}

/*
 * sets an MSN for the specified ttyI (originating MSN)
 * fd is assumed to be in command mode
 *
 * returns 0 on success, -1 otherwise
 */
int isdn_setMSN(int isdn_fd, char *msn) {
  char buf[256];

  if (snprintf(buf, sizeof(buf), "AT&E%s\n", msn) >= (int)sizeof(buf)) {
    fprintf(stderr, "Error: Specified MSN too long.\n");
    return -1;
  }
  tty_clear(isdn_fd);
  if (tty_write(isdn_fd, buf))
    return -1;
  else
    if (tty_read(isdn_fd, "OK\r\n", ISDN_COMMAND_TIMEOUT, NULL))
      return -1;
  return 0;
}

/*
 * sets MSNs for the specified ttyI (MSNs to listen on)
 * fd is assumed to be in command mode
 * msns: semicolon-separated list of msns
 *
 * returns 0 on success, -1 otherwise
 */
int isdn_setMSNs(int isdn_fd, char *msns) {
  char buf[256];

  if (snprintf(buf, sizeof(buf), "AT&L%s\n", msns) >= (int)sizeof(buf)) {
    fprintf(stderr, "Error: Specified MSNs string too long.\n");
    return -1;
  }
  tty_clear(isdn_fd);
  if (tty_write(isdn_fd, buf))
    return -1;
  else
    if (tty_read(isdn_fd, "OK\r\n", ISDN_COMMAND_TIMEOUT, NULL))
      return -1;
  return 0;
}

/*
 * stops audio mode (enter command mode)
 *
 * input: self_hangup: we want to hang up ourselves
 *
 * returns 0 on success, -1 otherwise
 */
int isdn_stop_audio(int isdn_fd, int self_hangup) {
  int result = 0;
  unsigned char abort_sending[] = {DLE, DC4, 0};
  unsigned char end_of_audio[] = {DLE, ETX, 0};

  char *got;

  tty_clear(isdn_fd);

  if (self_hangup) { /* we want to hang up ourselves */
    if (tty_write(isdn_fd, end_of_audio)) {   /* ETX - end of audio */
      fprintf(stderr, "Error sending ETX (End of audio).\n");
      return -1;
    }
    if (tty_write(isdn_fd, abort_sending)) {/* abort sending (request) (DC4) */
      fprintf(stderr, "Error sending DC4 (abort sending).\n");
      return -1;
    }
    if (tty_read(isdn_fd, end_of_audio, ISDN_COMMAND_TIMEOUT, NULL)) {
      fprintf(stderr, "Error waiting for ETX (End of audio).\n");
      return -1;
    }
    if (tty_read(isdn_fd, "\r\n", ISDN_COMMAND_TIMEOUT, NULL)) {
      fprintf(stderr, "Error getting line break.\n");
      return -1;
    }
    if (tty_read(isdn_fd, "\r\n", ISDN_COMMAND_TIMEOUT, &got)) {
      fprintf(stderr, "Error getting new line.\n");
    }
    if (!strstr(got, "VCON")) {
      fprintf(stderr, "Error getting status.\n");
      result = -1;
    }
    free(got);
  } else { /* remote side hangup */
    if (tty_write(isdn_fd, end_of_audio)) {   /* ETX - end of audio */
      fprintf(stderr, "Error sending ETX (End of audio).\n");
      return -1;
    }
    /* there doesn't seem to come anything after remote hangup ... (?) */
  }
  tty_clear(isdn_fd);
  return result;
}

/*
 * hang up isdn device
 *
 * NOTE: assumes command mode
 *
 * returns 0 on success, -1 otherwise
 */
int isdn_hangup(int isdn_fd) {
  tty_clear(isdn_fd);
  if (tty_write(isdn_fd, "ATH\n"))
    return -1;
  if (tty_read(isdn_fd, "OK\r\n", ISDN_COMMAND_TIMEOUT, NULL))
    return -1;
  tty_clear(isdn_fd);
  return 0;
}

/*
 * Set isdn device to block mode, assuming in audio mode
 *
 * in blockmode, the minimum number of bytes possible to read from specified
 *   ttyI will be set to DEFAULT_ISDNBUF_SIZE and non blocking reads and writes
 *   will be enabled
 *
 * input:
 *   isdn_fd         file descriptor
 *   flag            0 == off, 1 == on
 *
 * returns 0 on success, -1 otherwise
 */
int isdn_blockmode(int isdn_fd, int flag) {
  struct termios settings;
  int min, time;
  int flags;

  flags = fcntl(isdn_fd, F_GETFL, 0);
  if (flags == -1) {
    perror("F_GETFL");
    return -1;
  }

  if (flag) {
    min = DEFAULT_ISDNBUF_SIZE;
    time = 0;                    /* 10 == 1sec */
    flags |= O_NONBLOCK; /* select consumes much cpu time with this */
  } else {
    min = 1;
    time = 0;
    flags &= ~O_NONBLOCK;
  }

  if (fcntl(isdn_fd, F_SETFL, flags) == -1) {
    perror("G_GETFL");
    return -1;
  }

  if (tcgetattr(isdn_fd, &settings)) {
    perror("isdn_blockmode, tcgetattr");
    return -1;
  }

  settings.c_cc[VMIN] = min;
  settings.c_cc[VTIME] = time;
  
  if (tcsetattr(isdn_fd, TCSANOW, &settings)) {
    perror("isdn_blockmode, tcsetattr");
    return -1;
  }

  /* verify */
  if (tcgetattr(isdn_fd, &settings))
    return -1;
  if (settings.c_cc[VMIN] != min) {
    fprintf(stderr,"Error setting block size. New block size: %d.\n",
	    settings.c_cc[VMIN]);
    return -1;
  }

  return 0;
}

/*
 * closes ttyI device and removes lock file
 *
 * returns 0 on success, -1 otherwise
 */
int close_isdn_device(int isdn_fd, char *isdn_device_name,
		      char *isdn_lockfile_name) {
  if (close(isdn_fd)) {
    return -1;
  }
  if (unlink(isdn_lockfile_name)) {
    fprintf(stderr, "Removing isdn device lock file: unlink error.\n");
    return -1;
  }
  free(isdn_lockfile_name);
  free(isdn_device_name);
  return 0;
}

/*
 * dials specified number (voice call)
 * (proper ttyI init is assumed)
 *
 * returns:
 *   0 on success (VCON)
 *   1 on busy
 *  -1 otherwise (error / timeout)
 */
int isdn_dial(int isdn_fd, char *number, int timeout) {
  char *s;
  int result;
  char *got;

  if ((s = (char*) malloc(strlen(number) + 5))) {
  
    snprintf(s, strlen(number) + 5, "ATD%s\n", number);
    tty_clear(isdn_fd);
    result = tty_write(isdn_fd, s);
    free(s);
    if (result) {
      fprintf(stderr, "Error dialing.\n");
      return -1;
    }
  }

  if (tty_read(isdn_fd, "\r\n", timeout, NULL)) /* error / timeout */
    return -1;

  if (tty_read(isdn_fd, "\r\n", ISDN_COMMAND_TIMEOUT, &got))
    return -1;

  if (strstr(got, "BUSY\r\n")) {
    free(got);
    return 1;
  }

  if (strstr(got, "VCON\r\n")) {
    free(got);
    return 0;
  }

  free(got);
  return -1; /* failed somehow */
}

/*
 * sets ttyI to full duplex mode
 * (should be called directly after VCON)
 *
 * returns 0 on success, -1 otherwise
 */
int isdn_set_full_duplex(int isdn_fd) {
  tty_clear(isdn_fd);
  if (tty_write(isdn_fd, "AT+VRX+VTX\n"))
    return -1;
  return 0;
}

/*
 * returns the name of the calls file (originally just /var/lib/isdn/calls)
 * on error, returns NULL
 */
char* isdn_get_calls_filename(void) {
  unsigned int i;
  int fd;

  if (isdn_calls_filename_from_config &&
      (fd = open(isdn_calls_filename_from_config, O_RDONLY)) != -1)
  {
    close(fd);
    if (debug)
      fprintf(stderr,
	      "Using calls file listed in I4L config.\n");
    return isdn_calls_filename_from_config;
  }
  for (i = 0; i < sizeof(calls_filenames) / sizeof(char*); i++) {
    if ((fd = open(calls_filenames[i], O_RDONLY)) != -1) {
      close(fd);
      return calls_filenames[i];
    }
  }
  return NULL;
}

