/* $Id: opt_time.c,v 1.1 2003/10/29 17:41:35 tobiasb Exp $
 *
 * ISDN accounting for isdn4linux. (Report-module)
 * Time options parsing.  (Seperated from isdnrep.c 1.96)
 *
 * Copyright 1995 .. 2000 by Andreas Kool (akool@isdn4linux.de)
 *                     and Stefan Luethje (luethje@sl-gw.lake.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Copyright 1994, Steven Grimm <koreth@hyperion.com>.
 *
 *
 * $Log: opt_time.c,v $
 * Revision 1.1  2003/10/29 17:41:35  tobiasb
 * isdnlog-4.67:
 *  - Enhancements for isdnrep:
 *    - New option -r for recomputing the connection fees with the rates
 *      from the current (and for a different or the cheapest provider).
 *    - Revised output format of summaries at end of report.
 *    - New format parameters %j, %v, and %V.
 *    - 2 new input formats for -t option.
 *  - Fix for dualmode workaround 0x100 to ensure that incoming calls
 *    will not become outgoing calls if a CALL_PROCEEDING message with
 *    an B channel confirmation is sent by a terminal prior to CONNECT.
 *  - Fixed and enhanced t: Tag handling in pp_rate.
 *  - Fixed typo in interface description of tools/rate.c
 *  - Fixed typo in tools/isdnrate.man, found by Paul Slootman.
 *  - Minor update to sample isdn.conf files:
 *    - Default isdnrep format shows numbers with 16 chars (+ & 15 digits).
 *    - New isdnrep format (-FNIO) without display of transfered bytes.
 *    - EUR as currency in Austria, may clash with outdated rate-at.dat.
 *      The number left of the currency symbol is nowadays insignificant.
 *  - Changes checked in earlier but after step to isdnlog-4.66:
 *    - New option for isdnrate: `-rvNN' requires a vbn starting with NN.
 *    - Do not compute the zone with empty strings (areacodes) as input.
 *    - New ratefile tags r: und t: which need an enhanced pp_rate.
 *      For a tag description see rate-files(5).
 *    - Some new and a few updated international cellphone destinations.
 *
 * NOTE: If there any questions, problems, or problems regarding isdnlog,
 *    feel free to join the isdn4linux mailinglist, see
 *    https://www.isdn4linux.de/mailman/listinfo/isdn4linux for details,
 *    or send a mail in English or German to <tobiasb@isdn4linux.de>.
 *
 */


#define _OPT_TIME_C_

/* allow standalone binary for testing */
/* build with 
   gcc -g -DTEST_OPT_TIME -o opt_time opt_time.c 
 */
#ifdef TEST_OPT_TIME
#include <stdio.h>
#include <string.h>
#include <time.h>
#define UNKNOWN (-1)
#define min(x,y) ((x) < (y) ? (x) : (y))
char *Strncpy(char *dest, const char *src, int len);
#else

#include "isdnrep.h"

#endif

#define END_TIME    1
			
/*****************************************************************************/

static time_t get_month(char *String, int TimeStatus);
static time_t get_time(char *String, int TimeStatus);
static time_t get_gertime(char *s, int TimeStatus);
static time_t get_itime(char *s, int TimeStatus);

/*****************************************************************************/
/* parse time option, a return value of 0 means failure. */
int get_term (char *String, time_t *Begin, time_t *End,int delentries)
{
	time_t now;
	time_t  Date[2];
	int Cnt;
	char DateStr[2][256] = {"",""};
	int idate = 0;
	char sep[3] = { '-', 0, 0 };
	int sep_l = 1;
	char *p;

	time(&now);

	if (tolower(String[0])=='i' || strstr(String,"--")) {
		String += tolower(String[0])=='i';
		sep[1] = sep[0];
		sep_l++;
		idate++;
	}	

	if ( (p=strstr(String, sep)) )
	{
		if (String != p)                                    /* ...<sep> */ 
			Strncpy(DateStr[0], String, min(p-String+1,256));
		if (*(p+sep_l))                                     /* <sep>... */
			Strncpy(DateStr[1], p+sep_l, 256);
	}
	else
	{
		strcpy(DateStr[0],String);
		strcpy(DateStr[1],String);
	}

	for (Cnt = 0; Cnt < 2; Cnt++)
	{
		if ( strspn(DateStr[Cnt], idate ? "01234567890T:-" : "01234567890/.")
				!= strlen(DateStr[Cnt]) )
			return 0;		
		if (idate)
			Date[Cnt] = get_itime(DateStr[Cnt],delentries?0:Cnt);
		else if ( (p=strchr(DateStr[Cnt],'.')) && strchr(p+1,'.') )
			Date[Cnt] = get_gertime(DateStr[Cnt],delentries?0:Cnt);
		else if (strchr(DateStr[Cnt],'/'))
			Date[Cnt] = get_month(DateStr[Cnt],delentries?0:Cnt);
		else
			Date[Cnt] = get_time(DateStr[Cnt],delentries?0:Cnt);
	}

	*Begin = DateStr[0][0] == '\0' ? 0 : Date[0];
	*End = DateStr[1][0] == '\0' || !Date[1] ? now : Date[1];

	if (*Begin > *End)
		return 0;

	return 1;
}

