/*
** $Id: vboxgetty.c,v 1.5 1997/03/18 12:36:55 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <sys/vfs.h>
#include <signal.h>

#include "init.h"
#include "vboxgetty.h"
#include "log.h"
#include "modem.h"
#include "rcvbox.h"
#include "script.h"
#include "libvbox.h"

/** Prototypes ***********************************************************/

static void version(void);
static void usage(void);
static void main_program(void);
static void answer_call(void);
static int	check_spool_space(unsigned long);

/** Variables ************************************************************/

char *vbasename;

static int modemstate;

/** Structures ***********************************************************/

static struct option arguments[] =
{
	{ "version" , no_argument      , NULL, 'v' },
	{ "help"    , no_argument      , NULL, 'h' },
	{ "file"    , required_argument, NULL, 'f' },
	{ "device"  , required_argument, NULL, 'd' },
	{ "language", required_argument, NULL, 'l' },
	{ NULL      , 0                , NULL, 0   }
};

/*************************************************************************/
/** The magic main...																	**/
/*************************************************************************/

void main(int argc, char **argv)
{
	char *usevrc   = NULL;
	char *device   = NULL;
	char *language = NULL;
	int	opts;

	if (!(vbasename = rindex(argv[0], '/')))
		vbasename = argv[0];
	else
		vbasename++;

	usevrc = GETTYRC;
	device = "";

	while ((opts = getopt_long(argc, argv, "vhf:d:", arguments, (int *)0)) != EOF)
	{
		switch (opts)
		{
			case 'f':
				usevrc = optarg;
				break;
				
			case 'd':
				device = optarg;
				break;

			case 'l':
				language = optarg;
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

	if (language) setenv("LANG", language, 1);

#if HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif

#if ENABLE_NLS
	textdomain(PACKAGE);
#endif

	if (getuid() != 0)
	{
		log(L_STDERR, gettext("%s: must be run by root!\n"), vbasename);

		exit(5);
	}

	if (access(device, W_OK|R_OK|F_OK) != 0)
	{
		log(L_STDERR, gettext("%s: device \"%s\" is not accessable.\n"), vbasename, device);

		exit(5);
	}

	if (access(usevrc, R_OK|F_OK) != 0)
	{
		log(L_STDERR, gettext("%s: Setup \"%s\" doesn't exist.\n"), vbasename, usevrc);

		exit(5);
	}

	if (!init_program(device, usevrc)) exit(5);

	signal(SIGHUP	, exit_program);
	signal(SIGINT	, exit_program);
	signal(SIGTERM	, exit_program);

	main_program();
	exit_program(SIGTERM);
}

/*************************************************************************/
/** version():	Displays the package version.										**/
/*************************************************************************/

static void version(void)
{
#if HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif

#if ENABLE_NLS
	textdomain(PACKAGE);
#endif

	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("%s version %s (%s)\n"), vbasename, VERSION, VERDATE);
	fprintf(stderr, gettext("\n"));
	
	exit(1);
}

/*************************************************************************/
/** usage(): Displays usage message.												**/
/*************************************************************************/

static void usage(void)
{
#if HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif

#if ENABLE_NLS
	textdomain(PACKAGE);
#endif

	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("Usage: %s OPTION [ OPTION ] [ ... ]\n"), vbasename);
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("-f, --file FILE    Overwrites \"%s\".\n"), GETTYRC);
	fprintf(stderr, gettext("-d, --device TTY   Use device TTY for modem operations [required].\n"));
	fprintf(stderr, gettext("-l, --language     Overwrites environment LANG.\n"));
	fprintf(stderr, gettext("-h, --help         Displays this short help.\n"));
	fprintf(stderr, gettext("-v, --version      Displays the package version.\n"));
	fprintf(stderr, gettext("\n"));
	
	exit(1);
}

/*************************************************************************/
/** main_program(): Mainloop.														   **/
/*************************************************************************/

