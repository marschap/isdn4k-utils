/* $Id: sondernummern.c,v 1.4 1999/03/14 14:26:51 akool Exp $
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
 * int initSondernummern(char *info)
 *   initialisiert die Sonderrufnummerndatenbank, liefert die Anzahl der
 *   eingelesenen Datensaetze zurueck oder im Fehlerfall -1
 *   liefert Versionsinfo in `info' zurueck
 *
 * void exitSondernummern()
 *   deinitialisiert die Sonderrufnummerndatenbank
 *
 * int is_sondernummer(char *number, int provider)
 *   liefert die Nummer des ersten passenden Datensatzes zurueck oder -1 im
 *   Fehlerfall
 *
 * char *sondernummername(char *number, int provider)
 *   liefert die Beschreibung des ersten passenden Datensatzes zurueck oder
 *   NULL im Fehlerfall
 *
 * double sonderpreis(time_t connect, int duration, char *number, int provider)
 *   liefert folgendes zurueck:
 *   -3 -> Fehler
 *   -2 -> Preis nicht bekannt
 *   -1 -> Kosten eines Ortsgespraechs
 *    0 -> FreeCall
 *   >0 -> für das angegebene Gespraech angefallene Kosten
 *
 * double sondertaktlaenge(time_t connect, char *number, int provider, int *next)
 *   liefert folgendes zurueck:
 *   -3 -> Fehler
 *   -2 -> Taktlaenge nicht bekannt
 *   -1 -> Taktlaenge eines Ortsgespraechs
 *    0 -> kein Takt (FreeCall)
 *   >0 -> Taktlaenge des ersten Takts und in next die Laenge der folgenden
 *         Takte (momentan unbenutzt!)
 */

#define _SONDERNUMMERN_C_

#ifdef STANDALONE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#else
#include "isdnlog.h"
#endif

#ifdef STANDALONE

#undef DATADIR	/* already defined via -DDATADIR=... */
#define DATADIR   ".."

#define SO_FAIL      -3
#define SO_UNKNOWN   -2
#define SO_CITYCALL  -1
#define SO_FREE       0
#define SO_CALCULATE  1

typedef struct {
  int    provider; /* Provider */
  char  *number;   /* Telefonnummer */
  int    tarif;    /* 0 = free, -1 = CityCall, -2 = unknown, 1 = calculate */
  int    tday;     /* 0 = alle Tage, 1 = Wochentag, 2 = Wochenende */
  int    tbegin;   /* Tarifanfang */
  int    tend;     /* Tarifende */
  double grund;    /* Grundtarif */
  double takt;     /* Zeittakt */
  char  *info;     /* Kurzbeschreibung */
} SonderNummern;

SonderNummern *SN;
int nSN;
int interns0 = 0;
#endif

#define WA         0
#define WT         1
#define WE         2

char *strip(char *s)
{
  char   *p;

  while (*s == ' ') s++;
  p = s + strlen(s) - 1;
  while (*p == ' ') p--;
  p++;
  *p = '\0';

  return s;
}

int initSondernummern(char *msg)
{
  char   *s, *t, *pos, *number, *info, fn[BUFSIZ], version[BUFSIZ];
  int     provider, tarif, tday, tbegin, tend;
  FILE   *f;
  char    buf[BUFSIZ];
  double  grund, takt;


  SN = NULL; nSN = 0;

  sprintf(fn, "%s/sonderrufnummern.dat", DATADIR);
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

                        nSN++;
                        SN = (SonderNummern *)realloc(SN, sizeof(SonderNummern) * nSN);
                        SN[nSN - 1].provider = provider;
                        SN[nSN - 1].number = strdup(number);
                        SN[nSN - 1].tarif = tarif;
                        SN[nSN - 1].tday = tday;
                        SN[nSN - 1].tbegin = tbegin;
                        SN[nSN - 1].tend = tend;
                        SN[nSN - 1].grund = grund;
                        SN[nSN - 1].takt = takt;
                        SN[nSN - 1].info = strdup(info);
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
    sprintf(msg, "Sonderrufnummern Version %s loaded [%d entries]", version, nSN);
    return(nSN);
  }
  else {
    sprintf(msg, "*** Cannot load Sonderrufnummern (%s : %d)", fn, errno);
    return(-1);
  }
}

