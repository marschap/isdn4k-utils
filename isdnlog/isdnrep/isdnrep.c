/* $Id: isdnrep.c,v 1.1 1997/03/16 20:59:05 luethje Exp $
 *
 * ISDN accounting for isdn4linux. (Report-module)
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
 * $Log: isdnrep.c,v $
 * Revision 1.1  1997/03/16 20:59:05  luethje
 * Added the source code isdnlog. isdnlog is not working yet.
 * A workaround for that problem:
 * copy lib/policy.h into the root directory of isdn4k-utils.
 *
 * Revision 2.6.19  1997/01/04  13:00:35  akool
 * Korrektur bzgl. ISDN_CH
 * Danke an Markus Maeder (mmaeder@cyberlink.ch)
 *
 * Revision 2.6.15  1997/01/02  11:32:35  akool
 * -V Option added
 *
 * Revision 2.6.11  1996/12/30  10:17:35  akool
 * Dutch accounting (Ad Aerts <ad@aasup.nl>)
 *
 * Revision 2.50  1996/08/25  11:48:35  akool
 * Auswertung bei non-AOCE-Anschluessen verbessert
 *
 * Revision 2.13  1996/03/16  16:11:35  akool
 * Andruck der "ibytes" implementiert
 *
 * Revision 2.10  1996/01/16  01:04:35  sl
 * Ausgabe von bestimmten MSN's ergaenzt, Nur ankommende dor ausgehende
 * Gespraeche anzeigen, Ueberarbeitung der kompletten Sourcen.
 * Funktion fuer fremde Waehrungen ergaenzt
 *
 * Revision 2.03  1996/01/07  03:35:17  sl
 * Optionen -c und -6 entfernt, Tab-Bug behoben, Bug in automatischer
 * Gebuehrenberechnung beim alten Logformat behoben,
 * Zeitangabe beim Summary,
 *
 * Revision 2.02  1996/01/07  03:35:17  sl
 * Zeitraumausgabe angepasst, loeschen von isdn.log-Eintraegen
 *
 * Revision 2.01  1996/01/06  18:15:20  sl
 * Zeitraumausgabe, nur Calls-Anzeige, optionales Logfile
 *
 * Revision 2.00  1996/01/01  17:50:19  akool
 * Auswertung der neuen Eintraege (I/O, Cause) implementiert
 * Vollstaendiges Redesign
 *
 * Revision 1.25  1995/11/19  09:56:19  akool
 * Neue Option "-c" aktiviert die eigene Berechnung der Einheiten bei
 *   fehlender Gebuehreninfo
 *
 * Revision 1.24  1995/11/12  11:09:19  akool
 * Formatfelder vergroessert
 * Fremdverursachte Einheiten werden ge-()
 * Alle Floating-Point-Zahlen werden deutsch (mit Komma) ausgegeben
 *
 * Revision 1.23  1995/11/06  18:04:19  akool
 * Denkfehler in der "-6" Berechnung :-(
 *
 * Revision 1.22  1995/10/22  15:31:19  akool
 * Aufschluesselung bach Tarifzonen implementiert
 *
 * Revision 1.21  1995/10/17  19:53:19  akool
 * Auf echte Charging-Info umgestellt
 *
 * Revision 1.20  1995/10/15  17:58:19  akool
 * Vergleich errechnete zu uebermittelten Einheiten implementiert
 *
 * Revision 1.16  1995/10/12  13:50:19  akool
 * Neue Option "MYMSNS=x" in der "isdnlog.conf" zum definieren der Anzahl
 * eigener MSN's
 *
 * Revision 1.15  1995/10/09  18:42:18  akool
 * Offensichtlich sinnlose Daten i.d. "isdn.log" werden nun ignoriert
 *
 * Revision 1.14  1995/10/08  10:25:16  akool
 * Ueberschreitung verschiedener Zeittakte bei einer Verbindung implementiert.
 *
 * Revision 1.6  1995/09/30  19:55:17  akool
 * First public release
 *
 * Revision 1.1  1995/09/23  16:44:19  akool
 * Initial revision
 *
 */


#include "isdnrep.h"

/*****************************************************************************/

#define END_TIME		1
#define STDOUT			0
#define ERROUT			1

/*****************************************************************************/

#define LOG_VERSION_1 0
#define LOG_VERSION_2 1
#define LOG_VERSION_3 2

/*****************************************************************************/

static time_t get_month(char *String, int TimeStatus);
static time_t get_time(char *String, int TimeStatus);
static int get_term (char *String, time_t *Begin, time_t *End,int delentries);
static int set_msnlist(char *String);
static int show_msn(one_call *cur_call);
static int add_sum_calls(sum_calls *s1, sum_calls *s2);
static int print_sum_calls(sum_calls *s, int computed);
static int print_string(char *String, int outdir);
static int add_one_call(sum_calls *s1, one_call *s2, double units);
static int clear_sum(sum_calls *s1);
static char *print_currency(int units, double money, int Format);
static void strich(int type);
static int n_match(char *Pattern, char* Number, int version);

/*****************************************************************************/

typedef struct {
  char   num[NUMSIZE];
  int	 called;
  int	 connects;
  time_t connect[MAXCONNECTS];
} UNKNOWN;

/*****************************************************************************/

