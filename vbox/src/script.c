/*
** $Id: script.c,v 1.3 1997/02/26 13:10:49 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "runtime.h"
#include "script.h"
#include "log.h"
#include "voice.h"
#include "libvbox.h"

/** Prototypes ***********************************************************/

int vbox_breaklist(ClientData, Tcl_Interp *, int, char *[]);
int vbox_init_touchtones(ClientData, Tcl_Interp *, int, char *[]);
int vbox_put_message(ClientData, Tcl_Interp *, int, char *[]);
int vbox_get_message(ClientData, Tcl_Interp *, int, char *[]);
int vbox_wait(ClientData, Tcl_Interp *, int, char *[]);
int vbox_pause(ClientData, Tcl_Interp *, int, char *[]);

/** Defines **************************************************************/

#define TCLCREATECMD(N, P, X, Y) { needcreated++; if (Tcl_CreateCommand(interpreter, N, P, X, Y)) havecreated++; }
#define TCLCREATEVAR(C, A)			{ needcreated++; if (Tcl_VarEval(interpreter, "set ", C, " \"", A, "\"", NULL) == TCL_OK) havecreated++; }

/** Variables ************************************************************/

struct minlist breaklist;

/*************************************************************************
 ** script_run():	Starts a external tcl script.									**
 *************************************************************************/

int script_run(char *script)
{
	Tcl_Interp	*interpreter;
	struct tm	*timelocal;

	char		savename[VOICE_MAX_CALLERID + 32];
	char		savetime[32];
	int		needcreated;
	int		havecreated;
	int		result;
	time_t	timestamp;

	log(L_INFO, "Running tcl script \"%s\"...\n", script);

	list_init(&breaklist);

	result = FALSE;

	if ((interpreter = Tcl_CreateInterp()))
	{
		if (Tcl_Init(interpreter) == TCL_OK)
		{
			needcreated = 0;
			havecreated = 0;

			TCLCREATECMD("vbox_breaklist"			, vbox_breaklist			, NULL, NULL);
			TCLCREATECMD("vbox_put_message"		, vbox_put_message		, NULL, NULL);
			TCLCREATECMD("vbox_play_message"		, vbox_put_message		, NULL, NULL);
			TCLCREATECMD("vbox_get_message"		, vbox_get_message		, NULL, NULL);
			TCLCREATECMD("vbox_record_message"	, vbox_get_message		, NULL, NULL);
			TCLCREATECMD("vbox_init_touchtones"	, vbox_init_touchtones	, NULL, NULL);
			TCLCREATECMD("vbox_wait"				, vbox_wait					, NULL, NULL);
			TCLCREATECMD("vbox_pause"				, vbox_pause				, NULL, NULL);

         if (havecreated == needcreated)
         {
				needcreated = 0;
				havecreated = 0;

				timestamp = time(NULL);
				timelocal = localtime(&timestamp);

				if (strftime(savename, 28, "%y%m%d%H%M%S-", timelocal) == 13)
				{
					if (strcmp(setup.voice.callerid, "*** Unknown ***") != 0)
					{
						strcat(savename, setup.voice.callerid);
					}
					else strcat(savename, "0");

					sprintf(savetime, "%d", setup.voice.recordtime);

					TCLCREATEVAR("vbox_var_bindir"	, BINDIR);
					TCLCREATEVAR("vbox_var_savename"	, savename);
					TCLCREATEVAR("vbox_var_rectime"	, savetime);
					TCLCREATEVAR("vbox_var_spooldir"	, setup.spool);
					TCLCREATEVAR("vbox_var_checknew"	, setup.voice.checknewpath);

					TCLCREATEVAR("vbox_msg_standard"	, setup.voice.standardmsg);
					TCLCREATEVAR("vbox_msg_beep"		, setup.voice.beepmsg);
					TCLCREATEVAR("vbox_msg_timeout"	, setup.voice.timeoutmsg);

					TCLCREATEVAR("vbox_caller_id"		, setup.voice.callerid);
					TCLCREATEVAR("vbox_caller_phone"	, setup.voice.phone);
					TCLCREATEVAR("vbox_caller_name"	, setup.voice.name);

					TCLCREATEVAR("vbox_user_name"		, setup.users.name);
					TCLCREATEVAR("vbox_user_home"		, setup.users.home);

					TCLCREATEVAR("vbox_flag_standard", setup.voice.domessage ? "TRUE" : "FALSE");
					TCLCREATEVAR("vbox_flag_beep"    , setup.voice.dobeep    ? "TRUE" : "FALSE");
					TCLCREATEVAR("vbox_flag_timeout" , setup.voice.dotimeout ? "TRUE" : "FALSE");
					TCLCREATEVAR("vbox_flag_record"	, setup.voice.dorecord  ? "TRUE" : "FALSE");

					if (havecreated == needcreated)
					{
						*touchtones = '\0';

						if (Tcl_EvalFile(interpreter, script) != TCL_OK)
						{
							log(L_ERROR, "In \"%s\": %s (line %d).\n", script, interpreter->result, interpreter->errorLine);
						}
						else
						{
							log(L_DEBUG, "Back from tcl script...\n");

							result = TRUE;
						}
					}
					else log(L_ERROR, "In \"%s\": %s (line %d).\n", script, interpreter->result, interpreter->errorLine);
				}
				else log(L_FATAL, "Can't create record name (timestamp).\n");
			}
			else log(L_FATAL, "Can't create all new tcl commands.\n");

			if (Tcl_InterpDeleted(interpreter) == 0)
			{
				Tcl_DeleteInterp(interpreter);
			}
		}
		else log(L_FATAL, "Can't initialize tcl interpreter.\n");
	}
	else log(L_FATAL, "Can't create tcl interpreter.\n");

	list_exit(&breaklist);

	return(result);
}

