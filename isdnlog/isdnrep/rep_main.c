/* $Id: rep_main.c,v 1.4 1997/05/15 22:21:41 luethje Exp $
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
 * $Log: rep_main.c,v $
 * Revision 1.4  1997/05/15 22:21:41  luethje
 * New feature: isdnrep can transmit via HTTP fax files and vbox files.
 *
 * Revision 1.3  1997/05/04 20:19:58  luethje
 * README completed
 * isdnrep finished
 * interval-bug fixed
 *
 * Revision 1.2  1997/04/20 22:52:29  luethje
 * isdnrep has new features:
 *   -variable format string
 *   -can create html output (option -w1 or ln -s isdnrep isdnrep.cgi)
 *    idea and design from Dirk Staneker (dirk.staneker@student.uni-tuebingen.de)
 * bugfix of processor.c from akool
 *
 * Revision 1.1  1997/04/06 21:03:38  luethje
 * switch -f is working again
 * currency_factor is float again ;-)
 * renamed files isdnrep.c to rep_main.c and function.c isdnrep.c
 *
 * Revision 1.7  1997/04/03 22:30:02  luethje
 * improved performance
 *
 * Revision 1.6  1997/03/31 22:43:15  luethje
 * Improved performance of the isdnrep, made some changes of README
 *
 * Revision 1.5  1997/03/24 22:52:12  luethje
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

#include "isdnrep.h"

/*****************************************************************************/

static int print_in_modules(int Level, const char *fmt, ...);
static int set_linefmt(char *linefmt);

/*****************************************************************************/

int main(int argc, char *argv[], char *envp[])
{
  auto int   c;
	auto char  fnbuff[512] = "";
	auto char  usage[]     = "%s: usage: %s [ -%s ]\n";
	auto char  wrongdate[] = "unknown date: %s\n";
	auto char  options[]   = "ac:d:f:hinop:s:t:uvw:NVF:M:";
	auto char *myname      = basename(argv[0]);
	auto char *ptr         = NULL;
	auto char *linefmt     = "";
	auto char *htmlreq     = NULL;


	set_print_fct_for_tools(printf);
	set_print_fct_for_isdnrep(print_in_modules);

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

      case 'h' : header = 0;
      	       	 break;

      case 'n' : numbers++;
      	       	 break;

      case 'd' : delentries++;

      case 't' : strcpy(timestring, optarg);
                 if (!get_term(timestring,&begintime,&endtime,delentries))
                 {
                   printf(wrongdate, timestring);
                   return 1;
                 }
                 timearea++;
                 break;

      case 'u' : seeunknowns++;
      	       	 break;

      case 'v' : verbose++;
      	       	 break;

      case 'f' : strcpy(fnbuff, optarg);
                 break;

      case 'p' : if (!phonenumberonly) set_msnlist(optarg);
      	       	 phonenumberonly++;
      	       	 break;

      case 'w' : html = strtol(optarg, NIL, 0)?H_PRINT_HEADER:H_PRINT_HTML;
                 break;

      case 's' : lineformat = strdup(optarg);
                 linefmt = NULL;
                 break;

      case 'F' : linefmt = strdup(optarg);
                 break;

      case 'N' : use_new_config = 0;
                 break;

      case 'M' : htmlreq = strdup(optarg);
                 break;

      case 'V' : print_version(myname);
                 exit(0);

      case '?' : printf(usage, argv[0], argv[0], options);
                 return(1);
    } /* switch */

  if (readconfig(myname) != 0)
  	return 1;

  if (htmlreq)
  {
		send_html_request(myname,htmlreq);
		exit(0);
  }

	if (!html && (ptr = strrchr(myname,'.')) != NULL && !strcasecmp(ptr+1,"cgi"))
		html = H_PRINT_HEADER;

	if (html)
	{
		seeunknowns = 0;
		header++;
	}

	if (linefmt != NULL)
	{
		if (*linefmt == '\0')
		{
			if (html)
				set_linefmt("WWW");
			else
				set_linefmt(linefmt);
		}
		else
		{
			if (set_linefmt(linefmt))
			{
				printf("Error: %s can not find format `%s%s'!\n",myname,CONF_ENT_REPFMT,To_Upper(linefmt));
				exit(0);
			}
		}
	}

  if (!currency_factor)
    currency = "DM";

  if (fnbuff[0])
	  logfile = fnbuff;

	return (read_logfile(myname));
}

/*****************************************************************************/

static int print_in_modules(int Level, const char *fmt, ...)
{
	auto va_list ap;
	auto char    String[BUFSIZ*3];


	va_start(ap, fmt);
		(void)vsnprintf(String, BUFSIZ*3, fmt, ap);
	va_end(ap);

	return fprintf(Level == PRT_ERR?stderr:stdout, "%s", String);
} /* print_in_modules */

/*****************************************************************************/

int set_linefmt(char *linefmt)
{
	int i = 0;
	int RetCode = -1;

 	if (lineformats != NULL)
 	{
	 	while(lineformats[i] != NULL)
 		{
 			if (!strcasecmp(linefmt,lineformats[i][0]))
 			{
 				lineformat = lineformats[i][1];
 				RetCode = 0;
 				break;
 			}

 			i++;
 		}
 	}

 	return RetCode;
}

/*****************************************************************************/

