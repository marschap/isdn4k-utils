/* $Id: function.c,v 1.3 1997/04/03 22:30:00 luethje Exp $
 *
 * ISDN accounting for isdn4linux. (Report-module)
 *
 * Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
 *                     and Stefan Luethje (luethje@sl-gw.lake.de)
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
 * $Log: function.c,v $
 * Revision 1.3  1997/04/03 22:30:00  luethje
 * improved performance
 *
 * Revision 1.2  1997/03/31 22:43:11  luethje
 * Improved performance of the isdnrep, made some changes of README
 *
 * Revision 1.1  1997/03/24 22:52:09  luethje
 * isdnrep completed.
 *
 * Revision 1.4  1997/03/23 23:11:59  luethje
 * improved performance
 *
 * Revision 1.3  1997/03/23 20:25:23  luethje
 * tmporary (running) result of the new structure of isdnrep
 *
 * Revision 1.2  1997/03/20 00:19:13  luethje
 * inserted the line #include <errno.h> in avmb1/avmcapictrl.c and imon/imon.c,
 * some bugfixes, new structure in isdnlog/isdnrep/isdnrep.c.
 *
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


#define _REP_FUNC_C_

#include "isdnrep.h"

/*****************************************************************************/

#define END_TIME		1

#define SET_TIME		1
#define GET_TIME		2
#define GET_DATE		4
#define GET_YEAR		8

/*****************************************************************************/

static time_t get_month(char *String, int TimeStatus);
static time_t get_time(char *String, int TimeStatus);
static int show_msn(one_call *cur_call);
static int add_sum_calls(sum_calls *s1, sum_calls *s2);
static int print_sum_calls(sum_calls *s, int computed);
static int add_one_call(sum_calls *s1, one_call *s2, double units);
static int clear_sum(sum_calls *s1);
static char *print_currency(int units, double money, int Format);
static void strich(int type);
static int n_match(char *Pattern, char* Number, char* version);
static int set_caller_infos(one_call *cur_call, char *string, time_t from);
static int set_alias(one_call *cur_call, int *nx, char *myname);
static int print_header(int lday);
static int print_entries(one_call *cur_call, double unit, int *nx, char *myname);
static int print_bottom(double unit, char *start, char *stop);
static char *get_time_value(time_t t, int *day, int flag);
static char **string_to_array(char *string);

/*****************************************************************************/

static int      invertnumbers = 0;
static int      unknowns = 0;
static UNKNOWN  unknown[MAXUNKNOWN];
static int      zones[MAXZONES];
static int      zones_usage[MAXZONES];
static double  	zones_dm[MAXZONES];
static double  	zones_dur[MAXZONES];
static char**   ShowMSN = NULL;

/*****************************************************************************/

static char msg1[]    = "%s: Can't open %s (%s)\n";
static char wrongdate2[]   = "wrong date for delete: %s\n";
static char nomemory[]   = "Out of memory!\n";

/*****************************************************************************/

static double restdur    = 0.0;
static double restidur   = 0.0;
static double restdm     = 0.0;
static int	  restiusage = 0;
static int	  restusage  = 0;
static int 	  resteh     = 0;

static int Tarif96 = 0;
static int Tarif962 = 0;

static sum_calls day_sum;
static sum_calls day_com_sum;
static sum_calls all_sum;
static sum_calls all_com_sum;
  
/*****************************************************************************/

void set_print_fct_for_isdnrep(int (*new_print_msg)(int Level, const char *, ...))
{
  print_msg = new_print_msg;
}

/*****************************************************************************/

