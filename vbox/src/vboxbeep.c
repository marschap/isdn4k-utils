/*
** $Id: vboxbeep.c,v 1.3 1997/04/04 09:32:42 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include "config.h"

#if TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   if HAVE_SYS_TIME_H
#      include <sys/time.h>
#   else
#      include <time.h>
#   endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <paths.h>
#include <string.h>
#include <dirent.h>
#include <syslog.h>

#include "vboxbeep.h"
#include "libvbox.h"

/** Variables *************************************************************/

static struct messagebox messageboxes[MAX_MESSAGE_BOXES];

static char sound_available_hours[24];

static char	  *vbasename	= NULL;
static char	  *pidname		= NULL;
static int	 	killmode		= 0;
static time_t	starttime	= 0;
static int     dodebug     = FALSE;
static int     haveconnect = FALSE;
static int     downconnect = FALSE;

static struct option arguments[] =
{
	{ "version"		, no_argument			, NULL, 'v' },
	{ "help"			, no_argument			, NULL, 'h' },
	{ "kill"			, no_argument			, NULL, 'k' },
	{ "sound" 		, required_argument	, NULL, 's' },
	{ "messagebox" , required_argument  , NULL, 'm' },
	{ "machine"    , required_argument  , NULL, 'i' },
	{ "port"       , required_argument  , NULL, 'o' },
	{ "pause"      , required_argument  , NULL, 'p' },
	{ "debug"      , no_argument        , NULL, 'x' },
	{ NULL			, 0						, NULL, 0   }
};

/** Prototypes ************************************************************/

static void   lets_hear_the_sound(void);
static int	  add_dir_to_messagebox(int, char *);
static void	  free_all_messageboxes(void);
static time_t get_newest_message_time(char *);
static void   beep(int);
static int    get_pid_from_file(void);
static void   remove_pid_file(void);
static void   create_pid_file(void);
static void   leave_program(int);
static void   set_new_starttime(int);
static void   free_resources(void);
static void   init_signals();
static void   log(int, char *, ...);
static void   parse_sound_times(char *, int, int);
static void   version(void);
static void   usage(void);

/**************************************************************************/
/** The magic main...                                                    **/
/**************************************************************************/