static void main_program(void)
{
	int modeminits;

	modemstate = MODEM_STATE_INITIALIZE;
	modeminits = 0;

	while (modemstate != MODEM_STATE_EXIT)
	{
		switch (modemstate)
		{
			case MODEM_STATE_INITIALIZE:

				if (!modem_initialize())
				{
					if (setup.modem.badinitsexit > 0)
					{
						modeminits++;
						
						if (modeminits >= setup.modem.badinitsexit)
						{
							modemstate = MODEM_STATE_EXIT;
							modeminits = 0;

							log(L_FATAL, gettext("Exit program while bad init limit are reached.\n"));
						}
						else log(L_WARN, gettext("Bad initialization - Program will exist on %d trys!\n"), (setup.modem.badinitsexit - modeminits));
					}
				}
				else
				{
					modemstate = MODEM_STATE_WAITING;
					modeminits = 0;
				}

				break;

			case MODEM_STATE_WAITING:
			
				modem_flush(0);
				
				if (modem_wait())
				{
					modemstate = MODEM_STATE_RING;
					modeminits = 0;
				}
				else modemstate = MODEM_STATE_CHECK;
				
				break;

			case MODEM_STATE_CHECK:
			
				log(L_DEBUG, gettext("Checking if modem is still alive...\n"));

				if (!ctrl_ishere(setup.spool, CTRL_NAME_STOP))
				{
					if (modem_command("AT", "OK") >= 1)
					{
						modemstate = MODEM_STATE_WAITING;
						modeminits = 0;
					}
					else modemstate = MODEM_STATE_INITIALIZE;
				}
				else
				{
					log(L_INFO, gettext("Control file \"%s\" exists - program will quit...\n"), CTRL_NAME_STOP);

					modemstate = MODEM_STATE_EXIT;
				}
				
				break;

			case MODEM_STATE_RING:

				modemstate = MODEM_STATE_INITIALIZE;

				if (check_spool_space(setup.freespace))
				{
					modem_set_nocarrier_state(FALSE);

					if (modem_count_rings(vboxrc_get_rings_to_wait()))
					{
						if (!modem_get_nocarrier_state())
						{
							answer_call();
							modem_hangup();
						}
					}
				}

				break;
		}
	}
}

/*************************************************************************/
/** answer_call(): Answers the call and starts the tcl script.			   **/
/*************************************************************************/

static void answer_call(void)
{
	char run[PATH_MAX + 1];

	log(L_INFO, gettext("Answering call...\n"));

	if (modem_command("ATA", "VCON|CONNECT") <= 0)
	{
		log(L_ERROR, gettext("Can't answer call -- hanging up...\n"));
		
		return;
	}

	if (!index(setup.voice.tclscriptname, '/'))
	{
		xstrncpy(run, setup.spool, PATH_MAX);
		xstrncat(run, "/", PATH_MAX);
		xstrncat(run, setup.voice.tclscriptname, PATH_MAX);
	}
	else xstrncpy(run, setup.voice.tclscriptname, PATH_MAX);

	script_run(run);
}

/*************************************************************************/
/** check_spool_space(): Checks space on spoolpartition.                **/
/*************************************************************************/

static int check_spool_space(unsigned long need)
{
	struct statfs stat;
	unsigned long have;

	log(L_DEBUG, gettext("Checking free space on \"%s\"...\n"), setup.spool);

	if (need <= 0)
	{
		log(L_WARN, gettext("Free disc space check disabled!\n"));

		returnok();
	}

	if (statfs(setup.spool, &stat) == 0)
	{
		have = (stat.f_bfree * stat.f_bsize);

		log_line(L_JUNK, gettext("%ld bytes available; %ld bytes needed... "), have, need);

		if (have >= need)
		{
			log_text(L_JUNK, gettext("enough.\n"));

			returnok();
		}

		log_text(L_JUNK, gettext("not enough!\n"));
	}
	else log(L_ERROR, gettext("Can't get statistic about disc space!"));

	returnerror();
}

/*************************************************************************/
/** block_all_signals(): Blocks all signals.									   **/
/*************************************************************************/

void block_all_signals(void)
{
	int i;

	log(L_DEBUG, gettext("Blocking all signals (0-%d)...\n"), NSIG);
	
	for (i = 0; i < NSIG; i++) signal(i, SIG_IGN);
}
