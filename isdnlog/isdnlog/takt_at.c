/* $Id: takt_at.c,v 1.3 1998/11/24 20:51:52 akool Exp $
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
 * $Log: takt_at.c,v $
 * Revision 1.3  1998/11/24 20:51:52  akool
 *  - changed my email-adress
 *  - new Option "-R" to supply the preselected provider (-R24 -> Telepassport)
 *  - made Provider-Prefix 6 digits long
 *  - full support for internal S0-bus implemented (-A, -i Options)
 *  - isdnlog now ignores unknown frames
 *  - added 36 allocated, but up to now unused "Auskunft" Numbers
 *  - added _all_ 122 Providers
 *  - Patch from Jochen Erwied <mack@Joker.E.Ruhr.DE> for Quante-TK-Anlagen
 *    (first dialed digit comes with SETUP-Frame)
 *
 * Revision 1.2  1998/10/03 18:06:03  akool
 *  - processor.c, takt_at.c : Patch from Michael Reinelt <reinelt@eunet.at>
 *    try to guess the zone of the calling/called party
 *
 *  - isdnrep.c : cosmetics (i hope, you like it, Stefan!)
 *
 * Revision 1.1  1998/09/26 18:29:22  akool
 *  - quick and dirty Call-History in "-m" Mode (press "h" for more info) added
 *    - eat's one more socket, Stefan: sockets[3] now is STDIN, FIRST_DESCR=4 !!
 *  - Support for tesion)) Baden-Wuerttemberg Tarif
 *  - more Providers
 *  - Patches from Wilfried Teiken <wteiken@terminus.cl-ki.uni-osnabrueck.de>
 *    - better zone-info support in "tools/isdnconf.c"
 *    - buffer-overrun in "isdntools.c" fixed
 *  - big Austrian Patch from Michael Reinelt <reinelt@eunet.at>
 *    - added $(DESTDIR) in any "Makefile.in"
 *    - new Configure-Switches "ISDN_AT" and "ISDN_DE"
 *      - splitted "takt.c" and "tools.c" into
 *          "takt_at.c" / "takt_de.c" ...
 *          "tools_at.c" / "takt_de.c" ...
 *    - new feature
 *        CALLFILE = /var/log/caller.log
 *        CALLFMT  = %b %e %T %N7 %N3 %N4 %N5 %N6
 *      in "isdn.conf"
 *  - ATTENTION:
 *      1. "isdnrep" dies with an seg-fault, if not HTML-Mode (Stefan?)
 *      2. "isdnlog/Makefile.in" now has hardcoded "ISDN_DE" in "DEFS"
 *      	should be fixed soon
 *
 */

#define _TAKT_C_
#include "isdnlog.h"

#define KARF      4
#define OST1      5
#define OST2      6
#define CHRI      7
#define PFI1      8
#define PFI2      9
#define FRON     10

#define A_FEI 	 16

struct w_ftag {
  char  tag;
  char  monat;
  char  telekom; /* TRUE, wenn auch fuer die PTA ein Feiertag (siehe Telefonbuch!) */
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
  {  6,  1, 0, "Hl. Drei Könige" },
  {  1,  5, 1, "1. Mai" },
  {  0,  0, 1, "Muttertag" },
  {  0,  0, 1, "Karfreitag" },
  {  0,  0, 1, "Ostersonntag" },
  {  0,  0, 1, "Ostermontag" },
  {  0,  0, 1, "Christi Himmelfahrt" },
  {  0,  0, 1, "Pfingstsonntag" },
  {  0,  0, 1, "Pfingstmontag" },
  {  0,  0, 0, "Fronleichnam" },
  { 26, 10, 1, "Nationalfeiertag" },
  { 15,  8, 0, "Maria Himmelfahrt" },
  {  1, 11, 0, "Allerheiligen" },
  { 25, 12, 1, "Christtag" },
  { 26, 12, 1, "2. Weihnachtstag" }};

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


#define WT 0 /* Werktag */
#define FR 1 /* Freitag */
#define WE 2 /* Wochenende */

/* 
 * 0 .. Tageszeit 1:   Mo-Fr 08-12 Mo-Do 13-16
 * 1 .. Tageszeit 2:   Mo-Do 12-13 Mo-Do 16-18 Fr 12-18
 * 2 .. Sparzeit:      Mo-Fr 06-08 Mo-Fr 18-20 Sa+So+Feiertag 06-20
 * 3 .. Supersparzeit: täglich 06-20
 */

static int zeitzone [3][24] = {{ 3, 3, 3, 3, 3, 3, 2, 2, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3 },  /* Werktag */
			       { 3, 3, 3, 3, 3, 3, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3, 3, 3 },  /* Freitag */
			       { 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3 }}; /* Wochenende */


