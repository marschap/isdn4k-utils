/* $Id: takt_de.c,v 1.5 1998/12/16 20:57:24 akool Exp $
 *
 * ISDN accounting for isdn4linux. (log-module)
 *
 * Copyright 1995, 1998 by Andreas Kool (akool@isdn4linux.de)
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

#define _TAKT_C_
#include "isdnlog.h"

#define DTAG     0
#define MOBILCOM 1
#define TELE	  2

#define CITYCALL   0
#define REGIOCALL  1
#define GERMANCALL 2

#define DTAG_PREIS     0.121
#define MOBILCOM_PREIS 0.19

#define WT 0 /* Werktag */
#define WE 1 /* Wochenende */
#define FE 2 /* Feiertag */
#define ZJ 3 /* Werktag 27. .. 30.12. */


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


/*
    [Provider][Uhrzeit][Tarif][Zone]
        |         |       |      |
        |         |       |      +----- 1=CityCall, 2=RegioCall,  3=GermanCall
        |         |       +------------ 1=Werktag,  2=Wochenende, 3=27.12. - 30.12., 4=Feiertag
        |         +-------------------- 1=05:00 .. 09:00, 2=09:00 .. 12:00, 3=12:00 .. 18:00, 4=18:00 .. 21:00, 5=21:00 .. 02:00, 6=02:00 .. 05:00
        +------------------------------ 1=DTAG, 2=Mobilcom, 3=Tele2
*/

static int   zeit[24] = { 4, 4, 5, 5, 5, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4 };
static char *zeiten[6] = { "Freizeit", "Vormittag", "Nachmittag", "Freizeit", "Mondschein", "Nacht" };
static char *zonen[4] = { "CityCall", "RegioCall", "GermanCall", "GlobalCall" };


static float gebuehr[2][6][4][3] =
  {{{{ 150.0,  45.0,  22.5 },   /* DTAG */
     { 150.0,  45.0,  30.0 },
     { 150.0,  45.0,  36.0 },
     { 150.0,  45.0,  36.0 }},

    {{  90.0,  26.0,  13.0 },
     { 150.0,  45.0,  30.0 },
     {  90.0,  36.0,  36.0 },
     { 150.0,  45.0,  36.0 }},

    {{  90.0,  30.0,  14.0 },
     { 150.0,  45.0,  30.0 },
     {  90.0,  36.0,  36.0 },
     { 150.0,  45.0,  36.0 }},

    {{ 150.0,  45.0,  22.5 },
     { 150.0,  45.0,  30.0 },
     { 150.0,  45.0,  36.0 },
     { 150.0,  45.0,  36.0 }},

    {{ 240.0,  60.0,  36.0 },
     { 240.0,  60.0,  36.0 },
     { 240.0,  60.0,  36.0 },
     { 240.0,  60.0,  36.0 }},

    {{ 240.0, 120.0, 120.0 },
     { 240.0,  60.0,  36.0 },
     { 240.0, 120.0, 120.0 },
     { 240.0,  60.0,  36.0 }}},

   {{{  -1.0,  60.0,  60.0 },   /* Mobilcom */
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 }},

    {{  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 }},

    {{  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 }},

    {{  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 }},

    {{  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 }},

    {{  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 },
     {  -1.0,  60.0,  60.0 }}}};


