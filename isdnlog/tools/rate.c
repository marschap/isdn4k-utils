/* $Id: rate.c,v 1.37 1999/07/26 16:28:49 akool Exp $
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
 * Revision 1.37  1999/07/26 16:28:49  akool
 * getRate() speedup from Leo
 *
 * Revision 1.36  1999/07/18 08:41:19  akool
 * fix from Michael
 *
 * Revision 1.35  1999/07/15 16:42:10  akool
 * small enhancement's and fixes
 *
 * Revision 1.34  1999/07/12 18:50:06  akool
 * replace "0" by "+49"
 *
 * Revision 1.33  1999/07/03 10:24:18  akool
 * fixed Makefile
 *
 * Revision 1.32  1999/07/02 19:18:11  akool
 * rate-de.dat V:1.02-Germany [02-Jul-1999 21:27:20]
 *
 * Revision 1.31  1999/07/02 18:21:03  akool
 * rate-de.dat V:1.02-Germany [02-Jul-1999 20:29:21]
 * country-de.dat V:1.02-Germany [02-Jul-1999 19:13:54]
 *
 * Revision 1.30  1999/07/01 20:40:24  akool
 * isdnrate optimized
 *
 * Revision 1.29  1999/06/30 17:18:13  akool
 * isdnlog Version 3.39
 *
 * Revision 1.28  1999/06/29 20:11:43  akool
 * now compiles with ndbm
 * (many thanks to Nima <nima_ghasseminejad@public.uni-hamburg.de>)
 *
 * Revision 1.27  1999/06/28 19:16:49  akool
 * isdnlog Version 3.38
 *   - new utility "isdnrate" started
 *
 * Revision 1.26  1999/06/26 10:12:12  akool
 * isdnlog Version 3.36
 *  - EGCS 1.1.2 bug correction from Nima <nima_ghasseminejad@public.uni-hamburg.de>
 *  - zone-1.11
 *
 * Revision 1.25  1999/06/22 19:41:23  akool
 * zone-1.1 fixes
 *
 * Revision 1.24  1999/06/21 19:34:28  akool
 * isdnlog Version 3.35
 *   zone data for .nl (many thanks to Paul!)
 *
 *   WARNING: This version of isdnlog dont even compile! *EXPERIMENTAL*!!
 *
 * Revision 1.23  1999/06/16 23:37:50  akool
 * fixed zone-processing
 *
 * Revision 1.22  1999/06/16 19:13:00  akool
 * isdnlog Version 3.34
 *   fixed some memory faults
 *
 * Revision 1.21  1999/06/15 20:05:13  akool
 * isdnlog Version 3.33
 *   - big step in using the new zone files
 *   - *This*is*not*a*production*ready*isdnlog*!!
 *   - Maybe the last release before the I4L meeting in Nuernberg
 *
 * Revision 1.20  1999/06/09 19:59:20  akool
 * isdnlog Version 3.31
 *  - Release 0.91 of zone-Database (aka "Verzonungstabelle")
 *  - "rate-de.dat" V:1.02-Germany [09-Jun-1999 21:45:26]
 *
 * Revision 1.19  1999/06/01 19:33:43  akool
 * rate-de.dat V:1.02-Germany [01-Jun-1999 20:52:32]
 *
 * Revision 1.18  1999/05/22 10:19:28  akool
 * isdnlog Version 3.29
 *
 *  - processing of "sonderrufnummern" much more faster
 *  - detection for sonderrufnummern of other provider's implemented
 *    (like 01929:FreeNet)
 *  - Patch from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 *  - Patch from Markus Schoepflin <schoepflin@ginit.de>
 *  - easter computing corrected
 *  - rate-de.dat 1.02-Germany [22-May-1999 11:37:33] (from rate-CVS)
 *  - countries-de.dat 1.02-Germany [22-May-1999 11:37:47] (from rate-CVS)
 *  - new option "-B" added (see README)
 *    (using "isdnlog -B16 ..." isdnlog now works in the Netherlands!)
 *
 * Revision 1.17  1999/05/13 11:40:03  akool
 * isdnlog Version 3.28
 *
 *  - "-u" Option corrected
 *  - "ausland.dat" removed
 *  - "countries-de.dat" fully integrated
 *      you should add the entry
 *      "COUNTRYFILE = /usr/lib/isdn/countries-de.dat"
 *      into section "[ISDNLOG]" of your config file!
 *  - rate-de.dat V:1.02-Germany [13-May-1999 12:26:24]
 *  - countries-de.dat V:1.02-Germany [13-May-1999 12:26:26]
 *
 * Revision 1.16  1999/05/11 20:27:22  akool
 * isdnlog Version 3.27
 *
 *  - country matching fixed (and faster)
 *
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
 * void initRate(char *conf, char *dat, char *dom, char **msg)
 *   initialisiert die Tarifdatenbank
 *
 * char* getProvider (int prefix)
 *   liefert den Namen des Providers oder NULL wenn unbekannt
 *
 * int getArea (int prefix, char *number)
 *   überprüft, ob die Nummer einem A:-Tag entspricht
 *   wird für die Sondernummern benötigt
 *
 * void clearRate (RATE *Rate)
 *   setzt alle Felder von *Rate zurück
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
 * int guessZone (RATE *Rate, int aoc_units)
 *   versucht die Zone zu erraten, wenn mit den Daten in Rate
 *   aoc_units Einheiten gemeldet wurden
 *
 * char *explainRate (RATE *Rate)
 *   liefert eine textuelle Begründung für den Tarif in der Form
 *   "Provider, Zone, Wochentag, Zeit"
 *
 * char *printRate (double value)
 *   liefert eine formatierte Zahl mit Währung gemäß dem U:-Tag
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
#include <errno.h>
#ifndef __GLIBC__
extern const char *basename (const char *name);
#endif
#define mycountry "+43"
#define vbn "010"
#else
#include "isdnlog.h"
#include "tools.h"
#endif
#include "holiday.h"
#include "zone.h"
#include "country.h"
#include "rate.h"

#define LENGTH 250             /* max length of lines in data file */
#define STRINGS 8              /* number of buffers for printRate() */
#define STRINGL 64             /* length of printRate() buffer */
#define DEFAULT_FORMAT "%.2f"  /* default format for printRate() */