int main(int argc, char **argv)
{
	char  timestrings[32];
	int   i;
	int   b;
	int   opts;
	int   checkpause;
	int   port;
	char *machine;

#if HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif

#if ENABLE_NLS
	textdomain(PACKAGE);
#endif

	if (!(vbasename = rindex(argv[0], '/')))
		vbasename = argv[0];
	else
		vbasename++;

	for (i = 0; i < MAX_MESSAGE_BOXES; i++)
	{
		messageboxes[i].name = NULL;
		messageboxes[i].time = 0;
	}

	parse_sound_times(NULL, 0, 23);

	if (!(pidname = malloc(strlen(PIDFILEDIR) + strlen("/vboxbeep.pid") + 1)))
	{
		log(LOG_ERR, gettext("not enough memory to create pid name.\n"), vbasename);
		
		exit(5);
	}

	printstring(pidname, "%s/vboxbeep.pid", PIDFILEDIR);

	killmode		= 0;
	starttime	= 0;
	checkpause  = 5;
	port        = VBOXD_DEF_PORT;
	machine     = "localhost";
	
	b = 0;

	while ((opts = getopt_long(argc, argv, "vhko:i:s:m:p:x", arguments, (int *)0)) != EOF)
	{
		switch (opts)
		{
			case 'x':
				dodebug = TRUE;
				break;

			case 'k':
				killmode = 1;
				break;

			case 'p':
				checkpause = (int)xstrtol(optarg, 5);
				break;

			case 'm':
				if (add_dir_to_messagebox(b, optarg)) b++;
				break;
				
			case 's':
				parse_sound_times(strdup(optarg), 0, 23);
				break;

			case 'o':
				port = (int)xstrtol(optarg, VBOXD_DEF_PORT);
				break;

			case 'i':
				machine = optarg;
				break;

			case 'v':
				version();
				break;

			case 'h':
			default:
				usage();
				break;
		}
	}

	init_signals();

	i = get_pid_from_file();

	if (i)
	{
		if (killmode == 0)
		{
			fprintf(stderr, gettext("%s: sending signal to stop beep's to process %d...\n"), vbasename, i);

			if (kill(i, SIGUSR1) != 0)
			{
				fprintf(stderr, gettext("%s: error sending signal (%s).\n"), vbasename, strerror(errno));
			}
		}
		else
		{
			if (getuid() == 0)
			{
				fprintf(stderr, gettext("%s: sending terminate signal to process %d...\n"), vbasename, i);

				if (kill(i, SIGTERM) != 0)
				{
					fprintf(stderr, gettext("%s: error sending signal (%s).\n"), vbasename, strerror(errno));
				}
			}
			else
			{
				fprintf(stderr, gettext("%s: you must be *root* to kill a running %s.\n"), vbasename, vbasename);
			}
		}

		free_resources();
		
		exit(1);
	}

	if (killmode == 1)
	{
		fprintf(stderr, gettext("%s: can't find running %s process.\n"), vbasename, vbasename);

		free_resources();
		
		exit(5);
	}

	if (getuid() != 0)
	{
		fprintf(stderr, gettext("%s: you must be *root* to start %s.\n"), vbasename, vbasename);

		free_resources();
		
		exit(5);
	}
	
	if (!b)
	{
		log(LOG_CRIT, gettext("there are no directories to watch."));
		
		free_resources();

		exit(5);
	}

	create_pid_file();

	for (i = 0, *timestrings = '\0'; i < 24; i++)
	{
		strcat(timestrings, sound_available_hours[i] ? "x" : "-");
	}
	
	if (dodebug)
	{
		log(LOG_DEBUG, gettext("time range \"%s\"."), timestrings);
	}

	haveconnect = FALSE;

	while (TRUE)
	{
		downconnect = FALSE;

		if (!haveconnect)
		{
			if (vboxd_connect(machine, port) == VBOXC_ERR_OK)
			{
				if (dodebug) log(LOG_DEBUG, gettext("connected to vbox message server."));

				haveconnect = TRUE;
			}
			else log(LOG_ERR, gettext("can't connect to vbox message server."));
		}

		if (haveconnect)
		{
			for (i = 0; i < MAX_MESSAGE_BOXES; i++)
			{
				if (messageboxes[i].name)
				{
					messageboxes[i].time = get_newest_message_time(messageboxes[i].name);
					
					if (messageboxes[i].time > starttime)
					{
						lets_hear_the_sound();
					
						break;
					}
				}
			}

			if (downconnect)
			{
				vboxd_disconnect();

				if (dodebug) log(LOG_DEBUG, gettext("server connection canceled."));

				haveconnect = FALSE;
			}
		}

		xpause(checkpause * 1000);
	}

	leave_program(0);
}

/**************************************************************************/
/** version(): Displays package version.                                 **/
/**************************************************************************/

static void version(void)
{
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("%s version %s (%s)\n"), vbasename, VERSION, VERDATE);
	fprintf(stderr, gettext("\n"));
	
	free_resources();
	exit(1);
}

/**************************************************************************/
/** usage(): Displays usage message.                                     **/
/**************************************************************************/

static void usage(void)
{
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("Usage: %s OPTION [ OPTION ] [ ... ]\n"), vbasename);
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("-s, --sound HOURS      Hours to signal with sound (default: no time)\n"));
	fprintf(stderr, gettext("-m, --messagebox DIR   Watch directory DIR for new messages.\n"));
	fprintf(stderr, gettext("-p, --pause SECONDS    Pause in seconds to sleep between checks (default: 5).\n"));
	fprintf(stderr, gettext("-i, --machine HOST     Connect to message server on HOST (default: localhost).\n"));
	fprintf(stderr, gettext("-o, --port PORT        Connect to message server on PORT (default: %d).\n"), VBOXD_DEF_PORT);
	fprintf(stderr, gettext("-h, --help             Displays this help text.\n"));
	fprintf(stderr, gettext("-v, --version          Displays program version.\n"));
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("Hours to play sound with the pc speaker must be specified in 24-hour-format.\nyou can use ',' or '-' to seperate hours or specify time ranges (eg 9,17-22).\nA '--sound=\"*\"' means at every time and '--sound=\"-\"' means at no time.\n\nThe option '--messagebox' can be used more than one time, so you can watch\nmax. %d seperate directories.\n"), MAX_MESSAGE_BOXES);
	fprintf(stderr, gettext("\n"));

	free_resources();
	exit(1);
}

