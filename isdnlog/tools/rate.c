/* $Id: rate.c,v 1.60 1999/11/12 20:50:50 akool Exp $
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
 * Revision 1.60  1999/11/12 20:50:50  akool
 * isdnlog-3.66
 *   - Patch from Jochen Erwied <mack@joker.e.ruhr.de>
 *       makes the "-O" and "-C" options usable at the same time
 *
 *   - Workaround from Karsten Keil <kkeil@suse.de>
 *       segfault in ASN.1 parser
 *
 *   - isdnlog/tools/rate.c ... ignores "empty" providers
 *   - isdnlog/tools/telnum.h ... fixed TN_MAX_PROVIDER_LEN
 *
 * Revision 1.59  1999/11/08 21:09:41  akool
 * isdnlog-3.65
 *   - added "B:" Tag to "rate-xx.dat"
 *
 * Revision 1.58  1999/11/07 13:29:29  akool
 * isdnlog-3.64
 *  - new "Sonderrufnummern" handling
 *
 * Revision 1.57  1999/11/05 20:22:01  akool
 * isdnlog-3.63
 *  - many new rates
 *  - cosmetics
 *
 * Revision 1.56  1999/11/03 16:02:33  paul
 * snprintf call had too many arguments for the format string.
 *
 * Revision 1.55  1999/11/02 21:01:58  akool
 * isdnlog-3.62
 *  - many new rates
 *  - next try to fix "Sonderrufnummern"
 *
 * Revision 1.54  1999/10/31 11:19:11  akool
 * finally fixed a bug with "Sonderrufnummern"
 *
 * Revision 1.53  1999/10/30 18:03:31  akool
 *  - fixed "-q" option
 *  - workaround for "Sonderrufnummern"
 *
 * Revision 1.52  1999/10/30 13:42:37  akool
 * isdnlog-3.60
 *   - many new rates
 *   - compiler warnings resolved
 *   - fixed "Sonderrufnummer" Handling
 *
 * Revision 1.51  1999/10/28 18:36:48  akool
 * isdnlog-3.59
 *  - problems with gcc-2.7.2.3 fixed
 *  - *any* startup-warning solved/removed (only 4u, Karsten!)
 *  - many new rates
 *
 * Revision 1.50  1999/10/25 18:30:03  akool
 * isdnlog-3.57
 *   WARNING: Experimental version!
 *   	   Please use isdnlog-3.56 for production systems!
 *
 * Revision 1.49  1999/10/22 19:57:59  akool
 * isdnlog-3.56 (for Karsten)
 *
 * Revision 1.48  1999/09/26 10:55:20  akool
 * isdnlog-3.55
 *   - Patch from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 *     added hup3 to option file
 *   - changed country-de.dat to ISO 3166 Countrycode / Airportcode
 *
 * Revision 1.47  1999/09/22 09:03:00  akool
 * isdnlog-3.54
 *
 * Revision 1.46  1999/09/20 18:42:30  akool
 * cosmetics
 *
 * Revision 1.45  1999/09/19 14:16:27  akool
 * isdnlog-3.53
 *
 * Revision 1.44  1999/09/16 20:27:22  akool
 * isdnlog-3.52
 *
 * Revision 1.43  1999/09/13 09:09:44  akool
 * isdnlog-3.51
 *   - changed getProvider() to not return NULL on unknown providers
 *     many thanks to Matthias Eder <mateder@netway.at>
 *   - corrected zone-processing when doing a internal -> world call
 *
 * Revision 1.42  1999/09/09 11:21:05  akool
 * isdnlog-3.49
 *
 * Revision 1.41  1999/08/29 10:29:06  akool
 * isdnlog-3.48
 *   cosmetics
 *
 * Revision 1.40  1999/08/25 17:07:16  akool
 * isdnlog-3.46
 *
 * Revision 1.39  1999/08/20 19:29:02  akool
 * isdnlog-3.45
 *  - removed about 1 Mb of (now unused) data files
 *  - replaced areacodes and "vorwahl.dat" support by zone databases
 *  - fixed "Sonderrufnummern"
 *  - rate-de.dat :: V:1.10-Germany [20-Aug-1999 21:23:27]
 *
 * Revision 1.38  1999/07/31 09:25:45  akool
 * getRate() speedup
 *
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
 *   liefert den Namen des Providers oder dessen Prefix wenn unbekannt
 *
 * char* getComment(int prefix, char *key)
 *   liefert einen C:-Eintrag
 *
 * int getSpecial (char *number)
 *   überprüft, ob die Nummer einem N:-Tag = Service entspricht
 *   wird für die Sondernummern benötigt
 *
 * char *getSpecialName(char *number) 
 *   get the Service Name of a special number
 *
 * void clearRate (RATE *Rate)
 *   setzt alle Felder von *Rate zurück
 *
 * int getRate(RATE*Rate, char **msg)
 *   liefert die Tarifberechnung in *Rate, UNKNOWN im
 *   Fehlerfall, *msg enthält die Fehlerbeschreibung
 *
 * int getLeastCost (RATE *Current, RATE *Cheapest, int booked, int skip)
 *   berechnet den billigsten Provider zu *Rate, Rückgabewert
 *   ist der Prefix des billigsten Providers oder UNKNOWN wenn
 *   *Rate bereits den billigsten Tarif enthält. Der Provider
 *   'skip' wird übersprungen (falls überlastet).
 *   Ist 'booked' true, wernden nur Provider die in rate.conf gelistet sind
 *   verglichen.
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
 * inline int getNProvider( void )
 *   returns count of providers
 *
 * int pnum2prefix(int pnum, time_t when) 
 *   converts the external provider number to the internal prefix at
 *   the given date/time when, or know if when is 0
 *
 * int pnum2prefix_variant(char* pnum, time_t when) 
 *   same with a provider string pp_vv
 *
 * int isProviderValid(int prefix, time_t when)
 *   returns true, if the G:tag entries match when 
 *
 */

