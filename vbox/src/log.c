/*
** $Id: log.c,v 1.4 1997/02/27 15:43:47 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>
                     
#include "runtime.h"
#include "log.h"
#include "vboxgetty.h"
#include "libvbox.h"

/** Variables ************************************************************/

static FILE *logsio = NULL;
static long	 logdbg = L_DEFAULT;

/** Structures ***********************************************************/

static struct logsequence logsequence[] =
{
	{ ETX ,  "<ETX>" }, { NL  ,   "<NL>" }, { CR  ,   "<CR>" },
	{ DLE ,  "<DLE>" }, { XON ,  "<XON>" }, { XOFF, "<XOFF>" },
	{ DC4 ,  "<DC4>" }, { CAN ,  "<CAN>" }, { 0   ,     NULL }
};

/*************************************************************************
 ** log_init():	Initialize the log functions.									**
 *************************************************************************/

int log_init(void)
{
	char *device;

	if (!(device = rindex(setup.modem.device, '/')))
	{
		device = setup.modem.device;
	}
	else device++;

	setlocale(LC_CTYPE, "");

	if ((strlen(device) + strlen(LOGFILE)) < SETUP_MAX_LOGNAME)
	{
		sprintf(setup.logname, LOGFILE, device);

		if ((logsio = fopen(setup.logname, "a")))
		{
			log_debuglevel(L_DEFAULT);

			returnok();
		}
		else log(L_STDERR, "%s: Can't open log '%s'.\n", vbasename, setup.logname);
	}
	else log(L_STDERR, "%s: Logname too long. Please set 'SETUP_MAX_LOGNAME' in 'setup.h' to a higher value!\n", vbasename);

	returnerror();
}

/*************************************************************************
 ** log_exit():	Exits the log functions.										**
 *************************************************************************/

void log_exit(void)
{
	log(L_JUNK, "Closing log...\n");

	if (logsio) fclose(logsio);

	logsio = NULL;
}

/*************************************************************************
 ** log_debuglevel():	Sets the log debug level.								**
 *************************************************************************/

void log_debuglevel(long level)
{
	logdbg = level;
}

/*************************************************************************
 ** log_line():	Writes a line to the log including the current date &	**
 **					time and the log level.											**
 *************************************************************************/

void log_line(long level, char *fmt, ...)
{
	struct tm   *timel;
	time_t       timec;
	va_list      arg;
	char			 logsign;
	char         timeline[20];
	
	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR))
	{
		if (!logsio) level = L_STDERR;

		if (level == L_STDERR)
		{
			va_start(arg, fmt);
			vfprintf(stderr, fmt, arg);
			va_end(arg);
		}
		else
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
				case L_FATAL:	logsign = 'F';
									break;
				case L_ERROR:	logsign = 'E';
									break;
				case L_WARN:	logsign = 'W';
									break;
				case L_INFO:	logsign = 'I';
									break;
				case L_DEBUG:	logsign = 'D';
									break;
				case L_JUNK:	logsign = 'J';
									break;
				default:			logsign = '?';
									break;
			}

			fprintf(logsio, "%s <%c> ", timeline, logsign);

			va_start(arg, fmt);
			vfprintf(logsio, fmt, arg);
			va_end(arg);

			fflush(logsio);
		}
	}
}

/*************************************************************************
 ** log_char():	Writes a char to the log.										**
 *************************************************************************/

void log_char(long level, char c)
{
	int i;

	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR))
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

/*************************************************************************
 ** log_text():	Writes a line to the log.										**
 *************************************************************************/

void log_text(long level, char *fmt, ...)
{
	FILE *useio;
	va_list arg;

	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR))
	{
		if ((!logsio) || (level == L_STDERR))
		{
			useio = stderr;
			level = L_STDERR;
		}
		else useio = logsio;

		va_start(arg, fmt);
		vfprintf(useio, fmt, arg);
		va_end(arg);

		fflush(useio);
	}
}

/*************************************************************************
 ** log_code():	Writes a line with log_char() to the log.					**
 *************************************************************************/

void log_code(long level, char *sequence)
{
	int i;

	if ((logdbg & level) || (level == L_FATAL) || (level == L_STDERR))
	{
		for (i = 0; i < strlen(sequence); i++) log_char(level, sequence[i]);
	}
}