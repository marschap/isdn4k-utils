/* $Id: rate.c,v 1.5 1999/04/14 13:17:24 akool Exp $
 *
 * Tarifdatenbank
 *
 * Copyright 1995, 1999 by Andreas Kool (akool@isdn4linux.de)
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
 * $Log: rate.c,v $
 * Revision 1.5  1999/04/14 13:17:24  akool
 * isdnlog Version 3.14
 *
 * - "make install" now install's "rate-xx.dat", "rate.conf" and "ausland.dat"
 * - "holiday-xx.dat" Version 1.1
 * - many rate fixes (Thanks again to Michael Reinelt <reinelt@eunet.at>)
 *
 * Revision 1.4  1999/04/10 16:36:39  akool
 * isdnlog Version 3.13
 *
 * WARNING: This is pre-ALPHA-dont-ever-use-Code!
 * 	 "tarif.dat" (aka "rate-xx.dat"): the next generation!
 *
 * You have to do the following to test this version:
 *   cp /usr/src/isdn4k-utils/isdnlog/holiday-de.dat /etc/isdn
 *   cp /usr/src/isdn4k-utils/isdnlog/rate-de.dat /usr/lib/isdn
 *   cp /usr/src/isdn4k-utils/isdnlog/samples/rate.conf.de /etc/isdn/rate.conf
 *
 * After that, add the following entries to your "/etc/isdn/isdn.conf" or
 * "/etc/isdn/callerid.conf" file:
 *
 * [ISDNLOG]
 * SPECIALNUMBERS = /usr/lib/isdn/sonderrufnummern.dat
 * HOLIDAYS       = /usr/lib/isdn/holiday-de.dat
 * RATEFILE       = /usr/lib/isdn/rate-de.dat
 * RATECONF       = /etc/isdn/rate.conf
 *
 * Please replace any "de" with your country code ("at", "ch", "nl")
 *
 * Good luck (Andreas Kool and Michael Reinelt)
 *
 * Revision 1.3  1999/03/24 19:39:00  akool
 * - isdnlog Version 3.10
 * - moved "sondernnummern.c" from isdnlog/ to tools/
 * - "holiday.c" and "rate.c" integrated
 * - NetCologne rates from Oliver Flimm <flimm@ph-cip.uni-koeln.de>
 * - corrected UUnet and T-Online rates
 *
 * Revision 1.2  1999/03/16 17:38:09  akool
 * - isdnlog Version 3.07
 * - Michael Reinelt's patch as of 16Mar99 06:58:58
 * - fix a fix from yesterday with sondernummern
 * - ignore "" COLP/CLIP messages
 * - dont show a LCR-Hint, if same price
 *
 * Revision 1.1  1999/03/14 12:16:42  akool
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
 * Schnittstelle zur Tarifdatenbank:
 *
 * void exitRate(void)
 *   deinitialisiert die Tarifdatenbank
 *
 * void initRate(char *conf, char *dat, char **msg)
 *   initialisiert die Tarifdatenbank
 *
 * char* getProvidername (int prefix)
 *   liefert den Namen des Providers oder NULL wenn unbekannt
 *
 * int getZone (int prefix, char *num)
 *   liefert die Zone, mit der die Rufnummer beim Provider prefix
 *   verrechnet wird, oder unknown
 *
 * int getRate(RATE*Rate, char **msg)
 *   liefert die Tarifberechnung in *Rate, UNKNOWN im
 *   Fehlerfall, *msg enthält die Fehlerbeschreibung
 *
 * int getLeastCost (RATE *Rate, int skip)
 *   berechnet den billigsten Provider zu *Rate, Rückgabewert
 *   ist der Prefix des billigsten Providers oder UNKNOWN wenn
 *   *Rate bereits den billigsten Tarif enthält. Der Provider
 *   'skip' wird übersprungen (falls überlastet).
 *
 * int guessZone (RATE *Rate, int units)
 *   versucht die Zone zu erraten, wenn mit den Daten in Rate
 *   units Einheiten gemeldet wurden
 *
 * char *explainRate (RATE *Rate)
 *   liefert eine textuelle Begründung für den Tarif in der Form
 *   "Provider, Zone, Wochentag, Zeit"
 *
 */

