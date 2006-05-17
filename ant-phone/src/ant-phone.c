/*
 *
 *  ANT (ANT is Not a Telephone)
 *  Copyright 2002, 2003 Roland Stigge
 *
 *  ISDN Telephone application for Linux and OSS
 *  (full duplex or multiple sound devices needed)
 *
 *  Author: Roland Stigge, stigge@antcom.de
 *  Created: 2002-09-04  (got the idea already around christmas 2000)
 *
 *  Tested with: Debian GNU/Linux
 *
 *
 *  Some basic ideas for this program are based on:
 *    * IVCALL, Copyright 2000 Lennart Poettering <lennart@poettering.de> (GPL)
 *
 *  This program uses sources from:
 *    * g711.c (by Sun Microsystems, BSD like license) from SoX
 *
 *  Used libraries:
 *    * libsndfile (Erik de Castro Lopo <erikd@zip.com.au>)
 *    * GTK+
 *
 *  Contributors: see AUTHORS file
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "config.h"

/* regular GNU system includes */
#include <stdio.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#ifdef HAVE_STDDEF_H
  #include <stddef.h>
#endif
#include <string.h>
#include <math.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
  #include <sys/types.h>
#endif
#ifdef HAVE_TERMIOS_H
  #include <termios.h>
#endif
#ifdef HAVE_FCNTL_H
  #include <fcntl.h>
#endif
#include <sys/ioctl.h>
#ifdef HAVE_LIMITS_H
  #include <limits.h>
#endif
#include <sys/time.h>
#include <getopt.h> /* GNU extension: long options */
#include <sched.h>
#include <locale.h>
#include <libintl.h>

/* For debugging purposes */
#include <mcheck.h>

/* own header files */
#include "globals.h"
#include "isdn.h"
#include "sound.h"
#include "mediation.h"
#include "gtk.h"
#include "session.h"
#include "util.h"
#include "client.h"
#include "server.h"

int main(int argc, char *argv[]) {
  struct option long_options[] = {
    {"help",     no_argument,       0, 'h'},
    {"usage",    no_argument,       0, 'h'},
    {"version",  no_argument,       0, 'v'},
    {"cleanup",  no_argument,       0, 'r'},
    {"debug",    optional_argument, 0, 'd'},
    {"soundin",  required_argument, 0, 'i'},
    {"soundout", required_argument, 0, 'o'},
    {"msn",      required_argument, 0, 'm'},
    {"msns",     required_argument, 0, 'l'},
    {"call",     required_argument, 0, 'c'},
    {0, 0, 0, 0}
  };
  char *short_options = "hvrd::i:o:m:l:c:";
  int option_index = 0;
  int c;

  /* temporary default values = "", will be changed by command line options
     and options in options file */
  char *audio_device_name_in = "";
  char *audio_device_name_out = "";
  char *msn = "";
  char *msns = "";

  int gtk_result;

#ifdef DEBUG
  mtrace();
  debug = DEBUG;
#endif

  /* prepare for i18n */
#ifdef ENABLE_NLS
  setlocale(LC_ALL, "");
  if (!bindtextdomain(PACKAGE, LOCALEDIR)) {
    fprintf(stderr, "Error setting directory for textdomain (i18n).\n");
  }
  output_codeset_save();
  if (!textdomain(PACKAGE)) {
    fprintf(stderr, "Error setting domainname for gettext() "
                    "(internationalization).\n");
  }
#endif

  /* only useful for yet to code separate audio thread
     (rt only for audio thread!)
  struct sched_param schedpar; // scheduler structure for priority
  int priority_min, priority_max;

  // linux realtime scheduling
  if ((priority_min = sched_get_priority_min(SCHED_FIFO)) < 0) {
    perror("Realtime scheduling setup");
    exit(1);
  }
  if ((priority_max = sched_get_priority_max(SCHED_FIFO)) < 0) {
    perror("Realtime scheduling setup");
    exit(1);
  }
  schedpar.sched_priority = (priority_min + priority_max) / 2; // arbitrary
  if (sched_setscheduler(0, SCHED_FIFO, &schedpar) < 0) {
    perror("Activating realtime scheduler");
  }
  // give up root privileges
  if (setuid(getuid()) < 0) {
    perror("Giving up root privileges");
  }
  */

  /* first: GTK+ internal parsing of command line arguments */
  gtk_init(&argc, &argv);

  /* parse command line options */
  while ((c = getopt_long(argc, argv, short_options, long_options,
			  &option_index)) != -1) {
    switch(c) {
    case 'h': /* help */
      printf(_("Usage: %s [OPTION...]\n\
\n\
Options:\n\
  -h, --help              Show this help message\n\
  -v, --version           Print version information\n\
  -r, --cleanup           Remove stale socket file (left by accident by\n\
                            previous run)\n\
  -d, --debug[=level]     Print additional runtime debugging data to stdout\n\
                            level = 1..2\n\
  -i, --soundin=DEVICE    OSS compatible device for input (recording),\n\
                            default: /dev/dsp\n\
  -o, --soundout=DEVICE   OSS compatible device for output (playback),\n\
                            default: /dev/dsp\n\
  -m, --msn=MSN           identifying MSN (for outgoing calls), 0 for master\n\
                          MSN of this termination/port\n\
                            default: 0\n\
  -l, --msns=MSNS         MSNs to listen on, semicolon-separated list or '*'\n\
                            default: *\n\
  -c, --call=NUMBER       Call specified number\n\
\n\
Note: If arguments of --soundin and --soundout are equal, a full duplex\n\
      sound device is needed.\n"), argv[0]);
      exit(0);
    case 'v': /* version */
      printf("ANT " VERSION "\n");
      exit(0);
    case 'r': /* clean up: remove old locking files */
      if (-1 == unlink(server_local_socket_name())) {
	perror("unlink old socket file");
      }
      break;
    case 'd': /* debug mode */
      if (optarg) {
	debug = strtol(optarg, NULL, 0);
      } else {
	debug = 1;
      }
      break;
    case 'i': /* sound input device */
      audio_device_name_in = optarg;
      break;
    case 'o': /* sound output device */
      audio_device_name_out = optarg;
      break;
    case 'm': /* identifying msn */
      msn = optarg;
      break;
    case 'l': /* msns to listen on, '*' or semicolon-separated list */
      msns = optarg;
      break;
    case 'c':
      printf(_("Calling %s... "), optarg);
      if (client_make_call(optarg)) {
	printf("\nAn error occured while calling a running " PACKAGE ".\n");
      } else {
	printf(_("successful.\n"));
      }
      exit(0);
      break;
    case '?':
      exit(1);
    }
  }
  /* no further arguments expected, so not handled */
 
  output_codeset_set("UTF-8"); /* GTK needs UTF-8 strings */
  
  if (session_init(&session, audio_device_name_in, audio_device_name_out,
		   msn, msns))
  {
    fprintf(stderr, "Error at session init.\n");
    exit(1);
  } else {
    if (debug)
      fprintf(stderr, "Init OK.\n");
  }
      
  /* gtk stuff, main loop */
  gtk_result = main_gtk(&session);

  if (session_deinit(&session)) {
    fprintf(stderr, "Error at session exit\n");
    exit(1);
  } else {
    if (debug)
      fprintf(stderr, "Quit OK.\n");
  }
  output_codeset_set(NULL); /* restore saved codeset */

  return gtk_result;
}

