/*
** $Id: vboxrc.c,v 1.1 1998/07/06 09:05:37 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: vboxrc.c,v $
** Revision 1.1  1998/07/06 09:05:37  michael
** - New control file code added. The controls are not longer only empty
**   files - they can contain additional informations.
** - Control "vboxctrl-answer" added.
** - Control "vboxctrl-suspend" added.
** - Locking mechanism added.
** - Configuration parsing added.
** - Some code cleanups.
**
*/             

#include "../config.h"

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
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "vboxrc.h"
#include "log.h"
#include "stringutils.h"

/** Variables ************************************************************/

static char *weekdaynames[] =
{
	"SO" , "MO" , "DI" , "MI" , "DO" , "FR" , "SA" ,
	"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT",

	NULL
};

/** Prototypes ***********************************************************/

static int		vboxrc_check_user_section(FILE *, struct vboxcall *, char *);
static int		vboxrc_goto_section(FILE *, char *);
static int		vboxrc_parse_time(char *);
static int		vboxrc_parse_days(char *);
static time_t	vboxrc_return_time(time_t, char *, int);
static void		vboxrc_return_timestr(time_t, char *);

/*************************************************************************/
/** vboxrc_parse():	Liest die "vboxrc" eines Benutzers ein und gibt		**/
/**						die Anzahl der Klingelzeichen zurück nach denen		**/
/**						der Anruf beantwortet werden soll.						**/
/*************************************************************************/
/** => call				Zeiger auf die Call-Struktur.								**/
/** => home				Spoolverzeichnis des Benutzers.							**/
/** => id				Eingegangene CallerID.										**/
/*************************************************************************/

int vboxrc_parse(struct vboxcall *call, char *home, char *id)
{
   char	line[VBOXRC_MAX_RCLINE + 1];
	char  name[PATH_MAX + 1];
	FILE *rc;
	int	nr;
	char *dummy;
	char *owner;
	char *phone;
	char *table;	
	int	i;

	printstring(name, "%s/vboxrc", home);

	log_line(LOG_D, "Parsing \"%s\"...\n", name);

	xstrncpy(call->name	 , "*** Unknown ***", VBOXRC_MAX_RCLINE);
	xstrncpy(call->section, "*** Unknown ***", VBOXRC_MAX_RCLINE);
	xstrncpy(call->script , "answercall.tcl" , VBOXRC_MAX_RCLINE);

	call->ringring = 0;
	call->tollring = 0;
	call->savetime = 90;

	if ((rc = fopen(name, "r")))
	{
		if (vboxrc_goto_section(rc, "[CALLERID]") == 0)
		{
			log_line(LOG_D, "Searching callerid \"%s\"...\n", id);

			while (fgets(line, (VBOXRC_MAX_RCLINE - 3), rc))
			{
				line[strlen(line) - 1] = '\0';
				
				if ((*line == '\0') || (*line == '#')) continue;
				if ((*line == '[' ) || (*line == ']')) break;

				dummy = line;
				owner = NULL;
				table = NULL;
				phone = NULL;
                                                
				while (isspace(*dummy)) dummy++;

				if ((*dummy) && (dummy)) phone = strsep(&dummy, "\t ");

				while (isspace(*dummy)) dummy++;

				if ((*dummy) && (phone)) table = strsep(&dummy, "\t ");

				while (isspace(*dummy)) dummy++;

				if ((*dummy) && (table)) owner = dummy;

				if ((owner) && (phone) && (table))
				{
					if (fnmatch(phone, id, 0) == 0)
					{
						xstrncpy(call->name, owner, VBOXRC_MAX_RCLINE);

						switch (*table)
						{
							case '*':
								printstring(call->section, "[%s]", "default");
								break;
								
							case '-':
								printstring(call->section, "[%s]", owner);
								break;
								
							default:
								printstring(call->section, "[%s]", table);
								break;
						}

						log_line(LOG_D, "Found - name is \"%s\".\n", call->name);

						for (i = 0; i < strlen(call->section); i++)
						{
							call->section[i] = toupper(call->section[i]);
						}

						nr = vboxrc_check_user_section(rc, call, call->section);

						fclose(rc);
						return(nr);
					}
				}
			}

			log_line(LOG_W, "Unable to locate \"%s\" settings.\n", id);
		}
		else log_line(LOG_W, "Can't seek to section \"[CALLERID]\".\n");

		fclose(rc);
	}
	else log_line(LOG_W, "Can't open \"%s\".\n", name);

	return(0);
}