#ifdef STANDALONE
#define LCR_DURATION 153
#define MAXPROVIDER 1000
#define UNKNOWN -1
#endif

typedef struct {
  double Duration;
  double Delay;
  double Price;
} UNIT;

typedef struct {
  char     *Name;
  bitfield  Day;
  bitfield  Hour;
  int       Freeze;
  int       nUnit;
  UNIT     *Unit;
} HOUR;

typedef struct {
  char *Code;
  char *Name;
  int   Zone;
} AREA;

typedef struct {
  char *Name;
  int   Zone;
} SERVICE;

typedef struct {
  char  *Name;
  char  *Flag;
  int    nNumber;
  int   *Number;
  int    nHour;
  HOUR  *Hour;
} ZONE;

typedef struct {
  int      booked;
  int      used;
  char    *Name;
  int      nZone;
  ZONE    *Zone;
  int      nArea;
  AREA    *Area;
  int      nService;
  SERVICE *Service;
} PROVIDER;

static char      Format[STRINGL]="";
static PROVIDER *Provider=NULL;
static int      nProvider=0;
static int      line=0;


static void notice (char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  va_start (ap, fmt);
  vsnprintf (msg, BUFSIZ, fmt, ap);
  va_end (ap);
#ifdef STANDALONE
  fprintf(stderr, "%s\n", msg);
#else
  print_msg(PRT_ERR, "%s\n", msg);
#endif
}

static void warning (char *file, char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  va_start (ap, fmt);
  vsnprintf (msg, BUFSIZ, fmt, ap);
  va_end (ap);
  notice ("WARNING: %s line %3d: %s", basename(file), line, msg);
}


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

static char* str2list (char **s)
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

static char* strcat3 (char **s)
{
  static char buffer[BUFSIZ];

  strcpy (buffer, s[0]);
  strcat (buffer, s[1]);
  strcat (buffer, s[2]);

  return buffer;
}

static int appendArea (int prefix, char *code, char *name, int zone, int *domestic, char *msg)
{
  int i;

  for (i=0; i<Provider[prefix].nArea; i++) {
    if (strcmp (Provider[prefix].Area[i].Code,code)==0) {
      if (msg)
	warning (msg, "Duplicate area %s", code);
      return 0;
    }
  }

  if (strcmp(code, mycountry)==0)
    *domestic=1;

  Provider[prefix].Area=realloc(Provider[prefix].Area, (Provider[prefix].nArea+1)*sizeof(AREA));
  Provider[prefix].Area[Provider[prefix].nArea].Code=strdup(code);
  Provider[prefix].Area[Provider[prefix].nArea].Name=name?strdup(name):NULL;
  Provider[prefix].Area[Provider[prefix].nArea].Zone=zone;
  Provider[prefix].nArea++;
  return 1;
}

