/* $Id: cheap.c,v 1.3 1997/04/03 22:29:59 luethje Exp $
 *
 * ISDN accounting for isdn4linux. (Feiertagsberechnung)
 *
 * Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
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
 * $Log: cheap.c,v $
 * Revision 1.3  1997/04/03 22:29:59  luethje
 * improved performance
 *
 * Revision 1.2  1997/03/24 22:52:08  luethje
 * isdnrep completed.
 *
 * Revision 1.1  1997/03/16 20:59:02  luethje
 * Added the source code isdnlog. isdnlog is not working yet.
 * A workaround for that problem:
 * copy lib/policy.h into the root directory of isdn4k-utils.
 *
 *
 * Revision 2.6.24  1997/01/12  20:51:21  akool
 * City Weekend Tarif implemented (Thanks to Oliver Schoett <schoett@muc.de>)
 *
 * Revision 2.6.11  1996/12/30  10:05:21  Ad Aerts <ad@aasup.nl>
 * Dutch accounting
 *
 * Revision 2.50  1996/08/24  10:47:21  akool
 * Korrektur bei Taktberechnung Wochenende/Feiertage (Jochen Heuer)
 *
 * Revision 2.43  1996/07/04  09:52:21  akool
 * Korrektur bei Taktberechnung City (Jochen Heuer)
 *
 * Revision 2.40  1996/06/19  01:55:21  akool
 * Neue Tarife ab 1.7.96 implementiert
 *
 * Revision 2.3.27  1996/05/06  17:51:21  akool
 * Korrektur bei Taktberechnung Regio 200
 *
 * Revision 2.01  1996/01/19  19:12:21  akool
 * Korrektur bei Taktberechnung (Citytarif, 5:00 .. 9:00 war 240s, muss
 * aber 150s heissen!)
 *
 * Revision 2.00  1996/01/10  20:10:21  akool
 *
 * Revision 1.22  1995/10/21  17:23:21  akool
 * Tarife 1996 entsprechend Telekom-Unterlagen korrigiert
 *
 * Revision 1.14  1995/10/05  19:02:21  akool
 * Ueberschreitung verschiedener Zeittakte bei einer Verbindung implementiert.
 *
 * Revision 1.2  1995/09/30  16:39:21  akool
 * Um neue Tarifstruktur der Telekom ab 1.1.96 erweitert
 * First public release
 *
 * Revision 1.1  1990/11/10  23:15:11  akool
 * Initial revision (Code-Teile aus irgend einer alten c't)
 *
 */


#define _CHEAP_C_

#include "isdnrep.h"

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
  char tag;
  char monat;
  char telekom; /* TRUE, wenn auch fuer die Deutsche Telekom ein Feiertag (siehe Telefonbuch!) */
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
#ifdef ISDN_NL
  {  1,  1, 1 },  /* Neujahr */
  {  6,  1, 0 },  /* Erscheinungsfest - nur Baden-Wuerttemberg und Bayern */
  {  1,  5, 0 },  /* Maifeiertag */
  {  0,  0, 0 },  /* Muttertag */
  {  0,  0, 0 },  /* Karfreitag */
  {  0,  0, 1 },  /* Ostersonntag */
  {  0,  0, 1 },  /* Ostermontag */
  {  0,  0, 0 },  /* Christi Himmelfahrt */
  {  0,  0, 1 },  /* Pfingstsonntag */
  {  0,  0, 1 },  /* Pfingstmontag */
  {  0,  0, 0 },  /* Fronleichnam - nur in Baden-Wuerttemberg, Bayern, Hessen, Nordrhein-Westfalen, Rheinland-Pfalz und im Saarland */
  {  3, 10, 0 },  /* Tag der deutschen Einheit - vor 1990 am 17.6. */
  { 15,  8, 0 },  /* Maria Himmelfahrt - nur Saarland und ueberwiegend katholischen Gemeinden Bayerns */
  {  1, 11, 0 },  /* Allerheiligen - nur Baden-Wuerttemberg, Bayern, Nordrhein-Westfalen, Rheinland-Pfalz und im Saarland */
  {  0,  0, 0 },  /* Buss- und Bettag - nur bis incl. 1994 (wg. Pflegeversicherung abgeschafft) */
  { 25, 12, 1 },  /* 1. Weihnachtsfeiertag */
  { 26, 12, 1 }}; /* 2. Weihnachtsfeiertag */
