/*
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

#ifdef 0 /* STANDALONE */
#define TARIFE    "tarif.dat"
#define TARIFCONF "tarif.conf"
#define SPARBUCH  "sparbuch"
#else
#define TARIFE    "/usr/lib/isdn/tarif.dat"
#define TARIFCONF "/etc/isdn/tarif.conf"
#define SPARBUCH  "/etc/isdn/sparbuch"
#endif

#define TEST        181 /* Sekunden Verbindung kostet? */

#define SHIFT (double)1000.0
#define UNKNOWN      -1

#define MAXZONEN     17
#define MAXDAYS       2
#define MAXSTUNDEN   24
#define MAXPROVIDER 100

#define CITYCALL      0
#define REGIOCALL     1
#define GERMANCALL    2
#define C_NETZ        3
#define C_MOBILBOX    4
#define D1_NETZ	      5
#define D2_NETZ       6
#define E_PLUS_NETZ   7
#define E2_NETZ       8
#define EURO_CITY     9
#define EURO_1       10
#define EURO_2       11
#define WELT_1       12
#define WELT_2       13
#define WELT_3       14
#define WELT_4       15
#define INTERNET     16

#define WT            0 /* Werktag */
#define WE            1 /* Wochenende, Feiertag */
#define FE 	      2 /* Feiertag */
#define ZJ 	      3 /* Werktag 27. .. 30.12. */
#define IMMER        99 /* Werktag, Wochenende, Feiertag */

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


typedef struct {
  int    used;
  char  *Provider;
  char	*InternetZugang;
  int    takt1, takt2;
  double taktpreis;
  int    tarif[MAXZONEN][MAXDAYS][MAXSTUNDEN];
} TARIF;

static char *zonen[] = { "City", "Region 50", "Fern", "C-Netz", "C-Mobilbox",
                         "D1-Netz", "D2-Netz", "E-plus-Netz", "E2-Netz",
                         "Euro City", "Euro 1", "Euro 2", "Welt 1", "Welt 2",
                         "Welt 3", "Welt 4", "Internet" };


static TARIF t[MAXPROVIDER];
static int   line = 0;
static int   use[MAXPROVIDER];


static void warning(char *s)
{
#ifdef STANDALONE
  fprintf(stderr, "WARNING [@%d]: %s\n", line, s);
#else
  print_msg(PRT_NORMAL, "WARNING [@%d]: %s\n", line, s);
#endif
} /* warning */


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
    case '0' : return( 0); /* City        (CityCall)   */
    case '1' : return( 1); /* Region 50   (RegioCall)  */
    case '2' : return( 2); /* Fern      (GermanCall)   */
    case '3' : return( 3); /* C-Netz                   */
    case '4' : return( 4); /* C-Mobilbox               */
    case '5' : return( 5); /* D1-Netz                  */
    case '6' : return( 6); /* D2-Netz                  */
    case '7' : return( 7); /* E-plus-Netz              */
    case '8' : return( 8); /* E2-Netz                  */
    case '9' : return( 9); /* Euro City                */
    case 'A' : return(10); /* Euro 1                   */
    case 'B' : return(11); /* Euro 2                   */
    case 'C' : return(12); /* Welt 1                   */
    case 'D' : return(13); /* Welt 2                   */
    case 'E' : return(14); /* Welt 3                   */
    case 'F' : return(15); /* Welt 4                   */
    case 'G' : return(16); /* Internet		       */
     default : sprintf(sx, "Unknown zone \"%c\", please use 0 .. G", c);
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


static double tpreis(int prefix, int zone, int day, int hour, int duration)
{
  auto double tarif;
  auto int    takte;
  auto double preis1;


  if (!t[prefix].used)
    return(UNKNOWN);

  tarif = t[prefix].tarif[zone][day][hour] / SHIFT;

  if (tarif == UNKNOWN) /* Preis unbekannt oder nicht angeboten */
    return(tarif);

  if (t[prefix].takt1 == UNKNOWN) {  /* Abrechnung nach Takten */
    takte = (duration + tarif) / tarif;
    return(takte * t[prefix].taktpreis);
  }
  else {                        /* Abrechnung nach Preis/Minute */

#if 0 /* AK:FALSCH! So wird bei 60/60 aus z.b. 5 Sekunden immer 2 Takte! */
    if (t[prefix].takt1 > 1) {  /* Mindestdauer pro Verbindung */
      if (duration < t[prefix].takt1)
        duration = t[prefix].takt1;
    } /* if */
#endif

    if (t[prefix].takt2 == 1)   /* Abrechnung Sekundengenau */
      return(tarif * duration / 60);

    /* Abrechnung in "takt2" Einheiten */
    takte = (duration + t[prefix].takt2) / n0(t[prefix].takt2);

    if (t[prefix].takt2 == 60)  /* Abrechnung Minutengenau */
      return(tarif * takte);

    preis1 = tarif / (60 / n0(t[prefix].takt2));
    return(takte * preis1);
  } /* else */
} /* tpreis */


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