static int      invertnumbers = 0;
static int      unknowns = 0;
static UNKNOWN  unknown[MAXUNKNOWN];
static int  	zones[MAXZONES];
static int  	zones_usage[MAXZONES];
static double  	zones_dm[MAXZONES];
static double  	zones_dur[MAXZONES];
static char**   ShowMSN = NULL;

/*****************************************************************************/

static char msg1[]    = "%s: Can't open %s (%s)\n";
static char usage[]   = "%s: usage: %s [ -%s ]\n";
static char wrongdate[]   = "unknown date: %s\n";
static char wrongdate2[]   = "wrong date for delete: %s\n";
static char nomemory[]   = "Out of memory!\n";


static char options[] = "ac:nviot:f:d:p:NV";
static char fnbuff[512];
static char *lfnam = LOGFILE;

/*****************************************************************************/

int main(int argc, char *argv[], char *envp[])
{
  register char      *p, *p1, *p2;
  register int	      i, j, k, n, cc;
  auto     FILE      *fi, *ftmp = NULL;
  auto     char       string[BUFSIZ], s[BUFSIZ], s1[BUFSIZ];
  auto	   char	      start[20], stop[20], timestring[256] = "";
  auto     char       tmp_string[256];
  auto	   char	      tmpfile[256];
  auto	   int	      hit, lday = -1;
  auto	   double     restdm = 0.0;
  auto	   int	      computed, go, zone = 1;
  auto	   int	      compute = 0, zeit;
  extern   int        errno;
  auto	   time_t     now, from = (time_t)0;
  auto	   time_t     begintime, endtime;
  auto	   double     t1, t2, takt;
  auto 	   struct tm *tm;
  auto	   double     einheit = 0.23;
  auto	   int	      nx[2];
  auto	   int 	      Tarif96 = 0, Tarif962 = 0, c, notforus, resteh = 0;
  auto	   double     restdur = 0.0, restidur = 0.0;
  auto	   int	      numbers = 0, restusage = 0;
  auto	   int	      restiusage = 0;
  auto	   int        verbose = 0;
  auto     int        timearea = 0, phonenumberonly = 0;
  auto     int        delentries = 0;
  auto     int        incomingonly = 0;
  auto     int        outgoingonly = 0;
  one_call            cur_call;
  sum_calls           day_sum, day_com_sum, all_sum, all_com_sum, tmp_sum;
  char    *myname = basename(argv[0]);


	set_print_fct_for_tools(printf);

  clear_sum(&day_sum);
  clear_sum(&day_com_sum);
  clear_sum(&all_sum);
  clear_sum(&all_com_sum);

	use_new_config = 1;

  if (!currency_factor)
    currency = "DM";

  while ((c = getopt(argc, argv, options)) != EOF)
    switch (c) {
      case 'a' : timearea++;
                 begintime = 0;
		 time(&endtime);
      	       	 break;

      case 'c' : compute = strtol(optarg, NIL, 0);
      	       	 break;

      case 'i' : incomingonly++;
      	       	 break;

      case 'o' : outgoingonly++;
      	       	 break;

      case 'n' : numbers++;
      	       	 break;

      case 'd' : delentries++;

      case 't' : strcpy(timestring, optarg);
                 if (!get_term(timestring,&begintime,&endtime,delentries))
                 {
                   sprintf(tmp_string, wrongdate, timestring);
                   print_string(tmp_string,ERROUT);
                   return 1;
                 }
                 timearea++;
                 break;

      case 'v' : verbose++;
      	       	 break;

      case 'f' : strcpy(fnbuff, optarg);
                 lfnam = fnbuff;
                 break;

      case 'p' : if (!phonenumberonly) set_msnlist(optarg);
      	       	 phonenumberonly++;
      	       	 break;

      case 'N' : use_new_config = 0;
                 break;

      case 'V' : print_version(myname);
                 exit(0);

      case '?' : sprintf(tmp_string, usage, argv[0], argv[0], options);
                 print_string(tmp_string,ERROUT);
                 return(1);
    } /* switch */

  if (readconfig(myname) != 0)
  	return 1;

  if (delentries)
    if(begintime)
    {
      sprintf(tmp_string, wrongdate2, timestring);
      print_string(tmp_string,ERROUT);
      return 1;
    }
    else
    {
      sprintf(tmpfile,"/tmp/isdnrep%d",getpid());

  		if ((ftmp = fopen(tmpfile, "w")) == (FILE *)NULL)
      {
        sprintf(tmp_string, msg1, tmpfile, strerror(errno));
        print_string(tmp_string,ERROUT);
        return(1);
      }
    }

  if (!timearea) { /* from darf nicht gesetzt werden, wenn alle Telefonate angezeigt werden sollen */
    time(&now);
    	/* aktuelle Zeit wird gesetzt */
    tm = localtime(&now);
    	/* Zeit von 1970 nach Struktur */
    tm->tm_sec = tm->tm_min = tm->tm_hour = 0;
    from = mktime(tm);
    	/* Struktur nach Zeit von 1970 */
    	/* from hat den aktuellen Tag 0.00 Uhr */
  } /* if */

  if ((fi = fopen(lfnam, "r")) != (FILE *)NULL){

    memset(zones, 0, sizeof(zones));

    *start = 0;

    while (fgets(s, BUFSIZ, fi)) {
      strcpy(string,s);

      if ((*s != '#') && (p = strchr(s, '|'))) {
      /* hier wird das erste Trennzeichen gesucht */

      	cur_call.eh = notforus = 0;
        cur_call.dir = -1;
        cur_call.cause = -1;
        	/* Zeigt die Gespraechsrichtung an: rein oder raus */
        cur_call.ibytes = cur_call.obytes = 0L;
	cur_call.version = 0.0;
        cur_call.si = cur_call.si1 = 0;

     	if ((p1 = p2 = strchr(p + 1, '|'))) {
          do {
            *p1-- = 0; /* p1 ist Zaehler fuer Anrufer */
          } while (isspace(*p1));

       	  strcpy(cur_call.num[0], p + 1);
       	  	/* num[0] enthaelt die Anrufernummer */

       	  p = p2 + 1;

          if ((p1 = p2 = strchr(p, '|'))) {

            do {
              *p1-- = 0;
            } while (isspace(*p1));

            strcpy(cur_call.num[1], p);
            	/* num[1] enthaelt die angerufende Nummer */

            p = p2 + 1;

            cur_call.duration = (double)atoi(p);
              /* Gespraechsdauer auf sek. */

            if ((p1 = p2 = strchr(p, '|'))) {
              p = p2 + 1;
              cur_call.duration = atoi(p) / 100.0;
              	/* Gespraechsdauer auf 100stel sek. */

              if ((p1 = p2 = strchr(p, '|'))) {
                p = p2 + 1;
                cur_call.t = atol(p);
                	/* t ist Zeit seit 1970 von Gespraechsende ? */
              	if ((p1 = p2 = strchr(p, '|'))) { /* Gebuehrenimpuls */
                  p = p2 + 1;
                  cur_call.eh = atoi(p);
                  	/* Verbrauchte Einheiten (-1 ist keine Angabe ?) */

              	  if ((p1 = p2 = strchr(p, '|'))) { /* I/O */
                    p = p2 + 1;
                    cur_call.dir = (*p == 'I') ? DIALIN : DIALOUT;
              	    if ((p1 = p2 = strchr(p, '|'))) { /* Cause */
                      p = p2 + 1;
            	      cur_call.cause = atoi(p);

              	      if ((p1 = p2 = strchr(p, '|'))) { /* ibytes */
              	        p = p2 + 1;
                        cur_call.ibytes = atol(p);

              	      	if ((p1 = p2 = strchr(p, '|'))) { /* obytes */
              	          p = p2 + 1;
                          cur_call.obytes = atol(p);

                          if ((p1 = p2 = strchr(p, '|'))) { /* Version */
              	            p = p2 + 1;
                            cur_call.version = atof(p);

                            if ((p1 = p2 = strchr(p, '|'))) { /* SI */
                              p = p2 + 1;
                              cur_call.si = atoi(p);

                              if ((p1 = p2 = strchr(p, '|'))) { /* SI11 */
                                p = p2 + 1;
                                cur_call.si1 = atoi(p);

                                if ((p1 = p2 = strchr(p, '|'))) { /* currency_factor */
                                  p = p2 + 1;
                                  cur_call.currency_factor = atoi(p);

                                  if ((p1 = p2 = strchr(p, '|'))) { /* currency */
                                    p = p2 + 1;
                                    strncpy(cur_call.currency, p, 3);

                                    if ((p1 = p2 = strchr(p, '|'))) { /* pay */
                                      p = p2 + 1;
                                      cur_call.pay = atof(p);
                                    } /* if */
                                  } /* if */
                                } /* if */
                              } /* if */
                            } /* if */
			  } /* if */
                        } /* if */
              	      } /* if */
                    } /* if */
              	  } /* if */
                } /* if */
              	else
              	{
                  cur_call.dir = DIALOUT;
                }
              }
              else
                cur_call.t = atom(s);
                	/* Umrechnung des ersten Teilstring in Zeit von 1970 */
            }
            else
              cur_call.t = atom(s);
               	/* Umrechnung des ersten Teilstring in Zeit von 1970 */

            if (!begintime)
              begintime = cur_call.t;

            if (timearea)
            {
              if (delentries)
                if (cur_call.t > endtime)
                  fputs(string,ftmp);
                else
                  continue;

              if (cur_call.t < begintime || cur_call.t > endtime)
                continue;
            }
            else
            if (cur_call.t < from)
              continue; /* Einlesen des naechsten Telefonats */

            if (!verbose && cur_call.duration == 0)
                continue;

            if (phonenumberonly)

              if (!show_msn(&cur_call))
/*              if (!show_msn(cur_call.num[cur_call.dir==DIALOUT?CALLING:CALLED])) */
                continue;

            if (incomingonly && cur_call.dir == DIALOUT)
              continue;

            if (outgoingonly && cur_call.dir == DIALIN)
              continue;

            tm = localtime(&(cur_call.t));
    					/* Zeit von 1970 nach Struktur */
            day = tm->tm_mday;
            month = tm->tm_mon;

            Tarif96 = tm->tm_year > 95;

            /* Ab Juli '96 gibt's noch mal eine Senkung in den Zonen 3 und 4
               genaue Preise sind jedoch noch nicht bekannt. FIX-ME */

            Tarif962 = Tarif96 && (tm->tm_mon > 5);

            if (!currency_factor)
#ifdef ISDN_NL
              einheit = 0.0011; /* cost of one second local tariff
                                   during office hours */
#elif defined(ISDN_CH)
              einheit = 0.01;
#else
              einheit = Tarif96 ? 0.12 : 0.23;
#endif
            else
              einheit = currency_factor;

            if (cur_call.duration > 172800.0) /* laenger als 2 Tage? Das KANN nicht sein! */
              cur_call.duration = 0;


            for (n = CALLING; n <= CALLED; n++) {

              nx[n] = -1;
              hit = 0;

              if (!*(cur_call.num[n])) {
                if (!numbers)
                {
                  cur_call.num[n][0] = C_UNKNOWN;
                  cur_call.num[n][1] = '\0';
                }
                    /* Wenn keine Nummer, dann Name "UNKNOWN" */

                hit++;
              }
              else {
                /* In der folg. Schleife werden Nummern durch Namen ersetzt */
                cc = 0;

                for (j = 0; ((j < 2) && !cc); j++) {
                  for (i = 0; i < knowns; i++) {

                    if (*cur_call.num[n] != '0') {

                      /* Alte Syntax der "isdn.log" : Ohne vorlaufene "0" */
                      cc = ((known[i]->si == cur_call.si) || j) &&
                    	     !match(known[i]->num+1, cur_call.num[n], LOG_VERSION_1);

                      if (!cc) {

                        /* Ganz alte Syntax der "isdn.log" : Ohne Vorwahl */
                      	cc = ((known[i]->si == cur_call.si) || j) &&
                    	       !n_match(known[i]->num, cur_call.num[n], LOG_VERSION_1);
                      } /* if */
                    }
                    else if (*(cur_call.num[n] + 1) != '0') { /* pre 2.6 Syntax : ohne int. Vorwahl */
                      cc = ((known[i]->si == cur_call.si) || j) &&
                    	     !n_match(known[i]->num, cur_call.num[n], LOG_VERSION_2);
                    }
                    else
                      cc = ((known[i]->si == cur_call.si) || j) &&
                    	     !n_match(known[i]->num, cur_call.num[n], LOG_VERSION_3);

                    if (cc) {


                      if (!numbers)
                        strcpy(cur_call.num[n], known[i]->who);


                      nx[n] = i;

                      known[i]->usage[cur_call.dir]++;
                    	  /* Gesamte Anrufe auf gleiche Nummern werden gezeahlt */
                      known[i]->dur[cur_call.dir] += cur_call.duration;
                    	  /* Gesamte Gespraechsdauer auf gleiche Nummern wird gezeahlt */

                      hit++;
                      break;
                    } /* if */

                  } /* for */
                } /* for */

                /* In der naechsten Schleife werden die unbekannten Nummern
       						 registriert */
              	if (!hit) {
                  for (i = 0; i < unknowns; i++)
                    if (!strcmp(unknown[i].num, cur_call.num[n])) {
                      hit++;
                      break;
                    } /* if */

                  strcpy(unknown[i].num, cur_call.num[n]);
                  unknown[i].called = !n;
                  unknown[i].connect[unknown[i].connects] = cur_call.t;

                  /* ACHTUNG: MAXCONNECTS und MAXUNKNOWN sollten unbedingt groesser sein ! */
                  if (unknown[i].connects + 1 < MAXCONNECTS)
                    unknown[i].connects++;
                  else
                  {
                    sprintf(tmp_string, "%s: WARNING: Too many unknown connection's from %s\n", argv[0], unknown[i].num);
                    print_string(tmp_string,ERROUT);
                  }

                  if (!hit) {
                    if (unknowns < MAXUNKNOWN)
                      unknowns++;
                    else
                    {
                      sprintf(tmp_string, "%s: WARNING: Too many unknown number's\n", argv[0]);
                      print_string(tmp_string,ERROUT);
                    }
                  } /* if */

                } /* if */
              } /* else */
            } /* for */


            strcpy(s1, ctime(&cur_call.t));

            if ((p = strchr(s1, '\n')))
              *p = 0;

            if (day != lday) {
              if (lday == -1) {
                time(&now);
                sprintf(tmp_string,"I S D N  Connection Report  -  %s", ctime(&now));
                print_string(tmp_string,STDOUT);
              }
              else {
              	strich(1);
                print_sum_calls(&day_sum,0);

                if (day_com_sum.eh)
                {
                  print_sum_calls(&day_com_sum,1);

                  clear_sum(&tmp_sum);
                  add_sum_calls(&tmp_sum,&day_sum);
                  add_sum_calls(&tmp_sum,&day_com_sum);
                  strich(1);
                  print_sum_calls(&tmp_sum,0);
                }
                else
                  print_string("\n",STDOUT);

                add_sum_calls(&all_sum,&day_sum);
                clear_sum(&day_sum);

                add_sum_calls(&all_com_sum,&day_com_sum);
                clear_sum(&day_com_sum);
              } /* if */

              *(s1 + 10) = 0;
              sprintf(tmp_string,"\n\n%s %s\n", s1, s1 + 20);
              print_string(tmp_string,STDOUT);
              lday = day;

              if (!*start)
              {
                sprintf(start, "%s %s", s1, s1 + 20);
                sprintf(stop, "%s %s", s1, s1 + 20);
              }
              else
                sprintf(stop, "%s %s", s1, s1 + 20);

            } /* if */


            *(s1 + 19) = 0;

            sprintf(tmp_string,"  %s %s",
              s1 + 11,
              double2clock(cur_call.duration));
            print_string(tmp_string,STDOUT);

            if (cur_call.dir)
            {
              sprintf(tmp_string," %14s <- %-14s",
          			cur_call.num[CALLED], cur_call.num[CALLING]);
              print_string(tmp_string,STDOUT);
            }
            else
            {
              sprintf(tmp_string," %14s -> %-14s",
          			cur_call.num[CALLING], cur_call.num[CALLED]);
              print_string(tmp_string,STDOUT);
            }

            computed = 0;

#if 0
            if (compute && !currency_factor &&
                !cur_call.dir && ((cur_call.eh == -1) ||
                (!cur_call.eh && cur_call.duration && cur_call.cause == -1))) { /* DIALOUT, keine AOCE Meldung */
#else
            go = 0;

            if ((cur_call.eh == -1) && !cur_call.dir) { /* Rauswahl, Gebuehr unbekannt */
              if (nx[1] == -1) {       		      	/* Gegner unbekannt! */
                if (compute) {
                  zone = compute;                       /* in "-c x" Zone vermuten */
                  go = 1;
                }
                else {                                  /* mit 0 DM berechnen */
                  cur_call.eh = 0;
                  go = 0;
                } /* else */
              }
              else {
                go = 1;
                zone = known[nx[1]]->zone;
              } /* else */
            } /* if */

            if (go) {
#endif
              t1 = cur_call.t;
              t2 = cur_call.t + cur_call.duration;

              cur_call.eh = takt = 0;
              computed = 1;

              while (t1 < t2) {
                if (Tarif96)
                  takt = cheap96((time_t)t1, zone, &zeit);
                else
                  takt = cheap((time_t)t1, zone);

                if (!takt) {
                  sprintf(tmp_string, "%s: OOPS! Abbruch: Zeittakt==0 ???\n", argv[0]);
                  print_string(tmp_string,ERROUT);
                  break;
                } /* if */

                cur_call.eh++;
                t1 += takt;

              } /* while */
            } /* if */

            if (cur_call.duration || (cur_call.eh > 0)) {
              add_one_call(computed?&day_com_sum:&day_sum,&cur_call,einheit);

              if (cur_call.dir) {
                if (nx[CALLING] == -1) {
                  restiusage++;

                  if (cur_call.duration > 0)
                    restidur += cur_call.duration;
                } /* if */
              }
              else {

                if (nx[CALLED] == -1) {
                  resteh += cur_call.eh;
                  restdm += cur_call.dm;
                  restusage++;

                  if (cur_call.duration > 0)
                    restdur += cur_call.duration;
                }
                else {
                  known[nx[CALLED]]->eh += cur_call.eh;
                  known[nx[CALLED]]->dm += cur_call.dm;

                  zones[known[nx[CALLED]]->zone] += cur_call.eh;
                  zones_dm[known[nx[CALLED]]->zone] += cur_call.dm;

                  if (cur_call.duration > 0)
                    zones_dur[known[nx[CALLED]]->zone] += cur_call.duration;

                  zones_usage[known[nx[CALLED]]->zone]++;
                } /* if */

                sprintf(tmp_string," %s",
                  print_currency(cur_call.eh,cur_call.dm,4));
                print_string(tmp_string,STDOUT);

                if (computed)
                  print_string(" *",STDOUT);
              } /* else */
            }
            else {
              if (cur_call.cause != -1)
              {
                sprintf(tmp_string," %s", qmsg(TYPE_CAUSE, VERSION_EDSS1, cur_call.cause));
                print_string(tmp_string,STDOUT);
                day_sum.err++;
              }
            } /* else */

            sprintf(tmp_string, " %*s", cur_call.dir ? 21 : 0, "");
            print_string(tmp_string, STDOUT);

            if (cur_call.ibytes) {
              sprintf(tmp_string, " I=%s kB", double2str((double)cur_call.ibytes / 1024.0, 10, 2, 0));
              print_string(tmp_string, STDOUT);
            } /* if */

            if (cur_call.obytes) {
              sprintf(tmp_string, " O=%s kB", double2str((double)cur_call.obytes / 1024.0, 10, 2, 0));
              print_string(tmp_string, STDOUT);
            } /* if */

            print_string("\n",STDOUT);
          } /* if */
        } /* if */
      } /* if */
    } /* while */

    fclose(fi);

    if (delentries) /* Erzeugt neue verkuerzte Datei */
      lday = -1;

    if (lday != -1) {

      if (timearea) {
        strich(1);
        print_sum_calls(&day_sum,0);

        if (day_com_sum.eh)
        {
          print_sum_calls(&day_com_sum,1);

          clear_sum(&tmp_sum);
          add_sum_calls(&tmp_sum,&day_sum);
          add_sum_calls(&tmp_sum,&day_com_sum);
          strich(1);
          print_sum_calls(&tmp_sum,0);
        }
        else
          print_string("\n",STDOUT);
      } /* if */

      add_sum_calls(&all_sum,&day_sum);
      add_sum_calls(&all_com_sum,&day_com_sum);

      strich(2);
      print_sum_calls(&all_sum,0);

      if (all_com_sum.eh)
      {
        print_sum_calls(&all_com_sum,1);

        clear_sum(&tmp_sum);
        add_sum_calls(&tmp_sum,&all_sum);
        add_sum_calls(&tmp_sum,&all_com_sum);
        strich(2);
        print_sum_calls(&tmp_sum,0);
      }
      else
        print_string("\n",STDOUT);

      if (!incomingonly)
      {
        if (timearea)
        {
          sprintf(tmp_string,"\nDIALOUT Summary for %s .. %s\n", start, stop);
          print_string(tmp_string,STDOUT);
        }
        else
        {
          sprintf(tmp_string,"\nDIALOUT Summary for %s\n", start);
          print_string(tmp_string,STDOUT);
        }

        strich(3);

        for (i = mymsns; i < knowns; i++) {
          if (known[i]->usage[DIALOUT]) {
            sprintf(tmp_string,"%-14s %4d call(s) %s  %s\n",
              !numbers?known[i]->who:known[i]->num,
              known[i]->usage[DIALOUT],
              double2clock(known[i]->dur[DIALOUT]),
              print_currency(known[i]->eh,known[i]->dm,7));
            print_string(tmp_string,STDOUT);
          } /* if */
        } /* for */

        if (restusage) {
          sprintf(tmp_string,"%-14s %4d call(s) %s  %s\n",
            S_UNKNOWN,
            restusage,
            double2clock(restdur),
            print_currency(resteh,restdm,7));
          print_string(tmp_string,STDOUT);
        } /* if */

        print_string("\n",STDOUT);
      }

      if (!outgoingonly)
      {
        if (timearea)
        {
          sprintf(tmp_string,"\nDIALIN Summary for %s .. %s\n", start, stop);
          print_string(tmp_string,STDOUT);
        }
        else
        {
          sprintf(tmp_string,"\nDIALIN Summary for %s\n", start);
          print_string(tmp_string,STDOUT);
        }

        strich(3);

        for (i = mymsns; i < knowns; i++) {
          if (known[i]->usage[DIALIN]) {
            sprintf(tmp_string,"%-14s %4d call(s) %s\n",
              !numbers?known[i]->who:known[i]->num,
              known[i]->usage[DIALIN],
              double2clock(known[i]->dur[CALLED]));
            print_string(tmp_string,STDOUT);
          } /* if */
        } /* for */

        if (restiusage) {
          sprintf(tmp_string,"%-14s %4d call(s) %s\n",
            S_UNKNOWN,
            restiusage,
            double2clock(restidur));
          print_string(tmp_string,STDOUT);
        } /* if */

        print_string("\n\n",STDOUT);
      }

      for (i = 1; i < MAXZONES; i++)
        if (zones[i]) {

          p = "";

          switch (i) {
            case 1 : p = "City";       break;
            case 2 : p = "Region 50";  break;
            case 3 : p = "Region 200"; break;
            case 4 : p = "Fernzone";   break;
          } /* switch */

          sprintf(tmp_string,"Zone %d : %-15s %4d call(s) %s  %s\n", i, p,
            zones_usage[i],
            double2clock(zones_dur[i]),
            print_currency(zones[i],zones_dm[i],7));
        print_string(tmp_string,STDOUT);
        } /* if */

      if (resteh)
      {
        sprintf(tmp_string,"Zone x : %-15s %4d call(s) %s  %s\n", S_UNKNOWN,
          restusage,
          double2clock(restdur),
#ifdef ISDN_NL
          print_currency(resteh, resteh * einheit + restusage * 0.0825,7));
#else
          print_currency(resteh, resteh * einheit, 7));
#endif
        print_string(tmp_string,STDOUT);
      }

      if (unknowns) {
        print_string("\n\nUnknown caller(s)\n",STDOUT);
        strich(3);

        for (i = 0; i < unknowns; i++) {

          sprintf(tmp_string,"%s %-14s ", unknown[i].called ? "called by" : "  calling", unknown[i].num);
          print_string(tmp_string,STDOUT);

          for (k = 0; k < unknown[i].connects; k++) {
            strcpy(s1, ctime(&unknown[i].connect[k]));

            if ((p = strchr(s1, '\n')))
              *p = 0;

            *(s1 + 19) = 0;

            if (k && (k + 1) % 2) {
              print_string("\n\t\t\t ",STDOUT);
            } /* if */

            sprintf(tmp_string,"%s%s", k & 1 ? ", " : "", s1 + 4);
            print_string(tmp_string,STDOUT);
          } /* for */

          print_string("\n",STDOUT);
        } /* for */
      } /* if */
    } /* if */
  }
  else {
    sprintf(tmp_string, msg1, argv[0], lfnam, strerror(errno));
    print_string(tmp_string,ERROUT);
    return(1);
  } /* else */


  if (delentries) /* Erzeugt neue verkuerzte Datei */
  {
    fclose(ftmp);

 		if ((ftmp = fopen(tmpfile, "r")) == (FILE *)NULL)
    {
      sprintf(tmp_string, msg1, tmpfile, strerror(errno));
      print_string(tmp_string,ERROUT);
      return(1);
    }
 		if ((fi = fopen(lfnam, "w")) == (FILE *)NULL)
    {
      sprintf(tmp_string, msg1, lfnam, strerror(errno));
      print_string(tmp_string,ERROUT);
      return(1);
    }

    while (fgets(s, BUFSIZ, ftmp))
      fputs(s,fi);

    fclose(fi);
    fclose(ftmp);

    unlink(tmpfile);
  }

  return(0);
} /* main */