#else
  {  1,  1, 1 },  /* Neujahr */
  {  6,  1, 0 },  /* Erscheinungsfest - nur Baden-Wuerttemberg und Bayern */
  {  1,  5, 1 },  /* Maifeiertag */
  {  0,  0, 1 },  /* Muttertag */
  {  0,  0, 1 },  /* Karfreitag */
  {  0,  0, 1 },  /* Ostersonntag */
  {  0,  0, 1 },  /* Ostermontag */
  {  0,  0, 1 },  /* Christi Himmelfahrt */
  {  0,  0, 1 },  /* Pfingstsonntag */
  {  0,  0, 1 },  /* Pfingstmontag */
  {  0,  0, 0 },  /* Fronleichnam - nur in Baden-Wuerttemberg, Bayern, Hessen, Nordrhein-Westfalen, Rheinland-Pfalz und im Saarland */
  {  3, 10, 1 },  /* Tag der deutschen Einheit - vor 1990 am 17.6. */
  { 15,  8, 0 },  /* Maria Himmelfahrt - nur Saarland und ueberwiegend katholischen Gemeinden Bayerns */
  {  1, 11, 0 },  /* Allerheiligen - nur Baden-Wuerttemberg, Bayern, Nordrhein-Westfalen, Rheinland-Pfalz und im Saarland */
  {  0,  0, 0 },  /* Buss- und Bettag - nur bis incl. 1994 (wg. Pflegeversicherung abgeschafft) */
  { 25, 12, 1 },  /* 1. Weihnachtsfeiertag */
  { 26, 12, 1 }}; /* 2. Weihnachtsfeiertag */
#endif


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


static int billig(time_t when)
{
  register int        i;
  auto 	   struct tm *tm;


  tm = localtime(&when);


  if (tm->tm_wday == 6) /* Samstag immer */
    return(1);

  if (tm->tm_wday == 0) /* Sonntag immer */
    return(1);

  if ((tm->tm_hour > 17) || (tm->tm_hour < 8))   /* zwischen 18:00 .. 7:59 immer */
    return(1);

#ifndef ISDN_NL
  if ((tm->tm_mday == 24) && (tm->tm_mon == 11)) /* Heilig-Abend immer */
    return(1);

  if ((tm->tm_mday == 31) && (tm->tm_mon == 11)) /* Sylvester immer */
    return(1);

  comp_feier_tage(tm->tm_year + 1900);

  for (i = 0; i < A_FEI; i++)
    if ((t_ftag[i].monat == tm->tm_mon + 1) &&
        (t_ftag[i].tag == tm->tm_mday) &&
         t_ftag[i].telekom)
      return(1);
#endif

  return(0); /* Sorry, it's expensive! */

} /* billig */


double cheap(time_t when, int zone)
{
  auto double takt = 0.0;

#ifdef ISDN_NL
  switch (zone) {
    case 1 : takt =   1.000; break; /* local tariff  Dfl 0.066/min  */
    case 2 : takt =   0.316; break; /* long distance Dfl 0.2106/min */
    case 3 : takt =   0.074; break; /* International Dfl 0.90/min   */
    case 4 : takt =   0.070; break; /* International Dfl 0.95/min   */
    case 5 : takt =   0.060; break; /* International Dfl 1.10/min   */
  } /* switch */

  if (billig(when)) {
    takt *= 2.0;
  } /* if */
#else
  switch (zone) {
    case 1 : takt = 360.00; break;
    case 2 : takt =  60.00; break;
    case 3 : takt =  21.00; break;
    case 4 : takt =  21.00; break;
    case 5 : takt =  12.00; break;
  } /* switch */

  if (billig(when)) {
    if (takt == 12.00)
      takt = 16.00;
    else
      takt *= 2.0;
  } /* if */
#endif

  return(takt);
} /* cheap */


