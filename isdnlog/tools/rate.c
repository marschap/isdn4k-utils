/* $Id: rate.c,v 1.15 1999/05/10 20:37:42 akool Exp $
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
 * Revision 1.15  1999/05/10 20:37:42  akool
 * isdnlog Version 3.26
 *
 *  - fixed the "0800" -> free of charge problem
 *  - *many* additions to "ausland.dat"
 *  - first relase of "rate-de.dat" from the CVS-Server of the I4L-Tarif-Crew
 *
 * Revision 1.14  1999/05/09 18:24:24  akool
 * isdnlog Version 3.25
 *
 *  - README: isdnconf: new features explained
 *  - rate-de.dat: many new rates from the I4L-Tarifdatenbank-Crew
 *  - added the ability to directly enter a country-name into "rate-xx.dat"
 *
 * Revision 1.13  1999/05/04 19:33:41  akool
 * isdnlog Version 3.24
 *
 *  - fully removed "sondernummern.c"
 *  - removed "gcc -Wall" warnings in ASN.1 Parser
 *  - many new entries for "rate-de.dat"
 *  - better "isdnconf" utility
 *
 * Revision 1.12  1999/04/30 19:08:08  akool
 * isdnlog Version 3.23
 *
 *  - changed LCR probing duration from 181 seconds to 153 seconds
 *  - "rate-de.dat" filled with May, 1. rates
 *
 * Revision 1.11  1999/04/29 19:03:56  akool
 * isdnlog Version 3.22
 *
 *  - T-Online corrected
 *  - more online rates for rate-at.dat (Thanks to Leopold Toetsch <lt@toetsch.at>)
 *
 * Revision 1.10  1999/04/26 22:12:34  akool
 * isdnlog Version 3.21
 *
 *  - CVS headers added to the asn* files
 *  - repaired the "4.CI" message directly on CONNECT
 *  - HANGUP message extended (CI's and EH's shown)
 *  - reactivated the OVERLOAD message
 *  - rate-at.dat extended
 *  - fixes from Michael Reinelt
 *
 * Revision 1.9  1999/04/20 20:32:03  akool
 * isdnlog Version 3.19
 *   patches from Michael Reinelt
 *
 * Revision 1.8  1999/04/19 19:25:36  akool
 * isdnlog Version 3.18
 *
 * - countries-at.dat added
 * - spelling corrections in "countries-de.dat" and "countries-us.dat"
 * - LCR-function of isdnconf now accepts a duration (isdnconf -c .,duration)
 * - "rate-at.dat" and "rate-de.dat" extended/fixed
 * - holiday.c and rate.c fixed (many thanks to reinelt@eunet.at)
 *
 * Revision 1.7  1999/04/16 14:40:03  akool
 * isdnlog Version 3.16
 *
 * - more syntax checks for "rate-xx.dat"
 * - isdnrep fixed
 *
 * Revision 1.6  1999/04/15 19:15:17  akool
 * isdnlog Version 3.15
 *
 * - reenable the least-cost-router functions of "isdnconf"
 *   try "isdnconf -c <areacode>" or even "isdnconf -c ."
 * - README: "rate-xx.dat" documented
 * - small fixes in processor.c and rate.c
 * - "rate-de.dat" optimized
 * - splitted countries.dat into countries-de.dat and countries-us.dat
 *
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
extern const char *basename (const char *name);
#else
#include "isdnlog.h"
#include "tools.h"
#endif
#include "holiday.h"
#include "rate.h"

#define LENGTH 250  /* max length of lines in data file */
#ifdef STANDALONE
#define TESTDURATION 153
#define MAXPROVIDER 1000
#define UNKNOWN -1
#endif

#define  WMAX    64
#define  P        1
#define  Q        1
#define  R        1
#define  DISTANCE 2

typedef struct _STACK {
  int data;
  struct _STACK *next;
} STACK;

typedef struct {
  double Duration;
  double Delay;
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
  char *Code;
  int   Zone;
} AREA;

typedef struct {
  int    used;
  char  *Name;
  int    nHour;
  HOUR  *Hour;
} ZONE;

typedef struct {
  int    booked;
  int    used;
  char  *Name;
  int    nZone;
  ZONE  *Zone;
  int    nArea;
  AREA  *Area;
} PROVIDER;

typedef struct {
  char *prefix;
  char *name;
  char *match;
  char *hints;
} COUNTRY;

