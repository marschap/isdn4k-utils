/*
** $Id: vboxgetty.c,v 1.2 1998/06/17 12:20:37 michael Exp $
**
** Copyright 1997-1998 by Michael Herold <michael@abadonna.mayn.de>
**
** $Log: vboxgetty.c,v $
** Revision 1.2  1998/06/17 12:20:37  michael
** - Changes for automake/autoconf added.
**
** Revision 1.1  1998/06/10 13:31:58  michael
** Source added.
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>

#include "config.h"
#include "log.h"
#include "tcl.h"
#include "modem.h"
#include "rc.h"
#include "voice.h"
#include "stringutils.h"
#include "tclscript.h"
#include "vboxgetty.h"

/** Variables ************************************************************/

static char *progbasename;

char temppathname[PATH_MAX + 1];

/** Structures ***********************************************************/

static struct vboxrc rc_getty_c[] =
{
	{ "init"				, NULL },
	{ "initnumber"		, NULL },
	{ "badinitsexit"	, NULL },
	{ "initpause"		, NULL },
	{ "commandtimeout", NULL },
	{ "echotimeout"	, NULL },
	{ "ringtimeout"	, NULL },
	{ "alivetimeout"	, NULL },
	{ NULL				, NULL }
};

static struct option arguments[] =
{
	{ "version" , no_argument      , NULL, 'v' },
	{ "help"    , no_argument      , NULL, 'h' },
	{ "debug"   , required_argument, NULL, 'x' },
	{ "device"  , required_argument, NULL, 'd' },
	{ NULL      , 0                , NULL, 0   }
};

struct vboxmodem vboxmodem;

/** Prototypes ***********************************************************/

static int	parse_getty_rc(unsigned char *);
static void show_usage(int, int);
static int	process_incoming_call(void);
static int  run_modem_init(void);
static int	parse_user_rc(struct vboxincomingcall *);


/*************************************************************************/
/** The magic main...																	**/
/*************************************************************************/

