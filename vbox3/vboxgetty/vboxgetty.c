/*
** $Id: vboxgetty.c,v 1.8 1998/08/31 15:30:43 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: vboxgetty.c,v $
** Revision 1.8  1998/08/31 15:30:43  michael
** - Added touchtone support.
** - Added new tcl command "vbox_breaklist" to clear/set the touchtone
**   breaklist.
** - Removed the audio fragment size setting again. I don't know why this
**   crash my machine. The fragment size setting can be enabled in audio.h
**   with a define.
**
** Revision 1.7  1998/08/31 10:43:16  michael
** - Changed "char" to "unsigned char".
**
** Revision 1.6  1998/08/30 16:55:50  michael
** - Added initgroups() to the permission switch function. Since Kernel 2.1.x
**   (or 1.3.x?) setgid() don't reset the grouplist.
**
** Revision 1.5  1998/08/28 13:06:16  michael
** - Removed audio full duplex mode. Sorry, my soundcard doesn't support
**   this :-)
** - Added Fritz's /dev/audio setup. Pipe to /dev/audio now works great
**   (little echo but a clear sound :-)
** - Added better control support. The control now has the ttyname appended
**   (but there are some global controls without this) for controlling
**   more than one vboxgetty for a user.
** - Added support for "vboxcall" in the user spool directory. The file
**   stores information about the current answered call (needed by vbox,
**   vboxctrl or some other programs to create the right controls).
** - Added support for Karsten's suspend mode (support for giving a line
**   number is included also, but currently not used since hisax don't use
**   it).
**
** Revision 1.4  1998/07/06 09:05:35  michael
** - New control file code added. The controls are not longer only empty
**   files - they can contain additional informations.
** - Control "vboxctrl-answer" added.
** - Control "vboxctrl-suspend" added.
** - Locking mechanism added.
** - Configuration parsing added.
** - Some code cleanups.
**
** Revision 1.3  1998/06/18 12:38:18  michael
** - 2nd part of the automake/autoconf implementation (now compiles again).
**
** Revision 1.2  1998/06/17 17:01:24  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
*/

#ifdef HAVE_CONFIG_H
#  include "../config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <fnmatch.h>
#include <pwd.h>

#include "log.h"
#include "tcl.h"
#include "modem.h"
#include "rc.h"
#include "vboxrc.h"
#include "userrc.h"
#include "voice.h"
#include "stringutils.h"
#include "tclscript.h"
#include "vboxgetty.h"
#include "control.h"
#include "lock.h"
#include "breaklist.h"

/** Variables ************************************************************/

static unsigned char *progbasename;
static unsigned char *isdnttyname;

unsigned char temppathname[PATH_MAX + 1];
unsigned char savettydname[NAME_MAX + 1];

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
	{ "spooldir"		, NULL },
	{ "toggledtrtime"	, NULL },
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

static int	 vboxgettyrc_parse(unsigned char *);
static int	 process_incoming_call(void);
static int 	 run_modem_init(void);
static void	 pid_create(unsigned char *);
static void	 pid_remove(unsigned char *);
static void	 show_usage(int, int);

/************************************************************************* 
 ** The magic main...																	**
 *************************************************************************/