/*************************************************************************/
/** vboxrc_check_user_section():														**/
/*************************************************************************/

static int vboxrc_check_user_section(FILE *rc, struct vboxcall *call, char *section)
{
   char	line[VBOXRC_MAX_RCLINE + 1];
	char *time;
	char *days;
	char *name;
	char *save;
	char *ring;
	char *toll;
	char *stop;

	if (vboxrc_goto_section(rc, section) == 0)
	{
		while (fgets(line, (VBOXRC_MAX_RCLINE - 3), rc))
		{
			line[strlen(line) - 1] = '\0';
				
			if ((*line == '\0') || (*line == '#')) continue;
			if ((*line == '[' ) || (*line == ']')) break;

			time = strtok(line, "\t ");
			days = strtok(NULL, "\t ");
			name = strtok(NULL, "\t ");
			save = strtok(NULL, "\t ");
			ring = strtok(NULL, "\t ");
			toll = strtok(NULL, "\t ");

			if ((time) && (days) && (name) && (save) && (ring) && (toll))
			{
				if (vboxrc_parse_time(time) == 0)
				{
					if (vboxrc_parse_days(days) == 0)
					{
						call->ringring = xstrtol(ring, 0);
						call->tollring = xstrtol(toll, 0);
						call->savetime = xstrtol(save, 0);
						
						if ((stop = rindex(name, '/')))
						{
							stop++;

							xstrncpy(call->script, stop, VBOXRC_MAX_RCLINE);
						}
						else xstrncpy(call->script, name, VBOXRC_MAX_RCLINE);

						if (call->tollring > 0)
						{
						}

						return(call->ringring);
					}
				}
			}
		}
	}
	else log_line(LOG_W, "Can't seek to section \"%s\".\n", section);

	return(0);
}

/*************************************************************************/
/** vboxrc_goto_section():																**/
/*************************************************************************/

static int vboxrc_goto_section(FILE *rc, char *section)
{
   char line[VBOXRC_MAX_RCLINE + 1];

	log_line(LOG_D, "Seeking to section \"%s\"...\n", section);

	rewind(rc);
   
	while (fgets(line, (VBOXRC_MAX_RCLINE - 3), rc))
	{	
		line[strlen(line) - 1] = '\0';
		
		if ((*line == '\0') || (*line == '#')) continue;

		if (strcasecmp(line, section) == 0) return(0);
	}

	return(-1);
}

/*************************************************************************/
/** vboxrc_parse_time():	Checks if one or more timestring match the	**/
/**								current time.											**/
/*************************************************************************/

static int vboxrc_parse_time(char *timestr)
{
	char		timestring[VBOXRC_MAX_RCLINE + 1];
	char		timevaluebeg[8 + 1];
	char		timevalueend[8 + 1];
	char		timevaluenow[8 + 1];
	char	  *timeptr;
	char	  *timenxt;
	char	  *timebeg;
	char	  *timeend;
	time_t	timenow;
	time_t	timesecsbeg;
	time_t	timesecsend;

	log_line(LOG_D, "Parsing time(s) \"%s\"...\n", timestr);

	xstrncpy(timestring, timestr, VBOXRC_MAX_RCLINE);

	timeptr = timestring;
	timenow = time(NULL);

	vboxrc_return_timestr(timenow, timevaluenow);

	if (strcmp(timestring, "*") == 0)
	{
		log_line(LOG_D, "Range **:**:** - **:**:** (%s): match.\n", timevaluenow);
		
		return(0);
	}
	
	if ((strcmp(timestring, "!") == 0) || (strcmp(timestring, "-") == 0))
	{
		log_line(LOG_D, "Range --:--:-- - --:--:-- (%s): don't match.\n", timevaluenow);
		
		return(-1);
	}
	
	while (timeptr)
	{
		if ((timenxt = index(timeptr, ','))) *timenxt++ = '\0';

		timebeg = timeptr;
		timeend = index(timebeg, '-');

		if (timeend)
			*timeend++ = '\0';
		else
			timeend = timebeg;

		if (!timeend) timeend = timebeg;

		timesecsbeg = vboxrc_return_time(timenow, timebeg, 0);
		timesecsend = vboxrc_return_time(timenow, timeend, 1);

		if ((timesecsbeg > 0) && (timesecsend > 0))
		{
			if ((timesecsend >= timesecsbeg))
			{
				vboxrc_return_timestr(timesecsbeg, timevaluebeg);
				vboxrc_return_timestr(timesecsend, timevalueend);
				vboxrc_return_timestr(timenow    , timevaluenow);
			
				log_line(LOG_D, "Range %s - %s (%s): ", timevaluebeg, timevalueend, timevaluenow);

				if ((timenow >= timesecsbeg) && (timenow <= timesecsend))
				{
					log_text(LOG_D, "match.\n");
				
					return(0);
				}
				else log_text(LOG_D, "don't match.\n");
			}
			else log_line(LOG_W, "Bad time; start greater than end (%s-%s) - ignored...\n", timebeg, timeend);
		}
		else log_line(LOG_W, "Bad time; can't convert timestring (%s-%s) - ignored...\n", timebeg, timeend);

		timeptr = timenxt;
	}

	log_line(LOG_D, "String contains no matching time!\n");

	return(-1);
}

