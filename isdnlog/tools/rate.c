/* $Id: rate.c,v 1.1 1999/03/14 12:16:42 akool Exp $
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
 * void initRate(char *info)
 *   initialisiert die Tarifdatenbank, liefert Versionsinfo in `info'
 *   zurueck
 *
 * char *Providername(int prefix)
 *   liefert den Providernamen fuer Kennzahl `prefix', oder NULL, falls
 *   unbekannt
 *
 * char *Zonename(int prefix, int zone)
 *   liefert den Zonennamen fuer Provider `prefix', oder NULL, falls
 *   unbekannt
 *
 * int taktlaenge(int chan, char *why)
 *   liefert fuer die aktuell laufende Verbindung in `chan'
 *   die Laenge eines Tariftaktes in Sekunden, sowie evtl. in `why'
 *   eine textuelle Begruendung
 *
 * void preparecint(int chan, char *msg, char *hint)
 *   fuellt die aktuell laufende Verbindung in `chan' mit allen
 *   fuer die weitere Taktberechnung noetigen Daten
 *   liefert evtl. in `msg' sowie `hint' textuelle Informationen
 *   fuer die LCR-Optimierung
 *
 * void price(int chan, char *hint)
 *   berechnet fuer die gerade beendete Verbindung in `chan' den
 *   Endpreis, stellt diesen in `chan', und liefert in `hint' evtl.
 *   textuelle Informationen fuer die LCR-Optimierung
 *
 */

#define _RATE_C_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "rate.h"

#define MAXPROVIDER 100
#define LENGTH 250  /* max length of lines in data file */
#define UNKNOWN -1

typedef unsigned long bitfield;

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
} RATE;

typedef struct {
  int    used;
  char  *Name;
  int    nRate;
  RATE *Rate;
} ZONE;

typedef struct {
  int    used;
  char  *Name;
  char	*Internet;
  int    nZone;
  ZONE  *Zone;
} PROVIDER;

static PROVIDER Provider[MAXPROVIDER];
static int      use[MAXPROVIDER];
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

void exitRate(void)
{
  int i, j, k;
  
  for (i=0; i<MAXPROVIDER; i++) {
    if (Provider[i].used) {
      for (j=0; j<Provider[i].nZone; i++)
	if (Provider[i].Zone[j].used) {
	  Provider[i].Zone[j].used=0;
	  for (k=0; k<Provider[i].Zone[j].nRate; k++) {
	    if (Provider[i].Zone[j].Rate[k].Name) free (Provider[i].Zone[j].Rate[k].Name);
	    if (Provider[i].Zone[j].Rate[k].Unit) free (Provider[i].Zone[j].Rate[k].Unit);
	  }
	  if (Provider[i].Zone[j].Name) free (Provider[i].Zone[j].Name);
	  if (Provider[i].Zone[j].Rate) free (Provider[i].Zone[j].Rate); 
	}
      if(Provider[i].Zone) free (Provider[i].Zone); 
      if (Provider[i].Name) free (Provider[i].Name);
      if (Provider[i].Internet) free (Provider[i].Internet);
      Provider[i].used=0;
    }
  }
}