#define _RATE_C_

#ifdef STANDALONE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#else
#include "isdnlog.h"
#include "tools.h"
#endif
#include "holiday.h"
#include "rate.h"

#define MAXPROVIDER 1000
#define LENGTH 250  /* max length of lines in data file */
#define UNKNOWN -1

typedef struct _STACK {
  int data;
  struct _STACK *next;
} STACK;

typedef struct {
  int Duration;
  int Delay;
  double Price;
} UNIT;

typedef struct {
  char     *Name;
  bitfield  Day;
  bitfield  Hour;
  int       nUnit;
  UNIT     *Unit;
} HOUR;

typedef struct {
  int    used;
  char  *Name;
  int    nArea;
  char **Area;
  int    nHour;
  HOUR  *Hour;
} ZONE;

typedef struct {
  int    booked;
  int    used;
  char  *Name;
  int    nZone;
  ZONE  *Zone;
} PROVIDER;

static PROVIDER *Provider=NULL;
static int      nProvider=0;
static int      line=0;

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

static char* str2set (char **s)
{
  static char buffer[BUFSIZ];
  char *p=buffer;

  while (**s) {
    if (**s==',')
      break;
    else
      *p++=*(*s)++;
  }
  *p = '\0';
  return buffer;
}

static int strmatch (const char *pattern, const char *string)
{
  while (*pattern) {
    if (*pattern!=*string || !*string)
      return 0;
    pattern++;
    string++;
  }
  return 1;
}

static void push (STACK **stack, int data)
{
  STACK *new=malloc(sizeof(STACK));
  new->data=data;
  new->next=*stack;
  *stack=new;
}

static int pop (STACK **stack)
{
  STACK *old=*stack;
  int data;

  if (!old)
    return UNKNOWN;
  data=old->data;
  *stack=old->next;
  return (data);
}

static void empty (STACK **stack)
{
  STACK *old;
  while (*stack) {
    old=*stack;
    *stack=old->next;
    free(old);
  }
}

void exitRate(void)
{
  int i, j, k;

  for (i=0; i<nProvider; i++) {
    if (Provider[i].used) {
      for (j=0; j<Provider[i].nZone; j++)
	if (Provider[i].Zone[j].used) {
	  Provider[i].Zone[j].used=0;
	  if (Provider[i].Zone[j].Name) free (Provider[i].Zone[j].Name);
	  for (k=0; k<Provider[i].Zone[j].nArea; k++)
	    if (Provider[i].Zone[j].Area[k]) free (Provider[i].Zone[j].Area[k]);
	  if (Provider[i].Zone[j].Area) free (Provider[i].Zone[j].Area);
	  for (k=0; k<Provider[i].Zone[j].nHour; k++) {
	    if (Provider[i].Zone[j].Hour[k].Name) free (Provider[i].Zone[j].Hour[k].Name);
	    if (Provider[i].Zone[j].Hour[k].Unit) free (Provider[i].Zone[j].Hour[k].Unit);
	  }
	  if (Provider[i].Zone[j].Hour) free (Provider[i].Zone[j].Hour);
	}
      if(Provider[i].Zone) free (Provider[i].Zone);
      if (Provider[i].Name) free (Provider[i].Name);
      Provider[i].used=0;
    }
  }
}