/*****************************************************************************/
/* parse a date like [DD/][M]M/[[YY]YY] */
static time_t get_month(char *String, int TimeStatus)
{
	time_t now;
	int Cnt = 0;
	struct tm *TimeStruct;
	int Args[3];
	int ArgCnt;


	time(&now);
	TimeStruct = localtime(&now);
	TimeStruct->tm_sec = 0;
	TimeStruct->tm_min = 0;
	TimeStruct->tm_hour= 0;
	TimeStruct->tm_mday= 1;
	TimeStruct->tm_isdst= UNKNOWN;

	ArgCnt = sscanf(String,"%d/%d/%d",&(Args[0]),&(Args[1]),&(Args[2]));

	switch (ArgCnt)
	{
		case 3:
			TimeStruct->tm_mday = Args[0];
			Cnt++;
		case 2:
			/* if (Args[Cnt+1] > 99) */
			if (Args[Cnt+1] >= 1900)
				TimeStruct->tm_year = ((Args[Cnt+1] / 100) - 19) * 100 + (Args[Cnt+1]%100);
			else
				if (Args[Cnt+1] < 70)
					TimeStruct->tm_year = Args[Cnt+1] + 100;
				else
					TimeStruct->tm_year = Args[Cnt+1];
		case 1:
			TimeStruct->tm_mon = Args[Cnt];
			break;
		default:
			return 0;
	}

	if (TimeStatus == END_TIME)
	{
		if (ArgCnt == 3)	/* Wenn Tag angegeben ist */
		{
			TimeStruct->tm_mday++;
			TimeStruct->tm_mon--;
		}
	}
	else
		TimeStruct->tm_mon--;

	return mktime(TimeStruct);
}

/*****************************************************************************/
/* parse a date like [MM]DD[hhmm[[CC]YY][.ss]] */
static time_t get_time(char *String, int TimeStatus)
{
	time_t now;
	int Len = 0;
	int Year;
	char *Ptr;
	struct tm *TimeStruct;


	time(&now);
	TimeStruct = localtime(&now);
	TimeStruct->tm_sec = 0;
	TimeStruct->tm_min = 0;
	TimeStruct->tm_hour= 0;
	TimeStruct->tm_isdst= UNKNOWN;

	switch (strlen(String))
	{
		case 0:
						return 0;
		case 1:
		case 2:
						TimeStruct->tm_mday = atoi(String);
						break;
		default:
						Len = strlen(String);

			if ((Ptr = strchr(String,'.')) != NULL)
			{
				TimeStruct->tm_sec = atoi(Ptr+1);
				Len -= strlen(Ptr);
			}

			if (Len % 2)
				return 0;

			if (sscanf(String,"%2d%2d%2d%2d%d",
					&(TimeStruct->tm_mon),
					&(TimeStruct->tm_mday),
					&(TimeStruct->tm_hour),
					&(TimeStruct->tm_min),
					&Year		) 		> 4) {
				/* if (Year > 99) */
	if (Year >= 1900)
					TimeStruct->tm_year = ((Year / 100) - 19) * 100 + (Year%100);
				else {
		if (Year < 70)
			TimeStruct->tm_year = Year + 100;
		else
			TimeStruct->tm_year = Year;
				}
			}
			TimeStruct->tm_mon--;
			break;
	}

	if (TimeStatus == END_TIME) {
		if (TimeStruct->tm_sec == 0 &&
				TimeStruct->tm_min == 0 &&
				TimeStruct->tm_hour== 0   )
			TimeStruct->tm_mday++;
		else {
			if (TimeStruct->tm_sec == 0 &&
				TimeStruct->tm_min == 0   )
				TimeStruct->tm_hour++;
			else {
				if (TimeStruct->tm_sec == 0   )
					TimeStruct->tm_min++;
				else
					TimeStruct->tm_sec++;
			}
		}
	}
	return mktime(TimeStruct);
}

/*****************************************************************************/
/* parse a date like [D]D.[M]M.[[CC][Y]Y] */
static time_t get_gertime(char *s, int TimeStatus)
{
	/* convert to CCYY-MM-DD and give it to get_itime */
	char t[10+1], *sm, *sy;
	int day, month, year;
	if (strlen(s)>10 || strlen(s)<4)
		return 0;
	sm = strchr(s,'.') + 1;
	sy = strchr(sm,'.') + 1;

	if ( *sy )
		year = atoi(sy);                     /* (century and) year from input */
	else {                                 /* get current year */
		time_t now;
		struct tm *tm;
		time(&now);
		tm = localtime(&now);
		year = tm->tm_year+1900;
	}

	month = atoi(sm);
	day = atoi(s);
	if (year>9999 || month>12 || day>31)
		return 0;

	sprintf(t, "%02d-%02d-%02d", year, month, day);

	return get_itime(t, TimeStatus);
}