/**************************************************************************/
/** init_signals(): Initialize the signals.                              **/
/**************************************************************************/

static void init_signals(void)
{
	signal(SIGTERM	, leave_program);
	signal(SIGHUP	, leave_program);
	signal(SIGUSR1	, set_new_starttime);
	signal(SIGQUIT	, SIG_IGN);
	signal(SIGINT	, leave_program);
}

/**************************************************************************/
/** Free_Resources(): Frees all used resources.                          **/
/**************************************************************************/

static void free_resources(void)
{
	if (pidname) free(pidname);

	pidname = NULL;
	
	free_all_messageboxes();
}

/**************************************************************************/
/** leave_program(): Leave the program.                                  **/
/**************************************************************************/

static void leave_program(int sig)
{
	int vt;
	
	if ((vt = open("/dev/console", O_RDONLY)) != -1)
	{
		ioctl(vt, KIOCSOUND, 0);
		close(vt);
	}

	remove_pid_file();
	free_resources();

	if (haveconnect)
	{
		if (dodebug) log(LOG_DEBUG, gettext("disconnecting from server..."));

		vboxd_disconnect();
	}

	if (dodebug) log(LOG_DEBUG, gettext("terminate..."));

	exit(0);
}

/**************************************************************************/
/** set_new_starttime(): Sets new watch start time.                      **/
/**************************************************************************/

static void set_new_starttime(int sig)
{
	init_signals();

	starttime = time(NULL);

	if (dodebug) log(LOG_DEBUG, gettext("new starttime is %ld."), starttime);
}

/**************************************************************************/
/** lets_hear_the_sound(): Plays one sound sequence.                     **/
/**************************************************************************/

static void lets_hear_the_sound(void)
{
	struct tm *timel;
	time_t	  timec;
	int		  vt;

	timec = time(NULL);

	if (!(timel = localtime(&timec)))
	{
		log(LOG_CRIT, gettext("can't get current local time (%s)."), strerror(errno));
		
		return;
	}

	if ((timel->tm_hour < 0) || (timel->tm_hour > 23))
	{
		log(LOG_CRIT, gettext("bad hour in local time structure (%d)."), timel->tm_hour);
		
		return;
	}
	
	if (sound_available_hours[timel->tm_hour] == TRUE)
	{
		if ((vt = open("/dev/console", O_RDWR)) == -1)
		{
			log(LOG_CRIT, gettext("can't open \"/dev/console\" (%s)."), strerror(errno));
		
			return;
		}

		beep(vt);
		beep(vt);
		beep(vt);

		close(vt);
	}
}

/**************************************************************************/
/** beep(): Plays one beep.                                              **/
/**************************************************************************/

static void beep(int vt)
{
	ioctl(vt, KIOCSOUND, 3200);

	xpause(100);

	ioctl(vt, KIOCSOUND, 0);

	xpause(25);
}

/**************************************************************************/
/** add_dir_to_messagebox(): Adds a directory to the watchlist.          **/
/**************************************************************************/

static int add_dir_to_messagebox(int nr, char *box)
{
	char *name;

	if (nr < MAX_MESSAGE_BOXES)
	{
		if ((name = strdup(box)))
		{
			messageboxes[nr].name = name;
			messageboxes[nr].time = time(NULL);
			
			returnok();
		}
	}

	log(LOG_ERR, gettext("can't add \"%s\" to watchlist."), box);

	returnerror();
}

/**************************************************************************/
/** free_all_messageboxes(): Frees messagebox resources.                 **/
/**************************************************************************/

static void free_all_messageboxes(void)
{
	int i;
	
	for (i = 0; i < MAX_MESSAGE_BOXES; i++)
	{
		if (messageboxes[i].name) free(messageboxes[i].name);
			
		messageboxes[i].name = NULL;
	}
}

/**************************************************************************/
/** get_newest_message_time(): Scans a messagebox and returns the time   **/
/**                            of the newest message.                    **/
/**************************************************************************/