void exitRate(void)
{
  int i, j, k;

  for (i=0; i<nProvider; i++) {
    if (Provider[i].used) {
      for (j=0; j<Provider[i].nZone; j++) {
	if (Provider[i].Zone[j].Number) free (Provider[i].Zone[j].Number);
	if (Provider[i].Zone[j].Name) free (Provider[i].Zone[j].Name);
	if (Provider[i].Zone[j].Flag) free (Provider[i].Zone[j].Flag);
	for (k=0; k<Provider[i].Zone[j].nHour; k++) {
	  if (Provider[i].Zone[j].Hour[k].Name) free (Provider[i].Zone[j].Hour[k].Name);
	  if (Provider[i].Zone[j].Hour[k].Unit) free (Provider[i].Zone[j].Hour[k].Unit);
	}
	if (Provider[i].Zone[j].Hour) free (Provider[i].Zone[j].Hour);
      }
      if(Provider[i].Zone) free (Provider[i].Zone);
      for (j=0; j<Provider[i].nArea; j++)
	if (Provider[i].Area[j].Code) free (Provider[i].Area[j].Code);
      if (Provider[i].Area[j].Name) free (Provider[i].Area[j].Name);
      if(Provider[i].Area) free (Provider[i].Area);
      for (j=0; j<Provider[i].nService; j++)
	if (Provider[i].Service[j].Name) free (Provider[i].Service[j].Name);
      if(Provider[i].Service) free (Provider[i].Service);
      if (Provider[i].Name) free (Provider[i].Name);
      Provider[i].used=0;
    }
  }
}