/*****************************************************************************/

static int get_term (char *String, time_t *Begin, time_t *End,int delentries)
{
  time_t now;
  time_t  Date[2];
  int Cnt;
  char DateStr[2][256] = {"",""};


  time(&now);

  if (String[0] == '-')
    strcpy(DateStr[1],String+1);
  else
  if (String[strlen(String)-1] == '-')
  {
    strcpy(DateStr[0],String);
    DateStr[0][strlen(String)-1] ='\0';
  }
  else
  if (strchr(String,'-'))
  {
    if (sscanf(String,"%[/.0-9]-%[/.0-9]",DateStr[0],DateStr[1]) != 2)
      return 0;
  }
  else
  {
    strcpy(DateStr[0],String);
    strcpy(DateStr[1],String);
  }

  for (Cnt = 0; Cnt < 2; Cnt++)
  {
    if (strchr(DateStr[Cnt],'/'))
      Date[Cnt] = get_month(DateStr[Cnt],delentries?0:Cnt);
    else
      Date[Cnt] = get_time(DateStr[Cnt],delentries?0:Cnt);
  }

  *Begin = DateStr[0][0] == '\0' ? 0 : Date[0];
  *End = DateStr[1][0] == '\0' ? now : Date[1];

  return 1;
}

/*****************************************************************************/

