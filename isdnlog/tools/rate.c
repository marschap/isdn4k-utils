/* $Id: rate.c,v 1.2 1999/03/16 17:38:09 akool Exp $
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
 * int getRate(RATE*Rate, char **msg)
 *   liefert die Tarifberechnung in *Rate, UNKNOWN im 
 *   Fehlerfall, *msg enthält die Fehlerbeschreibung
 *
 * int getLeastCost (RATE *Rate)
 *   berechnet den billigsten Provider zu *Rate, Rückgabewert
 *   ist der Prefix des billigsten Providers oder UNKNOWN wenn
 *   *Rate bereits den billigsaten Tarif enthält
 *
 */

#define _RATE_C_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "holiday.h"
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
} HOUR;

typedef struct {
  int    used;
  char  *Name;
  int    nHour;
  HOUR  *Hour;
} ZONE;

typedef struct {
  int    used;
  char  *Name;
  char	*Internet;
  int    nZone;
  ZONE  *Zone;
} PROVIDER;

static PROVIDER Provider[MAXPROVIDER];
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
	  for (k=0; k<Provider[i].Zone[j].nHour; k++) {
	    if (Provider[i].Zone[j].Hour[k].Name) free (Provider[i].Zone[j].Hour[k].Name);
	    if (Provider[i].Zone[j].Hour[k].Unit) free (Provider[i].Zone[j].Hour[k].Unit);
	  }
	  if (Provider[i].Zone[j].Name) free (Provider[i].Zone[j].Name);
	  if (Provider[i].Zone[j].Hour) free (Provider[i].Zone[j].Hour); 
	}
      if(Provider[i].Zone) free (Provider[i].Zone); 
      if (Provider[i].Name) free (Provider[i].Name);
      if (Provider[i].Internet) free (Provider[i].Internet);
      Provider[i].used=0;
    }
  }
}