#ifndef STANDALONE
int taktlaenge(int chan, char *why)
{
  auto double tarif;
  auto struct tm *tm;


  if ((call[chan].sondernummer[CALLED] != UNKNOWN) &&
      (call[chan].provider == 33) &&
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
  tarif = t[call[chan].provider].tarif[call[chan].zone][call[chan].tz][tm->tm_hour] / SHIFT;

  if (tarif == UNKNOWN) { /* Preis unbekannt oder nicht angeboten */
    strcpy(why, "UNKNOWN TARIF");
    return(UNKNOWN);
  } /* if */

  if (t[call[chan].provider].takt1 == UNKNOWN) /* Abrechnung nach Takten */
    return(tarif);
  else if (t[call[chan].provider].takt2 < 10) { /* Wenn Takt < 10 Sekunden, 1 Minute! */
    sprintf(why, "TRUE charging is %d/%d", t[call[chan].provider].takt1, t[call[chan].provider].takt2);
    return(60);
  }
  else
    return(t[call[chan].provider].takt2);
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


  tm = localtime(&call[chan].connect);

  provider = ((call[chan].provider == UNKNOWN) ? preselect : call[chan].provider);

  if ((call[chan].sondernummer[CALLED] != UNKNOWN) &&
      (SN[call[chan].sondernummer[CALLED]].tarif == 1))
    zone = CITYCALL;
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
  else if ((t[provider].InternetZugang != NULL) && !strcmp(call[chan].onum[CALLED], t[provider].InternetZugang))
    zone = INTERNET;
  else {
    zone = area_diff(NULL, call[chan].num[CALLED]);

    if ((zone == AREA_ERROR) || (zone == AREA_UNKNOWN))
      zone = UNKNOWN;
    else if (zone == AREA_ABROAD) /* FIXME: muss noch stark verbessert werden! */
      zone = EURO_CITY;
    else
      zone--; /* area_diff() liefert relativ zu 1 */
  } /* else */

  provider = ((call[chan].provider == UNKNOWN) ? preselect : call[chan].provider);

  tz = tarifzeit(tm, why, ((provider == 33) && CityWeekend));

  if ((tz == FE) || (tz == ZJ)) /* FIXME: stimmt das bei allen Providern? */
    tz = WE;

  if (zone != UNKNOWN)
    tarif = t[provider].tarif[zone][tz][tm->tm_hour] / SHIFT;

  call[chan].zone = zone;
  call[chan].provider = provider;
  call[chan].tz = tz;

  if (t[provider].takt1 == UNKNOWN)
    sprintf(s, "DM %5.3f/%7.3fs", t[provider].taktpreis, tarif);
  else
    sprintf(s, "DM %5.3f/Min, Takt %d/%d", tarif, t[provider].takt1, t[provider].takt2);

  sprintf(msg, "CHARGE: %s, %s, %s", why, ((zone == UNKNOWN) ? "Unknown zone" : zonen[zone]), s);


  *hint = 0;
  cheaptarif = 99999.9;
  call[chan].tip = UNKNOWN;

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

    if ((cheapest != UNKNOWN) && (cheapest != provider)) {
      tarif = t[cheapest].tarif[zone][tz][tm->tm_hour] / SHIFT;

      if (t[cheapest].takt1 == UNKNOWN)
        sprintf(s, "DM %5.3f/%7.3fs", t[cheapest].taktpreis, tarif);
      else
        sprintf(s, "DM %5.3f/Min, Takt %d/%d", tarif, t[cheapest].takt1, t[cheapest].takt2);

      sprintf(hint, "HINT: Better use 010%02d:%s, %s, saves DM %5.3f/%d s",
        cheapest, t[cheapest].Provider, s, providertarif - cheaptarif, TEST);

      call[chan].tip = cheapest;
    } /* if */
  } /* if */

} /* preparecint */