#define _RATE_C_

#ifdef STANDALONE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#ifndef __GLIBC__
extern const char *basename (const char *name);
#endif
#else
#include "isdnlog.h"
#include "tools.h"
#endif

#include "holiday.h"
#include "zone.h"
#include "dest.h"
#include "rate.h"

#define LENGTH 1024            /* max length of lines in data file */
#define STRINGS 8              /* number of buffers for printRate() */
#define STRINGL 64             /* length of printRate() buffer */
#define DEFAULT_FORMAT "%.2f"  /* default format for printRate() */

#ifdef STANDALONE
#define mycountry "+43"
#define vbn "010"
#define verbose 3
#define LCR_DURATION 153
#define MAXPROVIDER 1000
#define UNKNOWN -1
#endif

#define FEDERAL  1
#define DOMESTIC 2

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
  double    Sales;
  int       nUnit;
  UNIT     *Unit;
} HOUR;

typedef struct {
  char *Code;
  char *Name;
  int   Zone;
} AREA;

typedef struct {
  char  *Name;
  char  *Flag;
  int    Domestic;
  int    nNumber;
  int   *Number;
  int    nHour;
  HOUR  *Hour;
} ZONE;

typedef struct {
  char *Name;
  int   nCode;
  char **Codes;
} SERVICE;

typedef struct {
  char *Key;
  char *Value;
} COMMENT;

typedef struct {
  int _prefix;
  int _variant;
} BOOKED;  

typedef struct {
  int      booked;
//  int      used;
  BOOKED _provider;
  char    *Vbn; /* B:-Tag */
  time_t  FromDate; /* N/Y */
  time_t  ToDate;   /* N/Y */
  char    *Name;
  int      nZone;
  ZONE    *Zone;
  int      nArea;
  AREA    *Area;
  int      nComment;
  COMMENT *Comment;
} PROVIDER;



static char      Format[STRINGL]="";
static PROVIDER *Provider=NULL;
static int      nProvider=0;
static BOOKED *Booked=NULL;
static int      nBooked=0;
static int      line=0;
static SERVICE * Service=NULL;
static int nService=0;
static char * mytld=0;

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
  print_msg(PRT_INFO, "%s\n", msg);
#endif
}

static void warning (char *file, char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  va_start (ap, fmt);
  vsnprintf (msg, BUFSIZ, fmt, ap);
  va_end (ap);
#ifdef STANDALONE
  fprintf(stderr, "%s\n", msg);
#else
  print_msg(PRT_WARN, "%s\n", msg);
#endif
}

static void error (char *file, char *fmt, ...)
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