static time_t get_month(char *String, int TimeStatus)
{
  time_t now;
  int Cnt = 0;
  struct tm *TimeStruct;
  int Args[3];
  int ArgCnt;


  time(&now);
  TimeStruct = localtime(&now);
  TimeStruct->tm_sec = 0;
  TimeStruct->tm_min = 0;
  TimeStruct->tm_hour= 0;
  TimeStruct->tm_mday= 1;
  TimeStruct->tm_isdst= -1;

  ArgCnt = sscanf(String,"%d/%d/%d",&(Args[0]),&(Args[1]),&(Args[2]));

  switch (ArgCnt)
  {
    case 3:
      TimeStruct->tm_mday = Args[0];
      Cnt++;
    case 2:
      if (Args[Cnt+1] > 99)
        TimeStruct->tm_year = ((Args[Cnt+1] / 100) - 19) * 100 + (Args[Cnt+1]%100);
      else
        TimeStruct->tm_year = Args[Cnt+1];
    case 1:
      TimeStruct->tm_mon = Args[Cnt];
      break;
    default:
      return 0;
  }

  if (TimeStatus == END_TIME)
  {
    if (ArgCnt == 3)	/* Wenn Tag angegeben ist */
    {
      TimeStruct->tm_mday++;
      TimeStruct->tm_mon--;
    }
  }
  else
    TimeStruct->tm_mon--;

  return mktime(TimeStruct);
}