int initRate(char *conf, char *dat, char *msg)
{
  FILE    *stream;
  char     buffer[LENGTH];
  char     Version[LENGTH]="";
  int      nProvider=0;
  int      nZone=0;
  int      nRate=0;
  int      ignore=0;
  int      prefix=UNKNOWN;
  int      version=UNKNOWN;
  int      zone1, zone2;
  int      day1, day2, hour1, hour2;
  bitfield day, hour;
  double   price;
  int      duration, delay;
  int      i, t, u;
  char    *s;

  for (i=0; i<MAXPROVIDER; i++) {
    Provider[i].used=0;
    use[i]=UNKNOWN;
  }

  if (conf && *conf && (stream=fopen(conf,"r"))) {
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
      case 'P':
	prefix = strtol(s+2, &s ,10);
	if (*s != '=') {
	  warning ("expected '=', got '%s'!", s);
	  continue;
	}
	if ((prefix < 0) || (prefix >= MAXPROVIDER)) {
	  warning ("Invalid provider-number %d", prefix);
	  continue;
	}
	use[prefix] = atoi(s+1);
	break;
	
      default:
	warning("Unknown tag '%c'", *s);
      }
    }
    fclose (stream);
  }

  if (!dat || !*dat || (stream=fopen(dat,"r"))==NULL)
    return -1;
  
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
    case 'P': /* P:nn[,v]=Bezeichnung */
      ignore = 0;
      version = UNKNOWN;
      zone1 = zone2 = UNKNOWN;

      prefix = strtol(s+2, &s ,10);
      if (*s == ',') {
	version = strtol(s+1, &s ,10);
      }
      if (*s == '=') s++;
      if ((prefix < 0) || (prefix >= MAXPROVIDER)) {
	warning ("Invalid provider-number %d", prefix);
	continue;
      }
      if ((use[prefix]!=UNKNOWN) && (version!=UNKNOWN) && (use[prefix]!=version)) {
	version = UNKNOWN;
	ignore = 1;
	continue;
      }
      if (Provider[prefix].used++) {
	warning ("Duplicate entry for provider %d (%s)", prefix, Provider[prefix].Name);
	if (Provider[prefix].Name) free(Provider[prefix].Name);
	if (Provider[prefix].Internet) free(Provider[prefix].Internet);
      } 
      Provider[prefix].Name=strdup(strip(s));
      Provider[prefix].Internet=NULL;
      Provider[prefix].nZone=0;
      Provider[prefix].Zone=NULL;
      nProvider++;
      break;

    case 'G': /* P:tt.mm.jjjj Rate gueltig ab */
      if (ignore) continue; 
      break;
    
    case 'C':  /* C:Comment */
      if (ignore) continue; 
      break;
      
    case 'I': /* I:nnn Internet-Zugangsnummer */
      if (ignore) continue; 
      if (prefix == UNKNOWN) {
	warning ("Unexpected tag '%c'", *s);
	break;
      }
      Provider[prefix].Internet = strdup(s+2);
      break;
      
    case 'Z': /* Z:n[-n][=]Bezeichnung */
      if (ignore) continue; 
      if (prefix == UNKNOWN) {
	warning ("Unexpected tag '%c'", *s);
	break;
      }
      zone1=strtol(s+2,&s,10);
      if (*s=='-') zone2=strtol(s+1,&s,10);
      else zone2=zone1;
      if (*s=='=')s++;
      if (zone1>zone2) {
	i=zone2; zone2=zone1; zone1=i;
      }
      if (zone2>=Provider[prefix].nZone) {
	Provider[prefix].Zone=realloc(Provider[prefix].Zone, (zone2+1)*sizeof(ZONE));
	Provider[prefix].nZone = zone2+1;
      }
      for (i=zone1; i<=zone2; i++) {
	Provider[prefix].Zone[i].used=1;
	Provider[prefix].Zone[i].Name=strdup(strip(s));
	Provider[prefix].Zone[i].nRate=0;
	Provider[prefix].Zone[i].Rate=NULL;
	nZone++;
      }
      break;

    case 'T':  /* T:d-d/h-h=p/s:t[=]Bezeichnung */
      if (ignore) continue; 
      if (zone1 == UNKNOWN) {
	warning ("Unexpected tag '%c'", *s);
	break;
      } 
      s+=2;
      day=0;
      while (1) {
	if (*s=='*') {                 /* jeder Tag */
	  day |= 0xff;                 /* 11111111 */
	  s++;
	} else if (*s=='W') {          /* Wochentag 1-5 */
	  day |= 0x1f;                 /* 00011111 */
	  s++;
	} else if (*s=='E') {          /* Wochenende 6-7 */
	  day |= 0x60;                 /* 01100000 */
	  s++;
	} else if (*s=='F'||*s=='H') { /* Feiertag (Holiday) */
	  day |= 0x80;                 /* 10000000 */
	  s++;
	} else if (isdigit(*s)) {      /* 1,2,3 oder 1-5 oder 6,7,F */
	  day1=strtol(s,&s,10);
	  if (*s=='-') day2=strtol(++s,&s,10);
	  else day2=day1;
	  for (i=day1; i<=day2; i++) day|=(1<<(i-1));
	} else if (*s==',') {
	  s++;
	} else if (*s=='/') {
	  s++;
	  break;
	} else {
	  warning ("invalid day format '%s'", s);
	  day=0;
	  break;
	}
      }
      
      while (1) {
	if (*s=='*') {                 /* jede Stunde */
	  hour |= 0xffffff;            /* alles 1er   */
	  s++;
	} else if (isdigit(*s)) {      /* 8-12 oder 1,5 */
	  hour1=strtol(s,&s,10);
	  if (*s=='-') hour2=strtol(s+1,&s,10);
	  else hour2=hour1+1;
	  if (hour2>=hour1)
	    for (i=hour1; i<hour2; i++) hour|=(1<<i);
	  else {
	    for (i=hour1; i<24; i++) hour|=(1<<i);
	    for (i=0; i<hour2; i++)  hour|=(1<<i);
	  }
	} else if (*s==',') {
	  s++;
	} else if (*s=='=') {
	  s++;
	  break;
	} else {
	  warning ("invalid hour format '%s'", s);
	  hour=0;
	  break;
	}
      }
      if (day==0 || hour==0) {
	warning ("I'm sorry, I don't understand this line...");
	continue;
      }
      for (i=zone1; i<=zone2; i++) {
	t=Provider[prefix].Zone[i].nRate++;
	Provider[prefix].Zone[i].Rate = realloc(Provider[prefix].Zone[i].Rate, (t+1)*sizeof(RATE));
	Provider[prefix].Zone[i].Rate[t].Name=NULL;
	Provider[prefix].Zone[i].Rate[t].Day=day;
	Provider[prefix].Zone[i].Rate[t].Hour=hour;
	Provider[prefix].Zone[i].Rate[t].nUnit=0;
	Provider[prefix].Zone[i].Rate[t].Unit=NULL;
      }
      while (1) {
	price=strtod(s,&s);
	duration=1;
	delay=0;
	if (*s=='/')
	  duration=strtol(s+1,&s,10);
	if (*s==':')
	  delay=strtol(s+1,&s,10);

	for (i=zone1; i<=zone2; i++) {
	  t=Provider[prefix].Zone[i].nRate-1;
	  u=Provider[prefix].Zone[i].Rate[t].nUnit++;
	  Provider[prefix].Zone[i].Rate[t].Unit=realloc(Provider[prefix].Zone[i].Rate[t].Unit, (u+1)*sizeof(UNIT));
	  Provider[prefix].Zone[i].Rate[t].Unit[u].Duration=duration;
	  Provider[prefix].Zone[i].Rate[t].Unit[u].Delay=delay;
	  Provider[prefix].Zone[i].Rate[t].Unit[u].Price=price;
	  nRate++;
	}
	if (*s!=',') break;
      }
      if (*s=='=') s++;
      for (i=zone1; i<=zone2; i++) {
	t=Provider[prefix].Zone[i].nRate-1;
	Provider[prefix].Zone[i].Rate[t].Name=strdup(strip(s));
      }
      break;

    case 'V': /* V:xxx Version der Ratedatenbank */
      strcpy(Version, s+2);
      break;
      
    default:
      warning ("Unknown tag '%c'", *s);
      break;
    }
  }
  fclose(stream);
  sprintf (msg, "Rate Version %s loaded [%d Providers, %d Zones, %d Rates]",
	   Version, nProvider, nZone, nRate);

  return 0;
}