static PROVIDER *Provider=NULL;
static COUNTRY *Country = (COUNTRY *)NULL;
static int      nProvider=0;
static int      line=0;
static int      nCountry = 0;

static void warning (char *file, char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  va_start (ap, fmt);
  vsnprintf (msg, BUFSIZ, fmt, ap);
  va_end (ap);
#ifdef STANDALONE
  fprintf(stderr, "WARNING: %s line %3d: %s\n", basename(file), line, msg);
#else
  print_msg(PRT_NORMAL, "WARNING: %s line %3d: %s\n", basename(file), line, msg);
#endif
}


static void down(register char *p)
{
  while (*p) {
    *p = tolower(*p);

    if ((*p < 'a') || (*p > 'z'))
      *p = ' ';

    p++;
  } /* while */
} /* down */


static void initCountry(char *fn)
{
  register char *p1, *p2, *p3;
  auto     char  s[BUFSIZ];
  auto     FILE *f;


  if ((f = fopen(fn, "r")) != (FILE *)NULL) {
    while (fgets(s, BUFSIZ, f)) {
      if ((p1 = strchr(s, '#')))
        *p1 = 0;

      if (*s && (p1 = strchr(s, ':'))) {
        *p1 = 0;

        if ((p2 = strchr(p1 + 1, ':')))
          *p2 = 0;

        if ((p3 = strchr(((p2 == NULL) ? p1 + 1 : p2 + 1), '\n')))
          *p3 = 0;

        Country = realloc(Country, (nCountry + 1) * sizeof(COUNTRY));
        Country[nCountry].prefix = strdup(s);
        Country[nCountry].name = strdup(p1 + 1);
        Country[nCountry].match = strdup(p1 + 1);
        down(Country[nCountry].match);

        if (p2 != NULL) {
	  Country[nCountry].hints = strdup(p2 + 1);
          down(Country[nCountry].hints);
        }
        else
	  Country[nCountry].hints = strdup("");

        nCountry++;
      } /* if */
    } /* while */

    fclose(f);

  } /* if */
} /* initCountry */


static int min3(register int x, register int y, register int z)
{
  if (x < y)
    y = x;
  if (y < z)
    z = y;

  return(z);
} /* min */


static int wld(register char *nadel, register char *heuhaufen) /* weighted Levenshtein distance */
{
  register int i, j;
  auto     int l1 = strlen(nadel);
  auto     int l2 = strlen(heuhaufen);
  auto     int dw[WMAX + 1][WMAX + 1];


  dw[0][0] = 0;

  for (j = 1; j <= WMAX; j++)
    dw[0][j] = dw[0][j - 1] + Q;

  for (i = 1; i <= WMAX; i++)
    dw[i][0] = dw[i - 1][0] + R;

  for (i = 1; i <= l1; i++)
    for (j = 1; j <= l2; j++)
      dw[i][j] = min3(dw[i - 1][j - 1] + ((nadel[i - 1] == heuhaufen[j - 1]) ? 0 : P), dw[i][j - 1] + Q, dw[i - 1][j] + R);

  return(dw[l1][l2]);
} /* wld */


static int countrymatch(char *name, char *num)
{
  register int 	 i, test = (num == NULL);
  auto	   char	 k[BUFSIZ];


  strcpy(k, name);
  down(k);

  for (i = 0; i < nCountry; i++)
    if (strstr(Country[i].match, k) && (test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))))
      return(nCountry);

  for (i = 0; i < nCountry; i++)
    if (strstr(Country[i].hints, k) && (test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))))
      return(nCountry);

  for (i = 0; i < nCountry; i++)
    if (strstr(k, Country[i].hints) && (test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))))
      return(nCountry);

  for (i = 0; i < nCountry; i++)
    if ((wld(k, Country[i].match) <= DISTANCE) && (test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))))
      return(nCountry);

  return(0);
} /* countymatch */


/*  INPUT: "+372"
   OUTPUT: "Estland"

    INPUT: "Estland"
   OUTPUT: "+372"
*/