/*****************************************************************************/

static time_t get_time(char *String, int TimeStatus)
{
  time_t now;
  int Len = 0;
  int Year;
  char *Ptr;
  struct tm *TimeStruct;


  time(&now);
  TimeStruct = localtime(&now);
  TimeStruct->tm_sec = 0;
  TimeStruct->tm_min = 0;
  TimeStruct->tm_hour= 0;
  TimeStruct->tm_isdst= -1;

  switch (strlen(String))
  {
    case 0:
            return 0;
    case 1:
    case 2:
            TimeStruct->tm_mday = atoi(String);
            break;
    default:
            Len = strlen(String);

      if ((Ptr = strchr(String,'.')) != NULL)
      {
        TimeStruct->tm_sec = atoi(Ptr+1);
        Len -= strlen(Ptr);
      }

      if (Len % 2)
        return 0;

      if (sscanf(String,"%2d%2d%2d%2d%d",
          &(TimeStruct->tm_mon),
          &(TimeStruct->tm_mday),
          &(TimeStruct->tm_hour),
          &(TimeStruct->tm_min),
          &Year		) 		> 4)
        if (Year > 99)
          TimeStruct->tm_year = ((Year / 100) - 19) * 100 + (Year%100);
        else
          TimeStruct->tm_year = Year;

      TimeStruct->tm_mon--;
      break;
  }

  if (TimeStatus == END_TIME)
    if (TimeStruct->tm_sec == 0 &&
        TimeStruct->tm_min == 0 &&
        TimeStruct->tm_hour== 0   )
      TimeStruct->tm_mday++;
    else
    if (TimeStruct->tm_sec == 0 &&
        TimeStruct->tm_min == 0   )
      TimeStruct->tm_hour++;
    else
    if (TimeStruct->tm_sec == 0   )
      TimeStruct->tm_min++;
    else
      TimeStruct->tm_sec++;

  return mktime(TimeStruct);
}