void price(int chan, char *hint)
{
  auto	   int	  duration = (int)(call[chan].disconnect - call[chan].connect);
  auto     double pay2 = -1.0, onesec, spar = 0.0;
  auto     char   sx[BUFSIZ], sy[BUFSIZ], sz[BUFSIZ];
  auto 	   struct tm *tm;
  register int 	  p, cheapest = UNKNOWN;
  auto	   double payx, payy, prepreis = -1.0, tippreis = -1.0;
  auto	   FILE	 *fo;


  *hint = 0;

  if (OUTGOING && (duration > 0) && *call[chan].num[CALLED]) {

    tm = localtime(&call[chan].connect);

    if ((call[chan].sondernummer[CALLED] != UNKNOWN) &&
        (call[chan].provider == 33) &&
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

      if ((duration > 600) && (call[chan].zone > 1) && (call[chan].provider == 33)) {
        onesec = call[chan].pay / duration;
        pay2 = (duration - 600) * onesec * 0.30;

        sprintf(sx, "10plus DM %s - DM %s = DM %s",
          double2str(call[chan].pay, 6, 2, DEB),
          double2str(pay2, 6, 2, DEB),
          double2str(call[chan].pay - pay2, 6, 2, DEB));

        call[chan].pay -= pay2;

        print_msg(PRT_NORMAL, sx);
      } /* if */
    }
    else
      call[chan].pay = pay2;


    cheapest = UNKNOWN;
    payx = 99999.9;

    for (p = 0; p < MAXPROVIDER; p++) {
      payy = tpreis(p, call[chan].zone, call[chan].tz, tm->tm_hour, duration);

      if (p == preselect)
        prepreis = payy;

      if (p == call[chan].tip)
        tippreis = payy;

      if ((payy > 0) && (payy < payx)) {
        payx = payy;
        cheapest = p;
      } /* if */
    } /* for */

    *sx = *sy = *sz = 0;

    if ((cheapest != UNKNOWN) && (cheapest != call[chan].provider))
      sprintf(sx, "Cheapest 010%02d:%s DM %s, more payed DM %s",
            cheapest, t[cheapest].Provider,
            double2str(payx, 6, 2, DEB),
            double2str(call[chan].pay - payx, 6, 2, DEB));

    if ((call[chan].provider != preselect) && (prepreis != -1.00))
      sprintf(sy, "saved vs. preselect (010%02d:%s) DM %s",
        preselect, t[preselect].Provider,
        double2str(prepreis - call[chan].pay, 6, 2, DEB));

    if ((call[chan].tip != UNKNOWN) && (call[chan].tip != cheapest))
      sprintf(sz, "saved vs. tip (010%02d:%s) DM %s",
        call[chan].tip, t[call[chan].tip].Provider,
        double2str(tippreis - call[chan].pay, 6, 2, DEB));

    if (*sx || *sy || *sz)
      sprintf(hint, "HINT: %s, %s, %s LCR:%s", sx, sy, sz, ((cheapest == call[chan].provider) ? "OK" : "FAILED"));

    if ((fo = fopen(SPARBUCH, "r")) != (FILE *)NULL) {
      fscanf(fo, "%lg", &spar);
      fclose(fo);
    } /* if */

    spar += (prepreis - call[chan].pay);

    if ((fo = fopen(SPARBUCH, "w")) != (FILE *)NULL) {
      fprintf(fo, "%g", spar);
      fclose(fo);
    } /* if */
  } /* if */
} /* price */
#endif