void main(int argc, char **argv)
{
	char *stop;
	int	opts;
	char *debugstr;
	int	debuglvl;
	int	i;
	int	modemstate;
	int	modeminits;

	breaklist_init();

	progbasename = argv[0];

	if ((stop = rindex(argv[0], '/'))) progbasename = ++stop;

		/* Die Argumente des Programms einlesen und den Debuglevel	*/
		/* setzen.																	*/

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

	umask(xstrtoo(VBOX_ROOT_UMASK, 0));

		/* Pfadangaben vom Devicenamen abschneiden und überprüfen ob	*/
		/* das Device vom Benutzer gelesen und beschrieben werden		*/
		/* kann (eigentlich nicht nötig, da nur unter Rootrechten ge-	*/
		/* startet werden kann.														*/

	if (isdnttyname)
	{
		if ((stop = rindex(isdnttyname, '/'))) isdnttyname = ++stop;

		printstring(savettydname, "%s"     , isdnttyname);
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

		/* Prüfen ob das Programm unter Rootrechten gestartet wurde. Die	*/
		/* Rechte werden später auf die des jeweiligen Benutzers geän-		*/
		/* dert, zum Start sind aber Rootrechte nötig.							*/

	if (getuid() != 0)
	{
		fprintf(stderr, "\n%s: error: need root privilegs to start!\n\n", progbasename);

		quit_program(100);
	}

		/* Jetzt wird der Log geöffnet. Der Name des aktuellen Devices	*/
		/* wird an das Ende angehängt.											*/

	printstring(temppathname, "%s/vboxgetty-%s.log", LOGDIR, isdnttyname);

	log_open(temppathname);

		/* Tcl-Interpreter starten. Für die momentanen Funktionen wird	*/
		/* Version 8 oder höher benötigt.										*/

	if (scr_create_interpreter() == -1)
	{
		log_line(LOG_E, "Can't create/initialize the tcl interpreter!\n");
		
		quit_program(100);
	}

	log_line(LOG_I, "Running vbox version %s (with tcl version %s).\n", VERSION, scr_tcl_version());

		/* Konfiguration des getty's abarbeiten. Zuerst wird die globale,	*/
		/* dann die des jeweiligen tty's eingelesen.								*/

	if (vboxgettyrc_parse(isdnttyname) == -1)
	{
		log_line(LOG_E, "Unable to read/parse configuration!\n");
	
		quit_program(100);
	}

		/* Modem Device öffnen und die interne Initialisierung	*/
		/* ausführen (nicht der normale Modeminit).					*/

	printstring(temppathname, "/dev/%s", isdnttyname);

	log_line(LOG_D, "Opening modem device \"%s\" (38400, CTS/RTS)...\n", temppathname);

	if (vboxmodem_open(&vboxmodem, temppathname) == -1)
	{
		log_line(LOG_E, "Can't open/setup modem device (%s).\n", vboxmodem_error());

		quit_program(100);
	}

		/* Lock- und PID-Datei für den getty und das entsprechende	*/
		/* Device erzeugen.														*/

	printstring(temppathname, "%s/LCK..%s", LOCKDIR, isdnttyname);
	
	if (lock_create(temppathname) == -1) quit_program(100);

	printstring(temppathname, "%s/vboxgetty-%s.pid", PIDDIR, isdnttyname);

	pid_create(temppathname);

		/* Signalhändler installieren. Alle möglichen Signale werden	*/
		/* auf quit_program() umgelenkt.											*/

	signal(SIGINT , quit_program);
	signal(SIGTERM, quit_program);
	signal(SIGHUP , quit_program);

		/* Hauptloop: Der Loop wird nur verlassen, wenn während der	*/
		/* Abarbeitung ein Fehler aufgetreten ist. Das Programm be-	*/
		/* endet sich danach!													*/
	
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

				if (set_process_permissions(0, 0, xstrtoo(VBOX_ROOT_UMASK, 0)) != 0)
					modemstate = VBOXMODEM_STAT_EXIT;
				else
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


/*************************************************************************
 ** quit_program():	Gibt alle belegten Resourcen frei und beendet das	**
 **						Programm.														**
 *************************************************************************
 ** => rc				Rückgabewert des Programms (1-99 ist reserviert).	**
 *************************************************************************/

void quit_program(int rc)
{
	set_process_permissions(0, 0, xstrtoo(VBOX_ROOT_UMASK, 0));

	modem_hangup(&vboxmodem);

	log_line(LOG_D, "Closing modem device (%d)...\n", vboxmodem.fd);

	if (vboxmodem_close(&vboxmodem) != 0)
	{
		log_line(LOG_E, "%s (%s)\n", vboxmodem_error(), strerror(errno));
	}

	if (isdnttyname)
	{
		printstring(temppathname, "%s/LCK..%s", LOCKDIR, isdnttyname);

		lock_remove(temppathname);

		printstring(temppathname, "%s/vboxgetty-%s.pid", PIDDIR, isdnttyname);

		pid_remove(temppathname);
	}

	scr_remove_interpreter();
	rc_free(rc_getty_c);
	breaklist_clear();
	log_close();

	exit(rc);
}

/*************************************************************************
 ** show_usage():	Zeigt Benutzermeldung/Version an und beendet dann das	**
 **					Programm.															**
 *************************************************************************
 ** => rc			Rückgabewert des Programms (1-99 ist reserviert).		**
 ** => help			1 wenn die Benutzermeldung oder 0 wenn die Version 	**
 **					angezeigt werden soll.											**
 *************************************************************************/

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
		fprintf(stdout, "Debugging codes:\n");
		fprintf(stdout, "\n");
		fprintf(stdout, "E    - Error messages\n");
		fprintf(stdout, "W    - Warnings\n");
		fprintf(stdout, "I    - Informations\n");
		fprintf(stdout, "A    - Action messages (main routines)\n");
		fprintf(stdout, "D    - Debugging messages (long output)\n");
		fprintf(stdout, "FULL - Full debugging\n");
		fprintf(stdout, "\n");
	}
	else fprintf(stdout, "%s version %s\n", progbasename, VERSION);

	exit(rc);
}

