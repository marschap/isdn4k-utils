/*
** $Id: vboxctrl.c,v 1.1 1997/02/26 13:10:54 michael Exp $
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

#include "libvbox.h"
#include "runtime.h"
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
static char *basename;
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

	if (!(basename = rindex(argv[0], '/')))
	{
		basename = argv[0];
	}
	else basename++;

	if (!(passwd = getpwuid(getuid())))
	{
		fprintf(stderr, "%s: can't get passwd entry for uid %d.\n", basename, getuid());
		
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
}

/*************************************************************************
 ** version():	Displays package version.											**
 *************************************************************************/

static void version(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "%s version %s (%s)\n", basename, VERSION, VERDATE);
	fprintf(stderr, "\n");
	
	exit(1);
}

/*************************************************************************
 ** usage():	Displays usage message.												**
 *************************************************************************/

static void usage(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s OPTION [ OPTION ] [ ... ]\n", basename);
	fprintf(stderr, "\n");
	fprintf(stderr, "-c, --create      Removes a control file (default).\n");
	fprintf(stderr, "-r, --remove      Creates a control file.\n");
	fprintf(stderr, "-n, --answernow   Control file \"vboxctrl-answernow\".\n");
	fprintf(stderr, "-a, --answerall   Control file \"vboxctrl-answerall\".\n");
	fprintf(stderr, "-p, --stop        Control file \"vboxctrl-stop\".\n");
	fprintf(stderr, "-j, --reject      Control file \"vboxctrl-reject\".\n");
	fprintf(stderr, "-s, --spooldir    Spooldirectory to create/remove control files.\n");
	fprintf(stderr, "-v, --version     Displays package version.\n");
	fprintf(stderr, "-h, --help        Displays this usage message.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "The spool directory is taken from one of the following:\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "   1) The default (buildin) value (%s).\n", spooldir);
	fprintf(stderr, "   2) The value of the environment variable VBOXSPOOL (if set).\n");
	fprintf(stderr, "   3) The directory of the command line option --spooldir.\n");
	fprintf(stderr, "\n");

	exit(1);
}




