/*
** $Id: log.c,v 1.6 1998/08/31 10:43:05 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: log.c,v $
** Revision 1.6  1998/08/31 10:43:05  michael
** - Changed "char" to "unsigned char".
**
** Revision 1.5  1998/07/29 11:02:37  michael
** - Added missing ATA to answer call (oh god, im so stupid).
** - Some junk removed.
**
** Revision 1.4  1998/07/07 17:14:52  michael
** - Since the ttyI interface will not work for me I have many time to write
**   the damn docu ;-)
**
** Revision 1.3  1998/07/06 09:05:25  michael
** - New control file code added. The controls are not longer only empty
**   files - they can contain additional informations.
** - Control "vboxctrl-answer" added.
** - Control "vboxctrl-suspend" added.
** - Locking mechanism added.
** - Configuration parsing added.
** - Some code cleanups.
**
** Revision 1.2  1998/06/17 17:01:20  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
*/

#ifdef HAVE_CONFIG_H
#	include "../config.h"
#endif

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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "voice.h"
#include "log.h"

/** Variables ************************************************************/

static FILE *logtxtio = NULL;
static int   loglevel = LOG_E|LOG_W|LOG_I;

/** Structures ***********************************************************/

static struct logsequence logsequence[] =
{
	{ ETX ,  "<ETX>" }, { NL  ,   "<NL>" }, { CR  ,   "<CR>" },
	{ DLE ,  "<DLE>" }, { XON ,  "<XON>" }, { XOFF, "<XOFF>" },
	{ DC4 ,  "<DC4>" }, { CAN ,  "<CAN>" }, { 0   ,     NULL }
};

/*************************************************************************/
/** log_open():	Öffnet die Logdatei im append mode.							**/
/*************************************************************************/
/** => name			Name der Logdatei.												**/
/**																							**/
/** <=				0 wenn die Datei geöffnet werden konnte oder -1 bei	**/
/**					einem Fehler.														**/
/*************************************************************************/

int log_open(unsigned char *name)
{
	if ((logtxtio = fopen(name, "a"))) return(0);

	return(-1);
}

/*************************************************************************/
/** log_close():	Schließt eine mit log_open() geöffnete Logdatei.		**/
/*************************************************************************/

void log_close(void)
{
	if (logtxtio) fclose(logtxtio);
	
	logtxtio = NULL;
}

/*************************************************************************/
/** log_set_debuglevel():	Setzt den Logdebuglevel.							**/
/*************************************************************************/
/** => level					Debuglevel der gesetzt werden soll.				**/
/*************************************************************************/

void log_set_debuglevel(int level)
{
	loglevel = level;
}

/*************************************************************************/
/** log_line():	Schreibt Text mit vorangestelltem Datum in den Log.	**/
/*************************************************************************/
/** => level		Debuglevel unter dem der Text ausgegeben werden soll.	**/
/** => fmt			Formatstring.														**/
/** => ...			Argumente für den Formatstring.								**/
/*************************************************************************/

void log_line(int level, unsigned char *fmt, ...)
{
	struct tm   *timel;
	time_t       timec;
	va_list      arg;
	char			 logsign;
	char         timeline[20];
	FILE			*useio;

	useio = (logtxtio ? logtxtio : stderr);
	
	if ((loglevel & level) || (level == LOG_E))
	{
		timec = time(NULL);
                        
		if ((timel = localtime(&timec)))
		{
			if (strftime(timeline, 20, "%d-%b %H:%M:%S", timel) != 15)
			{
				strcpy(timeline, "??-??? ??:??:??");
			}
		}

		switch (level)
		{
			case LOG_E:
				logsign = 'E';
				break;

			case LOG_W:
				logsign = 'W';
				break;

			case LOG_I:
				logsign = 'I';
				break;

			case LOG_D:
				logsign = 'D';
				break;

			case LOG_A:
				logsign = 'A';
				break;

			default:
				logsign = '?';
				break;
		}

		fprintf(useio, "%s <%c> ", timeline, logsign);

		va_start(arg, fmt);
		vfprintf(useio, fmt, arg);
		va_end(arg);

		fflush(useio);
	}
}

/*************************************************************************/
/** log_char():	Schreibt ein einzelnes Zeichen in den Log. Einige		**/
/**					nicht darstellbare Zeichen werden dabei ersetzt.		**/
/*************************************************************************/
/** => level		Debuglevel unter dem das Zeichen ausgegeben werden		**/
/**					soll.																	**/
/** => c				Zeichen das ausgegeben werden soll.							**/
/*************************************************************************/

void log_char(int level, unsigned char c)
{
	int i;

	if ((loglevel & level) || (level == LOG_E))
	{
		if (!isprint(c))
		{
			i = 0;
			
			while (logsequence[i].text)
			{
				if (logsequence[i].code == c)
				{
					log_text(level, "%s", logsequence[i].text);

					return;
				}

				i++;
			}

			log_text(level, "[0x%02X]", (unsigned char)c);
		}
		else log_text(level, "%c", c);
	}
}

/*************************************************************************/
/** log_text():	Schreibt Text in den Log. Die Funktion verhält sich	**/
/**					wie log_line(); es wird kein Datum vorangestellt.		**/
/*************************************************************************/
/** => level		Debuglevel unter dem der Text ausgegeben werden soll.	**/
/** => fmt			Formatstring.														**/
/** => ...			Argumente für den Formatstring.								**/
/*************************************************************************/

void log_text(int level, unsigned char *fmt, ...)
{
	FILE *useio;
	va_list arg;
	
	useio = (logtxtio ? logtxtio : stderr);

	if ((loglevel & level) || (level == LOG_E))
	{
		va_start(arg, fmt);
		vfprintf(useio, fmt, arg);
		va_end(arg);

		fflush(useio);
	}
}

/*************************************************************************/
/** log_code():	Schreibt einen String mittels log_char() in den Log.	**/
/*************************************************************************/
/** => level		Debuglevel unter dem der String ausgegeben werden		**/
/**					soll.																	**/
/** => sequence	String der ausgegeben werden soll.							**/
/*************************************************************************/

void log_code(int level, unsigned char *sequence)
{
	int i;

	if ((loglevel & level) || (level == LOG_E))
	{
		for (i = 0; i < strlen(sequence); i++)
		{
			log_char(level, sequence[i]);
		}
	}
}