/*************************************************************************
 ** run_modem_init():	Startet das Tcl-Skript zum initislisieren des	**
 **							Modems.														**
 *************************************************************************
 ** <=						0 wenn die Initialisierung geklappt hat, -1 bei	**
 **							einem Fehler.												**
 *************************************************************************/

static int run_modem_init(void)
{
	struct vbox_tcl_variable vars[] = 
	{
		{ "vbxv_init"			, rc_get_entry(rc_getty_c, "init"		) },
		{ "vbxv_initnumber"	, rc_get_entry(rc_getty_c, "initnumber") },
		{ NULL					, NULL											  }
	};

	log_line(LOG_D, "Initializing modem...\n");

	if (scr_init_variables(vars) == 0)
	{
		if (scr_execute("initmodem.tcl", NULL) == 0) return(0);
	}

	log_line(LOG_E, "Can't initialize modem device!\n");

	return(-1);
}

/*************************************************************************
 ** vboxgettyrc_parse():	Liest die Konfiguration des gettys ein. Zu-	**
 **								erst wird die globale und dann die des je-	**
 **								weiligen tty's eingelesen.							**
 *************************************************************************
 ** => tty						Name des benutzten tty's.							**
 **																							**
 ** <=							0 wenn alles eingelesen werden konnte oder	**
 **								-1 bei einem Fehler.									**
 *************************************************************************/

static int vboxgettyrc_parse(unsigned char *tty)
{
	printstring(temppathname, "%s/vboxgetty.conf", SYSCONFDIR);

	if (rc_read(rc_getty_c, temppathname, NULL) < 0)
	{
		if (errno != ENOENT)
		{
			log_line(LOG_E, "Can't open \"%s\" (%s)!\n", temppathname, strerror(errno));
		
			return(-1);
		}
	}

	printstring(temppathname, "%s/vboxgetty.conf.%s", SYSCONFDIR, tty);

	if (rc_read(rc_getty_c, temppathname, NULL) < 0)
	{
		if (errno != ENOENT)
		{
			log_line(LOG_E, "Can't open \"%s\" (%s)!\n", temppathname, strerror(errno));
		
			return(-1);
		}
	}

	log_line(LOG_D, "Filling unset configuration variables with defaults...\n");

	if (!rc_set_empty(rc_getty_c, "init"				, "ATZ&B512"		 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "badinitsexit"		, "10"				 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "initpause"			, "2500"				 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "commandtimeout"	, "4"					 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "echotimeout"		, "4"					 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "ringtimeout"		, "6"					 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "alivetimeout"		, "1800"				 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "toggledtrtime"	, "400"				 )) return(-1);
	if (!rc_set_empty(rc_getty_c, "spooldir"			, "/var/spool/vbox")) return(-1);

	modemsetup.echotimeout		= xstrtol(rc_get_entry(rc_getty_c, "echotimeout"	), 4		);
	modemsetup.commandtimeout	= xstrtol(rc_get_entry(rc_getty_c, "commandtimeout"), 4		);
	modemsetup.ringtimeout		= xstrtol(rc_get_entry(rc_getty_c, "ringtimeout"	), 6		);
	modemsetup.alivetimeout		= xstrtol(rc_get_entry(rc_getty_c, "alivetimeout"	), 1800	);
	modemsetup.toggle_dtr_time	= xstrtol(rc_get_entry(rc_getty_c, "toggledtrtime"	), 400	);

	if (!rc_get_entry(rc_getty_c, "initnumber"))
	{
		log_line(LOG_E, "Variable \"initnumber\" *must* be set!\n");
		
		return(-1);
	}

	return(0);
}