int abroad(char *key, char *result)
{
  register int   i;
  auto int mode, match = 0, res = 0;
  auto 	   char  k[BUFSIZ];


  *result = 0;

  if (!memcmp(key, countryprefix, strlen(countryprefix)))  /* +xxx */
    mode = 1;
  else {   	   		  			   /* "Estland" */
    mode = 2;

    strcpy(k, key);
    down(k);
  } /* else */

  for (i = 0; i < nCountry; i++) {
    if (mode == 1) {
      res = strlen(Country[i].prefix);
      match = !strncmp(Country[i].prefix, key, res);
    }
    else {
      res = 1;
      match = (strstr(Country[i].match, k) != NULL);

      if (!match)
        match = (strstr(Country[i].hints, k) != NULL);

      if (!match)
        match = (strstr(k, Country[i].hints) != NULL);

      if (!match)
        match = (wld(k, Country[i].match) <= DISTANCE);

    } /* else */

    if (match) {
      if (mode == 1)
        strcpy(result, Country[i].name);
      else
        strcpy(result, Country[i].prefix);

      return(res);
    } /* if */
  } /* for */

  return(0);
} /* abroad */


static char *strip (char *s)
{
  char *p;

  while (isblank(*s)) s++;
  for (p=s; *p; p++)
    if (*p=='#' || *p=='\n') {
      *p='\0';
      break;
    }
  for (p--; p>s && isblank(*p); p--)
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
  int l, length=0;
  while (*pattern) {
    if (*pattern=='*') {
      pattern+=strlen(pattern)-1;
      l=strlen(string);
      string+=l-1;
      length+=l;
      while (*pattern!='*') {
	if (*pattern!=*string && *pattern!='?')
	  return 0;
	pattern--;
	string--;
      }
      return length;
    }
    if ((*pattern!=*string && *pattern!='?') || *string=='\0')
      return 0;
    pattern++;
    string++;
    length++;
  }
  return length;
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

static int byArea(const void *a, const void *b)
{
  return strcmp (((AREA*)a)->Code, ((AREA*)b)->Code);
}

void exitRate(void)
{
  int i, j, k;

  for (i=0; i<nProvider; i++) {
    if (Provider[i].used) {
      for (j=0; j<Provider[i].nZone; j++) {
	if (Provider[i].Zone[j].used) {
	  Provider[i].Zone[j].used=0;
	  if (Provider[i].Zone[j].Name) free (Provider[i].Zone[j].Name);
	  for (k=0; k<Provider[i].Zone[j].nHour; k++) {
	    if (Provider[i].Zone[j].Hour[k].Name) free (Provider[i].Zone[j].Hour[k].Name);
	    if (Provider[i].Zone[j].Hour[k].Unit) free (Provider[i].Zone[j].Hour[k].Unit);
	  }
	  if (Provider[i].Zone[j].Hour) free (Provider[i].Zone[j].Hour);
	}
      }
      if(Provider[i].Zone) free (Provider[i].Zone);
      for (j=0; j<Provider[i].nArea; j++)
	if (Provider[i].Area[j].Code) free (Provider[i].Area[j].Code);
      if(Provider[i].Area) free (Provider[i].Area);
      if (Provider[i].Name) free (Provider[i].Name);
      Provider[i].used=0;
    }
  }
}

int initRate(char *conf, char *dat, char **msg)
{
  static char message[LENGTH];
  FILE    *stream;
  STACK   *zones=NULL, *zp;
  bitfield day, hour;
  double   price, divider, duration;
  char     buffer[LENGTH], Version[LENGTH]="<unknown>";
  char    *a, *c, *s;
  int      booked[MAXPROVIDER], variant[MAXPROVIDER];
  int      Providers=0, Areas=0, Zones=0, Hours=0;
  int      ignore=0, prefix=UNKNOWN;
  int      zone1, zone2, day1, day2, hour1, hour2, delay;
  int      i, t, u, v, z;

  if (msg)
    *(*msg=message)='\0';

  for (i=0; i<MAXPROVIDER; i++) {
    booked[i]=0;
    variant[i]=UNKNOWN;
  }

  initCountry("/usr/lib/isdn/ausland.dat");

  if (conf && *conf) {
    if ((stream=fopen(conf,"r"))==NULL) {
      if (msg) snprintf (message, LENGTH, "Error: could not load rate configuration from %s: %s",
			 conf, strerror(errno));
      return -1;
    }
    line=0;
    while ((s=fgets(buffer,LENGTH,stream))!=NULL) {
      line++;
      if (*(s=strip(s))=='\0')
	continue;
      if (s[1]!=':') {
	warning (conf, "expected ':', got '%s'!", s+1);;
	continue;
      }
      switch (*s) {
      case 'P':
	s+=2;
	while (isblank(*s)) s++;
	if (!isdigit(*s)) {
	  warning (conf, "Invalid provider-number %s", s);
	  continue;
	}
	prefix = strtol(s, &s ,10);
	if (prefix >= MAXPROVIDER) {
	  warning (conf, "Invalid provider-number %d", prefix);
	  continue;
	}
	booked[prefix]=1;
	while (isblank(*s)) s++;
	if (*s == '=') {
	  s++;
	  while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning (conf, "Invalid variant %s", s);
	    continue;
	  }
	  if ((v=strtol(s, &s, 10))<1) {
	    warning (conf, "Invalid variant %s", s);
	    continue;
	  }
	  variant[prefix]=v;
	  while (isblank(*s)) s++;
	}
	if (*s) {
	  warning (conf, "trailing junk '%s' ignored.", s);
	}
	break;

      default:
	warning(conf, "Unknown tag '%c'", *s);
      }
    }
    fclose (stream);
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
      warning (dat, "expected ':', got '%s'!", s+1);;
      continue;
    }

    switch (*s) {

    case 'P': /* P:nn[,v] Bezeichnung */
      v = UNKNOWN;
      ignore = 1;
      empty(&zones);

      s+=2; while (isblank(*s)) s++;
      if (!isdigit(*s)) {
	warning (dat, "Invalid provider-number '%c'", *s);
	continue;
      }
      prefix = strtol(s, &s ,10);
      if (prefix >= MAXPROVIDER) {
	warning (dat, "Invalid provider-number %d", prefix);
	continue;
      }
      while (isblank(*s)) s++;
      if (*s == ',') {
	s++; while (isblank(*s)) s++;
	if (!isdigit(*s)) {
	  warning (dat, "Invalid variant '%c'", *s);
	  continue;
	}
	v=strtol(s, &s, 10);
      }
      /* Fixme: is this correct? */
      /* if ((variant[prefix]!=UNKNOWN) && (v!=UNKNOWN) && (variant[prefix]!=v)) { */
      if (variant[prefix]==v) {
	ignore = 0;
      } else {
	v = UNKNOWN;
	continue;
      }
      if (prefix>=nProvider) {
	Provider=realloc(Provider, (prefix+1)*sizeof(PROVIDER));
	for (i=nProvider; i<=prefix; i++)
	  Provider[i].used=0;
	nProvider=prefix+1;
      }
      if (Provider[prefix].used++) {
	warning (dat, "Duplicate entry for provider %d (%s)", prefix, Provider[prefix].Name);
	if (Provider[prefix].Name) free(Provider[prefix].Name);
      }
      while (isblank(*s)) s++;
      Provider[prefix].Name=*s?strdup(s):NULL;
      Provider[prefix].booked=booked[prefix];
      Provider[prefix].nZone=0;
      Provider[prefix].Zone=NULL;
      Provider[prefix].nArea=0;
      Provider[prefix].Area=NULL;
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
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      empty(&zones);
      while (1) {
	while (isblank(*s)) s++;
	if (!isdigit(*s)) {
	  warning (dat, "Invalid zone '%c'", *s);
	  empty(&zones);
	  break;
	}
	zone1=strtol(s,&s,10);
	while (isblank(*s)) s++;
	if (*s=='-') {
	  s++; while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning (dat, "Invalid zone '%c'", *s);
	    empty(&zones);
	    break;
	  }
	  zone2=strtol(s,&s,10);
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
	Provider[prefix].Zone[z].nHour=0;
	Provider[prefix].Zone[z].Hour=NULL;
	Zones++;
      }
      break;

    case 'A': /* A:areacode[,areacode...] */
      if (ignore) continue;
      if (!zones) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      a=s;
      while(1) {
	if (*(c=strip(str2set(&a)))) {
	  for (i=0; i<Provider[prefix].nArea; i++) {
	    if (strcmp (Provider[prefix].Area[i].Code,c)==0) {
	      warning (dat, "Duplicate Area %s", c);
	      c=NULL;
	      break;
	    }
	  }
	  if (c) {
	    Provider[prefix].Area=realloc(Provider[prefix].Area, (Provider[prefix].nArea+1)*sizeof(AREA));

            if (isalpha(*c) && !countrymatch(c, NULL))
	      warning(dat, "Unknown country \"%s\"", c);

	    Provider[prefix].Area[Provider[prefix].nArea].Code=strdup(c);
	    Provider[prefix].Area[Provider[prefix].nArea].Zone=zones->data; /* ugly: use first zone */
	    Provider[prefix].nArea++;
	    Areas++;
	  }
	} else {
	  warning (dat, "Ignoring empty areacode");
	}
	if (*a==',') {
	  a++;
	  continue;
	}
	break;
      }
      s=a;
      break;

    case 'T':  /* T:d-d/h-h=p/s:t[=]Bezeichnung */
      if (ignore) continue;
      if (!zones) {
	warning (dat, "Unexpected tag '%c'", *s);
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
	      warning (dat, "invalid day '%s'", s);
	      day=0;
	      break;
	    }
	    day2=strtol(s,&s,10);
	  } else day2=day1;
	  if (day1<1 || day1>7) {
	    warning (dat, "invalid day %d", day1);
	    day=0;
	    break;
	  }
	  if (day2<1 || day2>7) {
	    warning (dat, "invalid day %d", day2);
	    day=0;
	    break;
	  }
	  if (day2<day1) {
	    i=day2; day2=day1; day1=i;
	  }
	  for (i=day1; i<=day2; i++)
	    day|=(1<<i);
	} else {
	  warning (dat, "invalid day '%c'", *s);
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
	warning (dat, "expected '/', got '%s'!", s);
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
	    warning (dat, "invalid hour %d", hour1);
	    hour=0;
	    break;
	  }
	  if (hour2<0 || hour2>24) {
	    warning (dat, "invalid hour %d", hour2);
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
	  warning (dat, "invalid hour '%c'", *s);
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
	warning (dat, "expected '=', got '%s'!", s);
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
	  warning (dat, "invalid price '%c'", *s);
	  break;
	}
	price=strtod(s,&s);
	while (isblank(*s)) s++;
	divider=0.0;
	duration=1.0;
	if (*s=='(') {
	  s++; while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning (dat, "invalid divider '%c'", *s);
	    break;
	  }
	  divider=strtod(s,&s);
	  while (isblank(*s)) s++;
	  if (*s!=')') {
	    warning (dat, "expected ')', got '%s'!", s);
	    break;
	  }
	  s++; while (isblank(*s)) s++;
	}
	while (1) {
	  if (*s=='/') {
	    s++; while (isblank(*s)) s++;
	    if (!isdigit(*s)) {
	      warning (dat, "invalid duration '%c'", *s);
	      break;
	    }
	    duration=strtod(s,&s);
	    while (isblank(*s)) s++;
	  }
	  if (*s==':') {
	    s++; while (isblank(*s)) s++;
	    if (!isdigit(*s)) {
	      warning (dat, "invalid delay '%c'", *s);
	      break;
	    }
	    delay=strtol(s,&s,10);
	    while (isblank(*s)) s++;
	  } else {
	    delay=UNKNOWN;
	  }
	  if ((*s==',' || *s=='/') && delay==UNKNOWN)
	    delay=duration;
	  if (*s!=',' && *s!='/' && delay!=UNKNOWN) {
	    warning(dat, "last rate must not have a delay, will be ignored!");
	    delay=UNKNOWN;
	  }
	  if (duration==0.0 && delay!=0 && delay != UNKNOWN) {
	    warning(dat, "zero duration must not have a delay, duration set to %d!", delay);
	    duration=delay;
	  }
	  zp=zones;
	  while (zp) {
	    z=pop(&zp);
	    t=Provider[prefix].Zone[z].nHour-1;
	    u=Provider[prefix].Zone[z].Hour[t].nUnit++;
	    Provider[prefix].Zone[z].Hour[t].Unit=realloc(Provider[prefix].Zone[z].Hour[t].Unit, (u+1)*sizeof(UNIT));
	    Provider[prefix].Zone[z].Hour[t].Unit[u].Duration=duration;
	    Provider[prefix].Zone[z].Hour[t].Unit[u].Delay=delay;
	    if (duration!=0.0 && divider!=0.0)
	      Provider[prefix].Zone[z].Hour[t].Unit[u].Price=price*duration/divider;
	    else
	      Provider[prefix].Zone[z].Hour[t].Unit[u].Price=price;
	  }
	  if (*s=='/') {
	    continue;
	  }
	Hours++;
	  break;
	}
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
	Hours++;
      }
      break;

    case 'V': /* V:xxx Version der Datenbank */
      strcpy(Version, s+2);
      break;

    default:
      warning (dat, "Unknown tag '%c'", *s);
      break;
    }
  }
  fclose(stream);
  empty(&zones);

  for (i=0; i<nProvider; i++) {
    if (Provider[i].used) {
      qsort(Provider[i].Area, Provider[i].nArea, sizeof(AREA), byArea);
    }
  }

  if (msg) snprintf (message, LENGTH, "Rate Version %s loaded [%d Providers, %d Zones, %d Areas, %d Rates from %s]",
		     Version, Providers, Zones, Areas, Hours, dat);

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
  int a, l, m, max, z;

  if (prefix<0 || prefix>=nProvider || !Provider[prefix].used) {
    return UNKNOWN;
  }

  l=0;
  max=0;
  z=UNKNOWN;
  for (a=0; a<Provider[prefix].nArea; a++) {

    if (isalpha(*Provider[prefix].Area[a].Code)) {
      if (countrymatch(Provider[prefix].Area[a].Code, number))
        return(Provider[prefix].Area[a].Zone);
    }
    else
    {
    m=strmatch(Provider[prefix].Area[a].Code, number);
    if (m>max) {
      z=Provider[prefix].Area[a].Zone;
      max=m;
    }
    l=m;
    } /* else */
  }
  return z;
}