void main(int argc, char **argv)
{
	char *isdnttyname;
	char *stop;
	int	opts;
	char *debugstr;
	int	debuglvl;
	int	i;
	int	modemstate;
	int	modeminits;

	progbasename = argv[0];

	if ((stop = rindex(argv[0], '/'))) progbasename = ++stop;

		/* Parse command line arguments and set the selected (or default)	*/
		/* debuglevel.																		*/

	debugstr		= NULL;
	isdnttyname	= NULL;

	while ((opts = getopt_long(argc, argv, "vhx:d:", arguments, (int *)0)) != EOF)
	{
		switch (opts)
		{
			case 'x':
				debugstr = optarg;
				break;

			case 'd':
				isdnttyname = optarg;
				break;

			case 'v':
				show_usage(200, 0);
				break;

			case 'h':
			default:
				show_usage(200, 1);
				break;
		}
	}

	if (debugstr)
	{
		if (strcasecmp(debugstr, "FULL") != 0)
		{
			debuglvl = LOG_E;

			for (i = 0; i < strlen(debugstr); i++)
			{
				switch (debugstr[i])
				{
					case 'W':
					case 'w':
						debuglvl |= LOG_W;
						break;
					
					case 'I':
						debuglvl |= LOG_I;
						break;
					
					case 'A':
						debuglvl |= LOG_A;
						break;
					
					case 'D':
						debuglvl |= LOG_D;
						break;
				}
			}
		}
		else debuglvl = LOG_X;

		log_set_debuglevel(debuglvl);
	}

		/* Remove all before the last '/' from the tty name. And check if	*/
		/* the device is accessable (not really needed since we need root	*/
		/* privilegs to start).															*/

	if (isdnttyname)
	{
		if ((stop = rindex(isdnttyname, '/'))) isdnttyname = ++stop;

		printstring(temppathname, "/dev/%s", isdnttyname);
		
		if (access(temppathname, F_OK|R_OK|W_OK) != 0)
		{
			fprintf(stderr, "\n%s: error: \"%s\" doesn't exist or is not accessable!\n\n", progbasename, temppathname);

			quit_program(100);
		}
	}
	else
	{
		fprintf(stderr, "\n%s: error: isdn tty name is required!\n", progbasename);

		show_usage(100, 1);
	}

		/* Check if we start with root privilegs. The permissions will be	*/
		/* dropped later, but we need root privilegs to open tty's, logs	*/
		/* etc.																				*/

	if (getuid() != 0)
	{
		fprintf(stderr, "\n%s: error: need root privilegs to start!\n\n", progbasename);

		quit_program(100);
	}

		/* Now its time to open the log. The name of the current tty will	*/
		/* be appended to the name.													*/

	printstring(temppathname, "%s/vboxgetty-%s.log", VBOX_LOGDIR, isdnttyname);

	log_open(temppathname);

		/* Start and initialize the tcl-interpreter (version 8.0 or	*/
		/* higher is required).													*/

	if (scr_create_interpreter() == -1)
	{
		log_line(LOG_E, "Can't create/initialize the tcl interpreter!\n");
		
		quit_program(100);
	}

	log_line(LOG_I, "Running vbox version %s (with tcl version %s).\n", VERSION, scr_tcl_version());

		/* Read the vboxgetty runtime configuration. 1st the global	*/
		/* and 2nd the tty.														*/

	if (parse_getty_rc(isdnttyname) == -1)
	{
		log_line(LOG_E, "Unable to read/parse configuration!\n");
	
		quit_program(100);
	}

		/* Open modem device and do the main loop (initialize, wait,	*/
		/* answer and alive check).												*/

	printstring(temppathname, "/dev/%s", isdnttyname);

	log_line(LOG_D, "Opening modem device \"%s\" (57600, CTS/RTS)...\n", temppathname);

	if (vboxmodem_open(&vboxmodem, temppathname) == -1)
	{
		log_line(LOG_E, "Can't open/setup modem device (%s).\n", vboxmodem_error());

		quit_program(100);
	}

signal(SIGINT , quit_program);
signal(SIGTERM, quit_program);
	
	modemstate = VBOXMODEM_STAT_INIT;
	modeminits = 0;
	
	while (modemstate != VBOXMODEM_STAT_EXIT)
	{
		switch (modemstate)
		{
			case VBOXMODEM_STAT_INIT:

				if (run_modem_init() == -1)
				{
					if ((i = (int)xstrtol(rc_get_entry(rc_getty_c, "badinitsexit"), 10)) > 0)
					{
						modeminits++;
						
						if (modeminits >= i)
						{
							modemstate = VBOXMODEM_STAT_EXIT;
							modeminits = 0;
							
							log_line(LOG_E, "Exit program while bad init limit are reached.\n");
						}
						else log_line(LOG_W, "Bad initialization - Program will exist on %d trys!\n", (i - modeminits));
					}
				}
				else
				{
					modemstate = VBOXMODEM_STAT_WAIT;
					modeminits = 0;
				}
				break;

			case VBOXMODEM_STAT_WAIT:
				
				modem_flush(&vboxmodem, 0);

				if (modem_wait(&vboxmodem) == 0)
				{
					modemstate = VBOXMODEM_STAT_RING;
					modeminits = 0;
				}
				else modemstate = VBOXMODEM_STAT_TEST;
				
				break;

			case VBOXMODEM_STAT_TEST:
			
				log_line(LOG_D, "Checking if modem is still alive...\n");
				
				if (modem_command(&vboxmodem, "AT", "OK") > 0)
				{
					modemstate = VBOXMODEM_STAT_WAIT;
					modeminits = 0;
				}
				else modemstate = VBOXMODEM_STAT_INIT;
				
				break;
				
			case VBOXMODEM_STAT_RING:
			
				modem_set_nocarrier(&vboxmodem, 0);
				process_incoming_call();
				modem_hangup(&vboxmodem);
				
				modemstate = VBOXMODEM_STAT_INIT;
				
				break;

			default:

				log_line(LOG_E, "Unknown modem status %d!\n", modemstate);
				
				modemstate = VBOXMODEM_STAT_INIT;
				
				break;
		}
	}

	quit_program(0);
}