/*****************************************************************************/

static int set_msnlist(char *String)
{
  int Cnt;
  int Value = 1;
  char tmp_string[256];
  char *Ptr = String;


  if (ShowMSN)
    return 0;

  while((Ptr = strchr(Ptr,',')) != NULL)
  {
    Ptr++;
    Value++;
  }

  ShowMSN = (char**) calloc(Value+1,sizeof(char*));
  for(Cnt = 0; Cnt < Value; Cnt++)
    ShowMSN[Cnt] = (char*) calloc(NUMSIZE,sizeof(char));

  if (!ShowMSN)
  {
    sprintf(tmp_string, nomemory);
    print_string(tmp_string,ERROUT);
    exit(1);
  }

  if (*String == 'n')
  {
    ++String;
    invertnumbers++;
  }

  Cnt = 0;

  while(String)
  {

    if (*String == 'm')
    {
      Value = atoi(++String);

      if (Value > 0 && Value <= mymsns)
      {
        strcpy(ShowMSN[Cnt],known[Value-1]->num);
        Cnt++;
      }
      else
      {
        sprintf(tmp_string,"Invalid MSN %d!\n",Cnt);
        print_string(tmp_string,ERROUT);
      }
    }
    else
      sscanf(String,"%[^,],",ShowMSN[Cnt++]);

    String = strchr(String,',');
    if (String) String++;
  }

  return 0;
}

