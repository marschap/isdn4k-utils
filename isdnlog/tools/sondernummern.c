/* $Id: sondernummern.c,v 1.2 1999/04/10 16:36:43 akool Exp $
 *
 * Gebuehrenberechnung fuer Sonderrufnummern
 *
 * Copyright 1999 by Mario Jou/3en (mario.joussen@post.rwth-aachen.de)
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
 * $Log: sondernummern.c,v $
 * Revision 1.2  1999/04/10 16:36:43  akool
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
 * Revision 1.1  1999/03/24 19:39:04  akool
 * - isdnlog Version 3.10
 * - moved "sondernnummern.c" from isdnlog/ to tools/
 * - "holiday.c" and "rate.c" integrated
 * - NetCologne rates from Oliver Flimm <flimm@ph-cip.uni-koeln.de>
 * - corrected UUnet and T-Online rates
 *
 * Revision 1.5  1999/03/20 14:33:36  akool
 * - isdnlog Version 3.08
 * - more tesion)) Tarife from Michael Graw <Michael.Graw@bartlmae.de>
 * - use "bunzip -f" from Franz Elsner <Elsner@zrz.TU-Berlin.DE>
 * - show another "cheapest" hint if provider is overloaded ("OVERLOAD")
 * - "make install" now makes the required entry
 *     [GLOBAL]
 *     AREADIFF = /usr/lib/isdn/vorwahl.dat
 * - README: Syntax description of the new "rate-at.dat"
 * - better integration of "sondernummern.c" from mario.joussen@post.rwth-aachen.de
 * - server.c: buffer overrun fix from Michael.Weber@Post.RWTH-Aachen.DE (Michael Weber)
 *
 * Revision 1.4  1999/03/14 14:26:51  akool
 * - isdnlog Version 3.05
 * - new Option "-u1" (or "ignoreRR=1")
 * - added version information to "sonderrufnummern.dat"
 * - added debug messages if sonderrufnummern.dat or tarif.dat could not be opened
 * - sonderrufnummern.dat V 1.01 - new 01805 rates
 *
 * Revision 1.3  1999/03/10 08:35:57  paul
 * use DATADIR from "make config" phase instead of hardcoded /usr/lib/isdn
 *
 * Revision 1.2  1999/03/07 18:19:02  akool
 * - new 01805 tarif of DTAG
 * - new March 1999 tarife
 * - added new provider "01051 Telecom"
 * - fixed a buffer overrun from Michael Weber <Michael.Weber@Post.RWTH-Aachen.DE>
 * - fixed a bug using "sondernnummern.c"
 * - fixed chargeint change over the time
 * - "make install" now install's "sonderrufnummern.dat", "tarif.dat",
 *   "vorwahl.dat" and "tarif.conf"! Many thanks to
 *   Mario Joussen <mario.joussen@post.rwth-aachen.de>
 * - Euracom Frames would now be ignored
 * - fixed warnings in "sondernnummern.c"
 * - "10plus" messages no longer send to syslog
 *
 * Revision 1.1  1999/02/28 19:32:50  akool
 * Fixed a typo in isdnconf.c from Andreas Jaeger <aj@arthur.rhein-neckar.de>
 * CHARGEMAX fix from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 * isdnrep fix from reinhard.karcher@dpk.berlin.fido.de (Reinhard Karcher)
 * "takt_at.c" fixes from Ulrich Leodolter <u.leodolter@xpoint.at>
 * sondernummern.c from Mario Joussen <mario.joussen@post.rwth-aachen.de>
 * Reenable usage of the ZONE entry from Schlottmann-Goedde@t-online.de
 * Fixed a typo in callerid.conf.5
 *
 * Revision 1.1  1999/01/31 16:44:13  mario
 * Initial revision
 *
 */

/*
 * Schnittstelle:
 *
 * int initSondernummern(char *fn, char **msg)
 *   initialisiert die Sonderrufnummerndatenbank, liefert Versionsinfo bzw.
 *   Fehlermeldung in msg zurueck
 *
 * void exitSondernummern()
 *   deinitialisiert die Sonderrufnummerndatenbank
 *
 * int is_sondernummer(char *number, int provider)
 *   liefert die Nummer des ersten passenden Datensatzes zurueck oder -1 im
 *   Fehlerfall
 *
 * char *sondernummername(int index)
 *   liefert die Beschreibung des entsprecheden Datensatzes zurueck oder NULL
 *   im Fehlerfall
 *
 * char *sondernum(int index)
 *   liefert die Sondernummer des entsprechenden Datensatzes zurueck oder NULL
 *   im Fehlerfall
 *
 * int sondertarif(int index)
 *   liefert den Tarif des entsprechenden Datensatzes zurueck oder SO_FAIL
 *   im Fehlerfall
 *
 * double sonderpreis(time_t connect, int duration, int index)
 *   liefert folgendes zurueck:
 *   -3 -> Fehler
 *   -2 -> Preis nicht bekannt
 *   -1 -> Kosten eines Ortsgespraechs
 *    0 -> FreeCall
 *   >0 -> für das angegebene Gespraech angefallene Kosten
 *
 * double sondertaktlaenge(time_t connect, int index, int *next)
 *   liefert folgendes zurueck:
 *   -3 -> Fehler
 *   -2 -> Taktlaenge nicht bekannt
 *   -1 -> Taktlaenge eines Ortsgespraechs
 *    0 -> kein Takt (FreeCall)
 *   >0 -> Taktlaenge des ersten Takts und in next die Laenge der folgenden
 *         Takte (momentan unbenutzt!)
 */