/*****************************************************************************/
/* parse a date like CCYY[-]MM[-]DD["T"]hh[:]mm[:]ss */
static time_t get_itime(char *s, int TimeStatus)
{
	time_t now, offset=0;
	struct tm *t;
	char *p;
	char a[5];
	int i[6];
	int n;

	time(&now);
	t = localtime(&now);
	t->tm_isdst = UNKNOWN;
	
	p = s;
	if (strlen(s)<4)                       /* at least CCYY or YY-MM */
		return 0;
	
	p = s;
	if ( !( isdigit(p[0]) && isdigit(p[1]) ) )
		return 0;
	if (p[2] == '-') {                     /* YY- */
		Strncpy(a, p, 2+1);
		i[0] = atoi(a);
		i[0] += ((t->tm_year+1900)/100)*100; /* add current century */
		p += 2;
	} 
	else {                                 /* CCYY */
		if ( !( isdigit(p[2]) && isdigit(p[3]) ) )
			return 0;
		Strncpy(a, p, 4+1);
		i[0] = atoi(a);
		p += 4;
	}

	for (n=1; n<6; n++)                    /*  MM DD hh mm ss */
	{                                      /* 1  2  3  4  5   */
		if (*p == ( n<3 ? '-' : (n<4 ? 'T' : ':') ) )
			p++;
		if (! *p)
			break;
		if ( !( isdigit(p[0]) && isdigit(p[1]) ) )
			return 0;
		Strncpy(a, p, 2+1);
		i[n] = atoi(a);
		p += 2;
	}
 
	t->tm_year = i[0] - 1900;

	if (n==2 && TimeStatus==END_TIME && i[1] == 12)
		n = 1;                             /* --2003-12 => --2003 */

	if (n==1 && TimeStatus==END_TIME)    /* --2003 => --2003-12-31T23:59:59 */
	{
		TimeStatus = 0;                    /* 1) => --2003-01-01T00:00:00 */
		t->tm_year++;                      /* 2) => --2004-01-01T00:00:00 */
		offset = -1;                       /* 3) => --2003-12-31T23:59:59 */
	}

	t->tm_mon  = (n>1) ? i[1]-1 : 0;

	if (n==2 && TimeStatus==END_TIME)    /* --2000-02 => --2000-02-29T23:59:59 */
	{
		TimeStatus = 0;                    /* 1) => --2000-02-01T00:00:00 */
		t->tm_mon++;                       /* 2) => --2000-03-01T00:00:00 */
		offset = -1;                       /* 3) => --2000-02-29T23:59:59 */
	}

	t->tm_mday = (n>2) ? i[2] : 1;

	t->tm_hour = (n>3) ? i[3] : ( (TimeStatus==END_TIME) ? 23 : 0);
	t->tm_min  = (n>4) ? i[4] : ( (TimeStatus==END_TIME) ? 59 : 0);
	t->tm_sec  = (n>5) ? i[5] : ( (TimeStatus==END_TIME) ? 59 : 0);

	return mktime(t)+offset;
}	

/*****************************************************************************/

#ifdef TEST_OPT_TIME

/* taken from ../../lib/libtools.c */
char *Strncpy(char *dest, const char *src, int len)
{
	int l = strlen(src);
	if (l > len - 1)
		l = len - 1;
	strncpy(dest,src,l);
	dest[l] = '\0';
	return dest;
}

#define STIME(z) a=localtime(&z); strftime(s, BUFSIZ, "%Y-%m-%d %H:%M:%S", a);

int main(int argc, char *argv[], char *envp[])
{
	int delentries = 0, res;
	time_t begintime, endtime, now;
	struct tm *a;
	char s[BUFSIZ];

	time(&now);
	STIME(now)
	printf("Testing get_term (parses isdnrep's -t option)\n"
	       "Enter . to quit, d to switch delentries.\n"
	       "Any other input will be parsed as option value\n"
	       "Current time: %s\n", s);
	while(1)
	{
		fgets(s, BUFSIZ, stdin);
		s[strlen(s)-1]=0;

		if (s[0] == '.' && s[1] == 0)
			break;
		else if (s[0] == 'd' && s[1] == 0) {
			delentries = !delentries;
			continue;
		}
		res = get_term (s, &begintime, &endtime, delentries);
		if (res) {
			STIME(begintime)
			printf("OK: %s -- ", s);
			STIME(endtime)
			printf("%s  (delentries=%d)\n", s, delentries);
		}
		else
			printf("ERR\n");
	}
	return 0;
}
#undef STIME
#endif

/*****************************************************************************/

/* vim:set ts=2: */

/*****************************************************************************/