/*****************************************************************************/

static int show_msn(one_call *cur_call)
{
  int Cnt;

  for(Cnt = 0; ShowMSN[Cnt] != NULL; Cnt++)
    if (!num_match(ShowMSN[Cnt], cur_call->num[0]) ||
        !num_match(ShowMSN[Cnt]   , cur_call->num[1]))
      return !invertnumbers;
    else
    if (!strcmp(ShowMSN[Cnt],"0"))
      return !invertnumbers;

  return invertnumbers;
}

/*****************************************************************************/

static int print_string(char *String, int outdir)
{
  if (outdir == ERROUT)
    return fprintf(stderr,"%s",String);
  else
    return printf("%s",String);
}

/*****************************************************************************/

static char *print_currency(int units, double money, int Format)
{
  static char RetCode[256];


  if (!currency_factor)
    sprintf(RetCode,"%*d EH  %s DM",Format,units,double2str(money,8,2,0));
  else
    sprintf(RetCode,"%.*s     %s %s",Format,"          ",
      double2str(money,8,2,0),currency);

  return RetCode;
}

/*****************************************************************************/

static int print_sum_calls(sum_calls *s, int computed)
{
  static char String[256];

  if (!computed)
    sprintf(String,"%3d IN=%s, %3d OUT=%s, %3d failed     %s  I=%s kB O=%s kB\n",
      s->in,
      double2clock(s->din),
      s->out,
      double2clock(s->dout),
      s->err,
      print_currency(s->eh,s->dm,7),
      double2str((double)s->ibytes / 1024.0, 10, 2, 0),
      double2str((double)s->obytes / 1024.0, 10, 2, 0));
  else
    sprintf(String,"                 %3d OUT=%s,                %s *\n",
      s->out,
      double2clock(s->dout),
      print_currency(s->eh,s->dm,7));

  return print_string(String,STDOUT);
}