#define _SONDERNUMMERN_C_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include "sondernummern.h"

SonderNummern *SN;
int nSN;
int interns0 = 0;
double currency_factor = 0.12;

#define WA         0
#define WT         1
#define WE         2

static char *strip(char *s)
{
  char   *p;

  while (*s == ' ') s++;
  p = s + strlen(s) - 1;
  while (*p == ' ') p--;
  p++;
  *p = '\0';

  return s;
}

void exitSondernummern()
{
  int i;

  for (i=0; i< nSN; i++) {
    if (SN[i].number)
      free (SN[i].number);
    if (SN[i].info)
      free (SN[i].info);
  }

  if (SN)
    free(SN);

  nSN = 0;
  SN = NULL;
}

int initSondernummern(char *fn, char **msg)
{
  char   *s, *t, *pos, *number, *info, version[BUFSIZ], buf[BUFSIZ];
  static char message[BUFSIZ];
  int     provider, tarif, tday, tbegin, tend;
  FILE   *f;
  double  grund, takt;


  if (msg)
    *(*msg=message)='\0';

  SN = NULL;
  nSN = 0;
  strcpy(version, "unknown");

  f = fopen(fn, "r");

  if (f != (FILE *)NULL) {
    while ((pos = fgets(buf, BUFSIZ, f)))
      if (!memcmp(buf, "V:", 2)) {
        strcpy(version, buf + 2);

      	if ((pos = strchr(version, '\n')))
          *pos = 0;
      }
      else if (*pos != '#') {
        if ((s = strsep(&pos, "|"))) {
          provider = strtol(s, (char **)NULL, 10);
          if ((s = strsep(&pos, "|"))) {
            number = strip(s);
            if ((s = strsep(&pos, "|"))) {
              if (strstr(s, "free"))
                tarif = SO_FREE;
              else if (strstr(s, "City"))
                tarif = SO_CITYCALL;
              else if (strstr(s, "?"))
                tarif = SO_UNKNOWN;
              else
                tarif = SO_CALCULATE;
              if ((s = strsep(&pos, "|"))) {
                if (strstr(s, "WE"))
                  tday = WE;
                else if (strstr(s,"WT"))
                  tday = WT;
                else
                  tday = WA;
                if ((s = strsep(&pos, "|"))) {
                  tbegin = -1;
                  tend = -1;
                  if (strcmp(strip(s) ,"") != 0) {
                    if ((t = strsep(&s, "-"))) {
                      tbegin = strtol(t, (char **)NULL, 10);
                      tend = strtol(s, (char **)NULL, 10);
                    }
                  }
                  if ((s = strsep(&pos, "|"))) {
                    grund = strtod(s, (char **)NULL);
                    if ((s = strsep(&pos, "|"))) {
                      takt = strtod(s, (char **)NULL);
                      if ((s = strsep(&pos, "\n"))) {
                        info = s;

                        SN = (SonderNummern *)realloc(SN, (nSN+1)*sizeof(SonderNummern));
                        SN[nSN].provider = provider;
                        SN[nSN].number = strdup(number);
                        SN[nSN].tarif = tarif;
                        SN[nSN].tday = tday;
                        SN[nSN].tbegin = tbegin;
                        SN[nSN].tend = tend;
                        SN[nSN].grund = grund;
                        SN[nSN].takt = takt;
                        SN[nSN].info = strdup(info);
                        nSN++;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    fclose(f);
    if (msg) sprintf(*msg=message, "Sonderrufnummern Version %s loaded [%d entries from %s]", version, nSN, fn);
    return nSN;
  }
  else {
    if (msg) sprintf(*msg=message, "*** Cannot load Sonderrufnummern (%s : %s)", fn, strerror(errno));
    return -1;
  }
}

int is_sondernummer(char *number, int provider)
{
  int i;

  if ((strlen(number) >= interns0) && ((*number == '0') || (*number == '1') ||
                                       (*number == '2')))
    for (i = 0; i < nSN; i++)
      if ((strncmp(number, SN[i].number, strlen(SN[i].number)) == 0) &&
          (provider == SN[i].provider))
        return(i);

  return(-1);
}

char *sondernummername(int index)
{
  if ((index > -1) && (index < nSN))
    return(SN[index].info);
  else
    return(NULL);
}

char *sondernum(int index)
{
  if ((index > -1) && (index < nSN))
    return(SN[index].number);
  else
    return(NULL);
}

int sondertarif(int index)
{
  if ((index > -1) && (index < nSN))
    return(SN[index].tarif);
  else
    return(SO_FAIL);
}

static int searchentry(time_t connect, int index)
{
  int        provider;
  char      *number;
  struct tm *t;

  if (index != -1) {
    number = SN[index].number;
    provider = SN[index].provider;
      t = localtime(&connect);
    while ((strncmp(number, SN[index].number, strlen(SN[index].number)) == 0)
           && (provider == SN[index].provider) &&
           !(((SN[index].tday == WA) ||
              ((SN[index].tday == WE) && ((t->tm_wday == 0) || (t->tm_wday == 6))) ||
              ((SN[index].tday == WT) && (t->tm_wday >= 1) && (t->tm_wday <= 5))) &&
             ((SN[index].tbegin == -1) ||
              ((t->tm_hour >= SN[index].tbegin) && (t->tm_hour < SN[index].tend)))))
      index++;
  }
  return(index);
}

double sonderpreis(time_t connect, int duration, int index)
{
  int       i;
  double    preis;

  if ((i = searchentry(connect, index)) != -1)
    if (SN[i].tarif == SO_CALCULATE) {
      if (SN[i].takt == 0)
        preis = SN[i].grund * currency_factor;
      else
        preis = (ceil(duration / SN[i].takt) + SN[i].grund) * currency_factor;
      return(preis);
    }
    else
      return(SN[i].tarif);
  else
    return(SO_FAIL);
}

double sondertaktlaenge(time_t connect, int index, int *next)
{
  int        i;
  double     taktlen;

  if ((i = searchentry(connect, index)) != -1)
    if (SN[i].tarif == SO_CALCULATE) {
      taktlen = SN[i].takt;
      return(taktlen);
    }
    else
      return(SN[i].tarif);
  else
    return(SO_FAIL);
}


#ifdef STANDALONE
#define TEST 90
int main(int argc, char *argv[])
{
  struct tm t;
  time_t    time;
  double    preis;
  char     *msg;
  int       index;

  if (argc > 6) {
    initSondernummern("../sonderrufnummern.dat", &msg);
    fprintf(stdout, "%s\n", msg);

    t.tm_sec = t.tm_min = 0;
    t.tm_hour = strtol(argv[3], (char **)NULL, 10);
    t.tm_mday = strtol(argv[4], (char **)NULL, 10);
    t.tm_mon = strtol(argv[5], (char **)NULL, 10) - 1;
    t.tm_year = 99;
    t.tm_isdst = -1;
    time = mktime(&t);

    /*
      fprintf(stdout, "Testausgabe für Eintrag: %d\n", TEST);
      fprintf(stdout, "Provider:                %d\n", SN[TEST].provider);
      fprintf(stdout, "Prefix:                  \"%s\"\n", SN[TEST].number);
      fprintf(stdout, "Tarif:                   %d\n", SN[TEST].tarif);
      fprintf(stdout, "Tariftag:                %d\n", SN[TEST].tday);
      fprintf(stdout, "Tarifanfang:             %d\n", SN[TEST].tbegin);
      fprintf(stdout, "Tarifende:               %d\n", SN[TEST].tend);
      fprintf(stdout, "Grundtarif:              %f\n", SN[TEST].grund);
      fprintf(stdout, "Takt:                    %f\n", SN[TEST].takt);
      fprintf(stdout, "Kurzbeschreibung:        \"%s\"\n", SN[TEST].info);
      exit(0);
    */

    if ((index = is_sondernummer(argv[2],
                                 strtol(argv[1], (char **)NULL, 10))) != -1) {
      fprintf(stdout, "Sonderrufnummer: %s\n", argv[2]);
      fprintf(stdout, "Beschreibung   : %s\n", sondernummername(index));
      preis = sonderpreis(time, strtol(argv[6], (char **)NULL, 10), index);
      if (preis == SO_CITYCALL)
        fprintf(stdout, "Kosten         : CityCall\n");
      else if (preis == SO_FREE)
        fprintf(stdout, "Kosten         : freecall\n");
      else if (preis == SO_UNKNOWN)
        fprintf(stdout, "Kosten         : unbekannt\n");
      else
        fprintf(stdout, "Kosten         : %f\n", preis);
      fprintf(stdout, "Taktlänge      : %f\n",
              sondertaktlaenge(time, index, NULL));
    }
    else
      fprintf(stdout, "Die Nummer %s ist keine Sonderrufnummer.\n", argv[2]);
  }
  else
    fprintf(stdout, "Usage: %s Provider Rufnummer Stunde Tag Monat Länge\n",
            argv[0]);
  exitSondernummern();
}
#endif
