 /* $Id: holiday.c,v 1.1 1999/03/14 12:16:23 akool Exp $
 *
 * Feiertagsberechnung
 *
 * Copyright 1999 by Michael Reinelt (reinelt@eunet.at)
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
 * $Log: holiday.c,v $
 * Revision 1.1  1999/03/14 12:16:23  akool
 * - isdnlog Version 3.04
 * - general cleanup
 * - new layout for "rate-xx.dat" and "holiday-xx.dat" files from
 *     Michael Reinelt <reinelt@eunet.at>
 *     unused by now - it's a work-in-progress !
 * - bugfix for Wolfgang Siefert <siefert@wiwi.uni-frankfurt.de>
 *     The Agfeo AS 40 (Software release 2.1b) uses AOC_AMOUNT, not AOC_UNITS
 * - bugfix for Ralf G. R. Bergs <rabe@RWTH-Aachen.DE> - 0800/xxx numbers
 *     are free of charge ;-)
 * - tarif.dat V 1.08 - new mobil-rates DTAG
 *
 */

/*
 * Schnittstelle:
 *
 * int initHoliday(char *path)
 *   initialisiert die Feiertagsberechnung, liest die Feiertagsdatei
 *   und gibt die Anzahl der Feiertage zurück, im Fehlerfall -1
 *
 * void exitHoliday()
 *   deinitialisiert die Feiertagsberechnung
 *
 * int isHoliday(struct tm *tm, char **name)
 *   0 .. kein Feiertag
 *   1 .. Feiertag, *name zeigt auf den Namen
 *
 * int getDay(struct tm *tm, char **name)
 *   1 .. Montag
 *   7 .. Sonntag
 *   8 .. Feiertag
 *   *name zeigt auf den Namen
 */

#define _HOLIDAY_C_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "holiday.h"

#define LENGTH 80  /* max length of lines in data file */

typedef unsigned int julian;

typedef struct {
  int day;
  int month;
  char *name;
} HOLIDATE;

static int        line = 0;
static char      *Weekday[7] = { NULL, };
static int        nHoliday = 0; 
static HOLIDATE  *Holiday = NULL;

static char *defaultWeekday[] = { "Monday",
				  "Thuesday",
				  "Wednesday",
				  "Thursday",
				  "Friday",
				  "Saturday",
				  "Sunday" };

static void warning (char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  va_start (ap, fmt);
  vsnprintf (msg, BUFSIZ, fmt, ap);
  va_end (ap);
#ifdef STANDALONE
  fprintf(stderr, "WARNING: line %3d: %s\n", line, msg);
#else
  print_msg(PRT_NORMAL, "WARNING: line %3d: %s\n", line, msg);
#endif
}

/* easter & julian calculations by Guenther Brunthaler (gbr001@yahoo.com) */

static julian date2julian(int y, int m, int d)
{
 if (m<3) {m+=9; y--;} else m-=3;
 return (146097*(y/100))/4+(1461*(y%100))/4+(153*m+2)/5+d;
}

static void julian2date(julian jd, int *yp, int *mp, int *dp)
{
  julian j,c,y,m,d;

  j=d*4-1;
  c=(j/146097)*100;
  d=(j%146097)/4;
  y=(4*d+3)/1461;
  d=(((4*d+3)%1461)+4)/4;
  m=(5*d-3)/153;
  d=(((5*d-3)%153)+5)/5;

  if (m>9) {m-=9; y++;} else m+=3;

  *yp=c+y;
  *mp=m;
  *dp=d;
}

static julian getEaster(int year)
{
  int g, c, x, z, d, e, n, m;

  g=year%19+1;
  c=year/100+1;
  x=3*c/4-12;
  z=(8*c+5)/25-5;
  d=5*year/4-x-10;
  e=(11*g+20+z-x)%30;
  if ((e==25 && g>11) || e==24) e++;
  n=44-e;
  if (n<21) n+=30;
  n+=7-(d+n)%7;
  if (n>31) {n-=31; m=4;} else m=3;
  return date2julian(year,m,n);
}

