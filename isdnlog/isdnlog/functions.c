/* $Id: functions.c,v 1.16 1998/12/09 20:39:24 akool Exp $
 *
 * ISDN accounting for isdn4linux. (log-module)
 *
 * Copyright 1995, 1998 by Andreas Kool (akool@isdn4linux.de)
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
 *
 * $Log: functions.c,v $
 * Revision 1.16  1998/12/09 20:39:24  akool
 *  - new option "-0x:y" for leading zero stripping on internal S0-Bus
 *  - new option "-o" to suppress causes of other ISDN-Equipment
 *  - more support for the internal S0-bus
 *  - Patches from Jochen Erwied <mack@Joker.E.Ruhr.DE>, fixes TelDaFax Tarif
 *  - workaround from Sebastian Kanthak <sebastian.kanthak@muehlheim.de>
 *  - new CHARGEINT chapter in the README from
 *    "Georg v.Zezschwitz" <gvz@popocate.hamburg.pop.de>
 *
 * Revision 1.15  1998/11/24 20:51:26  akool
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
 * Revision 1.14  1998/09/09 12:49:31  paul
 * fixed crash when using mysql (call to Providername() was omitted)
 *
 * Revision 1.13  1998/06/21 11:52:43  akool
 * First step to let isdnlog generate his own AOCD messages
 *
 * Revision 1.12  1998/06/14 15:33:48  akool
 * AVM B1 support (Layer 3)
 * Telekom's new currency DEM 0,121 supported
 * Disable holiday rates #ifdef ISDN_NL
 * memory leak in "isdnrep" repaired
 *
 * Revision 1.11  1998/06/07 21:08:26  akool
 * - Accounting for the following new providers implemented:
 *     o.tel.o, Tele2, EWE TEL, Debitel, Mobilcom, Isis, NetCologne,
 *     TelePassport, Citykom Muenster, TelDaFax, Telekom, Hutchison Telekom,
 *     tesion)), HanseNet, KomTel, ACC, Talkline, Esprit, Interoute, Arcor,
 *     WESTCom, WorldCom, Viag Interkom
 *
 *     Code shamelessly stolen from G.Glendown's (garry@insider.regio.net)
 *     program http://www.insider.org/tarif/gebuehr.c
 *
 * - Telekom's 10plus implemented
 *
 * - Berechnung der Gebuehrenzone implementiert
 *   (CityCall, RegioCall, GermanCall, GlobalCall)
 *   The entry "ZONE" is not needed anymore in the config-files
 *
 *   you need the file
 *     http://swt.wi-inf.uni-essen.de/~omatthes/tgeb/vorwahl2.exe
 *   and the new entry
 *     [GLOBAL]
 *       AREADIFF = /usr/lib/isdn/vorwahl.dat
 *   for that feature.
 *
 *   Many thanks to Olaf Matthes (olaf.matthes@uni-essen.de) for the
 *   Data-File and Harald Milz for his first Perl-Implementation!
 *
 * - Accounting for all "Sonderrufnummern" (0010 .. 11834) implemented
 *
 *   You must install the file
 *     "isdn4k-utils/isdnlog/sonderrufnummern.dat.bz2"
 *   as "/usr/lib/isdn/sonderrufnummern.dat"
 *   for that feature.
 *
 * ATTENTION: This is *NO* production-code! Please test it carefully!
 *
 * Revision 1.10  1998/03/29 23:18:03  luethje
 * mySQL-Patch of Sascha Matzke
 *
 * Revision 1.9  1998/03/08 11:42:48  luethje
 * I4L-Meeting Wuerzburg final Edition, golden code - Service Pack number One
 *
 * Revision 1.8  1997/05/29 17:07:19  akool
 * 1TR6 fix
 * suppress some noisy messages (Bearer, Channel, Progress) - can be reenabled with log-level 0x1000
 * fix from Bodo Bellut (bodo@garfield.ping.de)
 * fix from Ingo Schneider (schneidi@informatik.tu-muenchen.de)
 * limited support for Info-Element 0x76 (Redirection number)
 *
 * Revision 1.7  1997/05/09 23:30:45  luethje
 * isdnlog: new switch -O
 * isdnrep: new format %S
 * bugfix in handle_runfiles()
 *
 * Revision 1.6  1997/04/08 00:02:12  luethje
 * Bugfix: isdnlog is running again ;-)
 * isdnlog creates now a file like /var/lock/LCK..isdnctrl0
 * README completed
 * Added some values (countrycode, areacode, lock dir and lock file) to
 * the global menu
 *
 */