/*************************************************************************
 ** script_check_interpreter():	Checks tcl interpreter versions.			**
 *************************************************************************/

int script_check_interpreter(void)
{
	log(L_INFO, "Tcl interpreter version %s...\n", TCL_VERSION);

	returnok();
}

/*************************************************************************
 ** vbox_breaklist():	DTMF sequence support.									**
 *************************************************************************/

int vbox_breaklist(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	struct minnode *node;

	if (argc == 3)
	{
		if ((strcasecmp(argv[1], "ADD") == 0) || (strcasecmp(argv[1], "REM") == 0))
		{
			if (strcasecmp(argv[1], "ADD") == 0)
			{
				if ((node = list_make_node(argv[2])))
				{
					list_add_node(&breaklist, node);
					
					return(TCL_OK);
				}
			}

			if (strcasecmp(argv[1], "REM") == 0)
			{
				if (strcasecmp(argv[2], "ALL") != 0)
				{
					list_rem_node(&breaklist, list_find_node(&breaklist, argv[2]));
				}
				else list_exit(&breaklist);

				return(TCL_OK);
			}
		}
		else log(L_ERROR, "[vbox_breaklist] unsupported argument \"%s\"", argv[1]);
	}
	else
	{
		if (argc == 2)
		{
			if (strcasecmp(argv[1], "LIST") == 0)
			{
				node = breaklist.lh_beg;

				while (node)
				{
					log(L_INFO, "[vbox_breaklist] %s\n", node->ln_name);
			
					node = node->ln_next;
				}

				return(TCL_OK);
			}
			else log(L_ERROR, "[vbox_breaklist] unsupported argument \"%s\"", argv[1]);
		}
		else log(L_ERROR, "[vbox_breaklist] usage: vbox_breaklist <rem|add> <sequence>");
	}

	return(TCL_OK);
}

/*************************************************************************
 ** vbox_put_message():	Plays a voice message.									**
 *************************************************************************/