/*************************************************************************
 ** process_incoming_call():	Bearbeitet einen eingehenden Anruf.			**
 *************************************************************************/

static int process_incoming_call(void)
{
	struct vboxuser	 vboxuser;
	struct vboxcall	 vboxcall;
	unsigned char		 line[VBOXMODEM_BUFFER_SIZE + 1];
	int					 haverings;
	int					 waitrings;
	int					 usersetup;
	int					 ringsetup;
	int					 inputisok;
	unsigned char		*stop;
	unsigned char		*todo;

	memset(&vboxuser, 0, sizeof(vboxuser));
	memset(&vboxcall, 0, sizeof(vboxcall));

	haverings =  0;
	waitrings = -1;
	usersetup =  0;
	ringsetup =  0;

	while (modem_read(&vboxmodem, line, modemsetup.ringtimeout) == 0)
	{
		inputisok = 0;

			/* Wenn der Benutzer der angerufenen Nummer ermittelt ist und	*/
			/* dessen Konfigurations abgearbeitet wurde, wird überprüft ob	*/
			/* der Anruf angenommen werden soll.									*/

		if ((usersetup) && (ringsetup))
		{
			if (waitrings >= 0)
			{
				todo = savettydname;
				stop = ctrl_exists(vboxuser.home, "answer", todo);

				if (!stop)
				{
					todo = NULL;
					stop = ctrl_exists(vboxuser.home, "answer", todo);
				}

				if (stop)
				{
					log_line(LOG_D, "Control \"vboxctrl-answer\" detected: %s (%s)...\n", stop, ((char *)todo ? (char *)todo : "global"));

					if ((strcasecmp(stop, "no") == 0) || (strcasecmp(stop, "hangup") == 0) || (strcasecmp(stop, "reject") == 0))
					{
						log_line(LOG_D, "Incoming call will be rejected...\n");
						
						return(0);
					}

					if (strcasecmp(stop, "now") != 0)
					{
						vboxuser.space	= 0;
						waitrings		= xstrtol(stop, waitrings);
					}
					else
					{
						vboxuser.space = 0;
						waitrings		= 1;
					}

					log_line(LOG_D, "Call will be answered after %d ring(s).\n", waitrings);
				}
			}

			if (waitrings > 0)
			{
				if (haverings >= waitrings)
				{
					return(voice_init(&vboxuser, &vboxcall));
				}
			}
		}

			/* Ring abarbeiten: Beim ersten Ring wird die angerufene	*/
			/* Nummer gesichert, die durch ATS13.7=1 mit einem Slash	*/
			/* an den Ringstring angehängt ist.								*/

		if (strncmp(line, "RING/", 5) == 0)
		{
			inputisok++;
			haverings++;
			
			if (!ringsetup)
			{
	         xstrncpy(vboxuser.localphone, &line[5], VBOXUSER_NUMBER);

				ringsetup = 1;
			}				          

			log_line(LOG_A, "%s #%03d (%s)...\n", line, haverings, ((char *)usersetup ? (char *)vboxcall.name : "not known"));
		}

			/* CallerID aus dem Modeminput kopieren. Wenn bereits die	*/
			/* angerufene Nummer ermittelt wurde, wird einmalig die		*/
			/* Konfigurationsdatei des Benutzers abgearbeitet.				*/

		if (strncmp(line, "CALLER NUMBER: ", 15) == 0)
		{
			inputisok++;

			if ((ringsetup) && (!usersetup))
			{
				xstrncpy(vboxuser.incomingid, &line[15], VBOXUSER_CALLID);

				if (userrc_parse(&vboxuser, rc_get_entry(rc_getty_c, "spooldir"), vboxuser.localphone) == 0)
				{
					if ((vboxuser.uid != 0) && (vboxuser.gid != 0))
					{
							/* Nachdem "vboxgetty.user" abgearbeitet ist und	*/
							/* ein Benutzer gefunden wurde, werden einige der	*/
							/* Kontrolldateien gelöscht.								*/

						ctrl_remove(vboxuser.home, "suspend", savettydname);
						ctrl_remove(vboxuser.home, "suspend", NULL        );

							/* Die "effective Permissions" des Prozesses auf	*/
							/* die des Benutzers setzen und dessen Konfigurat-	*/
							/* ionsdatei abarbeiten.									*/

						if (set_process_permissions(vboxuser.uid, vboxuser.gid, vboxuser.umask) == 0)
						{
							usersetup = 1;
							waitrings = vboxrc_parse(&vboxcall, vboxuser.home, vboxuser.incomingid);

							if (waitrings <= 0)
							{
								if (waitrings < 0)
									log_line(LOG_W, "Incoming call will be ignored!\n");
								else
									log_line(LOG_D, "Incoming call will be ignored (user setup)!\n");
							}
							else log_line(LOG_D, "Call will be answered after %d ring(s).\n", waitrings);
						}
						else return(-1);
					}
					else log_line(LOG_W, "Useing uid/gid 0 is not allowed - call will be ignored!\n", vboxuser.incomingid);
				}
				else log_line(LOG_W, "Number \"%s\" not bound to a local user - call will be ignored!\n", vboxuser.localphone);
			}
		}

		if (!inputisok)
		{
			log_line(LOG_D, "Got junk line \"");
			log_code(LOG_D, line);
			log_text(LOG_D, "\"...\n");

			continue;
		}
	}

	return(-1);
}
/*************************************************************************
 ** set_process_permissions():	Setzt die effektive uid/gid des Pro-	**
 **										zesses und die umask.						**
 *************************************************************************/

