/* $Id:
 *
 * ISDN accounting for isdn4linux. (log-module)
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
 * $Log:
 */

/*
 * Schnittstelle zur Tarifdatenbank:
 *
 * void initTarife(char *info)
 *   initialisiert die Tarifdatenbank, liefert Versionsinfo in `info'
 *   zurueck
 *
 * int taktlaenge(int chan, char *why)
 *   liefert fuer die aktuell laufende Verbindung in `chan'
 *   die Laenge eines Tariftaktes in Sekunden, sowie evtl. in `why'
 *   eine textuelle Begruendung
 *
 * char *realProvidername(int prefix)
 *   liefert den Providernamen fuer Kennzahl `prefix', oder NULL, falls
 *   unbekannt
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
 * void exitTarife(void)
 *   deinitialisiert die Tarifdatenbank
 */

#define _TAKT_DE_C_

#ifdef STANDALONE
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#else
#include "isdnlog.h"
#endif

#define SPARBUCH  "/etc/isdn/sparbuch"
#define DATADIR	  "/usr/lib/isdn"

#define TEST        181 /* Sekunden Verbindung kostet? */

#define SHIFT (double)1 /* 1000.0 */

#define MAXDAYS       2
#define MAXSTUNDEN   24
#define MAXPROVIDER 100

#define WT            0 /* Werktag */
#define WE            1 /* Wochenende, Feiertag */
#define FE 	      2 /* Feiertag */
#define ZJ 	      3 /* Werktag 27. .. 30.12. */
#define IMMER        99 /* Werktag, Wochenende, Feiertag */


#ifndef STANDALONE
#define MUTT      3
#define KARF      4
#define OST1      5
#define OST2      6
#define CHRI      7
#define PFI1      8
#define PFI2      9
#define FRON     10
#define EINH     11
#define MARI     12
#define ALLE     13
#define BUSS     14

#define A_FEI 	 17

#define WAEHRUNG "DM"
#define FAKTOR	 1.95583

struct w_ftag {
  char  tag;
  char  monat;
  char  telekom; /* TRUE, wenn auch fuer die Deutsche Telekom ein Feiertag (siehe Telefonbuch!) */
  char *bez;
};


static struct {
  int tag;
  int monat;
  int jahr;
  int tgind;
} _datum;