void initTarife(char *msg)
{
  register char  *p;
  auto     char   s[BUFSIZ], sx[BUFSIZ], Version[BUFSIZ], infos[BUFSIZ];
  auto     FILE  *fi;
  auto     int    prefix = UNKNOWN, zone1 = UNKNOWN, zone2 = UNKNOWN;
  auto	   int 	  day, hour1, hour2, pay2, version = UNKNOWN, ignore = 0;
  auto	   int	  d, h, z;
  auto     double pay1;


  *msg = *infos = 0;
  line = 0;

  if ((fi = fopen(TARIFCONF, "r")) != (FILE *)NULL) {
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

  if ((fi = fopen(TARIFE, "r")) != (FILE *)NULL) {

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
                         t[prefix].takt1 = t[prefix].takt2 = UNKNOWN;
                         t[prefix].taktpreis = (double)UNKNOWN;

                         for (z = 0; z < MAXZONEN; z++)
                           for (d = 0; d < MAXDAYS; d++)
                             for (h = 0; h < MAXSTUNDEN; h++)
                               t[prefix].tarif[z][d][h] = UNKNOWN * SHIFT;

                         sprintf(sx, "%02d", prefix);

                         if (*infos)
                           strcat(infos, ", ");

                         strcat(infos, sx);
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

          case 'A' : if (!ignore) { /* A:n/n od. A:0.12 Taktpreis oder L„nge */
               	       if (prefix == UNKNOWN) {
                         warning("Unexpected tag 'A'");
                         break;
                       } /* if */

                       if ((p = strchr(s, '/'))) {
                         *p = 0;
                         t[prefix].takt1 = atoi(s + 2);
                         t[prefix].takt2 = atoi(p + 1);
                         t[prefix].taktpreis = (double)UNKNOWN;
                       }
                       else {
                         t[prefix].taktpreis = atof(s + 2);
                         t[prefix].takt1 = t[prefix].takt2 = UNKNOWN;
                       } /* else */
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

                       if (t[prefix].takt1 + t[prefix].takt2 + t[prefix].taktpreis == (double)-3.0) {
                         warning("Unexpected tag 'Z', please specify 'A' before 'Z'");
                         break;
                       } /* if */

                       if ((zone1 = zone2 = zones(s[2])) == UNKNOWN)
                         break;

                       if (s[3] == '-') {
                         if ((zone2 = zones(s[4])) == UNKNOWN)
                           break;
                       } /* if */
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
                         pay1 = atof(p + 1) * SHIFT;
                         pay2 = pay1;

                         hour2--;

                         for (z = zone1; z <= zone2; z++) {

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
    sprintf(msg, "Tarife Version %s loaded [Provider %s]", Version, infos);
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


#ifdef STANDALONE
int main(int argc, char *argv[], char *envp[])
{
  register int    prefix, z, d, h;
  auto	   double n;
  auto	   char	  why[BUFSIZ];

  printf("Initializing ...\n");

  initTarife(why);

  printf("sizeof(t) = %d\n\n", sizeof(t));

  for (prefix = 0; prefix < MAXPROVIDER; prefix++) {
    if (t[prefix].used) {
      printf("PROVIDER:%s (010%02d)\n", t[prefix].Provider, prefix);

      for (z = 0; z < MAXZONEN; z++) {
        printf("\tZone:%s:\n", zonen[z]);
          for (d = 0; d < MAXDAYS; d++) {
            printf("\t\t%s:\n", (d ? "Wochenende" : "Wochentag"));
            for (h = 0; h < MAXSTUNDEN; h++)
              printf("\t\t\t%2d Uhr: %10.3f DM\n", h, tpreis(prefix, z, d, h, TEST));
          } /* for */
      } /* for */
    } /* if */
  } /* for */

  printf("DIENSTAG, 16:00 Uhr, %d Sekunden:\n", TEST);

  for (prefix = 0; prefix < MAXPROVIDER; prefix++) {
    if (t[prefix].used) {
      printf("%s (010%02d)",
        t[prefix].Provider, prefix);

      if (t[prefix].takt1 == UNKNOWN)
        printf(", [DM %6.3f/%6f Sekunden]\n", t[prefix].taktpreis, (t[prefix].tarif[GERMANCALL][WT][16] / SHIFT));
      else
        printf(", [%d/%d] (%6.3f)\n", t[prefix].takt1, t[prefix].takt2, t[prefix].tarif[GERMANCALL][WE][13] / SHIFT);

      printf("\t\t\tCITY    : %6.3f DM\n", tpreis(prefix, CITYCALL, WT, 16, TEST));

      printf("\t\t\tREGIO   : %6.3f DM\n", tpreis(prefix, REGIOCALL, WT, 16, TEST));
      printf("\t\t\tFERN    : %6.3f DM\n", tpreis(prefix, GERMANCALL, WT, 16, TEST));
      printf("\t\t\tD2	    : %6.3f DM\n", tpreis(prefix, D2_NETZ, WT, 16, TEST));
      printf("\t\t\tINTERNET: %6.3f DM\n", tpreis(prefix, INTERNET, WT, 16, TEST));
    } /* if */
  } /* for */

  exitTarife();

  return(0);
} /* main */
#endif