static time_t get_newest_message_time(char *box)
{
	time_t  newest;
	char    response[128 + 1];
	char   *msg;
	char   *respcode;
	char   *respmsgs;
	char   *resptime;

	if (haveconnect)
	{
		newest = 0;

		vboxd_put_message("count %s", box);

		if ((msg = vboxd_get_message()))
		{
			if (!vboxd_test_response(VBOXD_VAL_SERVERQUIT))
			{
				if (vboxd_test_response(VBOXD_VAL_COUNT))
				{
					xstrncpy(response, msg, 128);

					respcode = strtok(response, "\t ");
					respmsgs = strtok(NULL    , "\t ");
					resptime = strtok(NULL    , "\t ");

					if ((respcode) && (respmsgs) && (resptime))
					{
						return(xstrtol(resptime, 0));
					}
					else log(LOG_ERR, gettext("bad server response \"%s\"."), msg);
				}
				else log(LOG_ERR, gettext("bad server response \"%s\"."), msg);
			}
			else log(LOG_ERR, gettext("server connection lost!"));
		}
		else log(LOG_ERR, gettext("can't get server response."));

		downconnect = TRUE;
	}

	return(0);
}

/**************************************************************************/
/** create_pid_file(): Creates the program pid lock.                     **/
/**************************************************************************/

static void create_pid_file(void)
{
	FILE *pid;

	if (!(pid = fopen(pidname, "w")))
	{
		log(LOG_CRIT, gettext("could not create PID file \"%s\" (%s)."), pidname, strerror(errno));

		leave_program(0);
	}

	fprintf(pid, "%d\n", getpid());

	fclose(pid);
}

/**************************************************************************/
/** remove_pid_file(): Remove the programs PID lock.                     **/
/**************************************************************************/

static void remove_pid_file(void)
{
	if (unlink(pidname) != 0)
	{
		log(LOG_CRIT, gettext("can't remove PID file \"%s\" (%s)."), pidname, strerror(errno));
	}
}

/**************************************************************************/
/** get_pid_from_file(): Returns the PID from the PID file.              **/
/**************************************************************************/

static int get_pid_from_file(void)
{
	char	temp[32];
	FILE *pid;

	if ((pid = fopen(pidname, "r")))
	{
		fgets(temp, 32, pid);
		fclose(pid);

		return(atoi(temp));
	}

	return(0);
}

/**************************************************************************/
/** parse_sound_times(): Parse the string for time zones.                **/
/**************************************************************************/

static void parse_sound_times(char *string, int min, int max)
{
	char *number;
	char *beg;
	char *end;
	int	begnr;
	int	endnr;
	int	i;

	if ((!string) || (strcmp(string, "!") == 0) || (strcmp(string, "-") == 0))
	{
		for (i = min; i <= max; i++)
		{
			sound_available_hours[i] = FALSE;
		}
		
		return;
	}

	if (strcmp(string, "*") == 0)
	{
		free(string);

		for (i = min; i <= max; i++)
		{
			sound_available_hours[i] = TRUE;
		}

		return;
	}
	
	number = strtok(string, ",;:");
	
	while (number)
	{
		beg = number;

		if ((end = index(number, '-')))
		{
			*end++ = '\0';
		}
		else end = beg;
		
		begnr = atoi(beg);
		endnr = atoi(end);

		if ((begnr >= min) && (begnr <= max) && (endnr >= min) && (endnr <= max))
		{
			if (endnr >= begnr)
			{
				for (i = begnr; i <= endnr; i++)
				{
					sound_available_hours[i] = TRUE;
				}
			}
			else log(LOG_ERR, gettext("starttime must be lower than endtime."));
		}
		else log(LOG_ERR, gettext("start- or endtime out of range."));

		number = strtok(NULL, ",;:");
	}

	free(string);
}

/**************************************************************************/
/** log(): Writes a message to the syslog.                               **/
/**************************************************************************/

static void log(int level, char *fmt, ...)
{
	va_list arg;

	openlog(vbasename, LOG_CONS|LOG_PID, LOG_USER);

   va_start(arg, fmt);
   vsyslog(level, fmt, arg);
   va_end(Arg);
	
	closelog();
}