int initRate(char *conf, char *dat, char *dom, char **msg)
{
  static char message[LENGTH];
  FILE    *stream;
  COUNTRY *Country;
  bitfield day, hour;
  double   price, divider, duration;
  char     buffer[LENGTH], path[LENGTH], Version[LENGTH]="<unknown>";
  char	   sx[BUFSIZ];
  char     *c, *s;
  int      booked[MAXPROVIDER], variant[MAXPROVIDER];
  int      Providers=0, Areas=0, Services=0, Zones=0, Hours=0;
  int      ignore=0, domestic=0, prefix=UNKNOWN;
  int      zone, zone1, zone2, day1, day2, hour1, hour2, freeze, delay;
  int     *number, numbers;
  int      d, i, n, t, u, v, z;


  if (msg)
    *(*msg=message)='\0';

  for (i=0; i<MAXPROVIDER; i++) {
    booked[i]=0;
    variant[i]=UNKNOWN;
  }

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
  zone=UNKNOWN;
  while ((s=fgets(buffer,LENGTH,stream))!=NULL) {
    line++;
    if (*(s=strip(s))=='\0')
      continue;
    if (s[1]!=':') {
      warning (dat, "expected ':', got '%s'!", s+1);;
      continue;
    }

    switch (*s) {

    case 'U': /* U:Format für printRate() */
      if (*Format) {
	warning (dat, "redefinition of currency format");
      }
      strcpy (Format, strip(s+2));
      break;

    case 'P': /* P:nn[,v] Bezeichnung */
      if (zone!=UNKNOWN && !ignore && !domestic)
	warning (dat, "Provider %d has no default domestic zone (missing 'A:%s')", prefix, mycountry);
      v = UNKNOWN;
      zone = UNKNOWN;
      ignore = 1;
      domestic = 0;

      s+=2; while (isblank(*s)) s++;
      if (!isdigit(*s)) {
	warning (dat, "Invalid provider-number '%c'", *s);
	prefix=UNKNOWN;
	continue;
      }
      prefix = strtol(s, &s ,10);
      if (prefix >= MAXPROVIDER) {
	warning (dat, "Invalid provider-number %d", prefix);
	prefix=UNKNOWN;
	continue;
      }
      while (isblank(*s)) s++;
      if (*s == ',') {
	s++; while (isblank(*s)) s++;
	if (!isdigit(*s)) {
	  warning (dat, "Invalid variant '%c'", *s);
	  prefix=UNKNOWN;
	  continue;
	}
	v=strtol(s, &s, 10);
      }
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
      Provider[prefix].nService=0;
      Provider[prefix].Service=NULL;
      Providers++;
      break;

    case 'G': /* G:tt.mm.jjjj Hour gueltig ab */
      if (ignore) continue;
      break;

    case 'C':  /* C:Comment */
      if (ignore) continue;
      break;

    case 'D':  /* D:Verzonung */
      if (ignore) continue;
      if (prefix == UNKNOWN || zone != UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2; while (isblank(*s)) s++;
      snprintf (path, LENGTH, dom, s);
      if (initZone(prefix, path, &c)==0) {
	if (msg) notice ("%s", c);
      } else {
	warning (dat, c);
      }
      break;

    case 'Z': /* Z:n[-n][,n] Bezeichnung */
      if (ignore) continue;
      if (prefix == UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      number=NULL;
      numbers=0;
      while (1) {
	while (isblank(*s)) s++;
	if (*s=='*') {
	  zone1=zone2=UNKNOWN;
	} else {
	  if (!isdigit(*s) && *s!='*') {
	    warning (dat, "Invalid zone '%c'", *s);
	    numbers=0;
	    break;
	  }
	  zone1=strtol(s,&s,10);
	  while (isblank(*s)) s++;
	  if (*s=='-') {
	    s++; while (isblank(*s)) s++;
	    if (!isdigit(*s)) {
	      warning (dat, "Invalid zone '%c'", *s);
	      numbers=0;
	      break;
	    }
	    zone2=strtol(s,&s,10);
	    if (zone2<zone1) {
	      i=zone2; zone2=zone1; zone1=i;
	    }
	  } else {
	    zone2=zone1;
	  }
	}
	for (i=zone1; i<=zone2; i++) {
	  for (z=0; z<Provider[prefix].nZone; z++)
	    for (n=0; n<Provider[prefix].Zone[z].nNumber; n++)
	      if (Provider[prefix].Zone[z].Number[n]==i) {
		warning (dat, "Duplicate zone %d", i);
		goto skip;
	      }
	  numbers++;
	  number=realloc(number, numbers*sizeof(int));
	  number[numbers-1]=i;
	skip:
	}

	while (isblank(*s)) s++;
	if (*s==',') {
	  s++;
	  continue;
	}
	break;
      }

      if (numbers==0) {
	if (number) {
	  free (number);
	  number=NULL;
	}
	break;
      }

      zone=Provider[prefix].nZone++;
      Provider[prefix].Zone=realloc(Provider[prefix].Zone, Provider[prefix].nZone*sizeof(ZONE));
      Provider[prefix].Zone[zone].Name=*s?strdup(s):NULL;
      Provider[prefix].Zone[zone].Flag=NULL;
      Provider[prefix].Zone[zone].nNumber=numbers;
      Provider[prefix].Zone[zone].Number=number;
      Provider[prefix].Zone[zone].nHour=0;
      Provider[prefix].Zone[zone].Hour=NULL;
      Zones++;
      break;

    case 'A': /* A:areacode[,areacode...] */
      if (ignore) continue;
      if (zone==UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      while(1) {
	if (*(c=strip(str2list(&s)))) {

	  if (*c == '0' && (*(c + 1) != '0')) {
	    sprintf(sx, "%s%s", mycountry, c + 1);
	    warning(dat, "Replacing %s by %s\n", c, sx);
	    c = sx;
	  } /* if */

	  if (!isdigit(*c) && (d=getCountry(c, &Country)) != UNKNOWN) {
	    if (*c=='+') {
	      Areas += appendArea (prefix, c, Country->Name, zone, &domestic, dat);
	    } else if (d>2) {
	      warning (dat, "Unknown country '%s' (%s?), ignoring", c, Country->Name);
	    } else {
	      if (d>0)
		warning (dat, "Unknown country '%s', using '%s'", c, Country->Name);
	      for (i=0; i<Country->nCode; i++)
		Areas += appendArea (prefix, Country->Code[i], Country->Name, zone, &domestic, NULL);
	    }
	  } else { /* unknown country or Sondernummer */
	    Areas += appendArea (prefix, c, NULL, zone, &domestic, dat);
	  }
	} else {
	  warning (dat, "Ignoring empty areacode");
	}
	if (*s==',') {
	  s++;
	  continue;
	}
	break;
      }
      break;

    case 'S': /* S:service[,service...] */
      if (ignore) continue;
      if (zone==UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      while(1) {
	if (*(c=strip(str2list(&s)))) {
	  for (i=0; i<Provider[prefix].nService; i++) {
	    if (strcmp (Provider[prefix].Service[i].Name,c)==0) {
	      warning (dat, "Duplicate Service %s", c);
	      c=NULL;
	      break;
	    }
	  }
	  if (c) {
	    Provider[prefix].Service=realloc(Provider[prefix].Service, (Provider[prefix].nService+1)*sizeof(SERVICE));
	    Provider[prefix].Service[Provider[prefix].nService].Name=strdup(c);
	    Provider[prefix].Service[Provider[prefix].nService].Zone=zone;
	    Provider[prefix].nService++;
	    Services++;
	  }
	} else {
	  warning (dat, "Ignoring empty service");
	}
	if (*s==',') {
	  s++;
	  continue;
	}
	break;
      }
      break;

    case 'F': /* F:Flags */
      if (ignore) continue;
      if (zone==UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      if (Provider[prefix].Zone[zone].Flag) {
	warning (dat, "Flags redefined");
	free (Provider[prefix].Zone[zone].Flag);
      }
      Provider[prefix].Zone[zone].Flag=strdup(strip(s+2));
      break;

    case 'T':  /* T:d-d/h-h=p/s:t[=]Bezeichnung */
      if (ignore) continue;
      if (zone==UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      day=0;
      hour=0;
      freeze=0;
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
	    day|=(1<<(i+MONDAY-1));
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

      if (*s=='!') {
	freeze=1;
	s++;
	while (isblank(*s)) s++;
      }

      if (*s!='=') {
	warning (dat, "expected '=', got '%s'!", s);
	hour=0;
      }

      t=Provider[prefix].Zone[zone].nHour++;
      Provider[prefix].Zone[zone].Hour = realloc(Provider[prefix].Zone[zone].Hour, (t+1)*sizeof(HOUR));
      Provider[prefix].Zone[zone].Hour[t].Name=NULL;
      Provider[prefix].Zone[zone].Hour[t].Day=day;
      Provider[prefix].Zone[zone].Hour[t].Hour=hour;
      Provider[prefix].Zone[zone].Hour[t].Freeze=freeze;
      Provider[prefix].Zone[zone].Hour[t].nUnit=0;
      Provider[prefix].Zone[zone].Hour[t].Unit=NULL;

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

	  u=Provider[prefix].Zone[zone].Hour[t].nUnit++;
	  Provider[prefix].Zone[zone].Hour[t].Unit=realloc(Provider[prefix].Zone[zone].Hour[t].Unit, (u+1)*sizeof(UNIT));
	  Provider[prefix].Zone[zone].Hour[t].Unit[u].Duration=duration;
	  Provider[prefix].Zone[zone].Hour[t].Unit[u].Delay=delay;
	  if (duration!=0.0 && divider!=0.0)
	    Provider[prefix].Zone[zone].Hour[t].Unit[u].Price=price*duration/divider;
	  else
	    Provider[prefix].Zone[zone].Hour[t].Unit[u].Price=price;
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
      Provider[prefix].Zone[zone].Hour[t].Name=*s?strdup(s):NULL;
      Hours++;
      break;

    case 'V': /* V:xxx Version der Datenbank */
      s+=2; while(isblank(*s)) s++;
      strcpy(Version, s);
      break;

    default:
      warning (dat, "Unknown tag '%c'", *s);
      break;
    }
  }
  fclose(stream);

  if (zone!=UNKNOWN && !domestic)
    warning (dat, "Provider %d has no default domestic zone (missing 'A:%s')", prefix, mycountry);

  if (!*Format) {
    warning (dat, "No currency format specified, using defaults");
    strncpy (Format, DEFAULT_FORMAT, STRINGL);
  }

  if (msg) snprintf (message, LENGTH,
		     "Rates   Version %s loaded [%d Providers, %d Zones, %d Areas, %d Services, %d Rates from %s]",
		     Version, Providers, Zones, Areas, Services, Hours, dat);

  return 0;
}

char *getProvider (int prefix)
{
  if (prefix<0 || prefix>=nProvider || !Provider[prefix].used) {
    return NULL;
  }
  return Provider[prefix].Name;
}

int getArea (int prefix, char *number)
{
  int l, i;

  if (prefix<0 || prefix>=nProvider || !Provider[prefix].used)
    return 0;

  l=strlen(number);
  for (i=0; i<Provider[prefix].nArea; i++) {
    if (strmatch(Provider[prefix].Area[i].Code, number)>=l)
      return 1;
  }
  return 0;
}

void clearRate (RATE *Rate)
{
  memset (Rate, 0, sizeof(RATE));
  Rate->prefix=UNKNOWN;
  Rate->zone=UNKNOWN;
  Rate->_area=UNKNOWN;
  Rate->_zone=UNKNOWN;
}

int getRate(RATE *Rate, char **msg)
{
  static char message[LENGTH];
  bitfield hourBits;
  ZONE  *Zone;
  HOUR  *Hour;
  UNIT  *Unit;
  int    prefix, freeze, cur, max, i, j;
  double now, end, jmp, leap;
  char  *day;
  time_t time;
  struct tm tm;
  
  if (msg)
    *(*msg=message)='\0';

  if (!Rate || Rate->_zone==UNZONE)
    return UNKNOWN;

  prefix=Rate->prefix;
  if (prefix<0 || prefix>=nProvider || !Provider[prefix].used) {
    if (msg) snprintf(message, LENGTH, "Unknown provider %d", prefix);
    return UNKNOWN;
  }

  if (Rate->_area==UNKNOWN) {
    int a, x=0;
    char *number=strcat3(Rate->dst);
    for (a=0; a<Provider[prefix].nArea; a++) {
      int m=strmatch(Provider[prefix].Area[a].Code, number);
      if (m>x) {
	x=m;
	Rate->_area = a;
	Rate->domestic = strcmp(Provider[prefix].Area[a].Code, mycountry)==0 || *(Rate->dst[0])=='\0';
      }
    }
    if (Rate->_area==UNKNOWN) {
      if (msg) snprintf (message, LENGTH, "No area info for provider %d, destination %s", prefix, number);
      Rate->_zone=UNZONE;
      return UNKNOWN;
    }
  }

  if (Rate->_zone==UNKNOWN) {
    Rate->_zone=Provider[prefix].Area[Rate->_area].Zone;
    if (Rate->domestic && *(Rate->dst[0])) {
      int z=getZone(prefix, Rate->src[1], Rate->dst[1]);
      if (z!=UNKNOWN) {
	for (i=0; i<Provider[prefix].nZone; i++) {
	  for (j=0; j<Provider[prefix].Zone[i].nNumber; j++) {
	    if (Provider[prefix].Zone[i].Number[j]==z) {
	      Rate->_zone=i;
	      goto done;
	    }
	  }
	}
	if (msg) snprintf (message, LENGTH, "Provider %d domestic zone %d not found in rate database", prefix, z);
	Rate->_zone=UNZONE;
	return UNKNOWN;
      done:
      }
    }
  }

  if (Rate->_zone<0 || Rate->_zone>=Provider[prefix].nZone) {
    if (msg) snprintf(message, LENGTH, "Invalid zone %d", Rate->_zone);
    return UNKNOWN;
  }

  Rate->Provider = Provider[prefix].Name;
  Rate->Country  = Provider[prefix].Area[Rate->_area].Name;
  Rate->Zone     = Provider[prefix].Zone[Rate->_zone].Name;
  Rate->zone     = Provider[prefix].Zone[Rate->_zone].Number[0];

  Rate->Basic=0;
  Rate->Price=0;
  Rate->Duration=0;
  Rate->Units=0;
  Rate->Charge=0;
  Rate->Rest=0;

  if (Rate->start==0)
    return 0;

  Zone=&Provider[prefix].Zone[Rate->_zone];
  Hour=NULL;
  Unit=NULL;
  freeze=0;
  now=0.0;
  end=Rate->now-Rate->start;
  Rate->Time=end;
  leap=UNKNOWN; /* Stundenwechsel erzwingen */
  
  while (now<end) {
    if (!freeze && now>=leap) { /* Neuberechnung bei Stundenwechsel */
      time=Rate->start+now;
      leap=3600*(int)(time/3600+1)-Rate->start;
      tm=*localtime(&time);
      hourBits=1<<tm.tm_hour;
      Hour=NULL;
      cur=max=0;
      for (i=0; i<Zone->nHour; i++) {
	if ((Zone->Hour[i].Hour & hourBits) && ((cur=isDay(&tm, Zone->Hour[i].Day, &day)) > max)) {
	  max=cur;
	  Rate->Day=day;
	  Hour=&(Zone->Hour[i]);
	}
      }
      if (!Hour) {
	if (msg) snprintf(message, LENGTH,
			  "No rate found for provider=%d, zone=%d day=%d hour=%d",
			  prefix, Zone->Number[0], tm.tm_wday+1, tm.tm_hour);
	return UNKNOWN;
      }
      freeze=Hour->Freeze;
      Rate->Hour=Hour->Name;
      Unit=Hour->Unit;
      jmp=now;
      while (Unit->Delay!=UNKNOWN && Unit->Delay<=jmp) {
	jmp-=Unit->Delay;
	Unit++;
      }
      if (now==0.0 && Unit->Duration==0.0)
	Rate->Basic=Unit->Price;
      else
	Rate->Price=Unit->Price;
      Rate->Duration=Unit->Duration;
    }

    now+=Unit->Duration; 
    Rate->Charge+=Unit->Price;

    if (Unit->Duration>0.0)
      Rate->Units++;

    if (Unit->Delay!=UNKNOWN && Unit->Delay<=now) {
      Unit++;
      Rate->Price=Unit->Price;
      Rate->Duration=Unit->Duration;
    } else if (Unit->Duration==0.0)
      break;
  }

  if (now>0.0)
    Rate->Rest=now-Rate->Time;

  return 0;
}

int getLeastCost (RATE *Current, RATE *Cheapest, int booked, int skip)
{
  int i, cheapest;
  RATE Skel, Rate;

  clearRate (&Skel);
  memcpy (Skel.src, Current->src, sizeof (Skel.src));
  memcpy (Skel.dst, Current->dst, sizeof (Skel.dst));
  Skel.start = Current->start;
  if (Current->start == Current->now)
    Skel.now = Current->start + LCR_DURATION;
  else
    Skel.now = Current->now;

  *Cheapest=*Current;
  Cheapest->Charge=1e9;
  cheapest=UNKNOWN;

  for (i=0; i<nProvider; i++) {
    if (!Provider[i].used || i==skip || (booked && !Provider[i].booked))
      continue;
    Rate=Skel;
    Rate.prefix=i;
    if (getRate(&Rate, NULL)!=UNKNOWN && Rate.Charge<Cheapest->Charge) {
      *Cheapest=Rate;
      cheapest=i;
    }
  }
  return (Cheapest->prefix==Current->prefix ? UNKNOWN : cheapest);
}

int guessZone (RATE *Rate, int aoc_units)
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
  static char buffer[BUFSIZ];
  char       *p=buffer;

  if (Rate->Provider && *Rate->Provider)
    p+=sprintf (p, "%s", Rate->Provider);
  else
    p+=sprintf (p, "%s%02d", vbn, Rate->prefix);

  if (Rate->Zone && *Rate->Zone)
    p+=sprintf (p, ", %s", Rate->Zone);
  else
    p+=sprintf (p, ", Zone %d", Rate->zone);

  if (!Rate->domestic && Rate->Country && *Rate->Country)
    p+=sprintf (p, " (%s)", Rate->Country);

  if (Rate->Day && *Rate->Day)
    p+=sprintf (p, ", %s", Rate->Day);

  if (Rate->Hour && *Rate->Hour)
    p+=sprintf (p, ", %s", Rate->Hour);

  return buffer;
}


char *printRate (double value)
{
  static char buffer[STRINGS][STRINGL];
  static int  index=0;

  if (++index>=STRINGS) index=0;
  snprintf (buffer[index], STRINGL, Format, value);
  return buffer[index];
}

#ifdef STANDALONE

void getNumber (char *s, char *num[3])
{
  num[0]=strsep(&s,"-");
  num[1]=strsep(&s,"-");
  num[2]=strsep(&s,"-");
}

void main (int argc, char *argv[])
{
  int i;
  char *msg;
  struct tm now;

  RATE Rate, LCR;

  initHoliday ("../holiday-at.dat", &msg);
  printf ("%s\n", msg);

  initCountry ("../country-de.dat", &msg);
  printf ("%s\n", msg);

  initRate ("/etc/isdn/rate.conf", "../rate-at.dat", "../zone-at-%s.gdbm", &msg);
  printf ("%s\n", msg);

  clearRate(&Rate);
  Rate.prefix = 2;

  if (argc==3) {
    getNumber (argv[1], Rate.src);
    getNumber (argv[2], Rate.dst);
  } else {
    getNumber (strdup("+43-316-698260"), Rate.src);
    if (argc==2)
      getNumber (argv[1], Rate.dst);
    else
      getNumber (strdup("+43-1-4711"), Rate.dst);
  }

  time(&Rate.start);
  Rate.now=Rate.start;
  
  for (i=0; i<5000; i++) {
    if (getRate(&Rate, &msg)==UNKNOWN) {
      printf ("Ooops: %s\n", msg);
      exit (1);
    }
    Rate.now++;
  }
  printf ("domestic=%d _area=%d _zone=%d zone=%d Country=%s Zone=%s Service=%s Flags=%s\n"
	  "current=%s\n\n",
	  Rate.domestic, Rate._area, Rate._zone, Rate.zone, Rate.Country, Rate.Zone,
	  Rate.Service, Rate.Flags, explainRate(&Rate));

  now=*localtime(&Rate.now);
  printf ("---Date--- --Time--  --Charge-- ( Basic  Price)  Unit   Dur  Time  Rest\n");
  printf ("%02d.%02d.%04d %02d:%02d:%02d  %10s (%6.3f %6.3f)  %4d  %4.1f  %4ld  %4ld  %s\n",
	  now.tm_mday, now.tm_mon+1, now.tm_year+1900,
	  now.tm_hour, now.tm_min, now.tm_sec,
	  printRate (Rate.Charge), Rate.Basic, Rate.Price, Rate.Units, Rate.Duration, Rate.Time, Rate.Rest,
	  explainRate(&Rate));
  
  exit (0);

  
#if 1
  time(&Rate.start);
  Rate.now=Rate.start+153;

  if (getRate(&Rate, &msg)==UNKNOWN) {
    printf ("Ooops: %s\n", msg);
    exit (1);
  }

  printf ("domestic=%d _area=%d _zone=%d zone=%d Country=%s Zone=%s Service=%s Flags=%s\n"
	  "current=%s\n\n",
	  Rate.domestic, Rate._area, Rate._zone, Rate.zone, Rate.Country, Rate.Zone,
	  Rate.Service, Rate.Flags, explainRate(&Rate));

  getLeastCost (&Rate, &LCR, 1, UNKNOWN);
  printf ("domestic=%d _area=%d _zone=%d zone=%d Country=%s Zone=%s Service=%s Flags=%s\n"
	  "booked cheapest=%s\n\n",
	  LCR.domestic, LCR._area, LCR._zone, LCR.zone, LCR.Country, LCR.Zone,
	  LCR.Service, LCR.Flags, explainRate(&LCR));

  getLeastCost (&Rate, &LCR, 0, UNKNOWN);
  printf ("domestic=%d _area=%d _zone=%d zone=%d Country=%s Zone=%s Service=%s Flags=%s\n"
	  "all cheapest=%s\n\n",
	  LCR.domestic, LCR._area, LCR._zone, LCR.zone, LCR.Country, LCR.Zone,
	  LCR.Service, LCR.Flags, explainRate(&LCR));


  printf ("---Date--- --Time--  --Charge-- ( Basic  Price)  Unit   Dur  Time  Rest\n");
  for (i=0; i<nProvider; i++) {
    Rate.prefix=i;
    Rate._area=UNKNOWN;
    Rate._zone=UNKNOWN;
    if (getRate(&Rate, NULL)!=UNKNOWN) {
      now=*localtime(&Rate.now);
      printf ("%02d.%02d.%04d %02d:%02d:%02d  %10s (%6.3f %6.3f)  %4d  %4.1f  %4ld  %4ld  %s\n",
	      now.tm_mday, now.tm_mon+1, now.tm_year+1900,
	      now.tm_hour, now.tm_min, now.tm_sec,
	      printRate (Rate.Charge), Rate.Basic, Rate.Price, Rate.Units, Rate.Duration, Rate.Time, Rate.Rest,
	      explainRate(&Rate));
    }
  }

  exit (0);

#else 

  printf ("---Date--- --Time--  --Charge-- ( Basic  Price)  Unit   Dur  Time  Rest\n");

  time(&Rate.start)
  while (1) {
    time(&Rate.now)
    if (getRate(&Rate, &msg)==UNKNOWN) {
      printf ("Ooops: %s\n", msg);
      exit (1);
    }
    now=*localtime(&Rate.now);
    printf ("%02d.%02d.%04d %02d:%02d:%02d  %10s (%6.3f %6.3f)  %4d  %4.1f  %4ld  %4ld\n",
	    now.tm_mday, now.tm_mon+1, now.tm_year+1900,
	    now.tm_hour, now.tm_min, now.tm_sec,
	    printRate (Rate.Charge), Rate.Basic, Rate.Price, Rate.Units, Rate.Duration, Rate.Time, Rate.Rest);

    sleep(1);
  }
#endif
}
#endif