/*************************************************************************/
/** quit_program():	Frees all used resources and exist.						**/
/*************************************************************************/
/** => rc				Exit return code (1-99 reserved for signals)			**/
/*************************************************************************/

void quit_program(int rc)
{
	modem_hangup(&vboxmodem);
	vboxmodem_close(&vboxmodem);

	scr_remove_interpreter();

	rc_free(rc_getty_c);

	log_close();

	exit(rc);
}

/*************************************************************************/
/** show_usage():	Shows usage/version message.									**/
/*************************************************************************/
/** => rc			Exit return level (1-99 reserved for signals)			**/
/** => help			1 shows help message, 0 version string						**/
/*************************************************************************/

static void show_usage(int rc, int help)
{
	if (help)
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "Usage: %s [OPTION] [OPTION] [...]\n", progbasename);
		fprintf(stdout, "\n");
		fprintf(stdout, "--device TTY   Name of the isdn tty to use (required).\n");
		fprintf(stdout, "--debug CODE   Sets debug level (default \"EWI\").\n");
		fprintf(stdout, "--version      Display version and exit.\n");
		fprintf(stdout, "--help         Display this help and exit.\n");
		fprintf(stdout, "\n");
	}
	else
	{
		fprintf(stdout, "%s version %s\n", progbasename, VERSION);
	}

	exit(rc);
}

/*************************************************************************/
/** run_modem_init():	Starts the tcl script to initialize the modem.	**/
/*************************************************************************/
/** <=						0 on success or -1 on error							**/
/*************************************************************************/

static int run_modem_init(void)
{
	struct vbox_tcl_variable vars[] = 
	{
		{ "vbxv_init"			, rc_get_entry(rc_getty_c, "init"		) },
		{ "vbxv_initnumber"	, rc_get_entry(rc_getty_c, "initnumber") },
		{ NULL					, NULL											  }
	};

	log_line(LOG_A, "Initializing modem...\n");

	if (scr_init_variables(vars) == 0)
	{
		if (scr_execute("initmodem.tcl", NULL) == 0) return(0);
	}

	log_line(LOG_E, "Can't initialize modem!\n");

	return(-1);
}











static int parse_getty_rc(unsigned char *tty)
{
	char *name;

	log_line(LOG_A, "Reading configuration...\n");

	name = "/usr/local/etc/vboxgetty.conf";

	if (rc_read(rc_getty_c, name, NULL) < 0)
	{
		if (errno != ENOENT)
		{
			log_line(LOG_E, "Can't open \"%s\" (%s)!\n", name, strerror(errno));
		
			return(-1);
		}
	}

	name = "/usr/local/etc/vboxgetty.conf.ttyI0";

	if (rc_read(rc_getty_c, name, NULL) < 0)
	{
		if (errno != ENOENT)
		{
			log_line(LOG_E, "Can't open \"%s\" (%s)!\n", name, strerror(errno));
		
			return(-1);
		}
	}

	log_line(LOG_D, "Filling unset configuration variables with defaults...\n");

	if (!rc_set_empty(rc_getty_c, "init"				, "ATZ&B512")) return(-1);
	if (!rc_set_empty(rc_getty_c, "badinitsexit"		, "10"		)) return(-1);
	if (!rc_set_empty(rc_getty_c, "initpause"			, "2500"		)) return(-1);
	if (!rc_set_empty(rc_getty_c, "commandtimeout"	, "4"			)) return(-1);
	if (!rc_set_empty(rc_getty_c, "echotimeout"		, "4"			)) return(-1);
	if (!rc_set_empty(rc_getty_c, "ringtimeout"		, "6"			)) return(-1);
	if (!rc_set_empty(rc_getty_c, "alivetimeout"		, "1800"		)) return(-1);

	if (!rc_get_entry(rc_getty_c, "initnumber"))
	{
		log_line(LOG_E, "Variable \"initnumber\" *must* be set!\n");
		
		return(-1);
	}

	return(0);
}


