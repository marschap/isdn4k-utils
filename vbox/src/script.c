/*
** $Id: script.c,v 1.7 1997/04/28 16:52:01 michael Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>
#include <unistd.h>
#include <string.h>

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
int vbox_get_nr_new_messages(ClientData, Tcl_Interp *, int, char *[]);
int vbox_get_nr_all_messages(ClientData, Tcl_Interp *, int, char *[]);

/** Defines **************************************************************/

#define TCLCREATECMD(N, P, X, Y) { needcreated++; if (Tcl_CreateCommand(interpreter, N, P, X, Y)) havecreated++; }
#define TCLCREATEVAR(C, A)			{ needcreated++; if (Tcl_VarEval(interpreter, "set ", C, " \"", A, "\"", NULL) == TCL_OK) havecreated++; }

/*************************************************************************/
/** script_run():	Starts a external tcl script.									**/
/*************************************************************************/

int script_run(char *script)
{
	Tcl_Interp	*interpreter;

	char		savename[32];
	char		savetime[32];
	int		needcreated;
	int		havecreated;
	int		result;

	log(L_INFO, gettext("Running tcl script \"%s\"...\n"), script);

	breaklist_init();

	result = FALSE;

	if ((interpreter = Tcl_CreateInterp()))
	{
		if (Tcl_Init(interpreter) == TCL_OK)
		{
			needcreated = 0;
			havecreated = 0;

			TCLCREATECMD("vbox_breaklist"				, vbox_breaklist				, NULL, NULL);
			TCLCREATECMD("vbox_put_message"			, vbox_put_message			, NULL, NULL);
			TCLCREATECMD("vbox_play_message"			, vbox_put_message			, NULL, NULL);
			TCLCREATECMD("vbox_get_message"			, vbox_get_message			, NULL, NULL);
			TCLCREATECMD("vbox_record_message"		, vbox_get_message			, NULL, NULL);
			TCLCREATECMD("vbox_init_touchtones"		, vbox_init_touchtones		, NULL, NULL);
			TCLCREATECMD("vbox_wait"					, vbox_wait						, NULL, NULL);
			TCLCREATECMD("vbox_pause"					, vbox_pause					, NULL, NULL);
			TCLCREATECMD("vbox_get_nr_new_messages", vbox_get_nr_new_messages	, NULL, NULL);
			TCLCREATECMD("vbox_get_nr_all_messages", vbox_get_nr_all_messages	, NULL, NULL);

         if (havecreated == needcreated)
         {
				needcreated = 0;
				havecreated = 0;

				sprintf(savename, "%14.14lu-%8.8lu", (unsigned long)time(NULL), (unsigned long)getpid());
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
						log(L_ERROR, gettext("In \"%s\": %s (line %d).\n"), script, interpreter->result, interpreter->errorLine);
					}
					else
					{
						log(L_DEBUG, gettext("Back from tcl script...\n"));

						result = TRUE;
					}
				}
				else log(L_ERROR, gettext("In \"%s\": %s (line %d).\n"), script, interpreter->result, interpreter->errorLine);
			}
			else log(L_FATAL, gettext("Can't create all new tcl commands.\n"));

			if (Tcl_InterpDeleted(interpreter) == 0)
			{
				Tcl_DeleteInterp(interpreter);
			}
		}
		else log(L_FATAL, gettext("Can't initialize tcl interpreter.\n"));
	}
	else log(L_FATAL, gettext("Can't create tcl interpreter.\n"));

	breaklist_exit();

	return(result);
}

/*************************************************************************/
/** script_check_interpreter(): Checks tcl interpreter versions.			**/
/*************************************************************************/

int script_check_interpreter(void)
{
	log(L_INFO, gettext("Tcl interpreter version %s...\n"), TCL_VERSION);

	returnok();
}

/*************************************************************************/
/** vbox_breaklist(): DTMF sequence support.									   **/
/*************************************************************************/