int read_logfile(char *myname)
{
  auto	   double     einheit = 0.23;
  auto	   int	      nx[2];
  auto	   time_t     now, from = 0;
  auto 	   struct tm *tm;
  auto	   int	      lday = -1;
  auto	   char	      start[20] = "", stop[20];
  auto	   char*      tmpfile = NULL;
  auto     FILE      *fi, *ftmp = NULL;
  auto     char       string[BUFSIZ], s[BUFSIZ];
  one_call            cur_call;


  clear_sum(&day_sum);
  clear_sum(&day_com_sum);
  clear_sum(&all_sum);
  clear_sum(&all_com_sum);

  if (delentries)
  {
    if(begintime)
    {
      print_msg(PRT_ERR, wrongdate2, timestring);
      return -1;
    }
    else
    {
      if ((tmpfile = tmpnam(NULL)) == NULL)
      	return -1;

  		if ((ftmp = fopen(tmpfile, "w")) == (FILE *)NULL)
      {
        print_msg(PRT_ERR, msg1, tmpfile, strerror(errno));
        return -1;
      }
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

  if ((fi = fopen(logfile, "r")) == (FILE *)NULL){
  	print_msg(PRT_ERR,"Can not open file `%s': %s!\n",logfile,strerror(errno));
  	return -1;
  }

  memset(zones, 0, sizeof(zones));

  while (fgets(s, BUFSIZ, fi)) {
    strcpy(string,s);

		if (*s == '#')
			continue;

		if (set_caller_infos(&cur_call,s,from) != 0)
			continue;

		if (!begintime)
			begintime = cur_call.t;

		if (timearea)
		{
			if (delentries)
				if (cur_call.t > endtime)
					fputs(string,ftmp);
				else
					continue;
		}

		if (!verbose && cur_call.duration == 0)
			continue;

		if (phonenumberonly)
			if (!show_msn(&cur_call))
				continue;

		if (incomingonly && cur_call.dir == DIALOUT)
			continue;

		if (outgoingonly && cur_call.dir == DIALIN)
			continue;

		get_time_value(cur_call.t,&day,SET_TIME);

		/* Andreas, warum ist diese Abfrage hier drin, warum soll das nicht moeglich
		   sein? */
		if (cur_call.duration > 172800.0) /* laenger als 2 Tage? Das KANN nicht sein! */
			cur_call.duration = 0;

		set_alias(&cur_call,nx,myname);

		if (day != lday) {
			if (header)
				print_header(lday);

			lday = day;

			if (!*start)
			{
				sprintf(start, "%s %s", get_time_value(0,NULL,GET_DATE),
				                        get_time_value(0,NULL,GET_YEAR));
				sprintf(stop, "%s %s", get_time_value(0,NULL,GET_DATE),
				                       get_time_value(0,NULL,GET_YEAR));
			}
			else
				sprintf(stop, "%s %s", get_time_value(0,NULL,GET_DATE),
				                       get_time_value(0,NULL,GET_YEAR));
		} /* if */

		if (!currency_factor)
#ifdef ISDN_NL
			einheit = 0.0011; /* cost of one second local tariff during office hours */
#elif defined(ISDN_CH)
			einheit = 0.01;
#else
			einheit = Tarif96 ? 0.12 : 0.23;
#endif
			else
				einheit = currency_factor;

		print_entries(&cur_call,einheit,nx,myname);

	} /* while */

	fclose(fi);

	if (delentries) /* Erzeugt neue verkuerzte Datei */
		lday = -1;

	if (lday != -1 && header)
		print_bottom(einheit, start, stop);

	if (delentries) /* Erzeugt neue verkuerzte Datei */
	{
		fclose(ftmp);

		if ((ftmp = fopen(tmpfile, "r")) == (FILE *)NULL)
		{
			print_msg(PRT_ERR, msg1, tmpfile, strerror(errno));
			return -1;
		}
		if ((fi = fopen(logfile, "w")) == (FILE *)NULL)
		{
			print_msg(PRT_ERR, msg1, logfile, strerror(errno));
			return -1;
		}

		while (fgets(s, BUFSIZ, ftmp))
			fputs(s,fi);

		fclose(fi);
		fclose(ftmp);

		unlink(tmpfile);
	}

	return 0;
} /* read_logfile */

/*****************************************************************************/

static int print_bottom(double unit, char *start, char *stop)
{
  auto     char       string[BUFSIZ];
  register int	      i, k;
  register char      *p = NULL;
  sum_calls           tmp_sum;

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
			print_msg(PRT_NORMAL,"\n");
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
		print_msg(PRT_NORMAL,"\n");

	if (!incomingonly)
	{
		if (timearea)
			print_msg(PRT_NORMAL,"\nDIALOUT Summary for %s .. %s\n", start, stop);
		else
			print_msg(PRT_NORMAL,"\nDIALOUT Summary for %s\n", start);

		strich(3);

		for (i = mymsns; i < knowns; i++) {
			if (known[i]->usage[DIALOUT]) {
				print_msg(PRT_NORMAL,"%-14s %4d call(s) %s  %s\n",
				          !numbers?known[i]->who:known[i]->num,
				          known[i]->usage[DIALOUT],
				          double2clock(known[i]->dur[DIALOUT]),
				          print_currency(known[i]->eh,known[i]->dm,7));
			} /* if */
		} /* for */

		if (restusage) {
			print_msg(PRT_NORMAL,"%-14s %4d call(s) %s  %s\n",
			          S_UNKNOWN, restusage, double2clock(restdur),
			          print_currency(resteh,restdm,7));
		} /* if */

		print_msg(PRT_NORMAL,"\n");
	}

	if (!outgoingonly)
	{
		if (timearea)
			print_msg(PRT_NORMAL,"\nDIALIN Summary for %s .. %s\n", start, stop);
		else
			print_msg(PRT_NORMAL,"\nDIALIN Summary for %s\n", start);

		strich(3);

		for (i = mymsns; i < knowns; i++) {
			if (known[i]->usage[DIALIN]) {
				print_msg(PRT_NORMAL,"%-14s %4d call(s) %s\n",
				          !numbers?known[i]->who:known[i]->num,
				          known[i]->usage[DIALIN],
				          double2clock(known[i]->dur[CALLED]));
			} /* if */
		} /* for */

		if (restiusage) {
			print_msg(PRT_NORMAL,"%-14s %4d call(s) %s\n",
			          S_UNKNOWN, restiusage, double2clock(restidur));
		} /* if */

		print_msg(PRT_NORMAL,"\n\n");
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

			print_msg(PRT_NORMAL,"Zone %d : %-15s %4d call(s) %s  %s\n", i, p,
			          zones_usage[i], double2clock(zones_dur[i]),
			          print_currency(zones[i],zones_dm[i],7));
		} /* if */

		if (resteh)
		{
			print_msg(PRT_NORMAL,"Zone x : %-15s %4d call(s) %s  %s\n", S_UNKNOWN,
			          restusage, double2clock(restdur),
#ifdef ISDN_NL
			          print_currency(resteh, resteh * unit + restusage * 0.0825,7));
#else
			          print_currency(resteh, resteh * unit, 7));
#endif
		}

		if (unknowns) {
			print_msg(PRT_NORMAL,"\n\nUnknown caller(s)\n");
			strich(3);

			for (i = 0; i < unknowns; i++) {

				print_msg(PRT_NORMAL,"%s %-14s ", unknown[i].called ? "called by" : "  calling", unknown[i].num);

				for (k = 0; k < unknown[i].connects; k++) {
					strcpy(string, ctime(&unknown[i].connect[k]));

					if ((p = strchr(string, '\n')))
						*p = 0;

					*(string + 19) = 0;

					if (k && (k + 1) % 2) {
						print_msg(PRT_NORMAL,"\n\t\t\t ");
				} /* if */

				print_msg(PRT_NORMAL,"%s%s", k & 1 ? ", " : "", string + 4);
			} /* for */

			print_msg(PRT_NORMAL,"\n");
		} /* for */
	} /* if */

	return 0;
}

/*****************************************************************************/

static int print_entries(one_call *cur_call, double unit, int *nx, char *myname)
{
	auto time_t  t1, t2;
	auto double  takt;
  auto int     computed = 0, go, zone = 1, zeit = -1;

	print_msg(PRT_NORMAL,"  %s %s", get_time_value(0,NULL,GET_TIME),
	                                double2clock(cur_call->duration));

	if (cur_call->dir)
		print_msg(PRT_NORMAL," %14s <- %-14s", cur_call->num[CALLED], cur_call->num[CALLING]);
	else
		print_msg(PRT_NORMAL," %14s -> %-14s", cur_call->num[CALLING], cur_call->num[CALLED]);

	if (cur_call->pay && !cur_call->eh)
	/* Falls Betrag vorhanden und Einheiten nicht, Einheiten berechnen */
		cur_call->eh = cur_call->pay/unit;
	else if (cur_call->currency_factor                               &&
		         cur_call->currency_factor != unit && cur_call->eh>0  )
		/* Falls Einheiten sich auf anderen Einheiten-Faktor beziehen, Einheiten korrigieren */
		cur_call->eh /= unit / cur_call->currency_factor;


#if 0
	if (compute && !currency_factor &&
	    !cur_call->dir && ((cur_call->eh == -1) ||
	    (!cur_call->eh && cur_call->duration && cur_call->cause == -1))) { /* DIALOUT, keine AOCE Meldung */
#else
	go = 0;

	if ((cur_call->eh == -1) && !cur_call->dir) { /* Rauswahl, Gebuehr unbekannt */
		if (nx[1] == -1) {       		      	/* Gegner unbekannt! */
			if (compute) {
				zone = compute;                       /* in "-c x" Zone vermuten */
				go = 1;
			}
			else {                                  /* mit 0 DM berechnen */
				cur_call->eh = 0;
				go = 0;
			} /* else */
		}
		else {
			go = 1;
			if (!(zone = known[nx[1]]->zone))
				go = cur_call->eh = 0;
		} /* else */
	} /* if */

	if (go) {
#endif
		t1 = cur_call->t;
		t2 = cur_call->t + cur_call->duration;

		cur_call->eh = takt = 0;
		computed = 1;

		while (t1 < t2) {
			if (Tarif96)
				takt = cheap96(t1, zone, &zeit);
			else
				takt = cheap(t1, zone);

			if (!takt) {
				print_msg(PRT_ERR, "%s: OOPS! Abbruch: Zeittakt==0 ???\n", myname);
				break;
			} /* if */

			cur_call->eh++;
			t1 += takt;

		} /* while */
	} /* if */

	if (cur_call->duration || (cur_call->eh > 0))
	{
		add_one_call(computed?&day_com_sum:&day_sum,cur_call,unit);

		if (cur_call->dir) {
			if (nx[CALLING] == -1) {
				restiusage++;

				if (cur_call->duration > 0)
					restidur += cur_call->duration;
			} /* if */
		}
		else {

			if (nx[CALLED] == -1) {
				resteh += cur_call->eh;
				restdm += cur_call->dm;
				restusage++;

				if (cur_call->duration > 0)
					restdur += cur_call->duration;
			}
			else {
				known[nx[CALLED]]->eh += cur_call->eh;
				known[nx[CALLED]]->dm += cur_call->dm;

				zones[known[nx[CALLED]]->zone] += cur_call->eh;
				zones_dm[known[nx[CALLED]]->zone] += cur_call->dm;

				if (cur_call->duration > 0)
					zones_dur[known[nx[CALLED]]->zone] += cur_call->duration;

				zones_usage[known[nx[CALLED]]->zone]++;
			} /* if */

			print_msg(PRT_NORMAL," %s", print_currency(cur_call->eh,cur_call->dm,4));

			if (computed)
				print_msg(PRT_NORMAL," *");
		} /* else */
	}
	else {
		if (cur_call->cause != -1)
		{
			print_msg(PRT_NORMAL," %s", qmsg(TYPE_CAUSE, VERSION_EDSS1, cur_call->cause));
			day_sum.err++;
		}
	} /* else */

	print_msg(PRT_NORMAL, " %*s", cur_call->dir ? 21 : 0, "");

	if (cur_call->ibytes)
		print_msg(PRT_NORMAL, " I=%s kB", double2str((double)cur_call->ibytes / 1024.0, 10, 2, 0));

	if (cur_call->obytes)
		print_msg(PRT_NORMAL, " O=%s kB", double2str((double)cur_call->obytes / 1024.0, 10, 2, 0));

	print_msg(PRT_NORMAL,"\n");
	return 0;
}

/*****************************************************************************/

static int print_header(int lday) 
{
  sum_calls tmp_sum;
	time_t now;


	if (lday == -1) {
		time(&now);
		print_msg(PRT_NORMAL,"I S D N  Connection Report  -  %s", ctime(&now));
	}
	else
	{
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
			print_msg(PRT_NORMAL,"\n");

		add_sum_calls(&all_sum,&day_sum);
		clear_sum(&day_sum);

		add_sum_calls(&all_com_sum,&day_com_sum);
		clear_sum(&day_com_sum);
	} /* if */

	print_msg(PRT_NORMAL,"\n\n%s %s\n", get_time_value(0,NULL,GET_DATE),
	                                    get_time_value(0,NULL,GET_YEAR));

	return 0;
}

/*****************************************************************************/

static char *get_time_value(time_t t, int *day, int flag)
{
	static char time_string[3][18] = {"","",""};
	static int  oldday = -1;
	static int  oldyear = -1;
	struct tm *tm;
	

	if (flag & SET_TIME)
	{
  	tm = localtime(&t);

		/*sprintf(time_string[0],"%d:%d:%d",tm->tm_hour,tm->tm_min,tm->tm_sec);*/
		strftime(time_string[0],17,"%X",tm);

		if (oldday != tm->tm_yday || oldyear != tm->tm_year)
		{
	 		*day = tm->tm_mday;

			Tarif96 = tm->tm_year > 95;

		/* Ab Juli '96 gibt's noch mal eine Senkung in den Zonen 3 und 4
		   genaue Preise sind jedoch noch nicht bekannt. FIX-ME */

			Tarif962 = (Tarif96 && (tm->tm_mon > 5)) || (tm->tm_year > 96);

			strftime(time_string[1],17,"%a %b %d",tm);
			/*strftime(time_string[1],17,"%x",tm);*/
			oldday  = tm->tm_yday;

			if (oldyear != tm->tm_year)
			{
				sprintf(time_string[2],"%d",1900+tm->tm_year);
				oldyear = tm->tm_year;
			}
		}
	}

	if (flag & GET_TIME)
		return time_string[0];
	else
	if (flag & GET_DATE)
		return time_string[1];
	else
	if (flag & GET_YEAR)
		return time_string[2];

	return NULL;

}

/*****************************************************************************/

static int set_alias(one_call *cur_call, int *nx, char *myname)
{
	auto int n, cc, i, j;
	auto int hit;


	for (n = CALLING; n <= CALLED; n++) {
		nx[n] = -1;
		hit = 0;

		if (!*(cur_call->num[n])) {
			if (!numbers)
			{
				cur_call->num[n][0] = C_UNKNOWN;
				cur_call->num[n][1] = '\0';
			}
			/* Wenn keine Nummer, dann Name "UNKNOWN" */

			hit++;
		} else {
			/* In der folg. Schleife werden Nummern durch Namen ersetzt */
			cc = 0;

			for (j = 0; ((j < 2) && !cc); j++) {
				for (i = 0; i < knowns; i++) {

					if (cur_call->version[0] != '\0')
					{
						if (!strcmp(cur_call->version,LOG_VERSION_2) ||
						    !strcmp(cur_call->version,LOG_VERSION_3)   )
							cc = ((known[i]->si == cur_call->si) || j) &&
							     !n_match(known[i]->num, cur_call->num[n], cur_call->version);
					}
					else
					{
						if (*cur_call->num[n] != '0') {

							/* Alte Syntax der "isdn.log" : Ohne vorlaufene "0" */
							cc = ((known[i]->si == cur_call->si) || j) &&
							     !match(known[i]->num+1, cur_call->num[n], 0);

							if (!cc) {
								/* Ganz alte Syntax der "isdn.log" : Ohne Vorwahl */
								cc = ((known[i]->si == cur_call->si) || j) &&
							     !n_match(known[i]->num, cur_call->num[n], LOG_VERSION_1);
							} /* if */
						}
					}

					if (cc) {

						if (!numbers)
							strcpy(cur_call->num[n], known[i]->who);

 						nx[n] = i;

						known[i]->usage[cur_call->dir]++;
						/* Gesamte Anrufe auf gleiche Nummern werden gezeahlt */
						known[i]->dur[cur_call->dir] += cur_call->duration;
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
					if (!strcmp(unknown[i].num, cur_call->num[n])) {
					hit++;
					break;
				} /* if */

				strcpy(unknown[i].num, cur_call->num[n]);
				unknown[i].called = !n;
				unknown[i].connect[unknown[i].connects] = cur_call->t;

				/* ACHTUNG: MAXCONNECTS und MAXUNKNOWN sollten unbedingt groesser sein ! */
				if (unknown[i].connects + 1 < MAXCONNECTS)
					unknown[i].connects++;
				else
					print_msg(PRT_ERR, "%s: WARNING: Too many unknown connection's from %s\n", myname, unknown[i].num);

				if (!hit) {
					if (unknowns < MAXUNKNOWN)
						unknowns++;
					else
						print_msg(PRT_ERR, "%s: WARNING: Too many unknown number's\n", myname);
				} /* if */
			} /* if */
		} /* else */
	} /* for */

	return 0;
}

/*****************************************************************************/

static int set_caller_infos(one_call *cur_call, char *string, time_t from)
{
  register int    i = 0;
  auto     char **array;


	array = string_to_array(string);

	if (array[5] == NULL)
		return -1;

	cur_call->t = atol(array[5]);

	if (timearea && (cur_call->t < begintime || cur_call->t > endtime))
		return -1;
	else
		if (cur_call->t < from)
			return -1;

	cur_call->eh = 0;
	cur_call->dir = -1;
	cur_call->cause = -1;
	cur_call->ibytes = cur_call->obytes = 0L;
	cur_call->dm    = 0.0;
	cur_call->version[0] = '\0';
	cur_call->pay = 0.0;
	cur_call->si = cur_call->si1 = 0;
	cur_call->dir = DIALOUT;

	for (i = 1; array[i] != NULL; i++)
	{
		switch (i)
		{
			case  0 : cur_call->t = atom(array[i]);
			          break;
			case  1 : strcpy(cur_call->num[0], Kill_Blanks(array[i]));
			          break;
			case  2 : strcpy(cur_call->num[1], Kill_Blanks(array[i]));
			          break;
			case  3 : cur_call->duration = (double)atoi(array[i]);
			          break;
			case  4 : cur_call->duration = atoi(array[i]) / 100.0;
			          break;
			case  5 : /*cur_call->t = atol(array[i]);*/
			          break;
			case  6 : cur_call->eh = atoi(array[i]);
			          break;
			case  7 : cur_call->dir = (*array[i] == 'I') ? DIALIN : DIALOUT;
			          break;
			case  8 : cur_call->cause = atoi(array[i]);
			          break;
			case  9 : cur_call->ibytes = atol(array[i]);
			          break;
			case  10: cur_call->obytes = atol(array[i]);
			          break;
			case  11: strcpy(cur_call->version,array[i]);
			          break;
			case  12: cur_call->si = atoi(array[i]);
			          break;
			case  13: cur_call->si1 = atoi(array[i]);
			          break;
			case  14: cur_call->currency_factor = atoi(array[i]);
			          break;
			case  15: strncpy(cur_call->currency, array[i], 3);
			          break;
			case  16: cur_call->pay = atof(array[i]);
			          break;
			default : print_msg(PRT_ERR,"Unknown element found `%s'!\n",array[i]); 
			          break;
		}
	}

	if (i < 3)
		return -1;

	return 0;
}

/*****************************************************************************/

static char **string_to_array(char *string)
{
	static char *array[30];
	auto   char *ptr;
	auto   int   i = 0;

	if ((ptr = strrchr(string,C_DELIM)) != NULL)
		*ptr = '\0';

	array[5] = NULL;
	array[0] = string;

	while((string = strchr(string,C_DELIM)) != NULL)
	{
		*string++ = '\0';
		array[++i] = string;
	}

	array[++i] = NULL;
	return array;
}

/*****************************************************************************/

int get_term (char *String, time_t *Begin, time_t *End,int delentries)
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

int set_msnlist(char *String)
{
  int Cnt;
  int Value = 1;
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
    print_msg(PRT_ERR, nomemory);
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
        print_msg(PRT_ERR,"Invalid MSN %d!\n",Cnt);
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

  return print_msg(PRT_NORMAL,String);
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
    case 1 : print_msg(PRT_NORMAL,"----------------------------------------------------------------------------------------\n");
    	     break;

    case 2 : print_msg(PRT_NORMAL,"========================================================================================\n");
    	     break;

    case 3 : print_msg(PRT_NORMAL,"------------------------------------------------------------\n");
    	     break;
  } /* switch */
} /* strich */

/*****************************************************************************/

static int n_match(char *Pattern, char* Number, char* version)
{
	int RetCode = -1;
	char s[SHORT_STRING_SIZE];

	if (!strcmp(version,LOG_VERSION_3))
	{
		RetCode = num_match(Pattern,Number);
	}
	else
	if (!strcmp(version,LOG_VERSION_2))
	{
		strcpy(s,expand_number(Number));
		RetCode = num_match(Pattern,s);
	}
	else
	if (!strcmp(version,LOG_VERSION_1))
	{
		if ((RetCode = match(Pattern, Number,0)) != 0            &&
		    !strncmp(Pattern,S_AREA_PREFIX,strlen(S_AREA_PREFIX))  )
		{
			sprintf(s,"*%s%s",myarea/*+strlen(S_AREA_PREFIX)*/,Pattern);
			RetCode = match(s,Number,0);
		}
	}
	else
		print_msg(PRT_ERR,"Unknown Version of logfile entries!\n");

	return RetCode;
}

/*****************************************************************************/