int getRate(RATE *Rate, char **msg)
{
  static char message[LENGTH];
  bitfield hourBits;
  ZONE  *Zone;
  HOUR  *Hour;
  UNIT  *Unit;
  int    prefix, zone, hour, i;
  double now, run, end;
  struct tm tm;
  time_t time;

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
  run=0.0;
  while (end>=now) {
    time=now;
    tm=*localtime(&time);
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

    Rate->Charge+=Unit->Price;
    Rate->Duration=Unit->Duration;
    Rate->Rest+=Unit->Duration;
    now+=Unit->Duration;
    run+=Unit->Duration;
    if (run==0.0 && Unit->Duration==0.0)
      Rate->Basic=Unit->Price;
    else
      Rate->Price=Unit->Price;
    if (Unit->Duration>0.0)
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
    Least.now+=TESTDURATION;
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

  initRate ("/etc/isdn/rate.conf", "../auskunft.dat", &msg);
  printf ("%s\n", msg);

  Rate.prefix = 1;
  Rate.zone = 1;

  if (argc==2) {
    z=getZone(01, argv[1]);
    printf("number <%s> is Zone <%d>\n", argv[1], z);
    exit (0);
  }
  if (argc==3) {
    printf ("strmatch(%s, %s)=%d\n", argv[1], argv[2], strmatch(argv[1], argv[2]));
    exit (0);
  }

  time(&Rate.start);
  time(&Rate.now);
  if (getRate(&Rate, &msg)==UNKNOWN) {
    printf ("Ooops: %s\n", msg);
    exit (1);
  }

  printf ("%s\n\n", explainRate(&Rate));
  printf ("---Date--- --Time--  --Charge-- ( Basic  Price)  Unit   Dur  Time  Rest\n");

  while (1) {
    time(&Rate.now);
    if (getRate(&Rate, &msg)==UNKNOWN) {
      printf ("Ooops: %s\n", msg);
      exit (1);
    }
    now=*localtime(&Rate.now);
    printf ("%02d.%02d.%04d %02d:%02d:%02d  ATS %6.3f (%6.3f %6.3f)  %4d  %4.1f  %4ld  %4ld\n",
	    now.tm_mday, now.tm_mon+1, now.tm_year+1900,
	    now.tm_hour, now.tm_min, now.tm_sec,
	    Rate.Charge, Rate.Basic, Rate.Price, Rate.Units, Rate.Duration, Rate.Time, Rate.Rest);

    LCR=Rate;
#if 0
    if (getLeastCost(&LCR,-1)!=UNKNOWN) {
      printf ("least cost would be: %s %6.2f %7.3f %3d %6.2f %3ld %3ld\n",
	      explainRate(&LCR),
	      LCR.Duration, LCR.Price, LCR.Units, LCR.Charge, LCR.Time, LCR.Rest);
    }
#endif
    sleep(1);
  }
}
#endif