static char *strip (char *s)
{
  char *p;

  while (*s==' ' || *s=='\t') s++;
  for (p=s; *p; p++)
    if (*p=='#' || *p=='\n') {
      *p='\0';
      break;
    }
  for (p--; p>s && (*p==' '||*p=='\t'); p--) 
    *p='\0';
  return s;
}

void exitHoliday(void)
{
  int i;
  
  for (i=0; i<7; i++) {
    if (Weekday[i]) {
      free (Weekday[i]);
      Weekday[i]=NULL;
    }
  }
  for (i=0; i<nHoliday; i++) {
    if (Holiday[i].name) free (Holiday[i].name);
  }
  if (Holiday) free (Holiday);
  nHoliday=0;
  Holiday=NULL;
}

int initHoliday(char *path)
{
  FILE *stream;
  char *s, *date, *name;
  char  buffer[LENGTH];
  int   i,m,d;

  exitHoliday();

  for (i=1; i<7; i++)
    Weekday[i]=strdup(defaultWeekday[i]);
  
  if (*path=='\0')
    return 0;

  if ((stream=fopen(path,"r"))==NULL)
    return -1;
  
  line=0;
  while ((s=fgets(buffer,LENGTH,stream))!=NULL) {
    line++;
    if (*(s=strip(s))=='\0')
      continue;
    if (s[1]!=':') {
      warning ("expected ':', got '%s'!", s+1);;
      continue;
    }
    switch (*s) {
    case 'W':
      d=strtol(s+2,&s,10);
      if (d<1 || d>7) {
	warning("invalid weekday %d", d);
	continue;
      }
      if (*(name=strip(s))=='\0') {
	warning("empty weekday %d", d);
	continue;
      }
      if (Weekday[d-1]) free(Weekday[d-1]);
      Weekday[d-1]=strdup(name);
      break;

    case 'D':
      name=s+2;
      if ((date=strsep(&name," \t"))==NULL) {
	warning("Syntax error");
	continue;
      }
      if (strncmp(date,"easter",6)==0) {
	m=-1;
	d=atoi(date+6);
      } else {
	d=atof(strsep(&date,"."));
	m=atof(strsep(&date,"."));
      }
      Holiday=(HOLIDATE*)realloc(Holiday,(nHoliday+1)*sizeof(HOLIDATE));
      Holiday[nHoliday].day=d;
      Holiday[nHoliday].month=m;
      Holiday[nHoliday].name=strdup(strip(name));
      nHoliday++;
      break;

    default:
      warning("Unknown tag '%c'", *s);
    }
  }
  fclose(stream);
  
  return nHoliday;
}

int isHoliday(struct tm *tm, char **name)
{
  int i;
  julian easter, day;
  
  day=date2julian(tm->tm_year,tm->tm_mon,tm->tm_mday);
  easter=getEaster(tm->tm_year);

  for (i=0; i<nHoliday; i++) {
    if ((Holiday[i].month==-1 && Holiday[i].day==day-easter) ||
	(Holiday[i].month==tm->tm_mon  && Holiday[i].day==tm->tm_mday)) {
      if(name) 
	*name=Holiday[i].name;
      return 1;
    }
  }
  return 0;
}

int getDay(struct tm *tm, char **name)
{
  julian day;
  
  if (isHoliday(tm, name))
    return HOLIDAY;
  
  day=(date2julian(tm->tm_year,tm->tm_mon,tm->tm_mday)-6)%7+1;
  if (name)
    *name=Weekday[day-1];

  return day;
}

#ifdef STANDALONE
void main (int argc, char *argv[])
{
  int i, d;
  char *name;
  struct tm tm;

  printf ("%d Feiertage\n", initHoliday("../holiday-de.dat"));

  for (i=1; i < argc; i++) {
    tm.tm_mday=atoi(strsep(argv+i,"."));
    tm.tm_mon=atoi(strsep(argv+i,"."));
    tm.tm_year=atoi(strsep(argv+i,"."));
    
    d=getDay(&tm,&name);
    printf ("%d.%d.%d\t%d=%s\n", tm.tm_mday,tm.tm_mon, tm.tm_year,d,name);
  }
}
#endif
