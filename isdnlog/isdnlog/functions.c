/* $Id: functions.c,v 1.8 1997/05/29 17:07:19 akool Exp $
 *
 * ISDN accounting for isdn4linux. (log-module)
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
 *
 * $Log: functions.c,v $
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


  strcpy(s, ctime(&call[chan].connect));

  if ((p = strchr(s, '\n')))
    *p = 0;


  /*
     .aoce :: -1 -> keine aOC Meldung von VSt, muss berechnet werden
     	       0 -> free of Charge (0130/...)
              >0 -> #of Units
  */

  if (!call[chan].aoc) {
    if (!call[chan].dialin)
      call[chan].aoce = -1;

    call[chan].pay = 0.0;
  } /* if */

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
			fprintf(flog, "%s|%-16s|%-16s|%5d|%10d|%10d|%5d|%c|%3d|%10ld|%10ld|%s|%d|%d|%g|%s|%8.2f|\n",
			              s + 4, call[chan].num[CALLING], call[chan].num[CALLED],
			              (int)(call[chan].disconnect - call[chan].connect),
			              (int)call[chan].duration, (int)call[chan].connect,
			              call[chan].aoce, call[chan].dialin ? 'I' : 'O',
			              call[chan].cause, call[chan].ibytes, call[chan].obytes,
			              LOG_VERSION, call[chan].si1, call[chan].si11,
			              currency_factor, currency, call[chan].pay);

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