int set_process_permissions(uid_t uid, gid_t gid, int mask)
{
	struct passwd *pwd;
	int            groupsset;

	log_line(LOG_D, "Setting effective permissions to %d.%d [%04o]...\n", uid, gid, mask);

	errno     = 0;
	groupsset = 0;

		/* Eintrag des zu setzenden Benutzers aus der passwd lesen. Mit	*/
		/* initgroups() werden dann die realen Gruppen des Benutzers ein-	*/
		/* gestellt. Ab Kernel 2.1.x scheint das nicht mehr von setgid()	*/
		/* gemacht zu werden!															*/

	if ((pwd = getpwuid(uid)))
	{
		if (uid != 0)
		{
			if (initgroups(pwd->pw_name, gid) == 0) groupsset = 1;
		}

		if (setegid(gid) == 0)
		{
			if (seteuid(uid) == 0)
			{
				if (mask != 0) umask(mask);

				if ((uid == 0) || (!groupsset))
				{
					if (initgroups(pwd->pw_name, gid) == 0) groupsset = 1;
				}

				if (!groupsset)
				{
					log(LOG_E, "Can't run initgroups(\"%s\", %d) (%s).\n", pwd->pw_name, gid, strerror(errno));

					return(-1);
				}
			
				return(0);
			}
			else log_line(LOG_E, "Can't set effective uid to %d (%s).\n", uid, strerror(errno));
		}
		else log_line(LOG_E, "Can't set effective gid to %d (%s).\n", gid, strerror(errno));
	}
	else log(LOG_E, "Can't get uid %d passwd entry (%s).", strerror(errno));

	return(-1);
}

/*************************************************************************
 ** pid_create():	Erzeugt die PID Datei für den getty.						**
 *************************************************************************
 ** => name			Name der Datei.													**
 *************************************************************************/

static void pid_create(unsigned char *name)
{
	FILE *pptr;

	log_line(LOG_D, "Creating \"%s\"...\n", name);
	
	if ((pptr = fopen(name, "w")))
	{
		fprintf(pptr, "%ld\n", getpid());
		fclose(pptr);
	}
}

/*************************************************************************
 ** pid_remove():	Löscht die PID Datei des getty.								**
 *************************************************************************
 ** => name			Name der Datei.													**
 *************************************************************************/

static void pid_remove(unsigned char *name)
{
	log_line(LOG_D, "Removing \"%s\"...\n", name);

	remove(name);
}