#define _FUNCTIONS_C_

#include "isdnlog.h"
#ifdef POSTGRES
#include "postgres.h"
#endif
#ifdef MYSQLDB
#include "mysqldb.h"
#endif

/*****************************************************************************/

static void saveCharge()
{
  register int   i;
  auto 	   FILE *f;
  auto 	   char  fn[BUFSIZ], fno[BUFSIZ];


  sprintf(fn, "%s/%s", confdir(), CHARGEFILE);
  sprintf(fno, "%s.old", fn);
  (void)rename(fn, fno);

  if ((f = fopen(fn, "w")) != (FILE *)NULL) {
    for (i = 0; i < knowns; i++)
      if (known[i]->day > -1)
        fprintf(f, "%s %d %g %g %d %g %g %g %g\n", known[i]->who,
          known[i]->day, known[i]->charge, known[i]->scharge,
          known[i]->month, known[i]->online, known[i]->sonline,
          known[i]->bytes, known[i]->sbytes);

    fclose(f);
  } /* if */
} /* saveCharge */

/*****************************************************************************/

void _Exit(char *File, int Line, int RetCode) /* WARNING: RetCode==-9 does _not_ call exit()! */
{
#ifdef Q931
  if (!q931dmp)
#endif

#ifdef DEBUG
    print_msg(PRT_NORMAL, "exit now %d in module `%s' at line %d!\n", RetCode, File, Line);
#else
    print_msg(PRT_NORMAL, "exit now %d\n", RetCode);
#endif

  if (socket_size(sockets) >= 2) {
    close(sockets[ISDNCTRL].descriptor);
    close(sockets[ISDNINFO].descriptor);

    if (xinfo && sockets[IN_PORT].descriptor != -2)
      close(sockets[IN_PORT].descriptor);
  } /* if */

  closelog();

	if (fout)
		fclose(fout);

#ifdef POSTGRES
  dbClose();
#endif
#ifdef MYSQLDB
  mysql_dbClose();
#endif

  if (!replay) {
    saveCharge();
    handle_runfiles(NULL,NULL,STOP_PROG);
  } /* if */

  if (RetCode != -9)
    exit(RetCode);
} /* Exit */

/*****************************************************************************/

int Change_Channel(int old_channel, int new_channel)
{
  change_channel(old_channel,new_channel);
  Change_Channel_Ring(old_channel,new_channel);

  return 0;
}

/*****************************************************************************/

void now(void)
{
  auto struct tms tms;


  time(&cur_time);
  tto = tt;
  tt = times(&tms);
  set_time_str();
} /* now */

/*****************************************************************************/

void set_time_str(void)
{
  auto struct tm *tm_time = localtime(&cur_time);


  tm_time->tm_isdst = 0;

  strftime(stl, 64, "%a %b %d %X %Y", tm_time);
  strftime(st, 64, "%a %b %d %X", tm_time);
  strftime(idate, 256, "%a%b%d%T", tm_time);

  fullhour = (!tm_time->tm_min && (tm_time->tm_sec <= (wakeup + 2)));

  day = tm_time->tm_mday;
  month = tm_time->tm_mon + 1;
  hour = tm_time->tm_hour;
} /* set_time_str */

/*****************************************************************************/