int initRate(char *conf, char *dat, char **msg)
{
  FILE    *stream;
  char     buffer[LENGTH];
  char     Version[LENGTH]="<unknown>";
  int      booked[MAXPROVIDER];
  int      variant[MAXPROVIDER];
  int      Providers=0;
  int      nZone=0;
  int      nHour=0;
  int      ignore=0;
  int      prefix=UNKNOWN;
  STACK   *zones=NULL, *zp;
  int      zone1, zone2, day1, day2, hour1, hour2;
  bitfield day, hour;
  double   price, divider, duration;
  int      delay;
  int      i, t, u, v, z;
  char    *a, *c, *s;
  static char message[LENGTH];

  if (msg)
    *(*msg=message)='\0';

  for (i=0; i<MAXPROVIDER; i++) {
    booked[i]=0;
    variant[i]=UNKNOWN;
  }

    line=0;
  if (conf && *conf) {
    if ((stream=fopen(conf,"r"))==NULL) {
      if (msg) snprintf (message, LENGTH, "Error: could not load rate configuration from %s: %s",
			 conf, strerror(errno));
      return -1;
    } else {
    while ((s=fgets(buffer,LENGTH,stream))!=NULL) {
      line++;
      if (*(s=strip(s))=='\0')
	continue;
      if (s[1]!=':') {
	warning ("expected ':', got '%s'!", s+1);;
	continue;
      }
      switch (*s) {
      case 'P':
	  s+=2;
	  while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning ("Invalid provider-number %s", s);
	    continue;
	  }
	  prefix = strtol(s, &s ,10);
	  if (prefix >= MAXPROVIDER) {
	    warning ("Invalid provider-number %d", prefix);
	    continue;
	  }
	  booked[prefix]=1;
	  while (isblank(*s)) s++;
	  if (*s == '=') {
	  s++;
	  while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning ("Invalid variant %s", s);
	  continue;
	}
	    if ((v=strtol(s, &s, 10))<1) {
	      warning ("Invalid variant %s", s);
	      continue;
	    }
	    variant[prefix]=v;
	  while (isblank(*s)) s++;
	  }
	  if (*s) {
	    warning ("trailing junk '%s' ignored.", s);
	  }
	break;

      default:
	warning("Unknown tag '%c'", *s);
      }
    }
    fclose (stream);
  }
  }

  if (!dat || !*dat) {
    if (msg) snprintf (message, LENGTH, "Warning: no rate database specified!",
		       conf, strerror(errno));
    return 0;
  }

  if ((stream=fopen(dat,"r"))==NULL) {
    if (msg) snprintf (message, LENGTH, "Error: could not load rate database from %s: %s",
		       dat, strerror(errno));
    return -1;
  }

  line=0;
  prefix=UNKNOWN;
  while ((s=fgets(buffer,LENGTH,stream))!=NULL) {
    line++;
    if (*(s=strip(s))=='\0')
      continue;
    if (s[1]!=':') {
      warning ("expected ':', got '%s'!", s+1);;
      continue;
    }

    switch (*s) {

    case 'P': /* P:nn[,v] Bezeichnung */
      ignore = 0;
      v = UNKNOWN;
      empty(&zones);

      s+=2; while (isblank(*s)) s++;
      if (!isdigit(*s)) {
	warning ("Invalid provider-number '%c'", *s);
	continue;
      }
      prefix = strtol(s, &s ,10);
      if (prefix >= MAXPROVIDER) {
	warning ("Invalid provider-number %d", prefix);
	continue;
      }
      while (isblank(*s)) s++;
      if (*s == ',') {
	s++; while (isblank(*s)) s++;
	if (!isdigit(*s)) {
	  warning ("Invalid variant '%c'", *s);
	  continue;
	}
	if ((v=strtol(s, &s ,10))<1) {
	  warning ("Invalid variant %d", v);
	  continue;
      }
      }
      if ((variant[prefix]!=UNKNOWN) && (v!=UNKNOWN) && (variant[prefix]!=v)) {
	v = UNKNOWN;
	ignore = 1;
	continue;
      }
      if (prefix>=nProvider) {
	Provider=realloc(Provider, (prefix+1)*sizeof(PROVIDER));
	for (i=nProvider; i<=prefix; i++)
	  Provider[i].used=0;
	nProvider=prefix+1;
      }
      if (Provider[prefix].used++) {
	warning ("Duplicate entry for provider %d (%s)", prefix, Provider[prefix].Name);
	if (Provider[prefix].Name) free(Provider[prefix].Name);
      }
      while (isblank(*s)) s++;
      Provider[prefix].Name=*s?strdup(s):NULL;
      Provider[prefix].booked=booked[prefix];
      Provider[prefix].nZone=0;
      Provider[prefix].Zone=NULL;
      Providers++;
      break;

    case 'G': /* P:tt.mm.jjjj Hour gueltig ab */
      if (ignore) continue;
      break;

    case 'C':  /* C:Comment */
      if (ignore) continue;
      break;

    case 'Z': /* Z:n[-n][,n] Bezeichnung */
      if (ignore) continue;
      if (prefix == UNKNOWN) {
	warning ("Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      empty(&zones);
      while (1) {
	while (isblank(*s)) s++;
      if (!isdigit(*s)) {
	warning ("Invalid zone '%c'", *s);
	  empty(&zones);
	  break;
	}
	zone1=strtol(s,&s,10);
	while (isblank(*s)) s++;
	if (*s=='-') {
	  s++; while (isblank(*s)) s++;
      if (!isdigit(*s)) {
	warning ("Invalid zone '%c'", *s);
	    empty(&zones);
	    break;
	  }
	  zone2=strtol(++s,&s,10);
	  if (zone2<zone1) {
	    i=zone2; zone2=zone1; zone1=i;
	  }
	  for (i=zone1; i<=zone2; i++)
	    push (&zones, i);
	} else {
	  push (&zones, zone1);
	}
	while (isblank(*s)) s++;
	if (*s==',') {
	  s++;
	continue;
      }
	break;
      }

      if (!zones)
	break;

      zp=zones;
      while (zp) {
	z=pop(&zp);
	if (z>=Provider[prefix].nZone) {
	  Provider[prefix].Zone=realloc(Provider[prefix].Zone, (z+1)*sizeof(ZONE));
	  for (i=Provider[prefix].nZone; i<z; i++)
	  Provider[prefix].Zone[i].used=0;
	  Provider[prefix].nZone = z+1;
      }
	Provider[prefix].Zone[z].used=1;
	Provider[prefix].Zone[z].Name=*s?strdup(s):NULL;
	Provider[prefix].Zone[z].nArea=0;
	Provider[prefix].Zone[z].Area=NULL;
	Provider[prefix].Zone[z].nHour=0;
	Provider[prefix].Zone[z].Hour=NULL;
	nZone++;
      }
      break;

    case 'A': /* A:areacode[,areacode...] */
      if (ignore) continue;
      if (!zones) {
	warning ("Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      zp=zones;
      while (zp) {
	z=pop(&zp);
	a=s;
	while(1) {
	  if (*(c=strip(str2set(&a)))) {
	    Provider[prefix].Zone[z].Area=realloc(Provider[prefix].Zone[z].Area,
						  (Provider[prefix].Zone[z].nArea+1)*sizeof(char*));
	    Provider[prefix].Zone[z].Area[Provider[prefix].Zone[z].nArea++]=strdup(c);
	  } else {
	    warning ("Ignoring empty areacode");
	  }
	  if (*a==',') {
	    a++;
	    continue;
	  }
	  break;
	}
      }
      s=a;
      break;

    case 'T':  /* T:d-d/h-h=p/s:t[=]Bezeichnung */
      if (ignore) continue;
      if (!zones) {
	warning ("Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      day=0;
      hour=0;
      while (1) {
	while (isblank(*s)) s++;
	if (*s=='*') {                 /* jeder Tag */
	  day |= 1<<EVERYDAY;
	  s++;
	} else if (*s=='W') {          /* Wochentag 1-5 */
	  day |= 1<<WORKDAY;
	  s++;
	} else if (*s=='E') {          /* weekEnd */
	  day |= 1<<WEEKEND;
	  s++;
	} else if (*s=='H') {          /* Holiday */
	  day |= 1<<HOLIDAY;
	  s++;
	} else if (isdigit(*s)) {      /* 1 oder 1-5 */
	  day1=strtol(s,&s,10);
	  while (isblank(*s)) s++;
	  if (*s=='-') {
	    s++; while (isblank(*s)) s++;
	    if (!isdigit(*s)) {
	      warning ("invalid day '%s'", s);
	      day=0;
	      break;
	    }
	    day2=strtol(s,&s,10);
	  } else day2=day1;
	  if (day1<1 || day1>7) {
	    warning ("invalid day %d", day1);
	    day=0;
	    break;
	  }
	  if (day2<1 || day2>7) {
	    warning ("invalid day %d", day2);
	    day=0;
	  break;
	  }
	  if (day2<day1) {
	    i=day2; day2=day1; day1=i;
	  }
	  for (i=day1; i<=day2; i++)
	    day|=(1<<i);
	} else {
	  warning ("invalid day '%c'", *s);
	  day=0;
	  break;
	}
	while (isblank(*s)) s++;
	if (*s==',') {
	  s++;
	  continue;
	}
	break;
      }

      if (!day)
	break;

      if (*s!='/') {
	warning ("expected '/', got '%s'!", s);
	day=0;
      }

      s++;
      while (1) {
	while (isblank(*s)) s++;
	if (*s=='*') {                 /* jede Stunde */
	  hour |= 0xffffff;            /* alles 1er   */
	  s++;
	} else if (isdigit(*s)) {      /* 8-12 oder 1,5 */
	  hour1=strtol(s,&s,10);
	  while (isblank(*s)) s++;
	  if (*s=='-') hour2=strtol(s+1,&s,10);
	  else hour2=hour1+1;
	  if (hour1<0 || hour1>24) {
	    warning ("invalid hour %d", hour1);
	    hour=0;
	    break;
	  }
	  if (hour2<0 || hour2>24) {
	    warning ("invalid hour %d", hour2);
	    hour=0;
	    break;
	  }
	  if (hour2>=hour1)
	    for (i=hour1; i<hour2; i++) hour|=(1<<i);
	  else {
	    for (i=hour1; i<24; i++) hour|=(1<<i);
	    for (i=0; i<hour2; i++)  hour|=(1<<i);
	  }
	} else {
	  warning ("invalid hour '%c'", *s);
	  hour=0;
	  break;
	}
	while (isblank(*s)) s++;
	if (*s==',') {
	  s++;
	continue;
      }
	break;
      }

      if (!hour)
	break;

      if (*s!='=') {
	warning ("expected '=', got '%s'!", s);
	hour=0;
      }

      zp=zones;
      while (zp) {
	z=pop(&zp);
	t=Provider[prefix].Zone[z].nHour++;
	Provider[prefix].Zone[z].Hour = realloc(Provider[prefix].Zone[z].Hour, (t+1)*sizeof(HOUR));
	Provider[prefix].Zone[z].Hour[t].Name=NULL;
	Provider[prefix].Zone[z].Hour[t].Day=day;
	Provider[prefix].Zone[z].Hour[t].Hour=hour;
	Provider[prefix].Zone[z].Hour[t].nUnit=0;
	Provider[prefix].Zone[z].Hour[t].Unit=NULL;
      }

      s++;
      while (1) {
	while (isblank(*s)) s++;
	if (!isdigit(*s)) {
	  warning ("invalid price '%c'", *s);
	  break;
	}
	price=strtod(s,&s);
	while (isblank(*s)) s++;
	divider=1;
	duration=1;
	delay=UNKNOWN;
	if (*s=='(') {
	  s++; while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning ("invalid divider '%c'", *s);
	    break;
	  }
	  divider=strtod(s,&s);
	  while (isblank(*s)) s++;
	  if (*s!=')') {
	    warning ("expected ')', got '%s'!", s);
	    break;
	  }
	  s++; while (isblank(*s)) s++;
	}
	if (*s=='/') {
	  s++; while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning ("invalid duration '%c'", *s);
	    break;
	  }
	  duration=strtod(s,&s);
	  while (isblank(*s)) s++;
	}
	if (*s==':') {
	  s++; while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning ("invalid delay '%c'", *s);
	    break;
	  }
	  delay=strtol(s,&s,10);
	  while (isblank(*s)) s++;
	}
	if (*s==',' && delay==UNKNOWN)
	  delay=duration;
	if (*s!=',' && delay!=UNKNOWN) {
	  warning("last rate must not have a delay, will be ignored!");
	  delay=UNKNOWN;
	}
	zp=zones;
	while (zp) {
	  z=pop(&zp);
	  t=Provider[prefix].Zone[z].nHour-1;
	  u=Provider[prefix].Zone[z].Hour[t].nUnit++;
	  Provider[prefix].Zone[z].Hour[t].Unit=realloc(Provider[prefix].Zone[z].Hour[t].Unit, (u+1)*sizeof(UNIT));
	  Provider[prefix].Zone[z].Hour[t].Unit[u].Duration=duration;
	  Provider[prefix].Zone[z].Hour[t].Unit[u].Delay=delay;
	  Provider[prefix].Zone[z].Hour[t].Unit[u].Price=price/divider;
	}
	nHour++;
	if (*s==',') {
	s++;
	  continue;
	}
	break;
      }
      while (isblank(*s)) s++;
      zp=zones;
      while (zp) {
	z=pop(&zp);
	t=Provider[prefix].Zone[z].nHour-1;
	Provider[prefix].Zone[z].Hour[t].Name=*s?strdup(s):NULL;
      }
      break;

    case 'V': /* V:xxx Version der Datenbank */
      strcpy(Version, s+2);
      break;

    default:
      warning ("Unknown tag '%c'", *s);
      break;
    }
  }
  fclose(stream);
  empty(&zones);

  if (msg) snprintf (message, LENGTH, "Rate Version %s loaded [%d Providers, %d Zones, %d Rates from %s]",
		     Version, Providers, nZone, nHour, dat);

  return 0;
}

char *getProvidername (int prefix)
{
  if (prefix<0 || prefix>=nProvider || !Provider[prefix].used) {
    return NULL;
  }
  return Provider[prefix].Name;
}

int getZone (int prefix, char *number)
{
  int a,z;

  if (prefix<0 || prefix>=nProvider || !Provider[prefix].used) {
    return UNKNOWN;
  }

  for (z=0; z<Provider[prefix].nZone; z++) {
    if (!Provider[prefix].Zone[z].used)
      continue;
    for (a=0; a<Provider[prefix].Zone[z].nArea; a++) {
      if (strmatch(Provider[prefix].Zone[z].Area[a], number))
	return z;
    }
  }
  return UNKNOWN;
}

int getRate(RATE *Rate, char **msg)
{
  static char message[LENGTH];
  bitfield hourBits;
  ZONE  *Zone;
  HOUR  *Hour;
  UNIT  *Unit;
  int    prefix, zone, hour, i;
  time_t now, run, end;
  struct tm tm;

  if (msg)
    *(*msg=message)='\0';

  if (!Rate)
    return UNKNOWN;

  prefix=Rate->prefix;
  if (prefix<0 || prefix>=nProvider || !Provider[prefix].used) {
    if (msg) snprintf(message, LENGTH, "unknown provider %d", prefix);
    return UNKNOWN;
  }

  zone=Rate->zone;
  if (zone<1 || zone>=Provider[prefix].nZone || !Provider[prefix].Zone[zone].used) {
    if (msg) snprintf(message, LENGTH, "unknown zone %d", zone);
    return UNKNOWN;
  }

  Zone=&Provider[prefix].Zone[zone];
  Rate->Provider=Provider[prefix].Name;
  Rate->Zone=Zone->Name;
  Rate->Basic=0;
  Rate->Price=0;
  Rate->Duration=0;
  Rate->Units=0;
  Rate->Charge=0;
  Rate->Rest=0;

  Hour=NULL;
  Unit=NULL;
  hour=UNKNOWN; /* Stundenwechsel erzwingen */
  now=Rate->start;
  end=Rate->now;
  Rate->Time=end-now;
  run=0;
  while (end>=now) {
    tm=*localtime(&now);
    if (hour!=tm.tm_hour) { /* Neuberechnung bei Stundenwechsel */
      hour=tm.tm_hour;
      hourBits=1<<tm.tm_hour;
      for (i=0; i<Zone->nHour; i++) {
	Hour = &Zone->Hour[i];
	if ((Hour->Hour & hourBits) && isDay(&tm, Hour->Day, &Rate->Day))
	  break;
      }
      if (i==Zone->nHour) {
	if (msg) snprintf(message, LENGTH,
			  "no rate found for provider=%d zone=%d day=%d hour=%d",
			  prefix, zone, tm.tm_wday+1, tm.tm_hour);
	return UNKNOWN;
      }
      Rate->Hour=Hour->Name;
      Unit=Hour->Unit;
    }

    Rate->Price=Unit->Price;
    Rate->Duration=Unit->Duration;
    Rate->Charge+=Unit->Price;
    Rate->Rest+=Unit->Duration;
    now+=Unit->Duration;
    run+=Unit->Duration;
    if (run==0 && Unit->Duration==0)
      Rate->Basic=Unit->Price;
    if (Unit->Duration>0)
      Rate->Units++;
    if (Unit->Delay!=UNKNOWN && Unit->Delay<=run)
      Unit++;
}

  Rate->Rest-=Rate->Time;
  return 0;
}

int getLeastCost (RATE *Rate, int skip)
{
  int i, min;
  RATE Curr, Least;

  Least=*Rate;
  Least.Charge=1e9;
  if (Least.start==Least.now)
    Least.now+=181;
  Curr=Least;
  min=UNKNOWN;

  for (i=0; i<nProvider; i++) {
    if (i==skip)
      continue;
    Curr.prefix=i;
    if (getRate(&Curr, NULL)!=UNKNOWN && Curr.Charge<Least.Charge) {
      min=i;
      Least=Curr;
  }
  }

  if (Least.prefix==Rate->prefix)
    return UNKNOWN;

  *Rate=Least;
  return min;
}

int guessZone (RATE *Rate, int units)
{
#if 0
  px="";
  err=60*60*24*365; /* sehr gross */
  for (c = 1; c < 31; c++) {
    call[chan].zone=c;
    tack = (-n -1) * (double)taktlaenge(chan, why);
    if ((tack > 0) && (abs(tack - tx)<err)) {
      call[chan].tick = tack;
      err = abs(tack) - tx;
      px = z2s(c);
    }
  }
  call[chan].zone=-1;
#else
  return UNKNOWN;
#endif
}

char *explainRate (RATE *Rate)
{
  char        p[BUFSIZ], z[BUFSIZ], d[BUFSIZ]="", h[BUFSIZ]="";
  static char r[BUFSIZ];

  if (Rate->Provider && *Rate->Provider)
    strncpy (p, Rate->Provider, BUFSIZ);
  else
    snprintf (p, BUFSIZ, "010%02d", Rate->prefix);

  if (Rate->Zone && *Rate->Zone)
    snprintf (z, BUFSIZ, ", %s", Rate->Zone);
  else
    snprintf (z, BUFSIZ, ", Zone %d", Rate->zone);

  if (Rate->Day && *Rate->Day)
    snprintf (d, BUFSIZ, ", %s", Rate->Day);

  if (Rate->Hour && *Rate->Hour)
    snprintf (h, BUFSIZ, ", %s", Rate->Hour);

  snprintf (r, BUFSIZ, "%s%s%s%s", p, z, d, h);
  return r;
}

#ifdef STANDALONE
void main (int argc, char *argv[])
{
  int z;
  char *msg;
  struct tm now;

  RATE Rate, LCR;

  initHoliday ("../holiday-at.dat", &msg);
  printf ("%s\n", msg);

  initRate ("/etc/isdn/rate.conf", "../rate-at.dat", &msg);
  printf ("%s\n", msg);

  Rate.prefix = 1;
  Rate.zone = 1;

  if (argc>1) {
    z=getZone(01, argv[1]);
    printf("number <%s> is Zone <%d>\n", argv[1], z);
    exit (0);
  }

  time(&Rate.start);
  while (1) {
    time(&Rate.now);
    if (getRate(&Rate, &msg)==UNKNOWN)
      printf ("Ooops: %s\n", msg);
    else {
      now=*localtime(&Rate.now);
      printf ("%02d.%02d.%04d %02d:%02d:%02d  %s %6.2f %7.3f %3d %6.2f %3ld %3ld\n",
	      now.tm_mday, now.tm_mon+1, now.tm_year+1900,
	      now.tm_hour, now.tm_min, now.tm_sec,
	      explainRate(&Rate),
	      Rate.Duration, Rate.Price, Rate.Units, Rate.Charge, Rate.Time, Rate.Rest);

      LCR=Rate;
      if (getLeastCost(&LCR,-1)!=UNKNOWN) {
      printf ("least cost would be: %s %6.2f %7.3f %3d %6.2f %3ld %3ld\n",
	      explainRate(&LCR),
	      LCR.Duration, LCR.Price, LCR.Units, LCR.Charge, LCR.Time, LCR.Rest);
    }
    }
    sleep(1);
  }
}
#endif