double cheap96(time_t when, int zone, int *zeit)
{
#ifdef ISDN_NL
  *zeit = 0;
  return(cheap(when, zone));
#else
  register int        i, takt = 0;
  auto     struct tm *tm;

  /* neue Tarife aus: iX 10/95, Pg 32 */
  static   double gebuehr[3][5][5] =
    {{{  90.00,  90.00, 150.00, 240.00, 240.00 },    /* City */
      {  26.00,  30.00,  45.00,  60.00, 120.00 },    /* Region 50 */
      {  12.00,  13.50,  21.50,  30.00, 120.00 },    /* Region 200 */
      {  11.50,  12.50,  20.00,  25.00, 120.00 },    /* Fern */
      {  30.00,  30.00,  30.00,  30.00,  30.00 }},   /* Angrenzende Laender */

        /* Tarife, Wochenendtarife ab 1.7.96 */
     {{  90.00,  90.00, 150.00, 240.00, 240.00 },    /* City */
      {  26.00,  30.00,  45.00,  60.00, 120.00 },    /* Region 50 */
      {  13.00,  14.00,  22.50,  36.00, 120.00 },    /* Region 200 */
      {  12.00,  13.50,  21.50,  30.00, 120.00 },    /* Fern */
      {  26.00,  30.00,  45.00,  60.00,  60.00 }},   /* Vis-'a-vis 1 */

        /* Tarife vom 27.-30.12., Feiertagstarife ab 1.7.96 */
     {{  90.00,  90.00, 150.00, 240.00, 240.00 },    /* City */
      {  36.00,  36.00,  45.00,  60.00, 120.00 },    /* Region 50 */
      {  36.00,  36.00,  36.00,  36.00, 120.00 },    /* Region 200 */
      {  36.00,  36.00,  36.00,  36.00, 120.00 },    /* Fern */
      {  26.00,  30.00,  45.00,  60.00,  60.00 }}};  /* Vis-'a-vis 1 */
/* Takt:  1      2       3       4       5

   Werktag :

     1 = Vormittag     9:00 .. 12:00
     2 = Nachmittag   12:00 .. 18:00
     3 = Freizeit     18:00 .. 21:00, 5:00 .. 9:00
     4 = Mondschein   21:00 ..  2:00
     5 = Nacht         2:00 ..  5:00

   Wochenende/Feiertag :

     3 = Freizeit      5:00 .. 21:00
     4 = Mondschein   21:00 ..  5:00

*/

  tm = localtime(&when);

  if ((tm->tm_wday == 6) || (tm->tm_wday == 0)) {     /* Samstag/Sonntag */
    if ((tm->tm_hour > 4) && (tm->tm_hour < 21)) {
      if (CityWeekend && (zone == 1))
        takt = 4;
      else
        takt = 3;
    }
    else
      takt = 4;
  } /* if */

  if ((tm->tm_mday == 24) && (tm->tm_mon == 11))      /* Heilig-Abend */
    takt = 6;

  if ((tm->tm_mday == 31) && (tm->tm_mon == 11))      /* Sylvester */
    takt = 6;

  comp_feier_tage(tm->tm_year + 1900);

  for (i = 0; i < A_FEI; i++)
    if ((t_ftag[i].monat == tm->tm_mon + 1) &&
        (t_ftag[i].tag == tm->tm_mday) &&
         t_ftag[i].telekom)
      takt = 6;

  if ((!takt) || (zone == 5)) {
    if ((tm->tm_hour > 8) && (tm->tm_hour < 12))
      takt = 1;
    else if ((tm->tm_hour > 11) && (tm->tm_hour < 18))
      takt = 2;
    else if ((tm->tm_hour > 20) || (tm->tm_hour < 2))
      takt = 4;
    else if ((tm->tm_hour > 1) && (tm->tm_hour < 5))
      takt = 5;
    else
      takt = 3;
  } /* if */

  if (((tm->tm_year > 95) && (tm->tm_mon > 5)) ||
       (tm->tm_year > 96)) {                          /* Neuer Tarif ab 1.7.96 */
    if ((takt == 6) && (zone < 5)) {                  /* Feiertage */
      if ((tm->tm_hour > 4) && (tm->tm_hour < 21)) {
        if (CityWeekend && (zone == 1)) {
          *zeit = 8;
          return(gebuehr[2][zone - 1][3]);
        }
        else
          return(gebuehr[2][zone - 1][*zeit = 2]);
      }
      else
        return(gebuehr[2][zone - 1][*zeit = 3]);
    }
    else if ((tm->tm_mon == 11) &&                    /* Werktage vom 27.12. .. 30.12. */
             (tm->tm_mday > 26) && (zone < 5)) {
      return(gebuehr[2][zone - 1][*zeit = --takt]);
    }
    else                                              /* Werktage bzw. Wochenenden */
      return(gebuehr[1][zone - 1][*zeit = --takt]);
  }
  else {
    if (zone == 5) {	       	       	 	      /* Angrenzende Laender */
      *zeit = 0;
      return(30);
    }
    else {
      if (takt == 6) { /* Feiertage */
        if ((tm->tm_hour > 4) && (tm->tm_hour < 21)) {
          *zeit = 10;
          return(gebuehr[0][zone - 1][2]);
        }
        else
          return(gebuehr[0][zone - 1][*zeit = 3]);
      }
      else
        return(gebuehr[0][zone - 1][*zeit = --takt]);
    } /* else */
  } /* else */
#endif
} /* cheap96 */


#ifdef DEBUG_CHEAP
main()
{
  auto time_t     cur_time, ltime;
  auto struct tm *tm;
  auto double 	  o = 0.0, n;


  time(&cur_time);
  tm = localtime(&cur_time);
  tm->tm_sec = 0;
  tm->tm_min = 0;
  tm->tm_hour = 0;
  cur_time = ltime = mktime(tm);


  while (cur_time - (7L * 24L * 60L * 60L) < ltime) {
    n = cheap96(cur_time, 3);

    if (n != o) {
      printf("%7.2f  %s", n, ctime(&cur_time));
      o = n;
    } /* if */

    cur_time += 1L; /* (60L * 60L); */
  } /* while */
} /* main */
#endif