int initRate(char *conf, char *dat, char **msg)
{
  FILE    *stream;
  char     buffer[LENGTH];
  char     Version[LENGTH]="";
  static char message[LENGTH];
  int      use[MAXPROVIDER];
  int      nProvider=0;
  int      nZone=0;
  int      nHour=0;
  int      ignore=0;
  int      prefix=UNKNOWN;
  int      version=UNKNOWN;
  int      zone=UNKNOWN;
  int      day1, day2, hour1, hour2;
  bitfield day, hour;
  double   price, duration;
  int      delay;
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
      zone = UNKNOWN;

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

    case 'G': /* P:tt.mm.jjjj Hour gueltig ab */
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
      zone=strtol(s+2,&s,10);
      if (*s=='=')s++;
      if (zone>=Provider[prefix].nZone) {
	Provider[prefix].Zone=realloc(Provider[prefix].Zone, (zone+1)*sizeof(ZONE));
	Provider[prefix].nZone = zone+1;
      }
      Provider[prefix].Zone[zone].used=1;
      Provider[prefix].Zone[zone].Name=strdup(strip(s));
      Provider[prefix].Zone[zone].nHour=0;
      Provider[prefix].Zone[zone].Hour=NULL;
	nZone++;
      break;

    case 'T':  /* T:d-d/h-h=p/s:t[=]Bezeichnung */
      if (ignore) continue; 
      if (zone == UNKNOWN) {
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
      
      hour=0;
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
      t=Provider[prefix].Zone[zone].nHour++;
      Provider[prefix].Zone[zone].Hour = realloc(Provider[prefix].Zone[zone].Hour, (t+1)*sizeof(HOUR));
      Provider[prefix].Zone[zone].Hour[t].Name=NULL;
      Provider[prefix].Zone[zone].Hour[t].Day=day;
      Provider[prefix].Zone[zone].Hour[t].Hour=hour;
      Provider[prefix].Zone[zone].Hour[t].nUnit=0;
      Provider[prefix].Zone[zone].Hour[t].Unit=NULL;
      while (1) {
	price=strtod(s,&s);
	duration=1;
	delay=UNKNOWN;
	if (*s=='/')
	  duration=strtod(s+1,&s);
	if (*s==':')
	  delay=strtol(s+1,&s,10);
	if (*s==',' && delay==UNKNOWN)
	  delay=duration;
	if (*s!=',' && delay!=UNKNOWN){
	  warning("last rate must not have a delay, will be ignored!");
	  delay=UNKNOWN;
	}
	t=Provider[prefix].Zone[zone].nHour-1;
	u=Provider[prefix].Zone[zone].Hour[t].nUnit++;
	Provider[prefix].Zone[zone].Hour[t].Unit=realloc(Provider[prefix].Zone[zone].Hour[t].Unit, (u+1)*sizeof(UNIT));
	Provider[prefix].Zone[zone].Hour[t].Unit[u].Duration=duration;
	Provider[prefix].Zone[zone].Hour[t].Unit[u].Delay=delay;
	Provider[prefix].Zone[zone].Hour[t].Unit[u].Price=price;
	nHour++;
	if (*s!=',') break;
	s++;
      }
      if (*s=='=') s++;
      t=Provider[prefix].Zone[zone].nHour-1;
      Provider[prefix].Zone[zone].Hour[t].Name=strdup(strip(s));
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

  if (msg) snprintf (*msg=message, LENGTH, "Rate Version %s loaded [%d Providers, %d Zones, %d Rates from %s]",
		     Version, nProvider, nZone, nHour, dat);

  return 0;
}

int getRate(RATE *Rate, char **msg)
{
  static char message[LENGTH];
  bitfield dayMask, hourMask;
  ZONE  *Zone;
  HOUR  *Hour;
  UNIT  *Unit;
  int    prefix, zone, day, hour, i;
  time_t now, run, end;
  struct tm tm;

  if (!Rate) 
    return UNKNOWN;

  prefix=Rate->prefix; 
  if (prefix<0 || prefix>MAXPROVIDER || !Provider[prefix].used) {
    if (msg) snprintf(*msg=message, LENGTH, "unknown provider '%d'", prefix);
    return UNKNOWN;
  }

  zone=Rate->zone;
  if (zone<1 || zone>=Provider[prefix].nZone || !Provider[prefix].Zone[zone].used) {
    if (msg) snprintf(*msg=message, LENGTH, "unknown zone '%d'", zone);
    return UNKNOWN;
  }

  Zone=&Provider[prefix].Zone[zone];
  Rate->Provider=Provider[prefix].Name;
  Rate->Zone=Zone->Name;
  Rate->Units=0;
  Rate->Duration=0;
  Rate->Price=0;
  Rate->Charge=0;
  Rate->Rest=0;

  Hour=NULL;
  Unit=NULL;
  hour=UNKNOWN; /* Stundenwechsel erzwingen */
  now=mktime(&Rate->start);
  end=mktime(&Rate->now);
  Rate->Time=end-now;
  run=0;
  while (end>=now) {
    tm=*localtime(&now);
    if (hour!=tm.tm_hour) { /* Neuberechnung bei Stundenwechsel */
      hour=tm.tm_hour;
      day=getDay(&tm, &Rate->Day);
      dayMask=1<<(day-1);
      hourMask=1<<tm.tm_hour;
      for (i=0; i<Zone->nHour; i++) {
	Hour = &Zone->Hour[i];
	if ((Hour->Day & dayMask) && (Hour->Hour & hourMask))
	  break;
      }
      if (i==Zone->nHour) {
	if (msg) snprintf(*msg=message, LENGTH, 
			  "no rate found for provider=%d zone=%d day=%d hour=%d", 
			  prefix, zone, day, tm.tm_hour);
	return UNKNOWN;
      }
      Rate->Hour=Hour->Name;
      Unit=Hour->Unit;
    }
    
    Rate->Duration=Unit->Duration;
    Rate->Price=Unit->Price;
    Rate->Charge+=Unit->Price;
    Rate->Rest+=Unit->Duration;
    now+=Unit->Duration;
    run+=Unit->Duration;
    if (Unit->Duration>0) 
      Rate->Units++;
    if (Unit->Delay!=UNKNOWN && Unit->Delay<=run)
      Unit++;
}
  Rate->Rest-=Rate->Time;
  return 0;
}

int getLeastCost (RATE *Rate)
{
  int i, cheaper;
  RATE LC;

  LC=*Rate;
  cheaper=UNKNOWN;

  for (i=0; i<MAXPROVIDER; i++) {
    Rate->prefix=i;
    if (getRate(Rate, NULL)!=UNKNOWN && Rate->Charge<LC.Charge) {
      cheaper=i;
      LC=*Rate;
  }
  }
  *Rate=LC;
  return cheaper;
}

#ifdef STANDALONE
void main (int argc, char *argv[])
{
  char *msg;
  time_t now;
  RATE Rate, LCR;
  int i;

  initHoliday ("../holiday-at.dat", &msg);
  printf ("%s\n", msg);
  
  initRate ("../rate-at.conf", "../rate-at.dat", &msg);
  printf ("%s\n", msg);

  Rate.prefix = 1;
  Rate.zone = 1;
  
  time(&now);
  Rate.start = *localtime(&now);

  while (1) {
    
    time(&now);
    Rate.now = *localtime(&now);
    if (getRate(&Rate, &msg)==UNKNOWN)
      printf ("Oops: %s\n", msg);
    else
      printf ("%02d.%02d.%04d %02d:%02d:%02d  %-5s %-12s %-12s %-12s %6.2f %7.3f %3d %6.2f %3ld %3ld\n", 
	      Rate.now.tm_mday, Rate.now.tm_mon+1, Rate.now.tm_year+1900,
	      Rate.now.tm_hour, Rate.now.tm_min, Rate.now.tm_sec,
	      Rate.Provider, Rate.Zone, Rate.Day, Rate.Hour,
	      Rate.Duration, Rate.Price, Rate.Units, Rate.Charge, Rate.Time, Rate.Rest);

    LCR=Rate;
    if (getLeastCost(&LCR)!=UNKNOWN) {
      printf ("least cost would be: %-5s %-12s %-12s %-12s %6.2f %7.3f %3d %6.2f %3ld %3ld\n", 
	      LCR.Provider, LCR.Zone, LCR.Day, LCR.Hour,
	      LCR.Duration, LCR.Price, LCR.Units, LCR.Charge, LCR.Time, LCR.Rest);
    }
  
    sleep(1);
  }
}
#endif