void logger(int chan)
{
  auto     int    tries, fd;
  register char  *p;
  auto 	   char   s[BUFSIZ];
#ifdef POSTGRES
  auto     DbStrIn db_set;
#endif
#ifdef MYSQLDB
  auto     mysql_DbStrIn mysql_db_set;
#endif


  strcpy(s, ctime(&call[chan].connect));

  if ((p = strchr(s, '\n')))
    *p = 0;


  /*
     .aoce :: -1 -> keine aOC Meldung von VSt, muss berechnet werden
     	       0 -> free of Charge (0130/...)
              >0 -> #of Units
  */

	tries = 0;

	if (access(logfile,W_OK) && errno == ENOENT)
	{
		if ((flog = fopen(logfile, "w")) == NULL)
		{
			tries = -1;
			print_msg(PRT_ERR,"Can not write file `%s' (%s)!\n", logfile, strerror(errno));
		}
		else
			fclose(flog);
	}

	if (tries != -1)
	{
		while (((fd = open(logfile, O_WRONLY | O_APPEND | O_EXCL)) == -1) && (tries < 1000))
			tries++;

		if ((tries >= 1000) || ((flog = fdopen(fd, "a")) == (FILE *)NULL))
			print_msg(PRT_ERR, "Can not open file `%s': %s!\n", logfile, strerror(errno));
		else
		{
			fprintf(flog, "%s|%-16s|%-16s|%5d|%10d|%10d|%5d|%c|%3d|%10ld|%10ld|%s|%d|%d|%g|%s|%g|%03d|\n",
			              s + 4, call[chan].num[CALLING], call[chan].num[CALLED],
			              (int)(call[chan].disconnect - call[chan].connect),
			              (int)call[chan].duration, (int)call[chan].connect,
			              call[chan].aoce, call[chan].dialin ? 'I' : 'O',
			              call[chan].cause, call[chan].ibytes, call[chan].obytes,
			              LOG_VERSION, call[chan].si1, call[chan].si11,
			              currency_factor, currency, call[chan].pay, call[chan].provider);

			fclose(flog);
		}
	}


#ifdef POSTGRES
  db_set.connect = call[chan].connect;
  strcpy(db_set.calling, call[chan].num[CALLING]);
  strcpy(db_set.called, call[chan].num[CALLED]);
  db_set.duration = (int)(call[chan].disconnect - call[chan].connect);
  db_set.hduration = (int)call[chan].duration;
  db_set.aoce = call[chan].aoce;
  db_set.dialin = call[chan].dialin ? 'I' : 'O';
  db_set.cause = call[chan].cause;
  db_set.ibytes = call[chan].ibytes;
  db_set.obytes = call[chan].obytes;
  db_set.version = atoi(LOG_VERSION);
  db_set.si1 = call[chan].si1;
  db_set.si11 = call[chan].si11;
  db_set.currency_factor = currency_factor;
  strcpy(db_set.currency, currency);
  db_set.pay = call[chan].pay;
  dbAdd(&db_set);
#endif
#ifdef MYSQLDB
  mysql_db_set.connect = call[chan].connect;
  strcpy(mysql_db_set.calling, call[chan].num[CALLING]);
  strcpy(mysql_db_set.called, call[chan].num[CALLED]);
  mysql_db_set.duration = (int)(call[chan].disconnect - call[chan].connect);
  mysql_db_set.hduration = (int)call[chan].duration;
  mysql_db_set.aoce = call[chan].aoce;
  mysql_db_set.dialin = call[chan].dialin ? 'I' : 'O';
  mysql_db_set.cause = call[chan].cause;
  mysql_db_set.ibytes = call[chan].ibytes;
  mysql_db_set.obytes = call[chan].obytes;
  mysql_db_set.version = atoi(LOG_VERSION);
  mysql_db_set.si1 = call[chan].si1;
  mysql_db_set.si11 = call[chan].si11;
  mysql_db_set.currency_factor = currency_factor;
  strcpy(mysql_db_set.currency, currency);
  mysql_db_set.pay = call[chan].pay;
  strcpy(mysql_db_set.provider, Providername(call[chan].provider));
  mysql_dbAdd(&mysql_db_set);
#endif
} /* logger */