static char tab_tage[2][12] = {{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	                       { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

static struct w_ftag t_ftag[A_FEI] = {
  {  1,  1, 1, "Neujahr" },
  {  6,  1, 0, "Erscheinungsfest" },   	      /* nur Baden-Wuerttemberg und Bayern */
  {  1,  5, 1, "Maifeiertag" },
  {  0,  0, 1, "Muttertag" },
  {  0,  0, 1, "Karfreitag" },
  {  0,  0, 1, "Ostersonntag" },
  {  0,  0, 1, "Ostermontag" },
  {  0,  0, 1, "Christi Himmelfahrt" },
  {  0,  0, 1, "Pfingstsonntag" },
  {  0,  0, 1, "Pfingstmontag" },
  {  0,  0, 0, "Fronleichnam" },     	      /* nur in Baden-Wuerttemberg, Bayern, Hessen, Nordrhein-Westfalen, Rheinland-Pfalz und im Saarland */
  {  3, 10, 1, "Tag der deutschen Einheit" }, /* vor 1990 am 17.6. */
  { 15,  8, 0, "Maria Himmelfahrt" }, 	      /* nur Saarland und ueberwiegend katholischen Gemeinden Bayerns */
  {  1, 11, 0, "Allerheiligen" },  	      /* nur Baden-Wuerttemberg, Bayern, Nordrhein-Westfalen, Rheinland-Pfalz und im Saarland */
  {  0,  0, 0, "Buss- und Bettag" }, 	      /* nur bis incl. 1994 (wg. Pflegeversicherung abgeschafft) */
  { 25, 12, 1, "1. Weihnachtsfeiertag" },
  { 26, 12, 1, "2. Weihnachtsfeiertag" }};
#endif


typedef struct {
  int    used;
  char  *Provider;
  char	*InternetZugang;
  double Verbindungsentgelt;
  int    takt1[MAXZONES], takt2[MAXZONES];
  double taktpreis[MAXZONES];
  double tarif[MAXZONES][MAXDAYS][MAXSTUNDEN];
  int 	 frei;
} TARIF;

char *zonen[MAXZONES] = { "Intern", "CityCall", "RegioCall", "GermanCall",
       	    	      	  "C-Netz", "C-Mobilbox", "D1-Netz", "D2-Netz",
       	    	  	  "E-plus-Netz", "E2-Netz", "Euro City", "Euro 1",
       	    	  	  "Euro 2", "Welt 1", "Welt 2", "Welt 3", "Welt 4",
       	    	  	  "Internet", "GlobalCall" };


static TARIF t[MAXPROVIDER];
static int   line = 0;
static int   use[MAXPROVIDER];
#ifdef STANDALONE
typedef struct {
  int    prefix;
  double tarif;
} SORT;

static SORT sort[MAXPROVIDER];
#endif

static void warning(char *s)
{
#ifdef STANDALONE
  fprintf(stderr, "WARNING [@%d]: %s\n", line, s);
#else
  print_msg(PRT_NORMAL, "WARNING [@%d]: %s\n", line, s);
#endif
} /* warning */


#ifndef STANDALONE
static int schalt(register int j)
{
  return(((j % 4 == 0) && (j % 100 != 0)) || (j % 400 == 0));
} /* schalt */


static int tag_num(register int t, register int m, register int j)
{
  register char *tm = tab_tage[schalt(j)];

  while (--m)
    t += *tm++;

  return(t);
} /* tag_num */


static void num_tag(int jahr, int lfd)
{
  register int   i;
  register char *t;


  while (lfd < 1)
    lfd += tag_num(31, 12, --jahr);

  t = tab_tage[schalt(jahr)];

  for (i = 1; lfd > *t && i < 13; i++)
    lfd -= *t++;

  if (i > 12)
    num_tag(++jahr, lfd);
  else {
    _datum.monat = i;
    _datum.tag = lfd;
    _datum.jahr = jahr;
  } /* else */
} /* num_tag */


static void comp_feier_tage(int jj)
{
  static   struct w_ftag t_stag[A_FEI];
  static   int 	  	 firsttime = 1;
  static   int		 l_jj = -1;
  register int 		 mm, tt, i, j, a, b;


  if (jj == l_jj)
    return;

  l_jj = jj;

  if (firsttime) {
    for (i = 0; i < A_FEI; i++)
      t_stag[i] = t_ftag[i];

    firsttime = 0;
  }
  else
    for (i = 0; i < A_FEI; i++)
      t_ftag[i] = t_stag[i];


  /* Berechnung von Ostern nach C.F.Gauss */

  i = jj / 100 - jj / 400 + 4;
  j = i - jj / 300 + 11;
  a = (((jj % 19) * 19) + j) % 30;
  b = (((jj % 4) * 2 + (4 * jj) + (6 * a) + i) % 7) + a - 9;

  if (b < 1) {
    tt = 31 + b;
    mm = 3;
  }
  else {
    if ((b == 26) || ((a == 28) && (b == 25) && ((11 * (j + 1) % 30) < 19)))
	b -= 7;
    tt = b;
    mm = 4;
  } /* else */

  num_tag(jj, tag_num(tt, mm, jj));

  t_ftag[OST1].monat = _datum.monat; t_ftag[OST1].tag = _datum.tag;

  num_tag(jj, 1 + tag_num(_datum.tag, _datum.monat, jj));
  t_ftag[OST2].monat = _datum.monat; t_ftag[OST2].tag = _datum.tag;

  num_tag(jj, - 3 + tag_num(_datum.tag, _datum.monat, jj));
  t_ftag[KARF].monat = _datum.monat; t_ftag[KARF].tag = _datum.tag;

  /* Pfingsten */
  num_tag(jj, 51 + tag_num(_datum.tag, _datum.monat, jj));
  t_ftag[PFI1].monat = _datum.monat; t_ftag[PFI1].tag = _datum.tag;
  num_tag(jj, 1 + tag_num(_datum.tag, _datum.monat, jj));
  t_ftag[PFI2].monat = _datum.monat; t_ftag[PFI2].tag = _datum.tag;

  /* Himmelfahrt */
  num_tag(jj, -10 + tag_num(t_ftag[PFI1].tag, t_ftag[PFI1].monat, jj));
  t_ftag[CHRI].monat = _datum.monat; t_ftag[CHRI].tag = _datum.tag;

  /* Fronleichnam */
  num_tag(jj, 11 + tag_num(t_ftag[PFI1].tag, t_ftag[PFI1].monat, jj));
  t_ftag[FRON].monat = _datum.monat; t_ftag[FRON].tag = _datum.tag;
} /* comp_feier_tage */


static int tarifzeit(struct tm *tm, char *why, int cwe)
{
  register int i;


  if ((tm->tm_mday == 24) && (tm->tm_mon == 11)) {
    strcpy(why, "Feiertag (Heilig-Abend)");
    return(FE);
  } /* if */

  if ((tm->tm_mday == 31) && (tm->tm_mon == 11)) {
    strcpy(why, "Feiertag (Sylvester)");
    return(FE);
  } /* if */

  if ((tm->tm_mday > 26) && (tm->tm_mon == 11)) {
    strcpy(why, "Jahresende (27. .. 30.12.)");
    return(ZJ);
  } /* if */

  comp_feier_tage(tm->tm_year + 1900);

  for (i = 0; i < A_FEI; i++)
    if ((t_ftag[i].monat == tm->tm_mon + 1) &&
        (t_ftag[i].tag == tm->tm_mday) &&
         t_ftag[i].telekom) {
      sprintf(why, "Feiertag (%s)", t_ftag[i].bez);
      return(FE);
    } /* if */

  if (tm->tm_wday == 6) {
    strcpy(why, cwe ? "CityWeekend (Samstag)" : "Wochenende (Samstag)");
    return(WE);
  } /* if */

  if (tm->tm_wday == 0) {
    strcpy(why, cwe ? "CityWeekend (Sonntag)" : "Wochenende (Sonntag)");
    return(WE);
  } /* if */

  strcpy(why, "Werktag");
  return(WT);
} /* tarifzeit */
#endif


static int days(register char c)
{
  auto     char sx[BUFSIZ];


  switch (toupper(c)) {
    case 'W' : return(WT);    /* Wochentag */
    case 'E' : return(WE);    /* Wochenende */
    case '*' : return(IMMER); /* jeder Tag */
     default : sprintf(sx, "Unknown day \"%c\", please use W, E or *", c);
               warning(sx);
               return(UNKNOWN);
  } /* switch */
} /* days */


static int zones(register char c)
{
  auto     char sx[BUFSIZ];


  switch (toupper(c)) {
    case '1' : return(CITYCALL);
    case '2' : return(REGIOCALL);
    case '3' : return(GERMANCALL);
    case '4' : return(C_NETZ);
    case '5' : return(C_MOBILBOX);
    case '6' : return(D1_NETZ);
    case '7' : return(D2_NETZ);
    case '8' : return(E_PLUS_NETZ);
    case '9' : return(E2_NETZ);
    case 'A' : return(EURO_CITY);
    case 'B' : return(EURO_1);
    case 'C' : return(EURO_2);
    case 'D' : return(WELT_1);
    case 'E' : return(WELT_2);
    case 'F' : return(WELT_3);
    case 'G' : return(WELT_4);
    case 'H' : return(INTERNET);
     default : sprintf(sx, "Unknown zone \"%c\", please use 1 .. H", c);
               warning(sx);
               return(UNKNOWN);
  } /* switch */
} /* zones */


static int n0(int n)
{
  if (!n)
    return(1);
  else
    return(n);
} /* n0 */


static int zoneknown(int prefix, int zone)
{
  if (!t[prefix].used)
    return(0);

  if ((t[prefix].takt1[zone] == UNKNOWN) &&
      (t[prefix].takt1[zone] == UNKNOWN) &&
      (t[prefix].taktpreis[zone] == (double)UNKNOWN))
    return(0);

  return(1);
} /* zoneknown */


static double tpreis(int prefix, int zone, int day, int hour, int duration)
{
  auto double tarif;
  auto int    takte;
  auto double preis1;


  if (!t[prefix].used)
    return(UNKNOWN);

  tarif = t[prefix].tarif[zone][day][hour];

  if (tarif == UNKNOWN) /* Preis unbekannt oder nicht angeboten */
    return(tarif);

  duration -= t[prefix].frei;

  if (t[prefix].takt1[zone] == UNKNOWN) {  /* Abrechnung nach Takten */
    takte = (duration + tarif) / tarif;
    return((takte * t[prefix].taktpreis[zone]) + t[prefix].Verbindungsentgelt);
  }
  else {                        /* Abrechnung nach Preis/Minute */

#if 0 /* AK:FALSCH! So wird bei 60/60 aus z.b. 5 Sekunden immer 2 Takte! */
    if (t[prefix].takt1[zone] > 1) {  /* Mindestdauer pro Verbindung */
      if (duration < t[prefix].takt1[zone])
        duration = t[prefix].takt1[zone];
    } /* if */
#endif

    if (t[prefix].takt2[zone] == 1)   /* Abrechnung Sekundengenau */
      return((tarif * duration / 60) + t[prefix].Verbindungsentgelt);

    /* Abrechnung in "takt2" Einheiten */
    takte = (duration + t[prefix].takt2[zone]) / n0(t[prefix].takt2[zone]);

    if (t[prefix].takt2[zone] == 60)  /* Abrechnung Minutengenau */
      return((tarif * takte) + t[prefix].Verbindungsentgelt);

    preis1 = tarif / (60 / n0(t[prefix].takt2[zone]));
    return((takte * preis1) + t[prefix].Verbindungsentgelt);
  } /* else */
} /* tpreis */


#if 0 /* ndef STANDALONE */
static double preisgenau(int prefix, int zone, int day, int hour, int duration, time_t dialin)
{
  /*
    wenn dialin in einer anderen Tarifzeit liegt, als (dialin + duration)
      1. Preis fuer Zeitanteil in der 1. Tarifzeit
      2. Preis fuer Zeitanteil in weiteren Tarifzeiten berechnen
    return(Summe)
  */
  return(UNKNOWN);
} /* preisgenau */
#endif


#ifndef STANDALONE
int taktlaenge(int chan, char *why)
{
  auto double tarif;
  auto struct tm *tm;


  if ((call[chan].sondernummer[CALLED] != UNKNOWN) &&
      (call[chan].provider == DTAG) &&
      ((call[chan].zone < C_NETZ) || (call[chan].zone > E2_NETZ)) &&
       !SN[call[chan].sondernummer[CALLED]].tarif) {
    strcpy(why, "FreeCall");
    return(60 * 60 * 24);              /* one day should be enough ;-) */
  } /* if */

  *why = 0;

  if (!t[call[chan].provider].used) {
    strcpy(why, "NO CHARGE INFOS FOR THIS PROVIDER");
    return(UNKNOWN);
  } /* if */

  tm = localtime(&call[chan].connect);
  tarif = t[call[chan].provider].tarif[call[chan].zone][call[chan].tz][tm->tm_hour];

  if (tarif == UNKNOWN) { /* Preis unbekannt oder nicht angeboten */
    strcpy(why, "UNKNOWN TARIF");
    return(UNKNOWN);
  } /* if */

  if (t[call[chan].provider].takt1[call[chan].zone] == UNKNOWN) /* Abrechnung nach Takten */
    return(tarif);
  else if (t[call[chan].provider].takt2[call[chan].zone] < 10) { /* Wenn Takt < 10 Sekunden, 1 Minute! */
    sprintf(why, "TRUE charging is %d/%d", t[call[chan].provider].takt1[call[chan].zone], t[call[chan].provider].takt2[call[chan].zone]);
    return(60);
  }
  else
    return(t[call[chan].provider].takt2[call[chan].zone]);
} /* taktlaenge */


char *realProvidername(int prefix)
{
  if (!t[prefix].used)
    return(NULL);

  return(t[prefix].Provider);
} /* realProvidername */


void preparecint(int chan, char *msg, char *hint)
{
  register int    zone = UNKNOWN, provider = UNKNOWN, tz, i, cheapest = UNKNOWN;
  auto 	   struct tm *tm;
  auto	   char	  why[BUFSIZ], s[BUFSIZ];
  auto	   double tarif = 0.0, tarif1, providertarif = 0.0, cheaptarif;


  *hint = 0;
  tm = localtime(&call[chan].connect);

  if (call[chan].intern[CALLED]) {
    call[chan].zone = INTERN;
    call[chan].tarifknown = 0;
    sprintf(msg, "CHARGE: free of charge - internal call");
    return;
  } /* if */

  provider = ((call[chan].provider == UNKNOWN) ? preselect : call[chan].provider);

  if ((call[chan].sondernummer[CALLED] != UNKNOWN) &&      /* Sonderrufnummer, Abrechnung zum CityCall-Tarif */
      (SN[call[chan].sondernummer[CALLED]].tarif == 1) &&
      (provider == DTAG))
    zone = CITYCALL;
  else if ((call[chan].sondernummer[CALLED] != UNKNOWN) && /* Sonderrufnummer, kostenlos */
      (SN[call[chan].sondernummer[CALLED]].tarif == 0) &&
      (provider == DTAG)) {
    call[chan].zone = CITYCALL;
    call[chan].tarifknown = 0;
    sprintf(msg, "CHARGE: free of charge - FreeCall");
    return;
  }
  else if (!memcmp(call[chan].num[CALLED], "01610", 5) ||
           !memcmp(call[chan].num[CALLED], "01617", 5) ||
           !memcmp(call[chan].num[CALLED], "01619", 5))
    zone = C_NETZ;
  else if (!memcmp(call[chan].num[CALLED], "01618", 5))
    zone = C_MOBILBOX;
  else if (!memcmp(call[chan].num[CALLED], "0170", 4) ||
           !memcmp(call[chan].num[CALLED], "0171", 4))
    zone = D1_NETZ;
  else if (!memcmp(call[chan].num[CALLED], "0172", 4) ||
           !memcmp(call[chan].num[CALLED], "0173", 4))
    zone = D2_NETZ;
  else if (!memcmp(call[chan].num[CALLED], "0177", 4) ||
           !memcmp(call[chan].num[CALLED], "0178", 4))
    zone = E_PLUS_NETZ;
  else if (!memcmp(call[chan].num[CALLED], "0176", 4) ||
           !memcmp(call[chan].num[CALLED], "0179", 4))
    zone = E2_NETZ;
  else if (t[provider].used && (t[provider].InternetZugang != NULL) && !strcmp(call[chan].onum[CALLED], t[provider].InternetZugang))
    zone = INTERNET;
  else {
    zone = area_diff(NULL, call[chan].num[CALLED]);

    if ((zone == AREA_ERROR) || (zone == AREA_UNKNOWN))
      zone = UNKNOWN;
    else if (zone == AREA_ABROAD) /* FIXME: muss noch stark verbessert werden! */
      zone = GLOBALCALL;
  } /* else */

  tz = tarifzeit(tm, why, ((provider == DTAG) && CityWeekend));

  if ((tz == FE) || (tz == ZJ)) /* FIXME: stimmt das bei allen Providern? */
    tz = WE;

  if (zone != UNKNOWN)
    tarif = t[provider].tarif[zone][tz][tm->tm_hour];

  call[chan].zone = zone;
  call[chan].provider = provider;
  call[chan].tz = tz;

  if (zoneknown(provider, zone)) {
    if (t[provider].takt1[zone] == UNKNOWN)
      sprintf(s, "%s %s/%ss", WAEHRUNG,
    	       	   	      double2str(t[provider].taktpreis[zone], 5, 3, DEB),
    	       	   	      double2str(tarif, 5, 1, DEB));
  else
      sprintf(s, "%s %s/Min, Takt %d/%d", WAEHRUNG, double2str(tarif, 5, 3, DEB),
    	       	   	   		  t[provider].takt1[zone], t[provider].takt2[zone]);

  sprintf(msg, "CHARGE: %s, %s, %s", why, ((zone == UNKNOWN) ? "Unknown zone" : zonen[zone]), s);
    call[chan].tarifknown = 1;
  }
  else {
    sprintf(msg, "CHARGE: Oppps: No charge infos for provider %d, Zone %d %s",
      provider, zone,
      ((call[chan].sondernummer[CALLED] != UNKNOWN) ? SN[call[chan].sondernummer[CALLED]].sinfo : ""));

    call[chan].tarifknown = 0;
  } /* else */

  cheaptarif = 99999.9;
  call[chan].hint = UNKNOWN;

  if (zone != UNKNOWN) {
    for (i = 0; i < MAXPROVIDER; i++) {
      if (t[i].used) {

        tarif1 = tpreis(i, zone, tz, tm->tm_hour, TEST);

        if (i == provider)
          providertarif = tarif1;

        if ((tarif1 > 0.0) && (tarif1 < cheaptarif)) {
          cheaptarif = tarif1;
          cheapest = i;
        } /* if */
      } /* if */
    } /* for */

    if ((cheapest != UNKNOWN) && (cheaptarif < providertarif)) {
      tarif = t[cheapest].tarif[zone][tz][tm->tm_hour];

      if (t[cheapest].takt1[zone] == UNKNOWN)
        sprintf(s, "%s %s/%ss", WAEHRUNG, double2str(t[cheapest].taktpreis[zone], 5, 3, DEB),
        	       		double2str(tarif, 5, 1, DEB));
      else
        sprintf(s, "%s %s/Min, Takt %d/%d", WAEHRUNG, double2str(tarif, 5, 3, DEB),
        	       	       	    	    t[cheapest].takt1[zone], t[cheapest].takt2[zone]);

      sprintf(hint, "HINT: Better use 010%02d:%s, %s, saving %s %s/%ds",
        cheapest, t[cheapest].Provider, s, WAEHRUNG,
        double2str(providertarif - cheaptarif, 5, 3, DEB), TEST);

      call[chan].hint = cheapest;
    } /* if */
  } /* if */

} /* preparecint */


void price(int chan, char *hint)
{
  auto	   int	  duration = (int)(call[chan].disconnect - call[chan].connect);
  auto     double pay2 = -1.0, onesec;
  auto     char   sx[BUFSIZ], sy[BUFSIZ], sz[BUFSIZ];
  auto 	   struct tm *tm;
  register int 	  p, cheapest = UNKNOWN;
  auto     double payx, payy, prepreis = -1.0, hintpreis = -1.0;
#if DEBUG
  auto	   FILE	 *fo;
  auto	   double spar = 0.0;
#endif


  *hint = 0;

  if (call[chan].zone == INTERN) {
    call[chan].pay = 0.0;
    return;
  } /* if */

  if (OUTGOING && (duration > 0) && *call[chan].num[CALLED]) {

    tm = localtime(&call[chan].connect);

    if ((call[chan].sondernummer[CALLED] != UNKNOWN) &&
        (call[chan].provider == DTAG) &&
        ((call[chan].zone < C_NETZ) || (call[chan].zone > E2_NETZ))) {
      switch (SN[call[chan].sondernummer[CALLED]].tarif) {
        case -1 : if (!strcmp(call[chan].num[CALLED] + 3, "11833")) /* Sonderbedingung Auskunft Inland */
                    duration -= 30;

                  pay2 = SN[call[chan].sondernummer[CALLED]].grund1 * currency_factor;
                  pay2 += (duration / SN[call[chan].sondernummer[CALLED]].takt1) * currency_factor;
                  break;

        case  0 : pay2 = 0.0;
                  break;
      } /* switch */
    } /* if */

    if (pay2 == -1.0) {

#if 0 /* auch Telekom _berechnen_ ! */
      if (call[chan].aoce > 0) /* Gebuehrentakt AOC-E kam (Komfortanschluss, via Telekom */
        call[chan].pay = call[chan].aoce * currency_factor;
      else
#endif
        call[chan].pay = tpreis(call[chan].provider, call[chan].zone, call[chan].tz, tm->tm_hour, duration);

      if ((duration > 600) && (call[chan].zone > CITYCALL) && (call[chan].provider == DTAG)) {
        onesec = call[chan].pay / duration;
        pay2 = (duration - 600) * onesec * 0.30;

        sprintf(sx, "10plus %s %s - %s %s = %s %s",
          WAEHRUNG,
          double2str(call[chan].pay, 6, 2, DEB),
          WAEHRUNG,
          double2str(pay2, 6, 2, DEB),
          WAEHRUNG,
          double2str(call[chan].pay - pay2, 6, 2, DEB));

        call[chan].pay -= pay2;

        /* print_msg(PRT_NORMAL, sx); FIXME */
      } /* if */
    }
    else
      call[chan].pay = pay2;


    if (call[chan].tarifknown) {
    cheapest = UNKNOWN;
    payx = 99999.9;

    for (p = 0; p < MAXPROVIDER; p++) {
      payy = tpreis(p, call[chan].zone, call[chan].tz, tm->tm_hour, duration);

      if (p == preselect)
        prepreis = payy;

        if (p == call[chan].hint)
          hintpreis = payy;

      if ((payy > 0) && (payy < payx)) {
        payx = payy;
        cheapest = p;
      } /* if */
    } /* for */

    *sx = *sy = *sz = 0;

    if ((cheapest != UNKNOWN) && (cheapest != call[chan].provider))
        sprintf(sx, "Cheapest 010%02d:%s %s %s, more payed %s %s",
              cheapest, t[cheapest].Provider, WAEHRUNG,
              double2str(payx, 6, 3, DEB),
              WAEHRUNG,
              double2str(call[chan].pay - payx, 6, 3, DEB));

    if ((call[chan].provider != preselect) && (prepreis != -1.00))
        sprintf(sy, " saving vs. preselect (010%02d:%s) %s %s",
        preselect, t[preselect].Provider,
          WAEHRUNG,
          double2str(prepreis - call[chan].pay, 6, 3, DEB));

      if ((call[chan].hint != UNKNOWN) && (call[chan].hint != cheapest))
        sprintf(sz, " saving vs. hint (010%02d:%s) %s %s",
          call[chan].hint, t[call[chan].hint].Provider,
          WAEHRUNG,
          double2str(hintpreis - call[chan].pay, 6, 3, DEB));

    if (*sx || *sy || *sz)
        sprintf(hint, "HINT: %s%s%s LCR:%s", sx, sy, sz, ((cheapest == call[chan].provider) ? "OK" : "FAILED"));

#if DEBUG
    if ((fo = fopen(SPARBUCH, "r")) != (FILE *)NULL) {
      fscanf(fo, "%lg", &spar);
      fclose(fo);
    } /* if */

    spar += (prepreis - call[chan].pay);

    if ((fo = fopen(SPARBUCH, "w")) != (FILE *)NULL) {
      fprintf(fo, "%g", spar);
      fclose(fo);
    } /* if */
#endif

    } /* if */
  } /* if */
} /* price */
#endif


void initTarife(char *msg)
{
  register char  *p, *p1;
  auto     char   s[BUFSIZ], sx[BUFSIZ], Version[BUFSIZ], infos[BUFSIZ], fn[BUFSIZ];
  auto     FILE  *fi;
  auto     int    prefix = UNKNOWN, zone1 = UNKNOWN, zone2 = UNKNOWN;
  auto     int    day, hour1, hour2, version = UNKNOWN, ignore = 0;
  auto	   int	  d, h, z;
  auto     int    nprovider = 0;
  auto     double pay1, pay2;
  auto     double taktpreis = (double)UNKNOWN;
  auto     int    takt1 = UNKNOWN, takt2 = UNKNOWN;


  *msg = *infos = 0;
  line = 0;

  for (d = 0; d < MAXPROVIDER; d++) {
    t[d].used = 0;

    for (z = 0; z < MAXZONES; z++) {
      t[d].takt1[z] = t[d].takt1[z] = UNKNOWN;
      t[d].taktpreis[z] = (double)UNKNOWN;
    } /* for */
  } /* for */

  sprintf(fn, "%s/tarif.conf", I4LCONFDIR);

  if ((fi = fopen(fn, "r")) != (FILE *)NULL) {
    while (fgets(s, BUFSIZ, fi)) {
      line++;

      if ((p = strchr(s, '#')))
        *p = 0;

      if ((p = strchr(s, '\n')))
        *p = 0;

      if (*s) {
        switch (*s) {
          case 'P' : if (s[4] == '=') {
                       prefix = atoi(s + 2);

                       if ((prefix >= 0) && (prefix < MAXPROVIDER))
                         use[prefix] = atoi(s + 5) + 1;
                       else {
                         sprintf(sx, "Invalid provider-number %d", prefix);
                         warning(sx);
                       } /* else */
          	     }
                     else
                       warning("Syntax error ('=' expected!)");
                     break;

           default : sprintf(sx, "Unknown tag %c", *s);
                     warning(sx);
                     break;
        } /* switch */
      } /* if */
    } /* while */

    fclose(fi);
  } /* if */

  line = 0;
  prefix = UNKNOWN;

  sprintf(fn, "%s/tarif.dat", DATADIR);

  if ((fi = fopen(fn, "r")) != (FILE *)NULL) {

    while (fgets(s, BUFSIZ, fi)) {
      line++;

      if ((p = strchr(s, '#')))
        *p = 0;

      if ((p = strchr(s, '\n')))
        *p = 0;

      if (*s) {

        switch (*s) {
          case 'P' : zone1 = zone2 = UNKNOWN; /* P:nn[,v]=Bezeichnung (Providervorwahl, Name) */
	       	     ignore = 0;
	       	     version = UNKNOWN;

                     if ((s[4] == '=') || (s[6] == '=')) {

                       prefix = atoi(s + 2);

                       if (s[4] == ',') {
                         version = atoi(s + 5) + 1;
			 p = s + 7;
                       }
                       else
                         p = s + 5;

                       if ((prefix >= 0) && (prefix < MAXPROVIDER)) {

                         if (use[prefix] && (version != UNKNOWN) && (use[prefix] != version)) {
                           ignore = 1;
                           version = UNKNOWN;
                           break;
                         } /* if */

                         if (++t[prefix].used > 1) {
                           sprintf(sx, "Duplicate entry for provider %d (%s)", prefix, t[prefix].Provider);
                       	   warning(sx);
                           free(t[prefix].Provider);
                           free(t[prefix].InternetZugang);
                         } /* if */

                         t[prefix].Provider = strdup(p);
                         t[prefix].Verbindungsentgelt = 0.0;

                         for (z = 0; z < MAXZONES; z++)
                           for (d = 0; d < MAXDAYS; d++)
                             for (h = 0; h < MAXSTUNDEN; h++)
                               t[prefix].tarif[z][d][h] = UNKNOWN;

                         sprintf(sx, "%02d", prefix);

                         if (*infos)
                           strcat(infos, ",");

                         strcat(infos, sx);
                         nprovider++;
                       }
                       else {
                         sprintf(sx, "Invalid provider-number %d", prefix);
                         warning(sx);

                         prefix = UNKNOWN;
                       } /* else */
                     }
                     else
                       warning("Syntax error ('=' expected!)");
                     break;

          case 'G' : if (!ignore) { /* P:tt.mm.jjjj Tarif gueltig ab */
	       	       break;
          	     } /* if */
                     break;

          case 'C' : if (!ignore) { /* C:Comment */
	       	       break;
          	     } /* if */
                     break;

          case '+' : if (!ignore) { /* +:nnn Verbindungsentgelt pro Gespraech */
               	       if (prefix == UNKNOWN) {
                         warning("Unexpected tag 'I'");
                         break;
                       } /* if */

                       t[prefix].Verbindungsentgelt = atof(s + 2);
                     } /* if */
                     break;

          case '-' : if (!ignore) { /* -:nnn kostenlose Sekunden */
                       if (prefix == UNKNOWN) {
                         warning("Unexpected tag 'I'");
                         break;
                       } /* if */

                       t[prefix].frei = atoi(s + 2);
          	     } /* if */
                     break;

          case 'I' : if (!ignore) { /* I:nnn Internet-Zugangsnummer */
                       if (prefix == UNKNOWN) {
                         warning("Unexpected tag 'I'");
                         break;
                       } /* if */

		       t[prefix].InternetZugang = strdup(s + 2);
          	     } /* if */
                     break;

          case 'Z' : if (!ignore) { /* Z:n[-n] Zone */
               	       if (prefix == UNKNOWN) {
                         warning("Unexpected tag 'Z'");
                         break;
                       } /* if */

                       p = s + 2;

                       if ((zone1 = zone2 = zones(*p)) == UNKNOWN)
                         break;

                       p++;

                       if (*p == '-') {
                         p++;
                         if ((zone2 = zones(*p)) == UNKNOWN)
                           break;
                         p++;
                       } /* if */

                       if (*p == ',') { /* Taktung */
                         if ((p1 = strchr(p + 1, '/'))) {
                           *p1 = 0;
                           takt1 = atoi(p + 1);
                           takt2 = atoi(p1 + 1);
                           taktpreis = (double)UNKNOWN;
                         }
                         else {
                           taktpreis = atof(p + 1);
                           takt1 = takt2 = UNKNOWN;
                         } /* else */
                       }
                       else
                         warning("Missing \",Taktung\"");

          	     } /* if */
                     break;

          case 'T' : if (!ignore) { /* T:axx-yy=0.12 Preis */
               	       if (zone1 == UNKNOWN) {
                         warning("Unexpected tag 'T'");
                         break;
                       } /* if */

                       p = s + 2;

                       if ((day = days(*p)) == UNKNOWN)
                         break;

                       p++;

                       if (*p != '=') {
                         hour1 = atoi(p);

                         if ((hour1 < 0) || (hour1 > 23)) {
                           sprintf(sx, "Invalid hour %d", hour1);
                           warning(sx);
                           break;
                         } /* if */

                         p += 2;

                         if (*p == '-') {
                           p++;

                           hour2 = atoi(p);

                           if ((hour2 < 0) || (hour2 > 23)) {
                             sprintf(sx, "Invalid hour %d", hour2);
                             warning(sx);
                             break;
                           } /* if */

                           p += 2;
                         }
                         else
                           hour2 = hour1;
                       }
                       else {
                         hour1 = 0;
                         hour2 = 24;
                       } /* else */

                       if (*p == '=') {
                         pay1 = atof(p + 1);
                         pay2 = pay1;

                         hour2--;

                         for (z = zone1; z <= zone2; z++) {

                           t[prefix].takt1[z] = takt1;
                           t[prefix].takt2[z] = takt2;
                           t[prefix].taktpreis[z] = taktpreis;

                           h = hour1;

                           while (h != (hour2 + 1)) {

                             if (h == 24)
                               h = 0;

                             if (day == IMMER)
                               t[prefix].tarif[z][0][h] =
                               t[prefix].tarif[z][1][h] = pay2;
                             else
                               t[prefix].tarif[z][day][h] = pay2;

                             h++;
                           } /* while */
                         } /* for */
                       }
                       else
                         warning("Syntax error ('=' expected!)");
                     } /* if */
                     break;

          case 'V' : strcpy(Version, s + 2); /* V:xxx Version der Tarifdatenbank */
               	     break;

           default : sprintf(sx, "Unknown tag %c", *s);
                     warning(sx);
                     break;
        } /* switch */
      } /* if */
    } /* while */

    fclose(fi);
    sprintf(msg, "Tarife Version %s loaded [%d Provider (%s), %d Tarife]",
      Version, nprovider, infos, nprovider * MAXZONES * MAXDAYS * MAXSTUNDEN);
  } /* if */
} /* initTarife */


void exitTarife()
{
  register int i;


  for (i = 0; i < MAXPROVIDER; i++)
    if (t[i].used) {
      free(t[i].Provider);
      free(t[i].InternetZugang);
    } /* if */
} /* exitTarife */


void initSondernummern()
{
  register char  *p1, *p2, *p3;
  register int    tarif;
  auto 	   FILE  *f;
  auto	   char   s[BUFSIZ], msn[128], sinfo[256], linfo[256], fn[BUFSIZ];
  auto	   double grund1, grund2, takt1, takt2;


  sprintf(fn, "%s/sonderrufnummern.dat", DATADIR);

  if ((f = fopen(fn, "r")) != (FILE *)NULL) {
    while ((p1 = fgets(s, BUFSIZ, f))) {
      if (*p1 != '#') {
        if ((p2 = strchr(p1, '|'))) {
          *p2 = 0;

          p3 = p2 - 1;
          while (*p3 == ' ')
            *p3-- = 0;

          strcpy(msn, p1);
          p1 = p2 + 1;
          if ((p2 = strchr(p1, '|'))) {
            *p2 = 0;

            if (!strcmp(p1, "City"))
              tarif = 1;
            else if (!strcmp(p1, "free"))
              tarif = 0;
            else
              tarif = UNKNOWN;

            p1 = p2 + 1;
            if ((p2 = strchr(p1, '|'))) {
              *p2 = 0;
              grund1 = atof(p1);
              p1 = p2 + 1;
              if ((p2 = strchr(p1, '|'))) {
                *p2 = 0;
                grund2 = atof(p1);
                p1 = p2 + 1;
                if ((p2 = strchr(p1, '|'))) {
                  *p2 = 0;
                  takt1 = atof(p1);
                  p1 = p2 + 1;
                  if ((p2 = strchr(p1, '|'))) {
                    *p2 = 0;
              	    takt2 = atof(p1);
              	    p1 = p2 + 1;
              	    if ((p2 = strchr(p1, '|'))) {
                      *p2 = 0;

        	      p3 = p2 - 1;
        	      while (*p3 == ' ')
          	        *p3-- = 0;

        	      while (*p1 == ' ')
                        p1++;

        	      strcpy(sinfo, p1);
                      p1 = p2 + 1;
                      if ((p2 = strchr(p1, '\n'))) {
                        *p2 = 0;

        	        p3 = p2 - 1;
        	        while (*p3 == ' ')
          	          *p3-- = 0;

        	        while (*p1 == ' ')
                          p1++;

                        strcpy(linfo, p1);

		        nSN++;
                        SN = realloc(SN, sizeof(SonderNummern) * nSN);
                        SN[nSN - 1].msn = strdup(msn);
                        SN[nSN - 1].sinfo = strdup(sinfo);
        	        SN[nSN - 1].tarif = tarif;
                        SN[nSN - 1].grund1 = grund1;
                        SN[nSN - 1].grund2 = grund2;
                        SN[nSN - 1].takt1  = takt1;
                        SN[nSN - 1].takt2  = takt2;
                      } /* if */
              	    } /* if */
                  } /* if */
                } /* if */
              } /* if */
            } /* if */
          } /* if */
        } /* if */
      } /* if */
    } /* while */

    fclose(f);
  } /* if */
} /* initSondernummern */


int is_sondernummer(char *num)
{
  register int i;


  if ((strlen(num) >= interns0) && ((*num == '0') || (*num == '1')))
    for (i = 0; i < nSN; i++)
      if (!strncmp(num, SN[i].msn, strlen(SN[i].msn)))
        return(i);

  return(-1);
} /* sondernummer */


#ifdef STANDALONE

int compare(const SORT *s1, const SORT *s2)
{
  return(s1->tarif > s2->tarif);
} /* compare */


int main(int argc, char *argv[], char *envp[])
{
  register int    prefix, z, d, h, n = 0, n1, cheapest = UNKNOWN;
  auto     char   why[BUFSIZ], s[BUFSIZ];
  auto	   double cheaptarif, providertarif, tarif;


  printf("Initializing ...\n");

  initTarife(why);

  if (*why)
    printf("%s\n", why);

  printf("sizeof(t) = %d\n\n", sizeof(t));

  if (argc > 1) {
    cheaptarif = 99999.9;
    z = GERMANCALL; /* CITYCALL; */

  for (prefix = 0; prefix < MAXPROVIDER; prefix++) {
    if (t[prefix].used) {

        tarif = tpreis(prefix, z, WT, 12, TEST);

	if (prefix == DTAG)
          providertarif = tarif;

        if ((tarif > 0.0) && (tarif < cheaptarif)) {
          cheaptarif = tarif;
          cheapest = prefix;
        } /* if */

	sort[n].prefix = prefix;
    	sort[n].tarif = tarif;
        n++;

      } /* if */
    } /* for */

    if (cheapest != UNKNOWN) {
      tarif = t[cheapest].tarif[z][WT][12];

      if (t[cheapest].takt1[z] == UNKNOWN)
        sprintf(s, "DM %5.3f/%7.3fs", t[cheapest].taktpreis[z], tarif);
      else
        sprintf(s, "DM %5.3f/Min, Takt %d/%d", tarif, t[cheapest].takt1[z], t[cheapest].takt2[z]);

      printf("Use 010%02d:%s, %s, costs DM %7.3f, saving DM %7.3f/%ds vs. DTAG\n",
        cheapest, t[cheapest].Provider, s,
        cheaptarif,
        providertarif - cheaptarif, TEST);

    } /* if */

    qsort(sort, n, sizeof(SORT), compare);

    for (n1 = 0; n1 < n; n1++)
      printf("010%02d:%s\t\tDM %5.3f\n", sort[n1].prefix, t[sort[n1].prefix].Provider, sort[n1].tarif);

  }
  else { /* dump */
    for (prefix = 0; prefix < MAXPROVIDER; prefix++) {
      if (t[prefix].used) {
        printf("PROVIDER:%s (010%02d)", t[prefix].Provider, prefix);

        if (t[prefix].Verbindungsentgelt)
          printf(" (zzgl. DM %6.3f/Verbindung)", t[prefix].Verbindungsentgelt);
        if (t[prefix].frei)
          printf(" (die ersten %d Sekunden frei)", t[prefix].frei);

        printf("\n");

        for (z = 0; z < MAXZONES; z++) {
          if (zoneknown(prefix, z)) {

            printf("\tZone:%s:", zonen[z]);

            if (t[prefix].takt1[z] == UNKNOWN)
              printf(" [Takt DM %6.3f/x s]\n", t[prefix].taktpreis[z]);
            else
              printf(" [Takt %d/%d]\n", t[prefix].takt1[z], t[prefix].takt2[z]);

          for (d = 0; d < MAXDAYS; d++) {
            printf("\t\t%s:\n", (d ? "Wochenende" : "Wochentag"));

            for (h = 0; h < MAXSTUNDEN; h++)
                printf("\t\t\t%2d Uhr: %10.3f %s, %10.3f DM\n",
                  h, t[prefix].tarif[z][d][h],
                  ((t[prefix].takt1[z] == UNKNOWN) ? "s" : "DM"),
                  tpreis(prefix, z, d, h, TEST));
          } /* for */
          } /* if */
      } /* for */
    } /* if */
  } /* for */

  printf("DIENSTAG, 16:00 Uhr, %d Sekunden:\n", TEST);

  for (prefix = 0; prefix < MAXPROVIDER; prefix++) {
    if (t[prefix].used) {

        printf("%s (010%02d):\n", t[prefix].Provider, prefix);

        if (zoneknown(prefix, CITYCALL))
      printf("\t\t\tCITY    : %6.3f DM\n", tpreis(prefix, CITYCALL, WT, 16, TEST));

        if (zoneknown(prefix, REGIOCALL))
      printf("\t\t\tREGIO   : %6.3f DM\n", tpreis(prefix, REGIOCALL, WT, 16, TEST));

        if (zoneknown(prefix, GERMANCALL))
      printf("\t\t\tFERN    : %6.3f DM\n", tpreis(prefix, GERMANCALL, WT, 16, TEST));

        if (zoneknown(prefix, D2_NETZ))
      printf("\t\t\tD2	    : %6.3f DM\n", tpreis(prefix, D2_NETZ, WT, 16, TEST));

        if (zoneknown(prefix, INTERNET))
      printf("\t\t\tINTERNET: %6.3f DM\n", tpreis(prefix, INTERNET, WT, 16, TEST));
    } /* if */
  } /* for */
  } /* else */

  exitTarife();

  return(0);
} /* main */
#endif