int vbox_breaklist(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int   i;
	char *line;

	if (argc < 3)
	{
		log(L_ERROR, gettext("[vbox_breaklist] usage: vbox_breaklist <rem|add> <sequence> [sequence] [...]\n"));

		return(TCL_OK);
	}

	if (strcasecmp(argv[1], "add") == 0)
	{
		for (i = 2; i < argc; i++)
		{
			if (!breaklist_add(argv[i]))
			{
				log(L_ERROR, gettext("[vbox_breaklist] can't add \"%s\".\n"), argv[i]);
			}
		}

		return(TCL_OK);
	}

	if (strcasecmp(argv[1], "rem") == 0)
	{
		for (i = 2; i < argc; i++)
		{
			if (strcasecmp(argv[i], "all") == 0)
			{
				breaklist_exit();
			}
			else breaklist_rem(argv[i]);
		}

		return(TCL_OK);
	}

	if (strcasecmp(argv[1], "list") == 0)
	{
		for (i = 0; i < VBOX_MAX_BREAKLIST; i++)
		{
			if ((line = breaklist_nr(i)))
			{
				log(L_DEBUG, "[vbox_breaklist] %s\n", line);
			}
		}

		return(TCL_OK);
	}

	log(L_ERROR, gettext("[vbox_breaklist] usage: vbox_breaklist <rem|add> <sequence> [sequence] [...]\n"));

	return(TCL_OK);
}

/*************************************************************************/
/** vbox_put_message():	Plays a voice message.									**/
/*************************************************************************/

int vbox_put_message(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int result;

	if (argc != 2)
	{
		log(L_ERROR, gettext("[vbox_put_message] usage: vbox_put_message <messagename>\n"));

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

		log(L_DEBUG, gettext("[vbox_put_message] result \"%s\".\n"), ip->result);
	}

	return(TCL_OK);
}

/*************************************************************************/
/** vbox_get_message():	Records a voice message.								**/
/*************************************************************************/

int vbox_get_message(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int result;

	if (argc != 3)
	{
		log(L_ERROR, gettext("[vbox_get_message] usage: vbox_get_message <messagename> <recordtime>\n"));

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

		log(L_DEBUG, gettext("[vbox_get_message] result \"%s\".\n"), ip->result);
	}

	return(TCL_OK);
}

/*************************************************************************/
/** vbox_wait(): Waits for DTMF input.											   **/
/*************************************************************************/

int vbox_wait(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int result;

	if (argc != 2)
	{
		log(L_ERROR, gettext("[vbox_wait] usage: vbox_wait <seconds>\n"));

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

		log(L_DEBUG, gettext("[vbox_wait] result \"%s\".\n"), ip->result);
	}

	return(TCL_OK);
}

/*************************************************************************/
/** vbox_pause():	Sleeps some milliseconds.										**/
/*************************************************************************/

int vbox_pause(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	long p;

	if (argc != 2)
	{
		log(L_ERROR, gettext("[vbox_pause] usage: vbox_pause <ms>\n"));

		printstring(ip->result, "ERROR");
	}
	else
	{
		p = xstrtol(argv[1], 800);

		log(L_JUNK, gettext("[vbox_pause] waiting %lu ms...\n"), p);

		xpause(p);

		printstring(ip->result, "OK");
	}

	return(TCL_OK);
}

/*************************************************************************/
/** vbox_init_touchtones(): Initialize touchtone sequence.				   **/
/*************************************************************************/

int vbox_init_touchtones(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	*touchtones = '\0';

	printstring(ip->result, "OK");

	return(TCL_OK);
}

/*************************************************************************/
/** vbox_get_nr_new_messages(): Returns the number of new messages.     **/
/*************************************************************************/

int vbox_get_nr_new_messages(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int n;

	if (argc != 2)
	{
		log(L_ERROR, gettext("[vbox_get_nr_new_messages] usage: vbox_get_nr_new_messages <path>\n"));

		printstring(ip->result, "0");
	}
	else
	{
		log(L_JUNK, gettext("[vbox_get_nr_new_messages] counting new messages in \"%s\"...\n"), argv[1]);

		n = get_nr_messages(argv[1], TRUE);

		log(L_DEBUG, gettext("[vbox_get_nr_new_messages] result \"%d\".\n"), n);

		printstring(ip->result, "%d", n);
	}

	return(TCL_OK);
}

/*************************************************************************/
/** vbox_get_nr_all_messages(): Returns the number of messages.         **/
/*************************************************************************/

int vbox_get_nr_all_messages(ClientData cd, Tcl_Interp *ip, int argc, char *argv[])
{
	int n;

	if (argc != 2)
	{
		log(L_ERROR, gettext("[vbox_get_nr_all_messages] usage: vbox_get_nr_all_messages <path>\n"));

		printstring(ip->result, "0");
	}
	else
	{
		log(L_JUNK, gettext("[vbox_get_nr_all_messages] counting all messages in \"%s\"...\n"), argv[1]);

		n = get_nr_messages(argv[1], FALSE);

		log(L_DEBUG, gettext("[vbox_get_nr_all_messages] result \"%d\".\n"), n);

		printstring(ip->result, "%d", n);
	}

	return(TCL_OK);
}