static void whimper (char *file, char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  if (verbose>2) {
    va_start (ap, fmt);
    vsnprintf (msg, BUFSIZ, fmt, ap);
    va_end (ap);
    notice ("WHIMPER: %s line %3d: %s", basename(file), line, msg);
  }
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

static int appendArea (int prefix, char *code, char *name, int zone, int *where, char *msg)
{
  int i;
  char *fmt;

  for (i=0; i<Provider[prefix].nArea; i++) {
    if (strcmp (Provider[prefix].Area[i].Code,code)==0) {
      if (Provider[prefix].Area[i].Zone!=zone && msg) {
	fmt = name && *name ? "Duplicate area %s (%s) @%d" : "Duplicate area %s @%d";
	warning (msg, fmt, code, line, name);
      }
      return 0;
    }
  }

  if (isdigit(*code) || strncmp(code,mycountry,strlen(mycountry))==0 ||
      strcmp(code, mytld) == 0) {
    if (strcmp(code, mycountry)==0 || strcmp(code, mytld) == 0)
      *where |= FEDERAL;
  } else if(strlen(code)==2 && strcmp(code,mytld) )
    *where &= ~DOMESTIC;

  Provider[prefix].Area=realloc(Provider[prefix].Area, (Provider[prefix].nArea+1)*sizeof(AREA));
  Provider[prefix].Area[Provider[prefix].nArea].Code=strdup(code);
  Provider[prefix].Area[Provider[prefix].nArea].Name=name?strdup(name):NULL;
  Provider[prefix].Area[Provider[prefix].nArea].Zone=zone;
  Provider[prefix].nArea++;
  return 1;
}

static void free_provider(i) {
  int j,k;
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
  for (j=0; j<Provider[i].nArea; j++) {
    if (Provider[i].Area[j].Code) free (Provider[i].Area[j].Code);
    if (Provider[i].Area[j].Name) free (Provider[i].Area[j].Name);
  } /* for */
  if(Provider[i].Area) free (Provider[i].Area);
  for (j=0; j<Provider[i].nComment; j++) {
    if (Provider[i].Comment[j].Key) free (Provider[i].Comment[j].Key);
    if (Provider[i].Comment[j].Value) free (Provider[i].Comment[j].Value);
  }
  if(Provider[i].Comment) free (Provider[i].Comment);
  if (Provider[i].Name) free (Provider[i].Name);
  if (Provider[i].Vbn) free (Provider[i].Vbn);
}

void exitRate(void)
{
  int i, j;

  for (i=0; i<nProvider; i++) {
    free_provider(i); 
  }
  free(Provider);
  Provider=0;
  nProvider=0;
  if(Booked)
    free(Booked);
  nBooked=0;
  for (i=0; i<nService; i++) {
    for(j=0; j<Service[i].nCode; j++) 
      free(Service[i].Codes[j]);
    if(Service[i].Codes) free(Service[i].Codes);
    if(Service[i].Name) free(Service[i].Name);
  }
/*  if(Service) free(Service);    this SIGSEGVs - why ??? */
  Service=0;
  nService=0;
}

char   *prefix2provider(int prefix, char *s)
{
  if (prefix<0 || prefix>=nProvider)
    return "?*?";
  strcpy(s,Provider[prefix].Vbn);
  return Provider[prefix].Vbn;
}

char   *prefix2provider_variant(int prefix, char *s)
{
  if (prefix<0 || prefix>=nProvider)
    return "?*?";
  if(Provider[prefix]._provider._variant != UNKNOWN)
    sprintf(s,"%s_%d",Provider[prefix].Vbn,Provider[prefix]._provider._variant);
  else  
    strcpy(s,Provider[prefix].Vbn);
  return s;
}

inline int getNProvider( void ) {
  return nProvider;
}  

int isProviderValid(int i, time_t when)
{
  return
       ( (Provider[i].FromDate == 0 && Provider[i].ToDate == 0) ||
         (Provider[i].FromDate == 0 && Provider[i].ToDate >= when) ||
         (Provider[i].FromDate < when && Provider[i].ToDate == 0) ||
	 (Provider[i].FromDate < when && Provider[i].ToDate >= when) ) ;
}

int pnum2prefix(int pnum, time_t when) {
  int i;
  time_t now;
  if(when==0) {
    time(&now);
    when=now;
  }  
  for(i=0;i<nProvider;i++) 
   if( (Provider[i]._provider._prefix == pnum &&
       ( Provider[i]._provider._variant == UNKNOWN ||
         Provider[i].booked==1) ) &&
	 isProviderValid(i, when) )
     return i;
  return UNKNOWN;   
}      	

int pnum2prefix_variant(char * pnum, time_t when) {
  int p,v;
  char *s;
  int i;
  time_t now;

  p=atoi(pnum);
  v=UNKNOWN;
  if ((s=strchr(pnum, '_')) != 0)
    v=atoi(s+1);
  if(when==0) {
    time(&now);
    when=now;
  }  
  for(i=0;i<nProvider;i++) 
   if( Provider[i]._provider._prefix == p &&
       Provider[i]._provider._variant == v &&
       isProviderValid(i, when) )
     return i;
  return UNKNOWN;   
}

int vbn2prefix(char *vbn, int *len) {	 
  int i;
  for(i=0;i<nProvider;i++) 
    if(Provider[i].Vbn && *Provider[i].Vbn) {
      if(strncmp(Provider[i].Vbn, vbn, strlen(Provider[i].Vbn))==0 &&
          (Provider[i]._provider._variant == UNKNOWN ||
           Provider[i].booked==1) ) {
	*len = strlen(Provider[i].Vbn);
	return i;
      }
    }	 
  return UNKNOWN;
}
  
static int parseDate(char **s, time_t *t) {
  struct tm tm;
  tm.tm_hour=tm.tm_min=tm.tm_sec=0;
  tm.tm_mday = strtoul(*s, s, 10);
  if(**s != '.') 
    return 0;
  (*s)++;  
  tm.tm_mon = strtoul(*s, s, 10)-1;
  if(**s != '.') 
    return 0;
  (*s)++;  
  tm.tm_year = strtoul(*s, s, 10)-1900;
  *t = mktime(&tm);
  return 1;
}
int initRate(char *conf, char *dat, char *dom, char **msg)
{
  static char message[LENGTH];
  FILE    *stream;
  bitfield day, hour;
  double   price, divider, duration;
  char     buffer[LENGTH], path[LENGTH], Version[LENGTH]="";
  char     *c, *s;
  int      Comments=0;
  int      Areas=0, Specials=0, Zones=0, Hours=0;
  int      where=DOMESTIC, prefix=UNKNOWN;
  int      zone, zone1, zone2, day1, day2, hour1, hour2, freeze, delay;
  int     *number, numbers;
  int      i, n, t, u, v, z;

  initTelNum(); /* we need defnum */
  mytld = getMynum()->tld;
  if(!*mytld || strlen(mytld) != 2)
    error( dat, "Defaultnumber not set, did you configure 'mycountry'?");

  if (msg)
    *(*msg=message)='\0';

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
	Booked = realloc(Booked, (nBooked+1)*sizeof(BOOKED));
	Booked[nBooked]._prefix=prefix;
	Booked[nBooked]._variant=UNKNOWN;
	nBooked++;
	while (isblank(*s)) s++;
	if (*s == '=') {
	  s++;
	  while (isblank(*s)) s++;
	  if (!isdigit(*s)) {
	    warning (conf, "Invalid variant %s", s);
	    continue;
	  }
	  if ((v=strtol(s, &s, 10))<0) {
	    warning (conf, "Invalid variant %s", s);
	    continue;
	  }
	  Booked[nBooked-1]._variant=v;
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
    if (msg) snprintf (message, LENGTH, "Warning: no rate database specified in %s!",
		       conf);
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

    case 'V': /* V:xxx Version der Datenbank */
      s+=2; while(isblank(*s)) s++;
      if (*Version) {
	warning (dat, "Redefinition of database version");
      }
      strcpy(Version, s);
      break;

    case 'U': /* U:Format für printRate() */
      if (*Format) {
	warning (dat, "Redefinition of currency format");
      }
      strcpy (Format, strip(s+2));
      break;

    case 'P': /* P:nn[,v] Bezeichnung */
      if (zone!=UNKNOWN) {
	Provider[prefix].Zone[zone].Domestic = (where & DOMESTIC) == DOMESTIC;
	line--;
	if (Provider[prefix].Zone[zone].nHour==0)
	  whimper (dat, "Zone has no 'T:' Entries", zone);
#if 1 /* AK:28Oct99 - lt 6.nov 99 turn off PRT_INFO if you want see it */
	if (!(where & FEDERAL))
	  whimper (dat, "Provider %d has no default domestic zone (missing 'A:%s')", prefix, mycountry);
#endif
	line++;
      }
      else if(nProvider) { /* silently ignore empty providers */
	free_provider(prefix);
	nProvider--;
      }  
      if(nProvider) {
	if(!Provider[prefix].Vbn || !*Provider[prefix].Vbn) {
	  error(dat, "Provider %s has no valid B:-Tag - ignored", getProvider(prefix));
	  free_provider(prefix);
	  nProvider--;
	}  
      } 	
      v = UNKNOWN;
      zone = UNKNOWN;
      where = DOMESTIC;

      s+=2; while (isblank(*s)) s++;
      if (!isdigit(*s)) {
	warning (dat, "Invalid provider-number '%c'", *s);
	prefix=UNKNOWN;
	continue;
      }
      prefix = strtol(s, &s ,10);
      Provider=realloc(Provider, (nProvider+1)*sizeof(PROVIDER));
      memset(&Provider[nProvider], 0, sizeof(PROVIDER));
      Provider[nProvider]._provider._prefix=prefix;
      prefix=nProvider; /* the internal prefix */
      nProvider++;
      Provider[prefix]._provider._variant=UNKNOWN;
      while (isblank(*s)) s++;
      if (*s == ',') {
	s++; while (isblank(*s)) s++;
	if (!isdigit(*s)) {
	  warning (dat, "Invalid variant '%c'", *s);
	  prefix=UNKNOWN;
	  continue;
	}
	v=strtol(s, &s, 10);
	Provider[prefix]._provider._variant=v;
      }
      while (isblank(*s)) s++;
      Provider[prefix].Name=*s?strdup(s):NULL;
      for (i = 0; i < nBooked; i++)
        if (Booked[i]._variant==Provider[prefix]._provider._variant &&
	    Booked[i]._prefix==Provider[prefix]._provider._prefix) {
          Provider[prefix].booked=1;
	  break;
        }	  
      break;

    case 'B':  /* B: VBN */
      s += 2;
      Provider[prefix].Vbn = strdup(strip(s));
      break;

    case 'C':  /* C:Comment */
      s+=2; while (isblank(*s)) s++;
      if ((c=strchr(s,':'))!=NULL) {
	*c='\0';
	c=strip(c+1);
	for (i=0; i<Provider[prefix].nComment; i++) {
	  if (strcmp (Provider[prefix].Comment[i].Key,s)==0) {
	    char **value=&Provider[prefix].Comment[i].Value;
	    *value=realloc(*value, strlen(*value)+strlen(c)+2);
	    strcat(*value, "\n");
	    strcat(*value, c);
	    s=NULL;
	    break;
	  }
	}
	if (s) {
	  Provider[prefix].Comment=realloc(Provider[prefix].Comment, (Provider[prefix].nComment+1)*sizeof(COMMENT));
	  Provider[prefix].Comment[Provider[prefix].nComment].Key=strdup(s);
	  Provider[prefix].Comment[Provider[prefix].nComment].Value=strdup(c);
	  Provider[prefix].nComment++;
	  Comments++;
	}
      }
      break;

    case 'D':  /* D:Verzonung */
      if (prefix == UNKNOWN || zone != UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2; while (isblank(*s)) s++;
      snprintf (path, LENGTH, dom, s);
      if (initZone(prefix, path, &c)==0) {
	if (msg && *c) whimper ("%s", c);
      } else {
	error (dat, c);
      }
      break;

    case 'Z': /* Z:n[-n][,n] Bezeichnung */
      if (prefix == UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      if (zone != UNKNOWN) {
	Provider[prefix].Zone[zone].Domestic = (where & DOMESTIC) == DOMESTIC;
	line--;
	if (Provider[prefix].Zone[zone].nHour==0)
	  whimper (dat, "Zone has no 'T:' Entries", zone);
	line++;
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
	  for (z=0; z<Provider[prefix].nZone; z++) {
	    for (n=0; n<Provider[prefix].Zone[z].nNumber; n++) {
	      if (Provider[prefix].Zone[z].Number[n]==i) {
		warning (dat, "Duplicate zone %d", i);
		goto skip;
	      }
	    }
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
#if 0 /* AK:28Oct99 */
      if (*s=='\0')
	whimper (dat, "Zone should have a name...");
#endif
      zone=Provider[prefix].nZone++;
      Provider[prefix].Zone=realloc(Provider[prefix].Zone, Provider[prefix].nZone*sizeof(ZONE));
      Provider[prefix].Zone[zone].Name=*s?strdup(s):NULL;
      Provider[prefix].Zone[zone].Flag=NULL;
      Provider[prefix].Zone[zone].Domestic=0;
      Provider[prefix].Zone[zone].nNumber=numbers;
      Provider[prefix].Zone[zone].Number=number;
      Provider[prefix].Zone[zone].nHour=0;
      Provider[prefix].Zone[zone].Hour=NULL;
      Zones++;
      break;

    case 'A': /* A:areacode[,areacode...] */
      if (zone==UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      while(1) {
	if (*(c=strip(str2list(&s)))) {
/* append areas as they are -lt- */
	    Areas += appendArea (prefix, c, NULL, zone, &where, dat);
	    Specials++;
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

    case 'S': /* S:service */
/* S:Service
   N:nn[,nn]
   ...
*/   
      if (nProvider) continue;
      s+=2;
      s=strip(s);
      Service=realloc(Service, (++nService)*sizeof(SERVICE));
      Service[nService-1].Name=strdup(s);
      Service[nService-1].Codes=0;
      Service[nService-1].nCode=0;
      break;

    case 'N': /* N:serviceNum[,serviceNum...] */
      if (nProvider) continue;
      if (Service==NULL) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      while(1) {
	if (*(c=strip(str2list(&s)))) {
	  Service[nService-1].Codes=realloc(Service[nService-1].Codes,
	  	++Service[nService-1].nCode * sizeof(char*));
	  Service[nService-1].Codes[Service[nService-1].nCode-1]=strdup(c);
	}  
	if (*s==',') {
	  s++;
	  continue;
	}
	break;
      }
      break;

    case 'F': /* F:Flags */
      break;
#if 0      
      if (zone==UNKNOWN) {
	warning (dat, "Unexpected tag '%c'", *s);
	break;
      }
      if (Provider[prefix].Zone[zone].Flag) {
	warning (dat, "Flags redefined");
	free (Provider[prefix].Zone[zone].Flag);
      }
      Provider[prefix].Zone[zone].Flag=strdup(strip(s+2));
#endif      
      break;

    case 'T':  /* T:d-d/h-h=p/s:t[=]Bezeichnung */
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
      Provider[prefix].Zone[zone].Hour[t].Sales=0.0;
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
	if (*s=='|') {
	  Provider[prefix].Zone[zone].Hour[t].Sales=price;
	  s++;
	  continue;
	}
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
	  if (*s!=',' && *s!='/') {
	    if (duration==0.0) {
	      warning(dat, "last rate must have a duration, set to 1!");
	      duration=1.0;
	    }
	    if (delay!=UNKNOWN) {
	      warning(dat, "last rate must not have a delay, will be ignored!");
	      delay=UNKNOWN;
	    }
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

    case 'G': /* G:[dd.mm.yyyy][-dd.mm.yyyy] */
      s+=2;
      while (isblank(*s)) s++;
      if(isdigit(*s)) {
        if(!parseDate(&s, &Provider[prefix].FromDate))
          warning (dat, "Invalid date '%s'", s);
      }
      while (isblank(*s)) s++;
      if (*s == '-') {
        s++;
        while (isblank(*s)) s++;
        if(isdigit(*s)) {
          if(!parseDate(&s, &Provider[prefix].ToDate))
            warning (dat, "Invalid date '%s'", s);
        }
      }	
      break;

    default:
      warning (dat, "Unknown tag '%c'", *s);
      break;
    }
  }
  fclose(stream);

  if (zone!=UNKNOWN) {
    Provider[prefix].Zone[zone].Domestic = (where & DOMESTIC) == DOMESTIC;
    line--;
    if (Provider[prefix].Zone[zone].nHour==0)
      whimper (dat, "Zone has no 'T:' Entries", zone);
#if 1 /* AK:28Oct99 - lt 6.nov 99 turn off PRT_INFO if you want see it */
    if (!(where & FEDERAL))
      whimper (dat, "Provider %d has no default domestic zone (missing 'A:%s')", prefix, mycountry);
#endif
    line++;
  }
  else if(nProvider) { /* silently ignore empty providers */
    free_provider(prefix);
    nProvider--;
  } 
  if(nProvider) 
    if(!Provider[prefix].Vbn || !*Provider[prefix].Vbn) {
      error(dat, "Provider %s has no valid B:-Tag - ignored", getProvider(prefix));
      free_provider(prefix);
      nProvider--;
    }  

  if (!*Version) {
    warning (dat, "Database version could not be identified");
    strcpy (Version, "<unknown>");
  }

  if (!*Format) {
    warning (dat, "No currency format specified, using defaults");
    strncpy (Format, DEFAULT_FORMAT, STRINGL);
  }

  if (msg) snprintf (message, LENGTH,
		     "Rates   Version %s loaded [%d Providers, %d Zones, %d Areas, %d Specials, %d Services, %d Comments, %d Rates from %s]",
		     Version, nProvider, Zones, Areas, Specials, nService, Comments, Hours, dat);

  return 0;
}

char *getProvider (int prefix)
{
  static char s[BUFSIZ];

  if (prefix<0 || prefix>=nProvider ||
  	!Provider[prefix].Name || !*Provider[prefix].Name) {
    prefix2provider(prefix, s);
    strcat(s," ???");
    return s;
  }
  return Provider[prefix].Name;
}

char *getComment (int prefix, char *key)
{
  int i;

  if (prefix<0 || prefix>=nProvider)
    return NULL;

  for (i=0; i<Provider[prefix].nComment; i++) {
    if (strcmp(Provider[prefix].Comment[i].Key,key)==0)
      return Provider[prefix].Comment[i].Value;
  }
  return NULL;
}

int getSpecial (char *number) {
  int i,j,l;
  l=strlen(number);
  for (i=0; i<nService; i++) 
    for(j=0; j<Service[i].nCode; j++) 
      if(strmatch(Service[i].Codes[j], number)>=l)
        return 1;
  return 0;  
}  

char *getSpecialName(char *number) {
  int i,j,l;
  l=strlen(number);
  for (i=0; i<nService; i++) 
    for(j=0; j<Service[i].nCode; j++) 
      if(strmatch(Service[i].Codes[j], number)>=l)
        return Service[i].Name;
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

static int leo (int a, int b, double c, double d)
{
  int x;

  if (a < b)
    b = a;
  if (b < c || c < 0)
    c = b;
  x = ceil(c/d);
  return x < 1 ? 1 : x;
}

int getRate(RATE *Rate, char **msg)
{
  static char message[LENGTH];
  bitfield hourBits;
  ZONE  *Zone;
  HOUR  *Hour;
  UNIT  *Unit;
  int    prefix, freeze, cur, max, i, j, n;
  double now, end, jmp, leap;
  char  *day;
  time_t time;
  struct tm tm;
  char *number=0;

  if (msg)
    *(*msg=message)='\0';

  if (!Rate || Rate->_zone==UNZONE)
    return UNKNOWN;

  prefix=Rate->prefix;
  if (prefix<0 || prefix>=nProvider) {
    if (msg) snprintf(message, LENGTH, "Unknown provider %d", prefix);
    return UNKNOWN;
  }

  if (Rate->_area==UNKNOWN) {
    int a, x=0;
    TELNUM num;
    number=strcat3(Rate->dst);
    if (*Rate->dst[0] && getDest(number, &num) == 0 && num.keys && *num.keys) {
      char *p;
#if 0
    printf("%s(%d) %s(%s) %s - %s\n",
	   num.country, num.ncountry, num.sarea, num.area,
	   num.msn, num.keys);
#endif
      p=strtok(num.keys, "/");
      while (p) {
        for (a=0; a<Provider[prefix].nArea; a++) {
          if (strcmp(Provider[prefix].Area[a].Code, p)==0) {
	    Rate->_area=a;
	    x=strlen(Provider[prefix].Area[a].Code);
	    Rate->domestic=atoi(mycountry+1)==num.ncountry;
	    break;
	  }
        }
	if(Rate->_area!=UNKNOWN)
	  break;
        p=strtok(0, "/");
      }
    }
    /* try find a longer match in codes e.g. for mobil phone nums */
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
  if (Rate->dst[0] && *Rate->dst[0])
    Rate->Zone     = Provider[prefix].Zone[Rate->_zone].Name;
  else if(number && *number)
    Rate->Zone = getSpecialName(number);
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

  while (1) {
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
      Rate->Sales=Hour->Sales;
      Unit=Hour->Unit;
      if (now==0.0 && Unit->Duration==0.0)
	Rate->Basic=Unit->Price;
      for (i=0; i<Hour->nUnit; i++)
	if ((Rate->Rhythm[0]=Unit[i].Duration)!=0)
	  break;
      Rate->Rhythm[1]=Unit[Hour->nUnit-1].Duration;
      jmp=now;
      while (Unit->Delay!=UNKNOWN && Unit->Delay<=jmp && jmp>0) {
	jmp-=Unit->Delay;
	Unit++;
      }
      Rate->Price=Unit->Price;
      Rate->Duration=Unit->Duration;
    }

    if (Unit->Duration==0.0) {
      Rate->Charge+=Unit->Price;
    } else {
      n=leo(end-now, leap-now, Unit->Delay, Unit->Duration);
      Rate->Units+=n;
      Rate->Charge+=n*Unit->Price;
      now+=n*Unit->Duration;
      if (now>end)
	break;
    }
    if (Unit->Delay!=UNKNOWN && Unit->Delay<=now) {
      Unit++;
      Rate->Price=Unit->Price;
      Rate->Duration=Unit->Duration;
    }
  }

  if (Rate->Charge < Rate->Sales)
    Rate->Charge = Rate->Sales;

  if (now>0.0)
    Rate->Rest=now-Rate->Time;

  return 0;
}

int getLeastCost (RATE *Current, RATE *Cheapest, int booked, int skip)
{
  int i, cheapest;
  RATE Skel, Rate;
  char *number;
  int serv=-1, j, cod, l;

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
  do {
    for (i=0; i<nProvider; i++) {
      if (i==skip || (booked && !Provider[i].booked))
        continue;
      Rate=Skel;
      Rate.prefix=i;
      if (getRate(&Rate, NULL)!=UNKNOWN && Rate.Charge<Cheapest->Charge) {
        *Cheapest=Rate;
        cheapest=i;
      }
    }
    number = strcat3(Skel.dst);
    if (!*Skel.dst[0] && !*Skel.dst[2] && getSpecial(number)) { /* try other numbers for this service */
      if(serv==-1) {
        l=strlen(number);
        for (i=0; i<nService && serv==-1; i++) 
          for(j=0; j<Service[i].nCode; j++) 
            if(strmatch(Service[i].Codes[j], number)>=l) {
	      serv=i;
	      break;
	    }
	if(serv==-1) /* not found - shouldn't be */
	  break;    
	cod=0;  
      }	
      if (cod < Service[serv].nCode) {
        Skel.dst[1] = Service[serv].Codes[cod];
	cod++;
      }
      else
        break;	
    }
    else
      break;
  } while(1);
  return (Cheapest->prefix==Current->prefix ? UNKNOWN : cheapest);
}

int getZoneRate(RATE* Rate, int domestic, int first)
{
  static int z;
  static int lasta;
  int prefix,i,a;
  char * countries = 0;
  char *area, *olda;
  int zone, found, res;
  TELNUM Num;

  if (first)
    lasta=z=0;
  prefix=Rate->prefix;
  while (1) {
    found=0;
    if (z >= Provider[prefix].nZone)
      return -2;
    if (Provider[prefix].Zone[z].Domestic != domestic ||
	Provider[prefix].Zone[z].Number[0] == 0) {
      z++;
      continue;
    }
    for (a=lasta; a<Provider[prefix].nArea; a++)  {
      if (Provider[prefix].Area[a].Zone == z) {
        found++;
        break;
      }
    }
    if(found)
      break;
    if(!found && domestic) {
      a=0;
      break;
    }
    z++;
  }
  Rate->_area=a;
  Rate->_zone=z;
  if (getRate(Rate, 0) == UNKNOWN)
    return UNKNOWN;
  Rate->Country=0;
  olda="#";
  if (domestic) {
    if (Provider[prefix].Zone[z].Name)
      countries = strdup(Provider[prefix].Zone[z].Name);
    else
      countries=strdup("");
  }
  else {
    countries=strdup("");
    for (i=a; i<Provider[prefix].nArea; i++) {
      zone=Provider[prefix].Area[i].Zone;
      area=Provider[prefix].Area[i].Code;
      res=getDest(area, &Num);
      if(res!=UNKNOWN)
        area=Num.scountry;
      if (res!=UNKNOWN && zone==z && area && *area) {
	countries = realloc(countries,strlen(countries)+strlen(area)+2);
	if(*countries)
          strcat(countries, ",");
        strcat(countries, area);
	olda=area;
      }
      else if(strcmp(Provider[prefix].Area[i].Code,"+")==0) {
	countries = realloc(countries,strlen(countries)+2);
	if(*countries)
          strcat(countries, ",");
        strcat(countries, "+");
      }
      else if(zone > z)
        break;
    }
    lasta=a;
  }
  if (countries && *countries)
    Rate->Country=countries;
  z++;
  return 0;
}

char *explainRate (RATE *Rate)
{
  static char buffer[BUFSIZ];
  char       *p=buffer;

  strcpy(p, getProvider(Rate->prefix));
  p += strlen(p);

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

/* char *vbn;
char *mycountry;
char *myarea; */

int main (int argc, char *argv[])
{
  int i;
  char *msg;
  struct tm now;

  RATE Rate, LCR;

//  vbn="01";
  myarea="02555";
//  mycountry="+43";
  initHoliday ("../holiday-at.dat", &msg);
  printf ("%s\n", msg);

  initDest ("dest/dest.gdbm", &msg);
  printf ("%s\n", msg);
  initTelnum();
  initRate ("/etc/isdn/rate.conf", "../rate-at.dat", "../zone-at-%s.gdbm", &msg);
  printf ("%s\n", msg);

  clearRate(&Rate);
  Rate.prefix = 1;

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
  Rate.now=Rate.start+153;

#if 0
  Rate.prefix = 2;
  for (i=0; i<10000; i++) {
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
#endif


#if 0
  time(&Rate.start);
  Rate.now=Rate.start + LCR_DURATION;

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


  printf ("---Date--- --Time--  --Charge-- ( Basic  Price)  Unit   Dur  Rhythm Time  Rest\n");
  for (i=0; i<nProvider; i++) {
    Rate.prefix=i;
    Rate._area=UNKNOWN;
    Rate._zone=UNKNOWN;
    if (getRate(&Rate, NULL)!=UNKNOWN) {
      now=*localtime(&Rate.now);
      printf ("%02d.%02d.%04d %02d:%02d:%02d  %10s (%6.3f %6.3f)  %4d  %4.1f  %2f/%2f %4ld  %4ld  %s\n",
	      now.tm_mday, now.tm_mon+1, now.tm_year+1900,
	      now.tm_hour, now.tm_min, now.tm_sec,
	      printRate (Rate.Charge),
	      Rate.Basic, Rate.Price,
	      Rate.Units, Rate.Duration,
	      Rate.Rhythm[0], Rate.Rhythm[1],
	      Rate.Time, Rate.Rest,
	      explainRate(&Rate));
    }
  }

  exit (0);

#else

  printf ("---Date--- --Time--  --Charge-- ( Sales Basic  Price)  Unit   Dur  Time  Rest\n");

  time(&Rate.start);
  time(&Rate.now);
  if (getRate(&Rate, &msg)==UNKNOWN) {
    printf ("Ooops: %s\n", msg);
    exit (1);
  }
  printf ("domestic=%d _area=%d _zone=%d zone=%d Country=%s Zone=%s Service=%s Flags=%s\n"
	  "current=%s\n\n",
	  Rate.domestic, Rate._area, Rate._zone, Rate.zone, Rate.Country, Rate.Zone,
	  Rate.Service, Rate.Flags, explainRate(&Rate));


  while (1) {
    time(&Rate.now);
    if (getRate(&Rate, &msg)==UNKNOWN) {
      printf ("Ooops: %s\n", msg);
      exit (1);
    }
    now=*localtime(&Rate.now);
    printf ("%02d.%02d.%04d %02d:%02d:%02d  %10s (%6.3f %6.3f %6.3f)  %4d  %4.1f  %4ld  %4ld\n",
	    now.tm_mday, now.tm_mon+1, now.tm_year+1900,
	    now.tm_hour, now.tm_min, now.tm_sec,
	    printRate (Rate.Charge), Rate.Sales, Rate.Basic, Rate.Price, Rate.Units, Rate.Duration, Rate.Time, Rate.Rest);

    sleep(1);
  }
#endif
 return EXIT_SUCCESS;
}
#endif