/*****************************************************************************/

int print_msg(int Level, const char *fmt, ...)
{
  /* ACHTUNG IN DIESER FKT DARF KEIN print_msg() AUFGERUFEN WERDEN !!!!! */
  auto int     SLevel = PRT_NOTHING, l;
  auto char    String[LONG_STRING_SIZE], s[LONG_STRING_SIZE];
  auto va_list ap;


  va_start(ap, fmt);
  l = vsnprintf(String, LONG_STRING_SIZE, fmt, ap);
  va_end(ap);

  if (width) {
    memcpy(s, String, l + 1);

    if (l > width) {
      if (s[l - 1] < ' ') {
        s[width] = s[l - 1];
        s[width + 1] = 0;
      }
      else
        s[width] = 0;
    } /* if */
  } /* if */

  SLevel = IS_DEBUG(Level) ? LOG_DEBUG : LOG_INFO;

  if (Level & syslogmessage)
    syslog(SLevel, "%s", String);

  if (Level & stdoutput) {
    (void)fputs(width ? s : String, stdout);
    fflush(stdout);
  } /* if */

  if (Level & message)
  {
    if (fcons == NULL) {
      fputs(width ? s : String, stderr);
      fflush(stderr);
    }
    else 
    if (!fout){
      fputs(width ? s : String, fcons);
      fflush(fcons);
    } /* else */

    if (fout)
    {
      fputs(width ? s : String, fout);
      fflush(fout);
    }
  }

  if (Level & xinfo)
    print_from_server(String);

  if (verbose && (Level & PRT_LOG)) {
    fprintf(fprot, "%s  %s", stl, String);

    if (synclog)
      fflush(fprot);
  } /* if */

  return(0);
} /* print_msg */

/*****************************************************************************/

int ringer(int chan, int event)
{
  register int i, j, c;
  int   ProcessStarted = 0;
  int   old_c = -1;
  info_args *infoarg = NULL;

  print_msg(PRT_DEBUG_EXEC, "Got ring event %d on channel %d, number of sockets: %d\n", event, chan, socket_size(sockets));

  /* chan; Der jeweilige B-Kanal (0 oder 1)

     event:
       RING_RING    = Nummern wurden uebertragen
       RING_CONNECT = Hoerer abgenommen
       RING_HANGUP  = Hoerer aufgelegt
       RING_AOCD    = Gebuehrenimpuls (wird noch nicht verwendet)
  */

  if (event != RING_HANGUP)
  {
    if (event == RING_RING || event == RING_CONNECT)
      call[chan].cur_event = event;
  }
  else
    call[chan].cur_event = 0;

  for (i = CALLING; i <= CALLED; i++) {   /* fuer beide beteilige Telefonnummern */
    c = call[chan].confentry[i];

    if (c != -1 && c != old_c) { /* wenn Eintrag in isdnlog.conf gefunden */
    	old_c = c;

      for (j = 0;known[c]->infoargs != NULL && (infoarg = known[c]->infoargs[j]) != NULL;j++)
        ProcessStarted += Start_Ring(chan,infoarg,event,0);
    }
  }

  if (ProcessStarted == 0)
  {
    for (j = 0;start_procs.infoargs != NULL && (infoarg = start_procs.infoargs[j]) != NULL;j++)
      ProcessStarted += Start_Ring(chan,infoarg,event,0);
  }

  return ProcessStarted;
} /* ringer */

/*****************************************************************************/

void initSondernummern()
{
  register char  *p1, *p2, *p3;
  register int    tarif;
  auto 	   FILE  *f = fopen("/usr/lib/isdn/sonderrufnummern.dat", "r");
  auto	   char   s[BUFSIZ], msn[128], sinfo[256], linfo[256];
  auto	   double grund1, grund2, takt1, takt2;


  if (f != (FILE *)NULL) {
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
              tarif = -1;

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