static int tarifzeit(struct tm *tm, char *why)
{
  int i;

  comp_feier_tage(tm->tm_year + 1900);
  
  for (i = 0; i < A_FEI; i++)
    if ((t_ftag[i].monat == tm->tm_mon + 1) &&
        (t_ftag[i].tag == tm->tm_mday) &&
	t_ftag[i].telekom) {
      sprintf(why, "Feiertag (%s)", t_ftag[i].bez);
      return(WE); /*  Feiertag wie Wochenende */
    }
  
  if (tm->tm_wday == 0 || tm->tm_wday == 6) {
    strcpy(why, "Wochenende");
    return(WE);
  }

  if (tm->tm_wday == 5) {
    strcpy(why, "Freitag");
    return(FR);
  }
  
  strcpy(why, "Werktag");
  return(WT);
}

/* Einheiten in 72 sec */

static double faktor [30][4] = {{  1.25,  1.00,  0.66,  0.45 },  /* Regionalzone bis 50 km */
				{  3.60,  2.88,  1.44,  1.00 },  /* Fernzone 1 bis 200 km */
				{  4.80,  3.75,  1.80,  1.44 },  /* Fernzone 2 über 200 km */
				{  0.60,  0.60,  0.20,  0.20 },  /* Online-Tarif */
				{  6.00,  6.00,  3.75,  3.75 },  /* Mobilfunk */
				{  6.00,  6.00,  5.00,  5.00 },  /* Ausland Zone 1 */
				{  8.00,  8.00,  6.00,  6.00 },  /* Ausland Zone 2 */
				{ 11.00, 11.00,  9.00,  9.00 },  /* Ausland Zone 3 */
				{ 14.00, 14.00, 12.00, 12.00 },  /* Ausland Zone 4 */
				{ 17.00, 17.00, 15.00, 15.00 },  /* Ausland Zone 5 */
				{ 20.00, 20.00, 17.00, 17.00 },  /* Ausland Zone 6 */
				{ 23.00, 23.00, 20.00, 20.00 },  /* Ausland Zone 7 */
				{ 26.00, 26.00, 24.00, 24.00 },  /* Ausland Zone 8 */
				{ 30.00, 30.00, 28.00, 28.00 },  /* Ausland Zone 9 */
				{ 34.00, 34.00, 32.00, 32.00 },  /* Ausland Zone 10 */
				{ 39.00, 39.00, 36.00, 36.00 },  /* Ausland Zone 11 */
				{ 44.00, 44.00, 40.00, 40.00 },  /* Ausland Zone 12 */
				{ 59.00, 59.00, 53.00, 53.00 },  /* Ausland Zone 13 */
				{  9.00,  9.00,  8.00,  8.00 },  /* Ausland Zone 14 */
				{  8.00,  8.00, 10.00, 10.00 },  /* Ausland Zone 15 */
				{  0.00,  0.00,  0.00,  0.00 },  /* Handvermittelter Verkehr */
				{  4.00,  4.00,  3.00,  3.00 },  /* Grenznahverkehr */
				{  6.67,  6.67,  6.67,  6.67 },  /* Teleinfo 04570 */
				{ 10.00, 10.00, 10.00, 10.00 },  /* Teleinfo 04500 */ 
				{ 16.00, 16.00, 16.00, 16.00 },  /* Telebusiness 04590 */
				{ 26.67, 26.67, 26.67, 26.67 },  /* Teleinfo 04580 */
				{  1.25,  1.00,  0.66,  0.45 },  /* Businessline 0711x */
				{  2.25,  2.25,  2.25,  2.25 },  /* Businessline 0713x */
				{  4.80,  4.80,  4.80,  4.80 },  /* Businessline 0714x */
				{  4.80,  4.80,  4.80,  4.80 }}; /* Votingline 0717x */



double taktlaenge(int chan, char *description)
{
  int         c;
  struct tm  *tm;
  char	      why[BUFSIZ];
  int	      provider, zone, fenster;
  time_t      connect;
  double      takt;
  
  if (description) description[0] = '\0';
  provider = call[chan].provider;
  connect = call[chan].connect;
  tm = localtime(&connect);
  
  if (call[chan].dialin)
    return -1;
  
  if (*call[chan].num[1] == '\0')
    return -1;
 
  if ((zone = call[chan].zone) == 0) {
    zone = -1;
    if ((c = call[chan].confentry[OTHER]) > -1) {
    zone = known[c]->zone;
  if (zone < 1 || zone > 30) 
	zone=-1;
    }
  call[chan].zone = zone;
  }
  
  if (zone < 0)
    return -1;
 
  fenster = zeitzone[tarifzeit(tm, why)][tm->tm_hour];
  
  if (faktor[zone-1][fenster] == 0.0)
    return -1;
  
  takt = 72.0/faktor[zone-1][fenster];
  
  if (description) sprintf(description, "%s, %s, %s", z2s(zone), why, t2tz(fenster));
  
  return (takt);
}