/*************************************************************************/
/** **/
/*************************************************************************/

static int process_incoming_call(void)
{
	struct vboxuser vboxuser;

	char	line[VBOXMODEM_BUFFER_SIZE + 1];
	int	haverings;
	int	waitrings;
	int	havesetup;

	haverings = 0;
	waitrings = 0;
	havesetup = 0;

	while (modem_read(&vboxmodem, line, (int)xstrtol(rc_get_entry(rc_getty_c, "ringtimeout"), 6)) == 0)
	{
		if ((strncmp(line, "CALLER NUMBER: ", 15) == 0) && (!havesetup))
		{
			xstrncpy(vboxuser.incomingid, &line[15], VBOX_CALL_ID);
			xstrncpy(vboxuser.localphone, "9317840513", VBOX_CALL_NUMBER);

			if (parse_user_rc(&vboxuser) == 0)
			{
				if ((vboxuser.uid == 0) || (vboxuser.gid == 0))
				{
					log_line(LOG_W, "No user for ID %s found - call will be ignored!\n", vboxcall.callerid);








				}

				havesetup = 1;
			}

			continue;
		}

		if (strcmp(line, "RING") == 0)
		{
			haverings++;
			
			if (havesetup)
				log_line(LOG_A, "RING #%03d (%s)...\n", haverings, incomingid);
			else
				log_line(LOG_A, "RING #%03d...\n", haverings);
		}
		else
		{
			log_line(LOG_D, "Got junk line \"");
			log_code(LOG_D, line);
			log_text(LOG_D, "\"...\n");
		}
	}

	return(-1);
}

/*************************************************************************/
/** **/
/*************************************************************************/

static int parse_user_rc(struct vboxuser *vboxuser)
{
	char  line[VBOX_RCLINE_SIZE + 1];
	FILE *rc;
	char *stop;
	int	linenr;

	log_line(LOG_D, "Searching local user for ID %s...\n", vboxuser->incomingid);

	vboxuser->uid		= 0;
	vboxuser->gid		= 0;
	vboxuser->home[0]	= 0
	vboxuser->umask	= -1;
	vboxuser->space	= -1;

	printstring(temppathname, "%s/vboxgetty.user", VBOX_ETCDIR);

	if ((rc = fopen(temppathname, "r")))
	{
		linenr = 0;

		while (fgets(line, VBOX_RCLINE_SIZE, rc))
		{
			linenr++;

			line[strlen(line) - 1] = '\0';
			
			if ((stop = rindex(line, '#'))) *stop = '\0';

			while (strlen(line) > 0)
			{
				if ((line[strlen(line) - 1] != ' ') && (line[strlen(line) - 1] != '\t')) break;
				
				line[strlen(line) - 1] = '\0';
			}
			
			if (*line == '\0') continue;

			pattern	= strtok(line, ":");
			name		= strtok(NULL, ":");
			group		= strtok(NULL, ":");
			mask		= strtok(NULL, ":");
			space		= strtok(NULL, ":");

			if ((!pattern) || (!name) || (!group) || (!mask) || (!space))
			{
				log_line(LOG_E, "Error in \"%s\" line %d.\n", temppathname, linenr);

				fclose(rc);
				return(-1);
			}








		}

		fclose(rc);
	}
	else
	{
		log_line(LOG_W, "Can't open \"%s\".\n", temppathname);
		
		return(-1);
	}

	return(0);
}