/*************************************************************************/
/** vboxrc_parse_days():	Checks if one or more daystrings match the	**/
/**								current day.											**/
/*************************************************************************/

static int vboxrc_parse_days(char *strdays)
{
	struct tm  *timelocal;
	char		  *beg;
	char		  *nxt;
	char			days[VBOXRC_MAX_RCLINE + 1];
	int			i;
	int			d;
	time_t		currenttime;

	xstrncpy(days, strdays, VBOXRC_MAX_RCLINE);

	log_line(LOG_D, "Parsing day(s) \"%s\"...\n", days);

	if (strcmp(days, "*") == 0)
	{
		log_line(LOG_D, "Range *: match.\n");
	
		return(0);
	}

	if ((strcmp(days, "-") == 0) || (strcmp(days, "!") == 0))
	{
		log_line(LOG_D, "Range -: don't match.\n");

		return(-1);
	}

	currenttime = time(NULL);

	if (!(timelocal = localtime(&currenttime)))
	{
		log_line(LOG_E, "Can't get local time (don't match)...\n");
		
		return(-1);
	}

	for (i = 0; i < strlen(days); i++)
	{
		if ((!isalpha(days[i])) && (days[i] != ','))
		{
			log_line(LOG_E, "Error in day string \"%s\" (don't match).\n", days);

			return(-1);
		}
	}

	beg = days;

	while (beg)
	{
		if ((nxt = index(beg, ','))) *nxt++ = 0;

		d = 0;
		i = 0;

		while (weekdaynames[i])
		{
			if (strcasecmp(weekdaynames[i], beg) == 0)
			{
				if (d == timelocal->tm_wday)
				{
					log_line(LOG_D, "Range %s: match.\n", beg);

					return(0);
				}
				else log_line(LOG_D, "Range %s: don't match.\n", beg);
			}

			d++;
			i++;
			
			if (d >= 7) d = 0;
		}

		beg = nxt;
	}

	log_line(LOG_D, "String contains no matching day!\n");

	return(-1);
}

/*************************************************************************/
/** vboxrc_return_time():	Converts a time string to seconds since the	**/
/**								1st januar 1970.										**/
/*************************************************************************/

static time_t vboxrc_return_time(time_t timenow, char *timestr, int mode)
{
	struct tm *locala;
	struct tm  localb;

	char	timestring[5 + 1];
	char *hourstr;
	char *minsstr;
	int	hourint;
	int	minsint;
	int	secsint;

	xstrncpy(timestring, timestr, 5);

	hourstr = timestring;
	minsstr = index(hourstr, ':');

	if (!minsstr)
	{
		if (mode == 0) minsstr = "00";
		if (mode != 0) minsstr = "59";
	}
	else *minsstr++ = '\0';

	if (mode == 0) secsint =  0;
	else           secsint = 59;

	hourint = atoi(hourstr);
	minsint = atoi(minsstr);

	if ((hourint < 0) || (hourint > 23)) return(0);
	if ((minsint < 0) || (minsint > 59)) return(0);

	if (!(locala = localtime(&timenow))) return(0);

	memcpy(&localb, locala, sizeof(struct tm));

	localb.tm_sec	= secsint;
	localb.tm_min	= minsint;
	localb.tm_hour	= hourint;

	return(mktime(&localb));
}

/*************************************************************************/
/** vboxrc_return_timestr():	Converts a unix time into a string like	**/
/**									HH:MM:SS.											**/
/*************************************************************************/

static void vboxrc_return_timestr(time_t timeint, char *timestr)
{
	struct tm *tm;

	if ((tm = localtime(&timeint)))
	{
		if (strftime(timestr, 20, "%H:%M:%S", tm) == 8) return;
	}

	xstrncpy(timestr, "??:??:??", 8);
}