/*****************************************************************************/

static int add_one_call(sum_calls *s1, one_call *s2, double units)
{
  if (s2->dir == DIALIN)
  {
    s1->in++;
    s1->din += s2->duration > 0?s2->duration:0;
  }
  else
  {
    s2->dm = s2->eh * units;

    s1->out++;
    s1->dout   += s2->duration > 0?s2->duration:0;
    s1->eh     += s2->eh;
    s1->dm     += s2->dm;
  }

  s1->ibytes += s2->ibytes;
  s1->obytes += s2->obytes;
#ifdef ISDN_NL
  s1->dm       += 0.0825; /* add call setup charge */
  s2->dm       += 0.0825;
#endif
  return 0;
}

/*****************************************************************************/

static int clear_sum(sum_calls *s1)
{
  s1->in     = 0;
  s1->out    = 0;
  s1->eh     = 0;
  s1->err    = 0;
  s1->din    = 0;
  s1->dout   = 0;
  s1->dm     = 0;
  s1->ibytes = 0L;
  s1->obytes = 0L;

  return 0;
}

/*****************************************************************************/

static int add_sum_calls(sum_calls *s1, sum_calls *s2)
{
  s1->in     += s2->in;
  s1->out    += s2->out;
  s1->eh     += s2->eh;
  s1->err    += s2->err;
  s1->din    += s2->din;
  s1->dout   += s2->dout;
  s1->dm     += s2->dm;
  s1->ibytes += s2->ibytes;
  s1->obytes += s2->obytes;
  return 0;
}

/*****************************************************************************/

static void strich(int type)
{
  switch (type) {
    case 1 : print_string("----------------------------------------------------------------------------------------\n", STDOUT);
    	     break;

    case 2 : print_string("========================================================================================\n", STDOUT);
    	     break;

    case 3 : print_string("------------------------------------------------------------\n", STDOUT);
    	     break;
  } /* switch */
} /* strich */

/*****************************************************************************/

static int n_match(char *Pattern, char* Number, int version)
{
	int RetCode = -1;
	char s[SHORT_STRING_SIZE];
	char p[SHORT_STRING_SIZE];

	switch(version)
	{
		case LOG_VERSION_1:	if ((RetCode = match(Pattern, Number,0)) != 0        &&
		                        !strncmp(Pattern,S_AREA_PREFIX,strlen(S_AREA_PREFIX)))
		                    {
		                    	sprintf(s,"*%s%s",myarea/*+strlen(S_AREA_PREFIX)*/,Pattern);
		                    	RetCode = match(s,Number,0);
		                    }
		                   	break;
		case LOG_VERSION_2:	strcpy(s,expand_number(Number));
		                   	strcpy(p,expand_number(Pattern));
		                   	RetCode = num_match(p,s);
		                   	break;
		case LOG_VERSION_3:	RetCode = num_match(Pattern,Number);
		                   	break;
		default           :	sprintf(s,"Unknown Version of logfile entries!\n");
		                   	print_string(s,ERROUT);
		                   	break;
	}

	return RetCode;
}

/*****************************************************************************/