void exitSondernummern()
{
  nSN = 0;
  free(SN);
  SN = NULL;
}

int is_sondernummer(char *number, int provider)
{
  int i;

  if ((strlen(number) >= interns0) && ((*number == '0') || (*number == '1')))
    for (i = 0; i < nSN; i++)
      if ((strncmp(number, SN[i].number, strlen(SN[i].number)) == 0) &&
          (provider == SN[i].provider))
        return(i);

  return(-1);
}

char *sondernummername(char *number, int provider)
{
  int i;

  if ((i = is_sondernummer(number, provider)) != -1)
    return(SN[i].info);
  else
    return(NULL);
}

int searchentry(time_t connect, char *number, int provider)
{
  int        i;
  struct tm *t;

  if ((i = is_sondernummer(number, provider)) != -1) {
      t = localtime(&connect);
      while ((strncmp(number, SN[i].number, strlen(SN[i].number)) == 0) &&
             (provider == SN[i].provider) &&
             !(((SN[i].tday == WA) ||
                ((SN[i].tday == WE) && ((t->tm_wday == 0) || (t->tm_wday == 6))) ||
                ((SN[i].tday == WT) && (t->tm_wday >= 1) && (t->tm_wday <= 5))) &&
               ((SN[i].tbegin == -1) ||
                ((t->tm_hour >= SN[i].tbegin) && (t->tm_hour < SN[i].tend)))))
        i++;
  }
  return(i);
}

double sonderpreis(time_t connect, int duration, char *number, int provider)
{
  int       i;
  double    preis;

  if ((i = searchentry(connect, number, provider)) != -1)
    if (SN[i].tarif == SO_CALCULATE) {
      if (SN[i].takt == 0)
        preis = SN[i].grund * 0.12;
      else
        preis = (ceil(duration / SN[i].takt) + SN[i].grund) * 0.12;
      return(preis);
    }
    else
      return(SN[i].tarif);
  else
    return(SO_FAIL);
}

double sondertaktlaenge(time_t connect, char *number, int provider, int *next)
{
  int        i;
  double     taktlen;

  if ((i = searchentry(connect, number, provider)) != -1)
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
  double    preis;

  if (argc > 6) {
    fprintf(stdout, "%d Einträge aus \"%s/sonderrufnummern.dat\" eingelesen.\n",
            initSondernummern(), DATADIR);
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
    if (is_sondernummer(argv[2], strtol(argv[1], (char **)NULL, 10)) != -1) {
      t.tm_sec = t.tm_min = 0;
      t.tm_hour = strtol(argv[3], (char **)NULL, 10);
      t.tm_mday = strtol(argv[4], (char **)NULL, 10);
      t.tm_mon = strtol(argv[5], (char **)NULL, 10) - 1;
      t.tm_year = 99;
      t.tm_isdst = -1;
      fprintf(stdout, "Sonderrufnummer: %s\n", argv[2]);
      fprintf(stdout, "Beschreibung   : %s\n",
              sondernummername(argv[2], strtol(argv[1], (char **)NULL, 10)));
      preis = sonderpreis(mktime(&t), strtol(argv[6], (char **)NULL, 10),
                          argv[2], strtol(argv[1], (char **)NULL, 10));
      if (preis == SO_CITYCALL)
        fprintf(stdout, "Kosten         : CityCall\n");
      else if (preis == SO_FREE)
        fprintf(stdout, "Kosten         : freecall\n");
      else if (preis == SO_UNKNOWN)
        fprintf(stdout, "Kosten         : unbekannt\n");
      else
        fprintf(stdout, "Kosten         : %f\n", preis);
      fprintf(stdout, "Taktlänge      : %f\n",
              sondertaktlaenge(mktime(&t), argv[2],
                               strtol(argv[1], (char **)NULL, 10), NULL));
    }
    else
      fprintf(stdout, "Die Nummer %s ist keine Sonderrufnummer.\n", argv[1]);
  }
  else
    fprintf(stdout, "Usage: %s Provider Rufnummer Stunde Tag Monat Länge\n",
            argv[0]);
  exitSondernummern();
}
#endif