char *Providername(int prefix)
{
  if (!Provider[prefix].used) return(NULL);
  return(Provider[prefix].Name);
}

char *Zonename(int prefix, int zone)
{
  if (!Provider[prefix].used) return(NULL);
  if (zone<1 || zone>=Provider[prefix].nZone) return (NULL);
  if (!Provider[prefix].Zone[zone].used) return (NULL);
  return(Provider[prefix].Zone[zone].Name);
}

static double tpreis(int prefix, int zone, int day, int hour, int duration)
{
  bitfield dayMask, hourMask;
  RATE *Rate;
  UNIT  *Unit;
  double price;
  int    delay, i;

  if (!Provider[prefix].used) return UNKNOWN;
  if (!Provider[prefix].Zone[zone].used) return UNKNOWN ;

  dayMask=1<<(day-1);
  hourMask=1<<hour;

  for (i=0; i<Provider[prefix].Zone[zone].nRate; i++) {
    Rate = Provider[prefix].Zone[zone].Rate+i;
    if ((Rate->Day & dayMask) && (Rate->Hour & hourMask))
      break;
  }
  if (i==Provider[prefix].Zone[zone].nRate) return UNKNOWN;
  
  i = 1;
  price = 0.0;
  delay = 0;
  Unit = Rate->Unit;
  while (duration>0) {
    duration -= Unit->Duration;
    delay +=  Unit->Duration;
    price += Unit->Price;
    if ((delay >= Unit->Delay) && (i < Rate->nUnit))
      Unit++;
  }

  return price;
}

#ifdef STANDALONE
void main (int argc, char *argv[])
{
  char msg[BUFSIZ];
  int provider, zone, day, hour, duration;
  
  initRate ("../rate-at.conf", "../rate-at.dat", msg);
  printf ("%s\n", msg);

  provider = 0;
  zone = 1;
  
  printf ("Providername(%d) = %s\n", provider, Providername(provider));
  printf ("Zonename(%d,%d) = %s\n", provider, zone, Zonename(provider, zone));

  day=1;
  hour=12;
  duration=300;
  
  printf ("tpreis(%d,%d,%d,%d,%d)=%f\n", provider, zone, day, hour, duration, tpreis(provider, zone, day, hour, duration)); 
}
#endif