int vbox_put_message(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int result;

	if (argc != 2)
	{
		log(L_ERROR, "[vbox_put_message] usage: vbox_put_message <messagename>\n");

		printstring(ip->result, "ERROR");
	}
	else
	{
		result = voice_put_message(argv[1]);

		switch (result)
		{
			case VOICE_ACTION_ERROR:
				printstring(ip->result, "ERROR");
				break;

			case VOICE_ACTION_LOCALHANGUP:
			case VOICE_ACTION_REMOTEHANGUP:
				printstring(ip->result, "HANGUP");
				break;

			case VOICE_ACTION_TOUCHTONES:
				printstring(ip->result, touchtones);
				touchtones[0] = '\0';
				break;

			default:
				printstring(ip->result, "OK");
				break;
		}

		log(L_DEBUG, "[vbox_put_message] result \"%s\".\n", ip->result);
	}

	return(TCL_OK);
}

/*************************************************************************
 ** vbox_get_message():	Records a voice message.								**
 *************************************************************************/

int vbox_get_message(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int result;

	if (argc != 3)
	{
		log(L_ERROR, "[vbox_get_message] vbox_get_message <messagename> <recordtime>\n");

		printstring(ip->result, "ERROR");
	}
	else
	{
		result = voice_get_message(argv[1], argv[2], TRUE);

		switch (result)
		{
			case VOICE_ACTION_ERROR:
				printstring(ip->result, "ERROR");
				break;

			case VOICE_ACTION_TIMEOUT:
				printstring(ip->result, "TIMEOUT");
				break;

			case VOICE_ACTION_LOCALHANGUP:
			case VOICE_ACTION_REMOTEHANGUP:
				printstring(ip->result, "HANGUP");
				break;

			case VOICE_ACTION_TOUCHTONES:
				printstring(ip->result, touchtones);
				touchtones[0] = '\0';
				break;

			default:
				printstring(ip->result, "OK");
				break;
		}

		log(L_DEBUG, "[vbox_get_message] result \"%s\".\n", ip->result);
	}

	return(TCL_OK);
}

/*************************************************************************
 ** vbox_wait():	Waits for DTMF input.											**
 *************************************************************************/

int vbox_wait(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int result;

	if (argc != 2)
	{
		log(L_ERROR, "[vbox_wait] usage: vbox_wait <seconds>\n");

		printstring(ip->result, "ERROR");
	}
	else
	{
		result = voice_get_message("", argv[1], FALSE);

		switch (result)
		{
			case VOICE_ACTION_ERROR:
				printstring(ip->result, "ERROR");
				break;

			case VOICE_ACTION_TIMEOUT:
				printstring(ip->result, "TIMEOUT");
				break;

			case VOICE_ACTION_LOCALHANGUP:
			case VOICE_ACTION_REMOTEHANGUP:
				printstring(ip->result, "HANGUP");
				break;

			case VOICE_ACTION_TOUCHTONES:
				printstring(ip->result, touchtones);
				touchtones[0] = '\0';
				break;

			default:
				printstring(ip->result, "OK");
				break;
		}

		log(L_DEBUG, "[vbox_wait] result \"%s\".\n", ip->result);
	}

	return(TCL_OK);
}

/*************************************************************************
 ** vbox_pause():	Sleeps some milliseconds.										**
 *************************************************************************/

int vbox_pause(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	long p;

	if (argc != 2)
	{
		log(L_ERROR, "[vbox_pause] usage: vbox_pause <ms>\n");

		printstring(ip->result, "ERROR");
	}
	else
	{
		p = xstrtol(argv[1], 800);

		log(L_JUNK, "[vbox_pause] waiting %lu ms...\n", p);

		xpause(p);

		printstring(ip->result, "OK");
	}

	return(TCL_OK);
}

/*************************************************************************
 ** vbox_init_touchtones():	Initialize touchtone sequence.				**
 *************************************************************************/

int vbox_init_touchtones(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	*touchtones = '\0';

	printstring(ip->result, "OK");

	return(TCL_OK);
}