float taktlaenge(int chan, char *description)
{
  register int        c, cwe, tz, z;
  auto     struct tm *tm;
  auto	   char	      why[BUFSIZ], zt[BUFSIZ];
  auto	   int	      provider = call[chan].provider;
  auto	   time_t     connect = call[chan].connect;
  auto	   int	      zone = -1, zone2 = -1;
  auto	   float      takt;


  if (description)
    *description = 0;

  if (!call[chan].dialin && *call[chan].num[CALLED]) {

    tm = localtime(&connect);

    if ((provider == 33) && (tm->tm_year + 1900 > 1998)) { /* neue 1999 Tarife Dt. Telekom */
      cwe = CityWeekend;
      tz = tarifzeit(tm, why, cwe);

      if (tz == WT) {
        z = zeit[tm->tm_hour];

        if (z == 5) {
	  strcpy(zt, "Nacht");
          takt = 120;
        }
        else if ((z == 0) || (z == 3) || (z == 4)) {
          strcpy(zt, "Freizeit");
          takt = 60;
        }
        else if ((z == 1) || (z == 2)) {
          strcpy(zt, "Tag");
          takt = 30;
        } /* else */
      }
      else {
        strcpy(zt, "");
        takt = 60;
      } /* else */

      if (description)
        sprintf(description, "%s, %s", zt, why);

      return(takt);
    } /* if */

    if ((provider == 11) || /* o.tel.o */
        (provider == 13) || /* Tele2 */
        (provider == 14) || /* EWE TEL */
        (provider == 15) || /* RSL COM */
        (provider == 23) || /* Tesion */
        (provider == 24) || /* TelePassport */
        (provider == 30) || /* TelDaFax */
        (provider == 39) || /* tesion */
        (provider == 41) || /* HanseNet */
        (provider == 46) || /* KomTel */
        (provider == 49) || /* ACC */
        (provider == 66) || /* Interoute */
        (provider == 70) || /* Arcor */
        (provider == 79) || /* Viatel */
        (provider == 90) || /* Viag Interkom */
        (provider == 98))   /* STAR Telecom */
      return(1);

    if (provider == 18)	    /* Debitel */
      return(30);

    if ((provider == 36) || /* Hutchison Telekom */
        (provider == 50))   /* Talkline */
      return(10);

    if (provider == 43)	    /* KielNet */
      return(60);

    if (provider == 9)	    /* ECONOphone - mindestens jedoch 30 Sekunden! */
      return(6);

    if (call[chan].sondernummer[CALLED] != -1) {
      switch (SN[call[chan].sondernummer[CALLED]].tarif) {
        case  0 : if (description) sprintf(description, "FreeCall");  /* Free of charge */
              	  return(60 * 60 * 24);              /* one day should be enough ;-) */

        case  1 : zone = 1;                          /* CityCall */
                  break;

        case -1 : if ((tm->tm_wday > 0) && (tm->tm_wday < 6)) {
              	    if ((tm->tm_hour > 8) && (tm->tm_hour < 18))
                      takt = SN[call[chan].sondernummer[CALLED]].takt1;  /* Werktag 9-18 Uhr */
              	    else
                      takt = SN[call[chan].sondernummer[CALLED]].takt2;  /* Restliche Zeit */
        	  }
    	    	  else
    	    	    takt = SN[call[chan].sondernummer[CALLED]].takt2;

                  if (description) strcpy(description, SN[call[chan].sondernummer[CALLED]].sinfo);
		  return(takt);
                  break;

      } /* switch */
    } /* if */

    if (zone == -1) {
      zone2 = area_diff(NULL, call[chan].num[CALLED]);

      if ((zone2 == -1) && (c = call[chan].confentry[OTHER]) > -1)
        zone = known[c]->zone;
      else
        zone = zone2;

    } /* if */

    if (zone != -1) {

  if (provider == -1)
    provider = preselect;

      call[chan].zone = zone;

  zone--;

  if ((zone < 0) || (zone > 3))
    return(-1);

  if ((provider == 19) || (provider == 33)) {
        cwe = (CityWeekend && (provider == 33) && (zone == 0));

        tz = tarifzeit(tm, why, cwe);

        if ((tz == WE) && cwe)
          z = 5;
        else
          z = zeit[tm->tm_hour];

        takt = gebuehr[(provider == 33) ? DTAG : MOBILCOM][z][tz][zone];

	if (description)
	  sprintf(description, "%s, %s, %s", zeiten[zeit[tm->tm_hour]], why, zonen[zone]);

        return(takt);
      }
      else
        return(-1);
    }
    else
      return(-1);
  }
  else
    return(-1);
} /* taktlaenge */


float preis(int chan)
{
  auto int        duration, takte;
  auto float 	  pay, minpr, takt;
  auto char  	  why[BUFSIZ];
  auto int        tz;
  auto struct tm *tm;
  auto time_t     connect = call[chan].connect;


  tm = localtime(&connect);

  if ((call[chan].provider == 33) && (tm->tm_year + 1900 > 1998)) { /* neue 1999 Tarife Dt. Telekom */
    takt = taktlaenge(chan, NULL);

    duration = call[chan].disconnect - call[chan].connect;
    takte = (int)(duration + (takt - 1) / takt);

    pay = takt * 0.12;
    return(pay);
  }
  else if (call[chan].provider == 13) { /* Tele 2 */

    if (call[chan].zone == CITYCALL) /* not possible with Tele 2 */
      return(-1.0);

    tm = localtime(&call[chan].connect);
    tz = tarifzeit(tm, why, 0);

    if ((tz == WE) || (tz == FE))
      minpr = 0.10;
    else {
      if ((tm->tm_hour > 8) && (tm->tm_hour < 18))
        minpr = 0.20;
      else
        minpr = 0.15;
    } /* else */

    duration = call[chan].disconnect - call[chan].connect;
    pay = minpr / 60 * duration;
    return(pay);
  } /* if */

  return(0);
} /* preis */
