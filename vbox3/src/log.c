/*
** $Id: log.c,v 1.1 1998/06/10 13:31:52 michael Exp $
**
** Copyright 1997-1998 by Michael Herold <michael@abadonna.mayn.de>
**
** $Log: log.c,v $
** Revision 1.1  1998/06/10 13:31:52  michael
** Source added.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

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
/** log_open():	Opens the log.														**/
/*************************************************************************/
/** => name			Name of the ttyI (appended to real logname)				**/
/** <=				0 on success or -1 on error									**/
/*************************************************************************/

int log_open(char *name)
{
	if ((logtxtio = fopen(name, "a"))) return(0);

	return(-1);
}

/*************************************************************************/
/** log_close():	Close the log.														**/
/*************************************************************************/

void log_close(void)
{
	if (logtxtio) fclose(logtxtio);
	
	logtxtio = NULL;
}

/*************************************************************************/
/** **/
/*************************************************************************/

void log_set_debuglevel(int level)
{
	loglevel = level;
}

/*************************************************************************/
/** log_line():	Writes a line to the log including the current date &	**/
/**					time and the log level.											**/
/*************************************************************************/
/** => level		Debuglevel															**/
/** => fmt			Formatstring														**/
/** => ...			Args																	**/
/*************************************************************************/

void log_line(int level, char *fmt, ...)
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
/** log_char():	Writes a char to the log.										**/
/*************************************************************************/
/** => level		Debuglevel															**/
/** => c				Char to log															**/
/*************************************************************************/

void log_char(int level, char c)
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
/** log_text():	Writes a line to the log.										**/
/*************************************************************************/
/** => level		Debuglevel															**/
/** => fmt			Formatstring														**/
/** => ...			Args																	**/
/*************************************************************************/

void log_text(int level, char *fmt, ...)
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
/** log_code():	Writes a line with log_char() to the log.					**/
/*************************************************************************/
/** => level		Debuglevel															**/
/** => sequence	Sequence of chars to log										**/
/*************************************************************************/

void log_code(int level, char *sequence)
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
