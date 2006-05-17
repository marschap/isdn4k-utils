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

#ifdef HAVE_TERMIOS_H
  #include <termios.h>
#endif

#define LOCK_PATH "/var/lock"

#define DEFAULT_ISDNBUF_SIZE 255
#define ISDN_COMMAND_TIMEOUT 1

/* 0 (master MSN) or MSN to use as identification on outgoing calls */
#define DEFAULT_MSN "0"

/* "*" (wildcard) or comma-separated list of MSNs */
#define DEFAULT_MSNS "*"

#define ISDN_SPEED 8000

#define ETX 0x03
#define DC4 0x14
#define DLE 0x10

#define ISDN_CONFIG_FILENAME "/etc/isdn/isdn.conf"

extern char* isdn_calls_filename_from_config;

int open_isdn_device(char **isdn_device_name, char **isdn_lockfile_name);
int init_isdn_device(int isdn_fd, struct termios *backup);
int deinit_isdn_device(int isdn_fd, struct termios *backup);
int isdn_setMSN(int isdn_fd, char *msn);
int isdn_setMSNs(int isdn_fd, char *msns);
int isdn_stop_audio(int isdn_fd, int self_hangup);
int isdn_hangup(int isdn_fd);
int isdn_blockmode(int isdn_fd, int flag);
int close_isdn_device(int isdn_fd, char *isdn_device_name,
		      char *isdn_lockfile_name);
int isdn_dial(int isdn_fd, char *number, int timeout);
int isdn_set_full_duplex(int isdn_fd);
int tty_read(int fd, char *s, int timeout, char **got);
int tty_write(int fd, char *s);
int tty_clear(int fd);
char* isdn_get_calls_filename(void);

