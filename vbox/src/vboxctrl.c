/*
** $Id: vboxctrl.c,v 1.4 1997/03/18 12:36:55 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <pwd.h>

#include "config.h"
#include "libvbox.h"
#include "vboxctrl.h"

/** Structures ***********************************************************/

static struct option arguments[] =
{
	{ "version"		, no_argument			, NULL, 'v' },
	{ "help"			, no_argument			, NULL, 'h' },
	{ "create"		, no_argument			, NULL, 'c' },
	{ "remove"		, no_argument			, NULL, 'r' },
	{ "answernow"	, no_argument			, NULL, 'n' },
	{ "answerall"	, no_argument			, NULL, 'a' },
	{ "stop"			, no_argument			, NULL, 'p' },
	{ "reject"		, no_argument			, NULL, 'j' },
	{ "spooldir" 	, required_argument	, NULL, 's' },
	{ NULL			, 0						, NULL, 0   }
};

/** Variables ************************************************************/

static char	 spooldir[PATH_MAX + 1];
static char *vbasename;
static char *usespool;

/** Prototypes ***********************************************************/

static void usage(void);
static void version(void);

/*************************************************************************
 ** The magic main...																	**
 *************************************************************************/

int main(int argc, char **argv)
{
	struct passwd *passwd;

	char *todo;
	int	mode;
	int	opts;

	if (!(vbasename = rindex(argv[0], '/')))
		vbasename = argv[0];
	else
		vbasename++;

#ifdef HAVE_LOCALE_H
	setlocale(LC_MESSAGES, "");
#endif

#ifdef HAVE_LIBINTL_H
	textdomain(PACKAGE);
#endif

	if (!(passwd = getpwuid(getuid())))
	{
		fprintf(stderr, gettext("%s: can't get passwd entry for uid %d.\n"), vbasename, getuid());
		
		exit(5);
	}

	xstrncpy(spooldir, SPOOLDIR			, PATH_MAX);
	xstrncat(spooldir, "/"					, PATH_MAX);
	xstrncat(spooldir, passwd->pw_name	, PATH_MAX);

	if (!(usespool = getenv("VBOXSPOOL"))) usespool = spooldir;

	mode = CTRL_MODE_REMOVE;
	todo = "";

	while ((opts = getopt_long(argc, argv, "vhcrnapjs:", arguments, (int *)0)) != EOF)
	{
		switch (opts)
		{
			case 'a':
				todo = CTRL_NAME_ANSWERALL;
				break;
				
			case 'n':
				todo = CTRL_NAME_ANSWERNOW;
				break;
			
			case 'p':
				todo = CTRL_NAME_STOP;
				break;
				
			case 'j':
				todo = CTRL_NAME_REJECT;
				break;

			case 'c':
				mode = CTRL_MODE_CREATE;
				break;
				
			case 'r':
				mode = CTRL_MODE_REMOVE;
				break;

			case 's':
				usespool = optarg;
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

	if (!*todo)
	{
		fprintf(stderr, gettext("%s: you must specify a control name to create/remove.\n"), vbasename);
		
		exit(5);
	}

	if (mode == CTRL_MODE_CREATE)
	{
		if (ctrl_ishere(usespool, todo))
		{
			fprintf(stderr, gettext("%s: control file \"%s\" allready exists.\n"), vbasename, todo);
			
			exit(5);
		}

		if (!ctrl_create(usespool, todo))
		{
			fprintf(stderr, gettext("%s: can't create control file \"%s\".\n"), vbasename, todo);
			
			exit(5);
		}

		fprintf(stderr, gettext("%s: control file \"%s\" created.\n"), vbasename, todo);
		
		exit(0);
	}

	if (mode == CTRL_MODE_REMOVE)
	{
		if (!ctrl_ishere(usespool, todo))
		{
			fprintf(stderr, gettext("%s: control file \"%s\" doesn't exist.\n"), vbasename, todo);
			
			exit(5);
		}

		if (!ctrl_remove(usespool, todo))
		{
			fprintf(stderr, gettext("%s: can't remove control file \"%s\".\n"), vbasename, todo);
			
			exit(5);
		}

		fprintf(stderr, gettext("%s: control file \"%s\" removed.\n"), vbasename, todo);
		
		exit(0);
	}

	fprintf(stderr, gettext("%s: oops - don't know what I should do!\n"), vbasename);

	exit(5);
}

/*************************************************************************
 ** version():	Displays package version.											**
 *************************************************************************/

static void version(void)
{
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("%s version %s (%s)\n"), vbasename, VERSION, VERDATE);
	fprintf(stderr, gettext("\n"));
	
	exit(1);
}

/*************************************************************************
 ** usage():	Displays usage message.												**
 *************************************************************************/

static void usage(void)
{
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("Usage: %s OPTION [ OPTION ] [ ... ]\n"), vbasename);
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("-c, --create      Removes a control file (default).\n"));
	fprintf(stderr, gettext("-r, --remove      Creates a control file.\n"));
	fprintf(stderr, gettext("-n, --answernow   Control file \"vboxctrl-answernow\".\n"));
	fprintf(stderr, gettext("-a, --answerall   Control file \"vboxctrl-answerall\".\n"));
	fprintf(stderr, gettext("-p, --stop        Control file \"vboxctrl-stop\".\n"));
	fprintf(stderr, gettext("-j, --reject      Control file \"vboxctrl-reject\".\n"));
	fprintf(stderr, gettext("-s, --spooldir    Spooldirectory to create/remove control files.\n"));
	fprintf(stderr, gettext("-v, --version     Displays package version.\n"));
	fprintf(stderr, gettext("-h, --help        Displays this usage message.\n"));
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("The spool directory is taken from one of the following:\n"));
	fprintf(stderr, gettext("\n"));
	fprintf(stderr, gettext("   1) The default (buildin) value (%s).\n"), spooldir);
	fprintf(stderr, gettext("   2) The value of the environment variable VBOXSPOOL (if set).\n"));
	fprintf(stderr, gettext("   3) The directory of the command line option \"--spooldir\".\n"));
	fprintf(stderr, gettext("\n"));

	exit(1);
}
