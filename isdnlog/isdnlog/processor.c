/* $Id: processor.c,v 1.35 1998/12/09 20:39:36 akool Exp $
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
 * $Log: processor.c,v $
 * Revision 1.35  1998/12/09 20:39:36  akool
 *  - new option "-0x:y" for leading zero stripping on internal S0-Bus
 *  - new option "-o" to suppress causes of other ISDN-Equipment
 *  - more support for the internal S0-bus
 *  - Patches from Jochen Erwied <mack@Joker.E.Ruhr.DE>, fixes TelDaFax Tarif
 *  - workaround from Sebastian Kanthak <sebastian.kanthak@muehlheim.de>
 *  - new CHARGEINT chapter in the README from
 *    "Georg v.Zezschwitz" <gvz@popocate.hamburg.pop.de>
 *
 * Revision 1.34  1998/11/24 20:51:45  akool
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
 * Revision 1.33  1998/11/07 17:13:01  akool
 * Final cleanup. This _is_ isdnlog-3.00
 *
 * Revision 1.32  1998/11/06 23:43:52  akool
 * for Paul
 *
 * Revision 1.31  1998/11/06 14:28:31  calle
 * AVM-B1 d-channel trace level 2 (newer firmware) now running with isdnlog.
 *
 * Revision 1.30  1998/11/05 19:09:49  akool
 *  - Support for all the new L2 frames from HiSax 3.0d (RR, UA, SABME and
 *    tei management)
 *  - CityWeekend reimplemented
 *    Many thanks to Rainer Gallersdoerfer <gallersd@informatik.rwth-aachen.de>
 *    for the tip
 *  - more providers
 *  - general clean-up
 *
 * Revision 1.29  1998/11/01 08:49:52  akool
 *  - fixed "configure.in" problem with NATION_*
 *  - DESTDIR fixes (many thanks to Michael Reinelt <reinelt@eunet.at>)
 *  - isdnrep: Outgoing calls ordered by Zone/Provider/MSN corrected
 *  - new Switch "-i" -> running on internal S0-Bus
 *  - more providers
 *  - "sonderrufnummern.dat" extended (Frag Fred, Telegate ...)
 *  - added AVM-B1 to the documentation
 *  - removed the word "Teles" from the whole documentation ;-)
 *
 * Revision 1.28  1998/10/04 12:04:05  akool
 *  - README
 *      New entries "CALLFILE" and "CALLFMT" documented
 *      Small Correction from Markus Werner <mw@empire.wolfsburg.de>
 *      cosmetics
 *
 *  - isdnrep.c
 *      Bugfix (Thanks to Arnd Bergmann <arnd@uni.de>)
 *
 *  - processor.c
 *      Patch from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 *        Makes CHARGEMAX work without AOC-D
 *
 *      Patch from Stefan Gruendel <sgruendel@adulo.de>
 *        gcc 2.7.2.1 Optimizer-Bug workaround
 *
 * Revision 1.27  1998/10/03 18:05:55  akool
 *  - processor.c, takt_at.c : Patch from Michael Reinelt <reinelt@eunet.at>
 *    try to guess the zone of the calling/called party
 *
 *  - isdnrep.c : cosmetics (i hope, you like it, Stefan!)
 *
 * Revision 1.26  1998/09/27 11:47:28  akool
 * fix segfault of isdnlog after each RELASE
 *
 * Revision 1.25  1998/09/26 18:29:15  akool
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
 * Revision 1.24  1998/09/22 20:59:15  luethje
 * isdnrep:  -fixed wrong provider report
 *           -fixed wrong html output for provider report
 *           -fixed strange html output
 * kisdnlog: -fixed "1001 message window" bug ;-)
 *
 * Revision 1.23  1998/08/04 08:17:41  paul
 * Translated "CHANNEL: B1 gefordet" messages into English
 *
 * Revision 1.22  1998/06/21 11:52:52  akool
 * First step to let isdnlog generate his own AOCD messages
 *
 * Revision 1.21  1998/06/16 15:05:31  paul
 * isdnlog crashed with 1TR6 and "Unknown Codeset 7 attribute 3 size 5",
 * i.e. IE 03 which is not Date/Time
 *
 * Revision 1.20  1998/06/14 15:33:51  akool
 * AVM B1 support (Layer 3)
 * Telekom's new currency DEM 0,121 supported
 * Disable holiday rates #ifdef ISDN_NL
 * memory leak in "isdnrep" repaired
 *
 * Revision 1.19  1998/06/07 21:08:43  akool
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
 * Revision 1.18  1998/04/09 19:15:07  akool
 *  - CityPlus Implementation from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 *  - dont change huptimeout, if disabled (via isdnctrl huptimeout isdnX 0)
 *  - Support for more Providers (TelePassport, Tele 2, TelDaFax)
 *
 * Revision 1.17  1998/03/25 20:58:34  luethje
 * isdnrep: added html feature (verbose on/off)
 * processor.c: Patch of Oliver Lauer
 *
 * Revision 1.16  1998/03/08 12:37:58  luethje
 * last changes in Wuerzburg
 *
 * Revision 1.15  1998/03/08 12:13:40  luethje
 * Patches by Paul Slootman
 *
 * Revision 1.14  1998/03/08 11:42:55  luethje
 * I4L-Meeting Wuerzburg final Edition, golden code - Service Pack number One
 *
 * Revision 1.13  1998/02/05 08:23:24  calle
 * decode also seconds in date_time if available, for the dutch.
 *
 * Revision 1.12  1997/10/08 05:37:10  calle
 * Added AVM B1 support to isdnlog, patch is from i4l@tenere.saar.de.
 *
 * Revision 1.11  1997/09/07 00:43:12  luethje
 * create new error messages for isdnrep
 *
 * Revision 1.10  1997/08/22 12:31:21  fritz
 * isdnlog now handles chargeint/non-chargeint Kernels automatically.
 * Manually setting of CONFIG_ISDNLOG_OLD_I4L no more needed.
 *
 * Revision 1.9  1997/06/22 23:03:25  luethje
 * In subsection FLAGS it will be checked if the section name FLAG is korrect
 * isdnlog recognize calls abroad
 * bugfix for program starts
 *
 * Revision 1.8  1997/05/29 17:07:22  akool
 * 1TR6 fix
 * suppress some noisy messages (Bearer, Channel, Progress) - can be reenabled with log-level 0x1000
 * fix from Bodo Bellut (bodo@garfield.ping.de)
 * fix from Ingo Schneider (schneidi@informatik.tu-muenchen.de)
 * limited support for Info-Element 0x76 (Redirection number)
 *
 * Revision 1.7  1997/05/28 21:22:53  luethje
 * isdnlog option -b is working again ;-)
 * isdnlog has new \$x variables
 * README completed
 *
 * Revision 1.6  1997/04/20 22:52:14  luethje
 * isdnrep has new features:
 *   -variable format string
 *   -can create html output (option -w1 or ln -s isdnrep isdnrep.cgi)
 *    idea and design from Dirk Staneker (dirk.staneker@student.uni-tuebingen.de)
 * bugfix of processor.c from akool
 *
 * Revision 1.5  1997/03/31 20:50:59  akool
 * fixed the postgres95 part of isdnlog
 *
 * Revision 1.4  1997/03/30 15:42:10  akool
 * Ignore invalid time from VSt
 *
 * Revision 1.3  1997/03/29 09:24:25  akool
 * CLIP presentation enhanced, new ILABEL/OLABEL operators
 *
 * Revision 1.2  1997/03/20 22:42:33  akool
 * Some minor enhancements.
 *
 * Revision 1.1  1997/03/16 20:58:47  luethje
 * Added the source code isdnlog. isdnlog is not working yet.
 * A workaround for that problem:
 * copy lib/policy.h into the root directory of isdn4k-utils.
 *
 * Revision 2.6.36  1997/02/10  09:30:43  akool
 * MAXCARDS implemented
 *
 * Revision 2.6.30  1997/02/05  20:14:46  akool
 * Dual-Teles Mode implemented
 *
 * Revision 2.6.24  1997/01/15  19:21:46  akool
 * AreaCode 0.99 added
 *
 * Revision 2.6.20  1997/01/05  20:02:46  akool
 * q931dmp added
 * Automatische Erkennung "-r" -> "-R"
 *
 * Revision 2.6.19  1997/01/04  15:21:46  akool
 * Korrektur bzgl. ISDN_CH
 * Danke an Markus Maeder (mmaeder@cyberlink.ch)
 *
 * Revision 2.6.17  1997/01/03  16:26:46  akool
 * BYTEMAX implemented
 *
 * Revision 2.6.15  1997/01/02  20:02:46  akool
 * Hopefully fixed b2c() to suppress faulty messages in processbytes()
 * CONNECTMAX implemented
 *
 * Revision 2.6.11  1996/12/31  15:11:46  akool
 * general cleanup
 *
 * Revision 2.6.6  1996/11/27  22:12:46  akool
 * CHARGEMAX implemented
 *
 * Revision 2.60  1996/11/03  09:31:46  akool
 * mit -DCHARGEINT wird Ende jedes "echten" AOC-D angezeigt
 *
 * Revision 2.3.28  1996/05/06  22:18:46  akool
 * "huptimeout" handling implemented (-hx)
 *
 * Revision 2.3.24  1996/05/04  23:03:46  akool
 * Kleiner Fix am ASN.1 Parser von Bernhard Kruepl
 * i/o byte Handing redesigned
 *
 * Revision 2.3.23  1996/04/28  12:44:46  akool
 * PRT_SHOWIMON eingefuehrt
 *
 * Revision 2.3.21  1996/04/26  11:43:46  akool
 * Faelschliche DM 0,12 Meldung an xisdn unterdrueckt
 *
 * Revision 2.3.19  1996/04/25  21:44:46  akool
 * -DSELECT_FIX, new Option "-M"
 * Optionen "-i" und "-c" entfernt
 *
 * Revision 2.3.17  1996/04/23  00:25:46  akool
 * isdn4kernel-1.3.93 voll implementiert
 *
 * Revision 2.3.16  1996/04/22  22:58:46  akool
 * Temp. Fix fuer isdn4kernel-1.3.91 implementiert
 *
 * Revision 2.3.15  1996/04/22  21:25:46  akool
 * general cleanup
 *
 * Revision 2.3.13  1996/04/18  20:36:46  akool
 * Fehlerhafte Meldung der Durchsatzrate auf unbenutztem Kanal unterdrueckt
 *
 * Revision 2.3.11  1996/04/14  21:26:46  akool
 *
 * Revision 2.3.4  1996/04/05  13:50:46  akool
 * NEWCPS-Handling implemented
 *
 * Revision 2.2.5  1996/03/25  19:47:46  akool
 * Fix in Exit() (sl)
 * 1TR6-Unterstuetzung fertiggestellt
 * Neuer Switch "-e" zum Unterdruecken der "tei" Angabe
 *
 * Revision 2.2.4  1996/03/24  12:17:46  akool
 * 1TR6 Causes implemented
 * 1TR6 / E-DSS1 Frames werden unterschieden
 * Pipe-Funktionalitaet reaktiviert 19-03-96 Bodo Bellut (lasagne@garfield.ping.de)
 * Alle Console-Ausgaben wieder mit \r
 * Gebuehrenauswertung fuer 1TR6 implementiert (Wim Bonis (bonis@kiss.de))
 *
 * Revision 2.23  1996/03/17  12:26:46  akool
 *
 * Revision 2.20  1996/03/11  21:15:46  akool
 * Calling/Called party decoding
 *
 * Revision 2.19  1996/03/10  19:46:46  akool
 * Alarm-Handling fuer /dev/isdnctrl0 funzt!
 *
 * Revision 2.17  1996/02/25  19:23:46  akool
 * Andy's Geburtstags-Release
 *
 * Revision 2.15  1996/02/21  20:38:46  akool
 * sl's Server-Verschmelzung
 * Gernot's Parken/Makeln
 *
 * Revision 2.15  1996/02/17  21:01:10  root
 * Nun geht auch Parken und Makeln
 *
 * Revision 2.14  1996/02/17  16:00:00  root
 * Zeitfehler weg
 *
 * Revision 2.15  1996/02/21  20:30:42  akool
 * sl's Serververschmelzung
 * Gernot's Makeln
 *
 * Revision 2.13  1996/02/15  21:03:42  akool
 * ein kleiner noch
 * Gernot's Erweiterungen implementiert
 * MSG_CALL_INFO enthaelt nun State
 *
 * Revision 2.12  1996/02/13  20:08:43  root
 * Nu geht's (oder?)
 *
 * Revision 1.4  1996/02/13  20:05:28  root
 * so nun gehts
 *
 * Revision 1.3  1996/02/13  18:08:45  root
 * Noch ein [ und ein ;
 *
 * Revision 1.2  1996/02/13  18:02:40  root
 * Haben wir's drin - erster Versuch!
 *
 * Revision 1.1  1996/02/13  14:28:14  root
 * Initial revision
 *
 * Revision 2.10  1996/02/12  20:38:16  akool
 * TEI-Handling von Gernot Zander
 *
 * Revision 2.06  1996/02/10  20:10:16  akool
 * Handling evtl. vorlaufender "0" bereinigt
 *
 * Revision 2.05  1996/02/05  21:42:16  akool
 * Signal-Handling eingebaut
 * AVON-Handling implementiert
 *
 * Revision 2.04  1996/01/31  18:30:16  akool
 * Bugfix im C/S
 * Neue Option "-R"
 *
 * Revision 2.03  1996/01/29  15:13:16  akool
 * Bugfix im C/S

 * Revision 2.02  1996/01/27  15:13:16  akool
 * Stefan Luethje's Client-/Server Anbindung implementiert
 * Bugfix bzgl. HANGUP ohne AOCE-Meldung
 *
 * Revision 2.01  1996/01/21  15:32:16  akool
 * Erweiterungen fuer Michael 'Ghandi' Herold implementiert
 * Syslog-Meldungen implementiert
 * Reread der isdnlog.conf bei SIGHUP implementiert
 * AOCD/AOCE Auswertungen fuer Oesterreich implementiert
 *
 * Revision 2.00  1996/01/10  20:10:16  akool
 * Vollstaendiges Redesign, basierend auf der "call reference"
 * WARNING: Requires Patch of 'q931.c'
 *
 * Revision 1.25  1995/11/18  14:38:16  akool
 * AOC von Anrufen auf 0130-xxx werden korrekt ausgewertet
 *
 * Revision 1.24  1995/11/12  11:08:16  akool
 * Auch die "call reference" wird (ansatzweise) ausgewertet
 * Neue Option "-x" aktiviert X11-Popup
 * Date/Time wird ausgewertet
 * AOC-D wird korrekt ausgewertet
 * Neue Option "-t" setzt Systemzeit auf die von der VSt gemeldete
 * Die "-m" Option kann nun auch mehrfach (additiv) angegeben werden
 *
 * Revision 1.23  1995/11/06  18:03:16  akool
 * "-m16" zeigt die Cause im Klartext an
 * Auch Gebuehreneinheiten > 255 werden korrekt ausgewertet
 *
 * Revision 1.22  1995/10/22  14:43:16  akool
 * General cleanup
 * "isdn.log" um 'I' == dialin / 'O' == dialout erweitert
 * Auch nicht zustande gekommene Verbindungen werden (mit cause)
 * protokolliert
 *
 * Revision 1.21  1995/10/18  21:25:16  akool
 * Option "-r" implementiert
 * Charging-Infos waehrend der Verbindung (FACILITY) werden ignoriert
 * "/etc/isdnlog.pid" wird erzeugt
 *
 * Revision 1.20  1995/10/15  17:23:16  akool
 * Volles D-Kanal Protokoll implementiert (fuer Teles-0.4d Treiber)
 *
 * Revision 1.13  1995/09/30  09:34:16  akool
 * Option "-m", Console-Meldung implementiert
 * Flush bei SIGTERM implementiert
 *
 * Revision 1.12  1995/09/29  17:21:13  akool
 * "isdn.log" um Zeiteintrag in UTC erweitert
 *
 * Revision 1.11  1995/09/28  18:51:17  akool
 * First public release
 *
 * Revision 1.1  1995/09/16  16:54:12  akool
 * Initial revision
 *
 */

#define _PROCESSOR_C_
#include "isdnlog.h"

#define OUTGOING  !call[chan].dialin


extern double cheap96(time_t when, int zone, int *zeit);
extern double taktlaenge(int chan, char *description);

static int    HiSax = 0, hexSeen = 0, uid = -1;
static char  *asnp, *asnm;
#ifdef Q931
static int    lfd = 0;
#endif


#ifdef Q931
static void Q931dump(int mode, int val, char *msg, int version)
{
  switch (mode) {
    case TYPE_STRING  : if (val == -4)
                          fprintf(stdout, "    ??  %s\n", msg);
                        else if (val == -3)
                          fprintf(stdout, "%s\n", msg);
                        else if (val == -2)
                          fprintf(stdout, "    ..  %s\n", msg);
                        else if (val == -1)
                          fprintf(stdout, "        %s\n", msg);
                        else
                          fprintf(stdout, "    %02x  %s\n", val, msg);
                        break;

    case TYPE_MESSAGE : fprintf(stdout, "\n%02x  %s\n", val, qmsg(mode, version, val));
                        break;

    case TYPE_ELEMENT : fprintf(stdout, "%02x ---> %s\n", val, qmsg(mode, version, val));
                        break;

    case TYPE_CAUSE   : fprintf(stdout, "    %02x  %s\n", val, qmsg(mode, version, val & 0x7f));
                        break;

  } /* switch */
} /* Q931dump */
#endif


static void diag(int cref, int tei, int sapi, int dialin, int net, int type, int version)
{
  char String[LONG_STRING_SIZE];
  char TmpString[LONG_STRING_SIZE];


  if (dialin != -1) {
    sprintf(String,"  DIAG> %s: %3d/%3d %3d %3d %s %s %s-> ",
      st + 4, cref, cref & 0x7f, tei, sapi,
      ((version == VERSION_1TR6) ? "1TR6" : "E-DSS1"),
      dialin ? " IN" : "OUT",
      net ? "NET" : "USR");

    if ((cref > 128) && (type == SETUP_ACKNOWLEDGE)) {
      sprintf(TmpString," *%d* ", cref);
      strcat(String, TmpString);
    } /* if */

  } /* if */

  print_msg(PRT_DEBUG_GENERAL, "%s%s\n", String, qmsg(TYPE_MESSAGE, VERSION_EDSS1, type));
} /* diag */


static char *location(int loc)
{
  switch (loc) {
    case 0x00 : return("User");                                break;
    case 0x01 : return("Private network serving local user");  break;
    case 0x02 : return("Public network serving local user");   break;
    case 0x03 : return("Transit network");                     break;
    case 0x04 : return("Public network serving remote user");  break;
    case 0x05 : return("Private network serving remote user"); break;
    case 0x07 : return("International network");               break;
    case 0x0a : return("Network beyond inter-working point");  break;
      default : return("");             	       	       break;
  } /* switch */
} /* location */


static void info(int chan, int reason, int state, char *msg)
{
  auto   char   s[BUFSIZ], *left = "", *right = "\n";
  static int    lstate = 0, lchan = -1;


  if (!newline) {

    if (state == STATE_BYTE) {
      right = "";

      if (lstate == STATE_BYTE)
        left = "\r";
      else
        left = "";
    }
    else {
      right = "\n";

      if (lstate == STATE_BYTE)
        left = "\n";
      else
        left = "";
    } /* else */

    if ((lchan != chan) && (lstate == STATE_BYTE))
      left = "\r\n";

    lstate = state;
    lchan = chan;
  } /* if */

  if (allflags & PRT_DEBUG_GENERAL)
    if (allflags & PRT_DEBUG_INFO)
      print_msg(PRT_DEBUG_INFO, "%d INFO> ", chan);

  (void)iprintf(s, chan, call[chan].dialin ? ilabel : olabel, left, msg, right);

  print_msg(PRT_DEBUG_INFO, "%s", s);

  print_msg(reason, "%s", s);

  if (xinfo) {
    strcpy(call[chan].msg, msg);
    call[chan].stat = state;

    message_from_server(&(call[chan]), chan);

    print_msg(PRT_DEBUG_CS, "SOCKET> %s: MSG_CALL_INFO chan=%d\n", st + 4, chan);
  } /* if */
} /* info */


static void buildnumber(char *num, int oc3, int oc3a, char *result, int version, int *provider, int *sondernummer, int *intern, int dir, int who)
{
  auto char n[BUFSIZ];
  auto int  partner = ((dir && (who == CALLING)) || (!dir && (who == CALLED)));


  *sondernummer = -1;
  *intern = 0;

#ifdef Q931
  if (q931dmp) {
    register char *ps;
    auto     char  s[BUFSIZ];


    ps = s + sprintf(s, "Type of number: ");

    switch (oc3 & 0x70) {
      case 0x00 : sprintf(ps, "Unknown");                break;
      case 0x10 : sprintf(ps, "International");          break;
      case 0x20 : sprintf(ps, "National");               break;
      case 0x30 : sprintf(ps, "Network specific");       break;
      case 0x40 : sprintf(ps, "Subscriber");             break;
      case 0x60 : sprintf(ps, "Abbreviated");            break;
      case 0x70 : sprintf(ps, "Reserved for extension"); break;
    } /* switch */

    Q931dump(TYPE_STRING, oc3, s, version);

    ps = s + sprintf(s, "Numbering plan: ");

    switch (oc3 & 0x0f) {
      case 0x00 : sprintf(ps, "Unknown");                break;
      case 0x01 : sprintf(ps, "ISDN/telephony");         break;
      case 0x03 : sprintf(ps, "Data");                   break;
      case 0x04 : sprintf(ps, "Telex");                  break;
      case 0x08 : sprintf(ps, "National standard");      break;
      case 0x09 : sprintf(ps, "Private");                break;
      case 0x0f : sprintf(ps, "Reserved for extension"); break;
    } /* switch */

    Q931dump(TYPE_STRING, -1, s, version);

    if (oc3a != -1) {
      ps = s + sprintf(s, "Presentation: ");

      switch (oc3a & 0x60) {
        case 0x00 : sprintf(ps, "allowed");                                     break;
        case 0x20 : sprintf(ps, "restricted");                                  break;
        case 0x40 : sprintf(ps, "Number not available due to internetworking"); break;
        case 0x60 : sprintf(ps, "Reserved for extension");                      break;
      } /* switch */

      Q931dump(TYPE_STRING, oc3a, s, version);

      ps = s + sprintf(s, "Screening indicator: ");

      switch (oc3a & 0x03) {
        case 0x00 : sprintf(ps, "User provided, not screened");        break;
        case 0x01 : sprintf(ps, "User provided, verified and passed"); break;
        case 0x02 : sprintf(ps, "User provided, verified and failed"); break;
        case 0x03 : sprintf(ps, "Network provided");                   break;
      } /* switch */

      Q931dump(TYPE_STRING, -1, s, version);

    } /* if */

    sprintf(s, "\"%s\"", num);
    Q931dump(TYPE_STRING, -2, s, version);
  } /* if */
#endif

  strcpy(n, num);
  strcpy(result, "");

  if (trim) {
    if (dir && (who == CALLING))
      num += min(trimi, strlen(num));
    else if (!dir && (who == CALLED))
      num += min(trimo, strlen(num));

    print_msg(PRT_DEBUG_DECODE, " TRIM> \"%s\" -> \"%s\" (trimi=%d, trimo=%d, %s, %s, %s)\n",
      n, num, trimi, trimo, (dir ? "DIALIN" : "DIALOUT"), (who ? "CALLED" : "CALLING"), (partner ? "PARTNER" : "MYSELF"));
  } /* if */

  if (*num && !dir && (who == CALLED)) {
    char *amt = amtsholung;

    while (amt && *amt) {
      int len = strchr(amt, ':') ? strchr(amt, ':') - amt : strlen(amt);

      if (len && !strncmp(num, amt, len)) {
#ifdef Q931
        if (q931dmp) {
          auto char s[BUFSIZ], c;

          c = num[len];
          num[len] = 0;

          sprintf(s, "Amtsholung: %s", num);
          num[len] = c;

          Q931dump(TYPE_STRING, -2, s, version);
        } /* if */
#endif
        num += len;

        break;
      } /* if */

      amt += len + (strchr(amt, ':') ? 1 : 0);
    } /* while */
  } /* if */

#ifdef ISDN_DE
  if (!dir && (who == CALLED) && !memcmp(num, "010", 3)) { /* Provider */
    register int l, c;

    if (num[3] == '0') /* dreistellige Verbindungsnetzbetreiberkennzahl? */
      l = 6;
    else
      l = 5;

    c = num[l];
    num[l] = 0;
    *provider = atoi(num + 3);

    /* die dreistelligen Verbindungsnetzbetreiberkennzahlen werden
       intern erst mal mit einem Offset von 100 verarbeitet
       "010001 Netnet" -> "001" + 100 -> 101

       Das geht gut, solange nur die ersten 99 der dreistelligen
       vergeben werden ...
    */

    if (l == 6)
      *provider += 100;

    num[l] = c;
    num += l;

#ifdef Q931
    if (q931dmp) {
      auto char s[BUFSIZ];

      if (*provider < 100)
      sprintf(s, "Via provider \"010%02d\", %s", *provider, Providername(*provider));
      else
	sprintf(s, "Via provider \"010%03d\", %s", *provider - 100, Providername(*provider));

      Q931dump(TYPE_STRING, -1, s, version);
    } /* if */
#endif
  } /* if */
#endif

  if (!dir && (who == CALLED))
    *sondernummer = is_sondernummer(num);
  else if ((dir && (who == CALLED)) || (!dir && (who == CALLING)))
    *intern = strlen(num) < interns0;

#ifdef Q931
                      if (q931dmp) {
    if (*sondernummer != -1) {
      auto char s[256];

      sprintf(s, "(Sonderrufnummer %s : %s)", num, SN[*sondernummer].sinfo);
      Q931dump(TYPE_STRING, -1, s, version);
    } /* if */

    if (*intern)
      Q931dump(TYPE_STRING, -1, "(Interne Nummer)", version);
                      } /* if */
#endif

  if ((*sondernummer == -1) && !*intern)

  switch (oc3 & 0x70) { /* Calling party number Information element, Octet 3 - Table 4-11/Q.931 */
    case 0x00 : if (*num) {                  /* 000 Unknown */
                  if (*num != '0')
                    sprintf(result, "%s%s", mycountry, myarea);
                  else {
                  	if (num[1] != '0') /* Falls es doch Ausland ist -> nichts machen!!! */
                    	strcpy(result, mycountry);
                    else
                    	strcpy(result, countryprefix);

                    while (*num == '0')
                   		num++;
                  } /* else */
                } /* if */
                break;

    case 0x10 : if (version != VERSION_1TR6)
                  strcpy(result, countryprefix);  /* 001 International */
                break;

    case 0x20 : if (version != VERSION_1TR6) {
                  strcpy(result, mycountry);    /* 010 National */

                    while (*num == '0')
                      num++;
    		} /* if */
                break;

    case 0x30 : break;                       /* 011 Network specific number */

    case 0x40 : if (*num != '0')             /* 100 Subscriber number */
                  sprintf(result, "%s%s", mycountry, myarea);
                else {
                  strcpy(result, mycountry);

                  while (*num == '0')
                    num++;
                } /* else */
                break;

    case 0x60 : break;                       /* 110 Abbreviated number */

    case 0x70 : break;                       /* 111 Reserved for extension */
  } /* switch */

  if (*num)
  strcat(result, num);
  else
    strcpy(result, "");

  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s: num=\"%s\", oc3=%s(%02x), result=\"%s\", sonder=%d, int=%d, partner=%d\n",
    st + 4, n, i2a(oc3, 8, 2), oc3 & 0x70, result, *sondernummer, *intern, partner);
} /* buildnumber */


static void aoc_debug(int val, char *s)
{
  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s: %s\n", st + 4, s);

#ifdef Q931
  if (q931dmp)
    Q931dump(TYPE_STRING, val, s, VERSION_EDSS1);
#endif
} /* aoc_debug */


/*
    currency_mode   := AOC_UNITS | AOC_AMOUNT
    currency_factor :=
    currency        := " DM" | "SCHILLING" | "NLG" | "FR."
*/

static int facility(int type, int l)
{
  register int   ls, i, ServedUserNr;
  register char *px, *px1, *px2, *px3;
  auto     int   c, result = 0, a1, a2, a3, oc3 = 0;
  static   int   ID = 0, OP = 0, EH = 0, MP = 0;
  static   char  curr[64];
  auto     char  s[BUFSIZ], s1[BUFSIZ], dst[BUFSIZ], src[BUFSIZ];
  auto	   char  vdst[BUFSIZ], vsrc[BUFSIZ];


  switch(type) {
    case AOC_INITIAL          : ID = OP = EH = MP = 0;

                                if (asnp == NULL)
                                  return(AOC_OTHER);

                                c = strtol(asnp += 3, NIL, 16);              /* Ext/Spare/Profile */

                                switch (c) {                                 /* Remote Operation Protocol */
                                  case 0x91 : aoc_debug(c, "Remote Operation Protocol"); break;
                                  case 0x92 : aoc_debug(c, "CMIP Protocol");             break;
                                  case 0x93 : aoc_debug(c, "ACSE Protocol");             break;
                                    default : aoc_debug(c, "UNKNOWN Protocol");
                                              return(AOC_OTHER);
                                } /* switch */

                                c = strtol(asnp += 3, NIL, 16);              /* Invoke Comp type */

                                switch (c) {
                                  case 0xa1 : aoc_debug(c, "InvokeComponent");       break;
                                  case 0xa2 : aoc_debug(c, "ReturnResultComponent"); break;
                                  case 0xa3 : aoc_debug(c, "ReturnErrorComponent");  break;
                                  case 0xa4 : aoc_debug(c, "RejectComponent");       break;
                                    default : aoc_debug(c, "UNKNOWN Component");
                                              return(AOC_OTHER);
                                } /* switch */

                                l = strtol(asnp += 3, NIL, 16);              /* Invoke Comp length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                c = strtol(asnp += 3, NIL, 16);              /* Invoke ID type */

                                sprintf(s, "InvokeIdentifier Type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);              /* Invoke ID length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                while (l--) {
                                  ID = ID << 8;
                                  c = strtol(asnp += 3, NIL, 16);          /* Invoke ID Contents */

                                  sprintf(s, "InvokeIdentifier Contents=%d", c);
                                  aoc_debug(c, s);

                                  ID += c;
                                } /* while */

                                c = strtol(asnp += 3, NIL, 16);              /* OPERATION type */

                                if (!c)
                                  return(AOC_OTHER);

                                sprintf(s, "OperationType=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);              /* OPERATION length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                while (l--) {
                                  OP = OP << 8;
                                  OP += strtol(asnp += 3, NIL, 16);          /* OPERATION contents */
                                } /* while */

                                switch (OP) {
                                  case 0x01 : asnm = "uUsa";                               break;
                                  case 0x02 : asnm = "cUGCall";                            break;
                                  case 0x03 : asnm = "mCIDRequest";                        break;
                                  case 0x04 : asnm = "beginTPY";                           break;
                                  case 0x05 : asnm = "endTPY";                             break;
                                  case 0x07 : asnm = "activationDiversion";                break;
                                  case 0x08 : asnm = "deactivationDiversion";              break;
                                  case 0x09 : asnm = "activationStatusNotificationDiv";    break;
                                  case 0x0a : asnm = "deactivationStatusNotificationDiv";  break;
                                  case 0x0b : asnm = "interrogationDiversion";             break;
                                  case 0x0c : asnm = "diversionInformation";               break;
                                  case 0x0d : asnm = "callDeflection";                     break;
                                  case 0x0e : asnm = "callRerouting";                      break;
                                  case 0x0f : asnm = "divertingLegInformation2";           break;
                                  case 0x10 : asnm = "invokeStatus";                       break;
                                  case 0x11 : asnm = "interrogationDiversion1";            break;
                                  case 0x12 : asnm = "divertingLegInformation1";           break;
                                  case 0x13 : asnm = "divertingLegInformation3";           break;
                                  case 0x14 : asnm = "explicitReservationCreationControl"; break;
                                  case 0x15 : asnm = "explicitReservationManagement";      break;
                                  case 0x16 : asnm = "explicitReservationCancel";          break;
                                  case 0x18 : asnm = "mLPP lfb Query";                     break;
                                  case 0x19 : asnm = "mLPP Call Request";                  break;
                                  case 0x1a : asnm = "mLPP Call preemption";               break;
                                  case 0x1e : asnm = "chargingRequest";                    break;
                                  case 0x1f : asnm = "aOCSCurrency";                       break;
                                  case 0x20 : asnm = "aOCSSpecialArrangement";             break;
                                  case 0x21 : asnm = "aOCDCurrency";                       break;
                                  case 0x22 : asnm = "aOCDChargingUnit";                   break;
                                  case 0x23 : asnm = "aOCECurrency";                       break;
                                  case 0x24 : asnm = "aOCEChargingUnit";                   break;
                                  case 0x25 : asnm = "identificationOfChange";             break;
                                  case 0x28 : asnm = "beginConf";                          break;
                                  case 0x29 : asnm = "addConf";                            break;
                                  case 0x2a : asnm = "splitConf";                          break;
                                  case 0x2b : asnm = "dropConf";                           break;
                                  case 0x2c : asnm = "IsolateConf";                        break;
                                  case 0x2d : asnm = "reattachConf";                       break;
                                  case 0x2e : asnm = "partyDISC";                          break;
                                  case 0x2f : asnm = "floatConf";                          break;
                                  case 0x30 : asnm = "endConf";                            break;
                                  case 0x31 : asnm = "identifyConferee";                   break;
                                  case 0x3c : asnm = "requestREV";                         break;
                                } /* switch */

                                px = s + sprintf(s, "OperationContents : %s", asnm);
                                aoc_debug(OP, s);

                                switch (OP) {
                                  case 0x09 : /* activationStatusNotificationDiv (Rufumleitung an) */

                                              c = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "SEQUENCE=%d", c);
                                	      aoc_debug(c, s);

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              c = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "Enumeration type procedure=%d", c);
                                	      aoc_debug(c, s);

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              switch (c = strtol(asnp += 3, NIL, 16)) {
                                                case 0x00 : px1 = "CFU";  break; /* Rufumleitung unbedingt */
                    				case 0x01 : px1 = "CFB";  break; /* Rufumleitung bei Besetzt */
                    				case 0x02 : px1 = "CFNR"; break; /* Rufumleitung wenn niemand h”rt */
                                                default   : px1 = "CF?";  break; /* UNKNOWN Rufumleitung */
                                              } /* switch */

                                	      aoc_debug(c, px1);

                                              c = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "Enumeration type BasicService=%d", c);
                                	      aoc_debug(c, s);

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              switch (c = strtol(asnp += 3, NIL, 16)) {
                    			        case 0x00 : px2 = "all Services";                     break;
                    				case 0x01 : px2 = "Speech";               	      break;
                    				case 0x02 : px2 = "unrestricted Digital Information"; break;
                    				case 0x03 : px2 = "3.1 kHz audio"; 		      break;
                    				case 0x20 : px2 = "Telephony"; 			      break;
                    				case 0x21 : px2 = "Teletex"; 			      break;
                    				case 0x22 : px2 = "Telefax G4"; 	   	      break;
                    				case 0x23 : px2 = "Videotex Syntax Based"; 	      break;
                    				case 0x24 : px2 = "Videotelephony"; 		      break;
                                                  default : px2 = "UNKNOWN BasicService"; 	      break;
                                              } /* switch */

                                	      aoc_debug(c, px2);

                                              c = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "SEQUENCE, Address (Zieladresse)=%d", c);
                                	      aoc_debug(c, s);

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              switch (c = strtol(asnp += 3, NIL, 16)) {
                                                case 0x80 : l = strtol(asnp += 3, NIL, 16);
                                              	       	    sprintf(s, "length=%d", l);
                                              	       	    aoc_debug(l, s);
                                                            px3 = "UNKNOWN PublicTypeOfNumber";
                                                       	    break;

                                              	case 0xa1 : l = strtol(asnp += 3, NIL, 16);
                                              	       	    sprintf(s, "length=%d", l);
                                              	       	    aoc_debug(l, s);

                                              	       	    c = strtol(asnp += 3, NIL, 16);
                                       	      	       	    sprintf(s, "Aufzaehlungstyp, PublicTypeOfNumber=%d", c);
                                	      		    aoc_debug(c, s);

                                              	       	    l = strtol(asnp += 3, NIL, 16);
                                              	       	    sprintf(s, "length=%d", l);
                                              	       	    aoc_debug(l, s);

                                              	       	    switch (oc3 = strtol(asnp += 3, NIL, 16)) {
						              case 0x00 : px3 = "Unknown Number";             break;
                    					      case 0x01 : px3 = "International Number";       break;
                    					      case 0x02 : px3 = "National Number"; 	      break;
                    					      case 0x03 : px3 = "Network Spezific Number";    break;
                    					      case 0x04 : px3 = "Subscriber Number";          break;
                    					      case 0x05 : px3 = "abbreviated Number";         break;
                                                                default : px3 = "UNKNOWN PublicTypeOfNumber"; break;
                                                            } /* switch */

                                	      		    aoc_debug(oc3, px3);
                                                            break;

                                                  default : px3 = "UNKNOWN PublicTypeOfNumber";
                                                  	    break;
                                              } /* switch */

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              ls = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", ls);
                                              aoc_debug(ls, s);
                                              l--;

                                              px = dst;
                                              while (ls--) {
                                                *px++ = strtol(asnp += 3, NIL, 16);
                                                l--;
                                              } /* while */

                                              *px = 0;

                                              sprintf(s1, "\"%s\"", dst);
                                              aoc_debug(-2, s1);

                                              px = s;
                                              c = strtol(asnp += 3, NIL, 16);
                                              sprintf(px, "%02x ", c);
                                              aoc_debug(-4, s);

                                              for (i = 0; i < 5; i++) {
                                                c = strtol(asnp += 3, NIL, 16);
                                              	sprintf(px, "%02x ", c);
                                              	aoc_debug(-4, s);
                                              } /* for */

                                              ls = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", ls);
                                              aoc_debug(l, s);
                                              l--;

                                              px = src;
                                              while (ls--) {
                                                *px++ = strtol(asnp += 3, NIL, 16);
                                                l--;
                                              } /* while */

                                              *px = 0;

                                              sprintf(s1, "\"%s\"", src);
                                              aoc_debug(-2, s1);

                    			      buildnumber(src, 0, 0, call[6].num[CLIP], VERSION_EDSS1, &a1, &a2, &a3, 0, 999);
                          		      strcpy(vsrc, vnum(6, CLIP));
                    			      buildnumber(dst, oc3 * 16, 0, call[6].num[CLIP], VERSION_EDSS1, &a1, &a2, &a3, 0, 999);
                          		      strcpy(vdst, vnum(6, CLIP));

                                              /* sprintf(s, "%s %s/%s -> %s (%s)", px1, src, px2, dst, px3); */
                                              sprintf(s, "%s %s/%s -> %s", px1, vsrc, px2, vdst);
                                              aoc_debug(-2, s);

                                              (void)iprintf(s1, -1, mlabel, "", s, "\n");
                                              print_msg(PRT_SHOWNUMBERS, "%s", s1);

                                              return(AOC_OTHER);

                                              break;

                                  case 0x08 : /* deactivationDiversion */
                                  case 0x0a : /* deactivationStatusNotificationDiv (Rufumleitung aus) */
                                              c = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "SEQUENCE=%d", c);
                                	      aoc_debug(c, s);

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              c = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "Enumeration type procedure=%d", c);
                                	      aoc_debug(c, s);

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              switch (c = strtol(asnp += 3, NIL, 16)) {
                                                case 0x00 : px1 = "CFU";  break; /* Rufumleitung unbedingt */
                    				case 0x01 : px1 = "CFB";  break; /* Rufumleitung bei Besetzt */
                    				case 0x02 : px1 = "CFNR"; break; /* Rufumleitung wenn niemand h”rt */
                                                default   : px1 = "CF?";  break; /* UNKNOWN Rufumleitung */
                                              } /* switch */

                                	      aoc_debug(c, px1);

                                              c = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "Enumeration type BasicService=%d", c);
                                	      aoc_debug(c, s);

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              switch (c = strtol(asnp += 3, NIL, 16)) {
                    			        case 0x00 : px2 = "all Services";                     break;
                    				case 0x01 : px2 = "Speech";               	      break;
                    				case 0x02 : px2 = "unrestricted Digital Information"; break;
                    				case 0x03 : px2 = "3.1 kHz audio"; 		      break;
                    				case 0x20 : px2 = "Telephony"; 			      break;
                    				case 0x21 : px2 = "Teletex"; 			      break;
                    				case 0x22 : px2 = "Telefax G4"; 	   	      break;
                    				case 0x23 : px2 = "Videotex Syntax Based"; 	      break;
                    				case 0x24 : px2 = "Videotelephony"; 		      break;
                                                  default : px2 = "UNKNOWN BasicService"; 	      break;
                                              } /* switch */

                                	      aoc_debug(c, px2);

                                              ServedUserNr = strtol(asnp += 3, NIL, 16);
                                       	      sprintf(s, "ServedUserNr=%d", ServedUserNr);
                                	      aoc_debug(c, s);

                                              if (ServedUserNr == 161)
                                                for (i = 0; i < 5; i++) {
                                                  l = strtol(asnp += 3, NIL, 16);
                                              	  sprintf(s, "%02x ", l);
                                              	  aoc_debug(-4, s);
                                                } /* for */

                                              l = strtol(asnp += 3, NIL, 16);
                                              sprintf(s, "length=%d", l);
                                              aoc_debug(l, s);

                                              px = dst;
                                              while (l--)
                                                *px++ = strtol(asnp += 3, NIL, 16);

                                              *px = 0;

                                              sprintf(s1, "\"%s\"", dst);
                                              aoc_debug(-2, s1);

                    			      buildnumber(dst, 0, 0, call[6].num[CLIP], VERSION_EDSS1, &a1, &a2, &a3, 0, 999);
                          		      strcpy(vdst, vnum(6, CLIP));

                                              sprintf(s, "deactivate %s %s/%s", px1, vdst, px2);

                                              (void)iprintf(s1, -1, mlabel, "", s, "\n");
                                              print_msg(PRT_SHOWNUMBERS, "%s", s1);

                                              return(AOC_OTHER);
                                              break;

                                  case 0x21 : (void)facility(AOCDCurrency, l);
                                              result = -EH;
                                              break;

                                  case 0x22 : (void)facility(AOCDChargingUnit, l);
                                              result = -EH;
                                              break;

                                  case 0x23 : (void)facility(AOCECurrency, l);
                                              result = EH;
                                              break;

                                  case 0x24 : (void)facility(AOCEChargingUnit, l);
                                              result = EH;
                                              break;

                                    default : sprintf(s, "UNKNOWN OperationContents=%d", OP);
                                              aoc_debug(OP, s);
                                              return(AOC_OTHER);
                                              break;
                                } /* switch OP */
                                break;

    case AOCDCurrency         : (void)facility(AOCDCurrencyInfo, l);
                                break;

    case AOCDChargingUnit     : (void)facility(AOCDChargingUnitInfo, l);
                                break;

    case AOCECurrency         : (void)facility(AOCECurrencyInfo, l);
                                break;

    case AOCEChargingUnit     : (void)facility(AOCEChargingUnitInfo, l);
                                break;

    case AOCDChargingUnitInfo : c = strtol(asnp += 3, NIL, 16);  /* specificChargingUnits SEQUENCE type / NULL */

                                sprintf(s, "Sequence, specificChargingUnits type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);  /* specificChargingUnits SEQUENCE length / NULL */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                if (!l)                                 /* freeOfCharge */
                                  break;

                                (void)facility(RecordedUnitsList, l);

                                (void)facility(TypeOfChargingInfo, l);

                                /*
                                (void)facility(AOCDBillingId, l);
                                */
                                break;

    case AOCEChargingUnitInfo : c = strtol(asnp += 3, NIL, 16);            /* SEQUENCE type */
                                aoc_debug(c, "AOCEChargingUnitInfo");

                                sprintf(s, "SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* SEQUENCE length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                c = strtol(asnp += 3, NIL, 16);  /* specificChargingUnits SEQUENCE type / NULL */

                                sprintf(s, "specificChargingUnits SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);  /* specificChargingUnits SEQUENCE length / NULL */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                if (!l)                                 /* freeOfCharge */
                                  break;

                                (void)facility(RecordedUnitsList, l);

                                (void)facility(TypeOfChargingInfo, l);

                                /*
                                (void)facility(AOCDBillingId, l);
                                */
                                break;

    case AOCDCurrencyInfo     : c = strtol(asnp += 3, NIL, 16);  /* specificCurrency SEQUENCE type / NULL */
                                aoc_debug(c, "AOCDCurrencyInfo");

                                sprintf(s, "specificCurrency SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);  /* specificCurrency SEQUENCE length / NULL */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                if (!l)                                 /* freeOfCharge */
                                  break;

                                c = strtol(asnp += 3, NIL, 16);            /* SEQUENCE type */

                                sprintf(s, "SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* SEQUENCE length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                (void)facility(RecordedCurrency, l);

                                (void)facility(TypeOfChargingInfo, l);

                                /*
                                (void)facility(AOCDBillingId, l);
                                */
                                break;

    case AOCECurrencyInfo     : c = strtol(asnp += 3, NIL, 16);            /* SEQUENCE type */
                                aoc_debug(c, "AOCECurrencyInfo");

                                sprintf(s, "SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* SEQUENCE length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                c = strtol(asnp += 3, NIL, 16);  /* specificCurrency SEQUENCE type / NULL */

                                sprintf(s, "specificCurrency SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);  /* specificCurrency SEQUENCE length / NULL */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                if (!l)                                 /* freeOfCharge */
                                  break;

                                c = strtol(asnp += 3, NIL, 16);            /* SEQUENCE type */

                                sprintf(s, "SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* SEQUENCE length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                (void)facility(RecordedCurrency, l);

                                /*
                                (void)facility(AOCDBillingId, l);
                                (void)facility(ChargingAssociation, l);
                                */
                                break;

    case RecordedCurrency     : c = strtol(asnp += 3, NIL, 16);            /* IA5String type */
                                aoc_debug(c, "RecordedCurrency");

                                sprintf(s, "IA5String type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* IA5String length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                (void)facility(Currency, l);

                                c = strtol(asnp += 3, NIL, 16);            /* SEQUENCE type */

                                sprintf(s, "SEQUENCE type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* SEQUENCE length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                (void)facility(Amount, l);
                                break;

    case Currency             : aoc_debug(-1, "Currency");
                                c = 0;

                                while (l--)
                                  curr[c++] = strtol(asnp += 3, NIL, 16);

                                curr[c] = 0;
                                currency = curr;

                                sprintf(s, "\"%s\"", currency);
                                aoc_debug(-2, s);
                                break;

    case Amount 	      : c = strtol(asnp += 3, NIL, 16);            /* INTEGER type */
                                aoc_debug(c, "Amount");

                                sprintf(s, "INTEGER type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* INTEGER length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                (void)facility(CurrencyAmount, l);

                                c = strtol(asnp += 3, NIL, 16);            /* ENUMERATED type */

                                sprintf(s, "ENUMERATED type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* ENUMERATED length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                (void)facility(Multiplier, l);
                                break;

    case CurrencyAmount       : aoc_debug(-1, "CurrencyAmount");
                                while (l--) {
                                  EH = EH << 8;
                                  EH += strtol(asnp += 3, NIL, 16);        /* Amount */
                                } /* while */

                                currency_mode = AOC_AMOUNT;

                                sprintf(s, "%d", EH);
                                aoc_debug(-2, s);
                                break;

    case Multiplier           : aoc_debug(-1, "Multiplier");

                                while (l--) {
                                  MP = MP << 8;
                                  MP += strtol(asnp += 3, NIL, 16);        /* Multiplier */
                                } /* while */

                                switch (MP) {
                                  case 0 : currency_factor = 0.001;
                                           break;

                                  case 1 : currency_factor = 0.01;
                                           break;

                                  case 2 : currency_factor = 0.1;
                                           break;

                                  case 3 : currency_factor = 1.0;
                                           break;

                                  case 4 : currency_factor = 10.0;
                                           break;

                                  case 5 : currency_factor = 100.0;
                                           break;

                                  case 6 : currency_factor = 1000.0;
                                           break;
                                } /* switch */

                                sprintf(s, "%g", currency_factor);
                                aoc_debug(-2, s);
                                break;

    case RecordedUnitsList    : c = strtol(asnp += 3, NIL, 16);            /* SEQUENCE SIZE type */

                                sprintf(s, "RecordedUnitsList=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* SEQUENCE SIZE length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                (void)facility(RecordedUnits, l);
                                break;

    case RecordedUnits        : c = strtol(asnp += 3, NIL, 16);            /* SEQUENCE type */

                                sprintf(s, "Sequence Size(1..32) Of RecordedUnits=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* SEQUENCE length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                if (!l)                                 /* notAvailable */
                                  break;

                                (void)facility(NumberOfUnits, l);
                                break;

   case NumberOfUnits         : c = strtol(asnp += 3, NIL, 16);            /* INTEGER type */

                                sprintf(s, "NumberOfUnits type=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* INTEGER length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                while (l--) {
                                  EH = EH << 8;
                                  EH += strtol(asnp += 3, NIL, 16);        /* NumberOfUnits */
                                } /* while */

                                currency_mode = AOC_UNITS;

                                sprintf(s, "NumberOfUnits=%d", EH);
                                aoc_debug(EH, s);
                                break;

    case TypeOfChargingInfo   : c = strtol(asnp += 3, NIL, 16);            /* typeOfChargingInfo type */

                                sprintf(s, "typeOfChargingInfo=%d", c);
                                aoc_debug(c, s);

                                l = strtol(asnp += 3, NIL, 16);            /* typeOfChargingInfo length */

                                sprintf(s, "length=%d", l);
                                aoc_debug(l, s);

                                c = strtol(asnp += 3, NIL, 16);            /* typeOfChargingInfo contents */

                                sprintf(s, "typeOfChargingInfo contents=%d", c);
                                aoc_debug(c, s);
                                break;
  } /* switch */

  return(result);
} /* facility */


/* gcc 2.7.2.1 Optimizer-Bug workaround from Stefan Gruendel <sgruendel@adulo.de> */
static int facility_start(char *p, int type, int l)
{
  asnp = p;
  return(facility(type, l));
} /* facility_start */


static int AOC_1TR6(int l, char *p)
{
  auto   int  EH = 0;
  auto   int  digit = 0;
#if 0
  static char curr[64];
#endif


#ifdef ISDN_NL
  /*
   *  NL ISDN: N40*<Einheiten>#, mit Einheiten ASCII kodiert.
   *  Beispiel 30 Einheiten: N40*30#
   *  Ich weiss nicht, fuer was 'N40' steht... Skip it.
   *  Einheit ist NLG 0.15, uebrigens.
   */

  p += 9;
  l -= 3;
  aoc_debug(-1, "AOC_INITIAL_NL");
#elif defined(ISDN_CH)
  /*
   * "FR. 0.10"
   *
   *
   */
  p += 9;
  l -= 3; /* Thanks to Markus Maeder (mmaeder@cyberlink.ch) */
  aoc_debug(-1, "AOC_INITIAL_CH");
#else
  aoc_debug(-1, "AOC_INITIAL_1TR6");
#endif

  while (l--) {
    digit = strtol(p += 3, NIL, 16) ;

    if ((digit >= '0') && (digit <= '9')) {
      EH = EH * 10;
      EH += (digit - '0'); /* Einheiten sind in ASCII */
    } /* if */
  } /* while */

  currency_mode = AOC_AMOUNT;
  return(EH);
} /* AOC_1TR6 */


static int detach()
{
  if (replay)
    return(1);

  if (!close(sockets[ISDNCTRL].descriptor)) {
    if (!*isdnctrl2 || !close(sockets[ISDNCTRL2].descriptor)) {
      if (!close(sockets[ISDNINFO].descriptor)) {
        return(1);
      }
      else {
        print_msg(PRT_DEBUG_CS, "cannot close \"%s\": %s\n", INFO, strerror(errno));
        Exit(33);
      } /* else */
    }
    else {
      print_msg(PRT_DEBUG_CS, "cannot close \"%s\": %s\n", isdnctrl2, strerror(errno));
      Exit(39);
    } /* else */
  }
  else {
    print_msg(PRT_DEBUG_CS, "cannot close \"%s\": %s\n", isdnctrl, strerror(errno));
    Exit(31);
  } /* else */

  return(0);
} /* detach */


static int attach()
{
  if (replay)
    return(1);

  if ((sockets[ISDNCTRL].descriptor = open(isdnctrl, O_RDONLY | O_NONBLOCK)) < 0) {
    print_msg(PRT_DEBUG_CS, "cannot open \"%s\": %s\n", isdnctrl, strerror(errno));
    Exit(30);
  } /* if */

  if (*isdnctrl2)
    if ((sockets[ISDNCTRL2].descriptor = open(isdnctrl2, O_RDONLY | O_NONBLOCK)) < 0) {
      print_msg(PRT_DEBUG_CS, "cannot open \"%s\": %s\n", isdnctrl2, strerror(errno));
      Exit(38); /* cannot (re)open "/dev/isdnctrl2" */
    } /* if */

  if ((sockets[ISDNINFO].descriptor = open(INFO, O_RDONLY | O_NONBLOCK)) < 0) {
    print_msg(PRT_DEBUG_CS, "cannot open \"%s\": %s\n", INFO, strerror(errno));
    Exit(32);
  } /* if */

  return(0);
} /* attach */


static void chargemaxAction(int chan, double charge_overflow)
{
  register int   cc = 0, c = call[chan].confentry[OTHER];
  auto     char  cmd[BUFSIZ], msg[BUFSIZ];


  sprintf(cmd, "%s/dontstop", confdir());

  if (access(cmd, F_OK)) {
    sprintf(cmd, "%s/%s", confdir(), STOPCMD);

    if (!access(cmd, X_OK)) {

      sprintf(cmd, "%s/%s %s %s %s",
        confdir(), STOPCMD, double2str((charge_overflow), 6, 2, DEB),
        known[c]->who,
        double2str(known[c]->scharge, 6, 2, DEB));

      sprintf(msg, "CHARGEMAX exhausted: %s", cmd);
      info(chan, PRT_ERR, STATE_AOCD, msg);

      (void)detach();

      cc = replay ? 0 : system(cmd);

      (void)attach();

      sprintf(msg, "CHARGEMAX exhausted: result = %d", cc);
      info(chan, PRT_ERR, STATE_AOCD, msg);
    } /* if */
    else
    {
      sprintf(msg, "CHARGEMAX exhausted: stop script `%s' doesn't exist! - NO ACTION! (%s)", cmd, strerror(errno));
      info(chan, PRT_ERR, STATE_AOCD, msg);
    }
  }
  else {
    sprintf(msg, "CHARGEMAX exhausted - NO ACTION!! - %s exists!", cmd);
    info(chan, PRT_ERR, STATE_AOCD, msg);
  } /* else */
} /* chargemaxAction */


static void emergencyStop(int chan, int strength)
{
  register int   cc = 0, c = call[chan].confentry[OTHER];
  register char *p;
  auto     char  cmd[BUFSIZ], msg[BUFSIZ];


  if (strength == 1) {
    if (c == -1)
      strength++;
    else if (*known[c]->interface < '@')
      strength++;
  } /* if */

  sprintf(cmd, "%s/%s", confdir(), RELOADCMD);

  if (strength == 2)
    if (access(cmd, X_OK))
      strength++;

  switch (strength) {
    case 1 : cc = replay ? 0 : ioctl(sockets[ISDNCTRL].descriptor, IIOCNETHUP, known[c]->interface);

             if (cc < 0)
               p = "failed";
             else if (cc)
               p = "not connected";
             else
               p = "hung up";

             sprintf(msg, "EMERGENCY-STOP#%d: no traffic since %d EH: hangup %s = %s\007\007",
               strength, call[chan].aoce - call[chan].traffic, known[c]->interface, p);
             break;

    case 2 : (void)detach();

    	     cc = replay ? 0 : system(cmd);

    	     (void)attach();

             sprintf(msg, "EMERGENCY-STOP#%d: no traffic since %d EH: reload = %d\007\007",
               strength, call[chan].aoce - call[chan].traffic, cc);
             break;

    case 3 : if (replay)
               cc = 0;
             else {
               if ((cc = ioctl(sockets[ISDNCTRL].descriptor, IIOCSETGST, 1)) < 0)
                 ;
               else
                 cc = ioctl(sockets[ISDNCTRL].descriptor, IIOCSETGST, 0);
             } /* if */

             sprintf(msg, "EMERGENCY-STOP#%d: no traffic since %d EH: system off = %d\007\007",
               strength, call[chan].aoce - call[chan].traffic, cc);

             break;

    case 4 : sprintf(msg, "EMERGENCY-STOP#%d: no traffic since %d EH: REBOOT!!\007\007",
               strength, call[chan].aoce - call[chan].traffic);

             info(chan, PRT_ERR, STATE_AOCD, msg);

             if (!replay) {
               Exit(-9);               /* cleanup only! */
               system(REBOOTCMD);
             } /* if */
             break;
  } /* switch */

  info(chan, PRT_ERR, STATE_AOCD, msg);

} /* emergencyStop */


static int expensive(int bchan)
{
  return( (ifo[bchan].u & ISDN_USAGE_OUTGOING) &&
        (((ifo[bchan].u & ISDN_USAGE_MASK) == ISDN_USAGE_NET) ||
         ((ifo[bchan].u & ISDN_USAGE_MASK) == ISDN_USAGE_MODEM)));
} /* expensive */


static void decode(int chan, register char *p, int type, int version, int tei)
{
  register char     *pd, *px, *py;
  register int       i, element, l, l1, c, oc3, oc3a, n, sxp = 0, warn;
  auto	   int	     zeit, loc, cause;
  auto     char      s[BUFSIZ], s1[BUFSIZ];
  auto     char      sx[10][BUFSIZ];
  auto     int       sn[10];
  auto     struct tm tm;
  auto	   time_t    t;
  auto     double    tx, err, tack, pay = 0.0;


  while (1) {

    if (!*(p + 2))
      break;

    element = strtol(p += 3, NIL, 16);

    if (element < 128) {

      l = strtol(p += 3, NIL, 16);

#ifdef Q931
      if (q931dmp) {
        auto char s[BUFSIZ];

        Q931dump(TYPE_ELEMENT, element, NULL, version);

        sprintf(s, "length=%d", l);
        Q931dump(TYPE_STRING, l, s, version);
      } /* if */
#endif

      pd = qmsg(TYPE_ELEMENT, version, element);

      if (strncmp(pd, "UNKNOWN", 7) == 0) {
        register char *p1 = p, *p2;
        register int   i, c;
        auto     char  s[LONG_STRING_SIZE];


        p2 = s;
        p2 += sprintf(p2, "UNKNOWN ELEMENT %02x:", element);

        for (i = 0; i < l; i++)
          p2 += sprintf(p2, " %02x", (int)strtol(p1 += 3, NIL, 16));

        p2 += sprintf(p2, " [");
        p1 = p;

        for (i = 0; i < l; i++) {
          c = (int)strtol(p1 += 3, NIL, 16);
          p2 += sprintf(p2, "%c", isgraph(c) ? c : ' ');
        } /* for */

        p2 += sprintf(p2, "], length=%d", l);
        info(chan, PRT_SHOWNUMBERS, STATE_RING, s);
      }
      else
        print_msg(PRT_DEBUG_DECODE, " DEBUG> %s: ELEMENT %02x:%s (length=%d)\n", st + 4, element, pd, l);

      switch (element) {
        case 0x08 : /* Cause */
                    if (version == VERSION_1TR6) {
                      switch (l)  {
                         case 0 : call[chan].cause = 0;
                                  break;

                         case 1 : call[chan].cause = strtol(p += 3, NIL, 16) & 0x7f;
                                  break;

                         case 2 : call[chan].cause = strtol(p += 3, NIL, 16) & 0x7f;
                                  c = strtol(p += 3, NIL, 16); /* Sometimes it 0xc4 or 0xc5 */
                                  break;

                        default : p += (l * 3);
                                  print_msg(PRT_ERR, "Wrong Cause (more than two bytes)\n");
                                  break;
                      } /* switch l */

                      info(chan, PRT_SHOWCAUSE, STATE_CAUSE, qmsg(TYPE_CAUSE, version, call[chan].cause));

                      if (sound) {
                        if (call[chan].cause == 0x3b) /* "User busy" */
                          ringer(chan, RING_BUSY);
                        else
                          ringer(chan, RING_ERROR);
                      } /* if */
                    }
                    else { /* E-DSS1 */
                      c = strtol(p + 3, NIL, 16);

#ifdef Q931
                      if (q931dmp) {
                        register char *ps;
                        auto     char  s[BUFSIZ];


                        ps = s + sprintf(s, "Coding: ");

                        switch (c & 0xf0) {
                          case 0x00 :
                          case 0x80 : sprintf(ps, "CCITT standardisierte Codierung");     break;
                          case 0x20 :
                          case 0xa0 : sprintf(ps, "Reserve");                             break;
                          case 0x40 :
                          case 0xc0 : sprintf(ps, "reserviert fuer nationale Standards"); break;
                          case 0x60 :
                          case 0xe0 : sprintf(ps, "Standard bzgl. Localierung");          break;
                            default : sprintf(ps, "UNKNOWN #%d", c & 0xf0);               break;
                        } /* switch */

                        Q931dump(TYPE_STRING, c, s, version);

                        ps = s + sprintf(s, "Location: ");

                        switch (c & 0x0f) {
                          case 0x00 : sprintf(ps, "Nutzer");                                   break;
                          case 0x01 : sprintf(ps, "Privates Netz des Nutzers");                break;
                          case 0x02 : sprintf(ps, "Oeffentliches Netz des Nutzers");           break;
                          case 0x03 : sprintf(ps, "Transitnetz");                              break;
                          case 0x04 : sprintf(ps, "Oeffentliches Netz beim fernen Nutzer");    break;
                          case 0x05 : sprintf(ps, "Privates Netz beim fernen Nutzer");         break;
                          case 0x07 : sprintf(ps, "Internationales Netz");                     break;
                          case 0x0a : sprintf(ps, "Netzwerk jenseits des interworking point"); break;
                            default : sprintf(ps, "UNKNOWN #%d", c & 0x0f);                    break;
                        } /* switch */

                        Q931dump(TYPE_STRING, -1, s, version);
                      } /* if */
#endif

		      switch ((loc = (c & 0x0f))) {
                        case 0x00 : py = "User";                                break;
                        case 0x01 : py = "Private network serving local user";  break;
                        case 0x02 : py = "Public network serving local user";   break;
                        case 0x03 : py = "Transit network";                     break;
                        case 0x04 : py = "Public network serving remote user";  break;
                        case 0x05 : py = "Private network serving remote user"; break;
                        case 0x07 : py = "International network";               break;
                        case 0x0a : py = "Network beyond inter-working point";  break;
                          default : py = "";             	       	     	break;
                      } /* switch */

                      c = strtol(p + 6, NIL, 16);
		      cause = c & 0x7f;

		      if ((tei != call[chan].tei) && (chan == 6)) { /* AK:26-Nov-98 */
#ifdef Q931
                        if (q931dmp) {
                          auto char s[256];

                          Q931dump(TYPE_CAUSE, c, NULL, version);

                          sprintf(s, "IGNORING CAUSE: tei=%d, call.tei=%d, chan=%d", tei, call[chan].tei, chan);
          		  Q931dump(TYPE_STRING, -2, s, version);
                        }
#endif
                        p += (l * 3);
                        break;
		      } /* if */

                      /* Remember only the _first_ cause
                      	 except this was "Normal call clearing", "No user responding"
                         or "non-selected user clearing"
                      */

                      if ((call[chan].cause == -1) || /* The first cause */
                          (call[chan].cause == 16) || /* "Normal call clearing" */
                          (call[chan].cause == 18) || /* "No user responding" */
                          (call[chan].cause == 26)) { /* "non-selected user clearing" */

#if 0
                        auto char sx[200];

                        if ((call[chan].cause != -1) && (call[chan].cause != cause)) {
                          sprintf(sx, "USING cause %d:%s (%s),\nOVERWRITING cause %d:%s (%s)",
                            cause, qmsg(TYPE_CAUSE, version, cause), location(loc),
                            call[chan].cause, qmsg(TYPE_CAUSE, version, call[chan].cause), location(call[chan].loc));
                          info(chan, PRT_SHOWHANGUP, STATE_HANGUP, sx);
                        } /* if */
#endif

                        call[chan].cause = cause;
                        call[chan].loc = loc;
                      }
#if 0
                      else {
                        auto char sx[200];

                        sprintf(sx, "IGNORING cause %d:%s (%s),\nLEAVING cause %d:%s (%s)",
                          cause, qmsg(TYPE_CAUSE, version, cause), location(loc),
                          call[chan].cause, qmsg(TYPE_CAUSE, version, call[chan].cause), location(call[chan].loc));
                        info(chan, PRT_SHOWHANGUP, STATE_HANGUP, sx);
                      } /* else */
#endif

#ifdef Q931
                      if (q931dmp)
                        Q931dump(TYPE_CAUSE, c, NULL, version);
#endif

                      if (HiSax || (
                          (call[chan].cause != 0x10) &&  /* "Normal call clearing" */
                          (call[chan].cause != 0x1a) &&  /* "non-selected user clearing" */
                          (call[chan].cause != 0x1f) &&  /* "Normal, unspecified" */
                          (call[chan].cause != 0x51))) { /* "Invalid call reference value" <- dies nur Aufgrund eines Bug im Teles-Treiber! */
                        sprintf(s, "%s (%s)", qmsg(TYPE_CAUSE, version, call[chan].cause), py);

                        if (tei == call[chan].tei)
                        info(chan, PRT_SHOWCAUSE, STATE_CAUSE, s);
                        else if (other) {
                          auto char sx[256];

                          sprintf(sx, "TEI %d : %s", tei, s);
                          info(chan, PRT_SHOWCAUSE, STATE_CAUSE, sx);
                        } /* else */

                        if (sound) {
                          if (call[chan].cause == 0x11) /* "User busy" */
                            ringer(chan, RING_BUSY);
                          else if ((call[chan].cause != 0x10) &&
                                   (call[chan].cause != 0x1a) &&
                                   (call[chan].cause != 0x1f) &&
                                   (call[chan].cause != 0x51))
                            ringer(chan, RING_ERROR);
                        } /* if */
                      } /* if */

                      p += (l * 3);
                    } /* else */

                    break;

#ifdef ISDN_DE
        case 0x28 : /* DISPLAY ... z.b. Makelweg, AOC-E ... */
#ifdef Q931
                    if (q931dmp) {
                      auto     char  s[BUFSIZ];
                      register char *ps = s;


                      while (l--)
                        *ps++ = strtol(p += 3, NIL, 16);

                      *ps = 0;

                      Q931dump(TYPE_STRING, -2, s, version);
                    }
                    else
#endif
                      p += (l * 3);
                    break;
#endif

        case 0x2d : /* SUSPEND ACKNOWLEDGE (Parkweg) */
                    p += (l * 3);
                    break;

        case 0x2e : /* RESUME ACKNOWLEDGE (Parkran) */
                    p += (l * 3);
                    /* ggf. neuer Channel kommt gleich mit */
                    break;

        case 0x33 : /* makel resume acknowledge (Makelran) */
                    p += (l * 3);
                    /* ggf. neuer Channel kommt gleich mit */
                    break;

        case 0x02 : /* Facility AOC-E on 1TR6 */
        case 0x1c : /* Facility AOC-D/AOC-E on E-DSS1 */
#if defined(ISDN_NL) || defined(ISDN_CH)
        case 0x28 : /* DISPLAY: Facility AOC-E on E-DSS1 in NL, CH */
#endif
                    if ((element == 0x02) && (version == VERSION_1TR6)) {
                      n = AOC_1TR6(l, p);           /* Wieviele Einheiten? */

                      if (type == AOCD_1TR6) {
                        n = -n;  /* Negativ: laufende Verbindung */
                                 /* ansonsten wars ein AOCE */
                        print_msg(PRT_DEBUG_DECODE, " DEBUG> %s: 1TR6 AOCD %i\n", st + 4, n);
                      } /* if */
                    }
                    else {
#if defined(ISDN_NL) || defined(ISDN_CH)
                      n = AOC_1TR6(l, p);
#else
                      n = facility_start(p, AOC_INITIAL, 0);
#endif

                      if (n == AOC_OTHER)
                        ; /* info(chan, PRT_SHOWAOCD, STATE_AOCD, asnm); */
#if 0
                      else if (!memcmp(call[chan].provider, "01019", 5) ||
                      	       !memcmp(call[chan].provider, "01070", 5)) {

		        if (type != FACILITY) { /* "AOC-E" Meldung */
			  if (!memcmp(call[chan].provider, "01019", 5)) { /* Mobilcom */
                            tx = cur_time - call[chan].connect;

                            call[chan].aoce = (int)((tx + 59) / 60);
                            call[chan].pay = call[chan].aoce * 0.19;

                            if (tx)
                              sprintf(s, "%s %s (%s)",
                                currency,
                                double2str(call[chan].pay, 6, 2, DEB),
                                double2clock(tx));
                            else
                              sprintf(s, "%s %s",
                                currency,
                                double2str(call[chan].pay, 6, 2, DEB));

                            info(chan, PRT_SHOWAOCD, STATE_AOCD, s);
                          } /* if */
                        } /* if */
                      }
#endif
                      else {

                        /* Dirty-Hack: Falls auch AOC-E als AOC-D gemeldet wird:

                           Ist Fehler in der VSt! Wird gerne bei nachtraeglicher
                           Beantragung von AOC-D falsch eingestellt :-(
                           -> Telekom treten!
                        */

                        if ((type != FACILITY) && (n < 0)) {
                          aoc_debug(-1, "DIRTY-HACK: AOC-D -> AOC-E");
                          n = -n;
                        } /* if */

                        if (n) {
                          pay = abs(n) * currency_factor;

                          sprintf(s, "%d * %g = %g%s", abs(n), currency_factor, pay, currency);
                          aoc_debug(-2, s);
                        } /* if */

                        if (n < 0)
                          call[chan].aoce++;

                        call[chan].pay = pay;
                        call[chan].aoce = n;  /* AK:08-May-98 */

                        if (n < 0)
                          sprintf(s, "aOC-D=%d", -n);
                        else if (!n)
                          sprintf(s, "aOC-E=FREE OF CHARGE");
                        else
                          sprintf(s, "aOC-E=%d", n);
                        aoc_debug(-1, s);

                        if (!n)
                          info(chan, PRT_SHOWAOCD, STATE_AOCD, "Free of charge");
                        else if (n < 0) {
                          tx = cur_time - call[chan].connect;

                          if ((c = call[chan].confentry[OTHER]) > -1) {
                            /* tack = cheap96(cur_time, known[c]->zone, &zeit); */
			    tack = taktlaenge (chan, NULL);
                            err  = call[chan].tick - tx;
                            call[chan].tick += tack;

                            if (message & PRT_SHOWTICKS)
                              sprintf(s, "%d.EH %s %s (%s %d) C=%s",
                                abs(call[chan].aoce),
                                currency,
                                double2str(call[chan].pay, 6, 2, DEB),
                                tx ? double2clock(tx) : "", (int)err,
                                double2clock(call[chan].tick - tx) + 4);
                            else {
                              if (tx)
                                sprintf(s, "%d.EH %s %s (%s)",
                                  abs(call[chan].aoce),
                                  currency,
                                  double2str(call[chan].pay, 6, 2, DEB),
                                  double2clock(tx));
                              else
                                sprintf(s, "%d.EH %s %s",
                                  abs(call[chan].aoce),
                                  currency,
                                  double2str(call[chan].pay, 6, 2, DEB));
                            } /* else */

                            if (chargemax != 0.0) {
                              if (day != known[c]->day) {
                                sprintf(s1, "CHARGEMAX resetting %s's charge (day %d->%d)",
                                  known[c]->who, (known[c]->day == -1) ? 0 : known[c]->day, day);

                                info(chan, PRT_SHOWCHARGEMAX, STATE_AOCD, s1);

                                known[c]->scharge += known[c]->charge;
                                known[c]->charge = known[c]->rcharge = 0.0;
                                known[c]->day = day;
                              } /* if */

                              known[c]->charge += (pay / call[chan].aoce);
                              known[c]->rcharge += (pay / call[chan].aoce);

                              sprintf(s1, "CHARGEMAX remaining=%s %s %s %s",
                                currency,
                                double2str((chargemax - known[c]->charge), 6, 2, DEB),
                                (connectmax == 0.0) ? "" : double2clock(connectmax - known[c]->online - tx),
                                (bytemax == 0.0) ? "" : double2byte((double)(bytemax - known[c]->bytes)));


                              info(chan, PRT_SHOWCHARGEMAX, STATE_AOCD, s1);

                              if ((known[c]->charge >= chargemax) && (*known[c]->interface > '@'))
                                chargemaxAction(chan, (known[c]->charge - chargemax));
                            } /* if */

                            if (connectmax != 0.0) {
                              if (month != known[c]->month) {
                                sprintf(s1, "CONNECTMAX resetting %s's online (month %d->%d)",
                                  known[c]->who, (known[c]->month == -1) ? 0 : known[c]->month, month);

                                info(chan, PRT_SHOWCHARGEMAX, STATE_AOCD, s1);

                                known[c]->sonline += known[c]->online;
                                known[c]->online = 0.0;
                                known[c]->month = month;

                                known[c]->sbytes += known[c]->bytes;
                                known[c]->bytes = 0.0;
                              } /* if */
                            } /* if */
                          }
                          else if (-n > 1) { /* try to guess Gebuehrenzone */
#ifdef ISDN_AT
			    px="";
			    err=60*60*24*365; /* sehr gross */
			    for (c = 1; c < 31; c++) {
			      call[chan].zone=c;
			      tack = (-n-1) * taktlaenge (chan, NULL);
			      if ((tack > 0) && (abs(tack - tx)<err)) {
				call[chan].tick = tack;
				err = abs(tack) - tx;
				px = z2s(c);
			      }
			    }
			    call[chan].zone=-1;
#else
                            tack = 0;
                            err = 0;
                            px = "";

                            for (c = 4; c > 0; c--) {
                              call[chan].tick = 0;

                              for (i = 0; i < -n - 1; i++) {
                                tack = cheap96(cur_time, c, &zeit);
                                call[chan].tick += tack;
                              } /* for */

                              err = call[chan].tick - tx;

                              if (err >= 0) {
                                switch (c) {
                                  case 4 : px = "Fern";      break;
                                  case 3 : px = "Regio 200"; break;
                                  case 2 : px = "Regio 50";  break;
                                  case 1 : px = "City";      break;
                                } /* switch */

                                break;
                              } /* if */
                            } /* for */
#endif
                            if (message & PRT_SHOWTICKS)
                              sprintf(s, "%d.EH %s %s (%s %d %s?) C=%s",
                                abs(call[chan].aoce),
                                currency,
                                double2str(call[chan].pay, 6, 2, DEB),
                                tx ? double2clock(tx) : "", (int)err, px,
                                double2clock(call[chan].tick - tx) + 4);
                            else {
                              if (tx)
                                sprintf(s, "%d.EH %s %s (%s)",
                                  abs(call[chan].aoce),
                                  currency,
                                  double2str(call[chan].pay, 6, 2, DEB),
                                  double2clock(tx));
                              else
                                sprintf(s, "%d.EH %s %s",
                                  abs(call[chan].aoce),
                                  currency,
                                  double2str(call[chan].pay, 6, 2, DEB));
                            } /* else */
                          }
                          else {
                            sprintf(s, "%d.EH %s %s",
                              abs(call[chan].aoce),
                              currency,
                              double2str(call[chan].pay, 6, 2, DEB));
                          } /* else */

                          info(chan, PRT_SHOWAOCD, STATE_AOCD, s);

                          if (sound)
                            ringer(chan, RING_AOCD);

                          /* kostenpflichtiger Rausruf (wg. FACILITY) */
                          /* muss mit Teles-Karte sein, da eigene MSN bekannt */
                          /* seit 2 Gebuehrentakten kein Traffic mehr! */

                          if (watchdog && ((c = call[chan].confentry[OTHER]) > -1)) {
                            if ((type == FACILITY) && (version == VERSION_EDSS1) && expensive(call[chan].bchan) && (*known[c]->interface > '@')) {
                              if (call[chan].aoce > call[chan].traffic + watchdog + 2)
                                emergencyStop(chan, 4);
                              else if (call[chan].aoce > call[chan].traffic + watchdog + 1)
                                emergencyStop(chan, 3);
                              else if (call[chan].aoce > call[chan].traffic + watchdog)
                                emergencyStop(chan, 2);
                              else if (call[chan].aoce > call[chan].traffic + watchdog - 1)
                                emergencyStop(chan, 1);
                            } /* if */
                          } /* if */
                        }
                        else { /* AOC-E */
                          if ((c = call[chan].confentry[OTHER]) > -1) {
                            known[c]->charge -= known[c]->rcharge;
                            known[c]->charge += pay;

                            if (chargemax != 0.0) { /* only used here if no AOC-D */
                              if (day != known[c]->day) {
                                sprintf(s, "CHARGEMAX resetting %s's charge (day %d->%d)",
                                  known[c]->who, (known[c]->day == -1) ? 0 : known[c]->day, day);

                                info(chan, PRT_SHOWCHARGEMAX, STATE_AOCD, s);

                                known[c]->scharge += known[c]->charge;
                                known[c]->charge = 0.0;
                                known[c]->day = day;
                              } /* if */
                            } /* if */

                            if (connectmax != 0.0) { /* only used here if no AOC-D */
                              if (month != known[c]->month) {
                                sprintf(s, "CONNECTMAX resetting %s's online (month %d->%d)",
                                  known[c]->who, (known[c]->month == -1) ? 0 : known[c]->month, month);

                                info(chan, PRT_SHOWCHARGEMAX, STATE_AOCD, s);

                                known[c]->sonline += known[c]->online;
                                known[c]->online = 0.0;
                                known[c]->month = month;

                                known[c]->sbytes += known[c]->bytes;
                                known[c]->bytes = 0.0;
                              } /* if */
                            } /* if */
                          } /* if */
                        } /* else */
                      } /* if */
                    } /* if */

                    p += (l * 3);
                    break;

	case 0x03 : /* Date/Time 1TR6   */
        case 0x29 : /* Date/Time E-DSS1 */
                    if ((element == 0x03) && (version == VERSION_1TR6)) {
			if (l != 17)	/* 1TR6 date/time is always 17? */
  				/* "Unknown Codeset 7 attribute 3 size 5" */
				goto UNKNOWN_ELEMENT;
			tm.tm_mday  = (strtol(p+=3,NIL,16)-'0') * 10;
			tm.tm_mday +=  strtol(p+=3,NIL,16)-'0';
			p += 3;	/* skip '.' */
			tm.tm_mon   = (strtol(p+=3,NIL,16)-'0') * 10;
			tm.tm_mon  +=  strtol(p+=3,NIL,16)-'0' - 1;
			p += 3;	/* skip '.' */
			tm.tm_year  = (strtol(p+=3,NIL,16)-'0') * 10;
			tm.tm_year +=  strtol(p+=3,NIL,16)-'0';
			if (tm.tm_year < 70)
			    tm.tm_year += 100;
			p += 3; /* skip '-' */
			tm.tm_hour  = (strtol(p+=3,NIL,16)-'0') * 10;
			tm.tm_hour +=  strtol(p+=3,NIL,16)-'0';
			p += 3; /* skip ':' */
			tm.tm_min   = (strtol(p+=3,NIL,16)-'0') * 10;
			tm.tm_min  +=  strtol(p+=3,NIL,16)-'0';
			p += 3; /* skip ':' */
			tm.tm_sec   = (strtol(p+=3,NIL,16)-'0') * 10;
			tm.tm_sec  +=  strtol(p+=3,NIL,16)-'0';
		    }
		    else if ((element == 0x29) && (version != VERSION_1TR6)) {
			tm.tm_year  = strtol(p += 3, NIL, 16);
			tm.tm_mon   = strtol(p += 3, NIL, 16) - 1;
			tm.tm_mday  = strtol(p += 3, NIL, 16);
			tm.tm_hour  = strtol(p += 3, NIL, 16);
			tm.tm_min   = strtol(p += 3, NIL, 16);
			if (l > 5)
			  tm.tm_sec = strtol(p += 3, NIL, 16);
			else
			  tm.tm_sec = 0;
		    }
		    else {
		    	goto UNKNOWN_ELEMENT; /* no choice... */
		    }
                    tm.tm_wday  = tm.tm_yday = 0;
                    tm.tm_isdst = -1;

                    t = mktime(&tm);

                    if (t != (time_t)-1) {
                      call[chan].time = t;

                      if (settime) {
                        auto time_t     tn;
                      	auto struct tms tms;

                      	time(&tn);

                      	if (labs(tn - call[chan].time) > 61) {
                          (void)stime(&call[chan].time);

                          /* Nicht gerade sauber, sollte aber all zu
                             grosse Spruenge verhindern! */

                          if (replay)
                            cur_time = tt = tto = call[chan].time;
                          else {
                            time(&cur_time);
                            tt = tto = times(&tms);
                          } /* else */

                          set_time_str();

                        } /* if */

                        if (settime == 1)
                          settime--;
                      } /* if */
                    } /* if */

                    sprintf(s, "Time:%s", (t == (time_t)-1) ? "INVALID - ignored" : ctime(&call[chan].time));
                    if ((px = strchr(s, '\n')))
                      *px = 0;

#ifdef Q931
                    if (q931dmp) {
                      sprintf(s1, "Y=%02d M=%02d D=%02d H=%02d M=%02d",
                        tm.tm_year,
                    	tm.tm_mon + 1,
                    	tm.tm_mday,
                    	tm.tm_hour,
                    	tm.tm_min);
                      Q931dump(TYPE_STRING, -2, s1, version);
                      Q931dump(TYPE_STRING, -2, s + 5, version);
                    } /* if */
#endif
                    info(chan, PRT_SHOWTIME, STATE_TIME, s);
                    break;


        case 0x4c : /* COLP */
                    oc3 = strtol(p += 3, NIL, 16);

                    if (oc3 < 128) { /* Octet 3a : Screening indicator */
                      oc3a = strtol(p += 3, NIL, 16);
                      l--;
                    }
                    else
                      oc3a = -1;

                    pd = s;

                    while (--l)
                      *pd++ = strtol(p += 3, NIL, 16);

                    *pd = 0;

                    if (dual && !*s)
                      strcpy(s, call[chan].onum[CALLED]);
                    else
                    strcpy(call[chan].onum[CALLED], s);

                    buildnumber(s, oc3, oc3a, call[chan].num[CALLED], version, &call[chan].provider, &call[chan].sondernummer[CALLED], &call[chan].intern[CALLED], 0, 0);

                    if (!dual)
                    strcpy(call[chan].vnum[CALLED], vnum(chan, CALLED));

#ifdef Q931
                    if (q931dmp && (*call[chan].vnum[CALLED] != '?') && *call[chan].vorwahl[CALLED] && oc3 && ((oc3 & 0x70) != 0x40)) {
                      auto char s[BUFSIZ];

                      sprintf(s, "%s %s/%s, %s",
                        call[chan].areacode[CALLED],
                        call[chan].vorwahl[CALLED],
                        call[chan].rufnummer[CALLED],
                        call[chan].area[CALLED]);

                      Q931dump(TYPE_STRING, -2, s, version);
                    } /* if */
#endif

                    sprintf(s1, "COLP %s", call[chan].vnum[CALLED]);
                    info(chan, PRT_SHOWNUMBERS, STATE_RING, s1);

                    break;


        case 0x6c : /* Calling party number */
                    oc3 = strtol(p += 3, NIL, 16);

                    if (oc3 < 128) { /* Octet 3a : Screening indicator */
                      oc3a = strtol(p += 3, NIL, 16);
                      l--;
                    }
                    else
                      oc3a = -1;

                    /* Screening-Indicator:
                       -1 : UNKNOWN
                        0 : User-provided, not screened
                        1 : User-provided, verified and passed
                        2 : User-provided, verified and failed
                        3 : Network provided
                    */

                    pd = s;

                    while (--l)
                      *pd++ = strtol(p += 3, NIL, 16);

                    *pd = 0;

                    warn = 0;

                    if (*call[chan].onum[CALLING]) /* another Calling-party? */
                      if (strcmp(call[chan].onum[CALLING], s)) /* different! */
                        if ((call[chan].screening == 3) && ((oc3a & 3) < 3)) { /* we believe the first one! */
                          strcpy(call[chan].onum[CLIP], s);
                          buildnumber(s, oc3, oc3a, call[chan].num[CLIP], version, &call[chan].provider, &call[chan].sondernummer[CLIP], &call[chan].intern[CLIP], 0, 0);
                          strcpy(call[chan].vnum[CLIP], vnum(6, CLIP));
#ifdef Q931
                          if (q931dmp && (*call[chan].vnum[CLIP] != '?') && *call[chan].vorwahl[CLIP] && oc3 && ((oc3 & 0x70) != 0x40)) {
                            auto char s[BUFSIZ];

                            sprintf(s, "%s %s/%s, %s",
                              call[chan].areacode[CLIP],
                              call[chan].vorwahl[CLIP],
                              call[chan].rufnummer[CLIP],
                              call[chan].area[CLIP]);

                            Q931dump(TYPE_STRING, -2, s, version);
                          } /* if */
#endif

                          sprintf(s1, "CLIP %s", call[chan].vnum[CLIP]);
                          info(chan, PRT_SHOWNUMBERS, STATE_RING, s1);

                          break;
                        }
                        else {
                          warn = 1;

			  strcpy(call[chan].onum[CLIP],      call[chan].onum[CALLING]);
			  strcpy(call[chan].num[CLIP],       call[chan].num[CALLING]);
			  strcpy(call[chan].vnum[CLIP],      call[chan].vnum[CALLING]);
			  call[chan].confentry[CLIP] = call[chan].confentry[CALLING];
			  strcpy(call[chan].areacode[CLIP],  call[chan].areacode[CALLING]);
			  strcpy(call[chan].vorwahl[CLIP],   call[chan].vorwahl[CALLING]);
			  strcpy(call[chan].rufnummer[CLIP], call[chan].rufnummer[CALLING]);
			  strcpy(call[chan].alias[CLIP],     call[chan].alias[CALLING]);
			  strcpy(call[chan].area[CLIP],      call[chan].area[CALLING]);

                          /* fall thru, and overwrite ... */
                        } /* else */

                    call[chan].screening = (oc3a & 3);

                    strcpy(call[chan].onum[CALLING], s);
                    buildnumber(s, oc3, oc3a, call[chan].num[CALLING], version, &call[chan].provider, &call[chan].sondernummer[CALLING], &call[chan].intern[CALLING], call[chan].dialin, CALLING);

                    strcpy(call[chan].vnum[CALLING], vnum(chan, CALLING));
#ifdef Q931
                    if (q931dmp && (*call[chan].vnum[CALLING] != '?') && *call[chan].vorwahl[CALLING] && oc3 && ((oc3 & 0x70) != 0x40)) {
                      auto char s[BUFSIZ];

                      sprintf(s, "%s %s/%s, %s",
                        call[chan].areacode[CALLING],
                        call[chan].vorwahl[CALLING],
                        call[chan].rufnummer[CALLING],
                        call[chan].area[CALLING]);

                      Q931dump(TYPE_STRING, -2, s, version);
                    } /* if */
#endif
		    if (callfile && call[chan].dialin) {
		      FILE *cl = fopen(callfile, "a");

		    /* Fixme: what is short for 'Calling Party Number'? */
		    sprintf(s1, "CPN %s", call[chan].num[CALLING]);
		    info(chan, PRT_SHOWNUMBERS, STATE_RING, s1);

		      if (cl != NULL) {
			iprintf(s1, chan, callfmt);
			fprintf(cl, "%s\n", s1);
			fclose(cl);
		      } /* if */
		    } /* if */

                    if (warn) {
                      sprintf(s1, "CLIP %s", call[chan].vnum[CLIP]);
                      info(chan, PRT_SHOWNUMBERS, STATE_RING, s1);
                    } /* if */

                    break;


        case 0x70 : /* Called party number */
                    oc3 = strtol(p += 3, NIL, 16);

                    pd = s;

                    while (--l)
                      *pd++ = strtol(p += 3, NIL, 16);

                    *pd = 0;

                    if (dual && ((type == INFORMATION) || ((type == SETUP) && OUTGOING))) { /* Digit's beim waehlen mit ISDN-Telefon */
                      strcat(call[chan].digits, s);
                      call[chan].oc3 = oc3;
#ifdef Q931
                      if (q931dmp)
                        buildnumber(s, oc3, -1, call[chan].num[CALLED], version, &call[chan].provider, &call[chan].sondernummer[CALLED], &call[chan].intern[CALLED], call[chan].dialin, CALLED);
#endif

                      buildnumber(call[chan].digits, oc3, -1, call[chan].num[CALLED], version, &call[chan].provider, &call[chan].sondernummer[CALLED], &call[chan].intern[CALLED], call[chan].dialin, CALLED);

                      	strcpy(call[chan].vnum[CALLED], vnum(chan, CALLED));

                      if (dual > 1) {
                        auto char sx[BUFSIZ];

                        if (*call[chan].vorwahl[CALLED])
                      	  sprintf(sx, "DIALING %s [%s] %s %s/%s, %s",
                            s, call[chan].digits,
                            call[chan].areacode[CALLED],
                            call[chan].vorwahl[CALLED],
                            call[chan].rufnummer[CALLED],
                            call[chan].area[CALLED]);
                        else
                          sprintf(sx, "DIALING %s [%s]", s, call[chan].digits);

                      	info(chan, PRT_SHOWNUMBERS, STATE_RING, sx);
                      } /* if */
		    }
                    else {
                      strcpy(call[chan].onum[CALLED], s);
                      buildnumber(s, oc3, -1, call[chan].num[CALLED], version, &call[chan].provider, &call[chan].sondernummer[CALLED], &call[chan].intern[CALLED], call[chan].dialin, CALLED);

                      strcpy(call[chan].vnum[CALLED], vnum(chan, CALLED));
#ifdef Q931
                      if (q931dmp && (*call[chan].vnum[CALLED] != '?') && *call[chan].vorwahl[CALLED] && oc3 && ((oc3 & 0x70) != 0x40)) {
                        auto char s[BUFSIZ];

                      	sprintf(s, "%s %s/%s, %s",
                          call[chan].areacode[CALLED],
                          call[chan].vorwahl[CALLED],
                          call[chan].rufnummer[CALLED],
                          call[chan].area[CALLED]);

                        Q931dump(TYPE_STRING, -2, s, version);
                      } /* if */
#endif

                      if ((c = call[chan].confentry[OTHER]) > -1)
                        known[c]->rcharge = 0.0;

                      /* This message comes before bearer capability */
                      /* So dont show it here, show it at Bearer capability */

                      if (version != VERSION_1TR6) {
                        if (call[chan].knock)
                      	  info(chan, PRT_SHOWNUMBERS, STATE_RING, "********************");

                        sprintf(s, "RING (%s)", call[chan].service);
                      	info(chan, PRT_SHOWNUMBERS, STATE_RING, s);

                        if (call[chan].knock) {
                      	  info(chan, PRT_SHOWNUMBERS, STATE_RING, "NO FREE B-CHANNEL !!");
                      	  info(chan, PRT_SHOWNUMBERS, STATE_RING, "********************");
                        } /* if */

                      	if (sound)
                          ringer(chan, RING_RING);
                      } /* if */
                    } /* else */
                    break;


        case 0x74 : /* Redirecting number */
        case 0x76 : /* Redirection number */

                    oc3 = strtol(p += 3, NIL, 16);

                    pd = s;

                    while (--l)
                      *pd++ = strtol(p += 3, NIL, 16);

                    *pd = 0;

                    strcpy(call[chan].onum[REDIR], s);
                    buildnumber(s, oc3, -1, call[chan].num[REDIR], version, &call[chan].provider, &call[chan].sondernummer[REDIR], &call[chan].intern[REDIR], 0, 0);

                    strcpy(call[chan].vnum[REDIR], vnum(chan, REDIR));
#ifdef Q931
                    if (q931dmp && (*call[chan].vnum[REDIR] != '?') && *call[chan].vorwahl[REDIR] && oc3 && ((oc3 & 0x70) != 0x40)) {
                      auto char s[BUFSIZ];

                      sprintf(s, "%s %s/%s, %s",
                        call[chan].areacode[REDIR],
                        call[chan].vorwahl[REDIR],
                        call[chan].rufnummer[REDIR],
                        call[chan].area[REDIR]);

                      Q931dump(TYPE_STRING, -2, s, version);
                    } /* if */
#endif
                    break;


        case 0x01 : /* Bearer capability 1TR6 */
                    if (l > 0)
                      call[chan].bearer = strtol(p + 3, NIL, 16);
                    else
                      call[chan].bearer = 1; /* Analog */

                    px = s;
#ifdef Q931
                    if (!q931dmp)
#endif
                      px += sprintf(px, "RING (");

                    px += sprintf(px, "%s", qmsg(TYPE_SERVICE, version, call[chan].bearer));

#ifdef Q931
                    if (q931dmp) {
                      Q931dump(TYPE_STRING, call[chan].bearer, s, version);

                      if (l > 1) {
                        c = strtol(p + 6, NIL, 16);
                        sprintf(s1, "octet 3a=%d", c);
                        Q931dump(TYPE_STRING, c, s1, version);
                      } /* if */
                    } /* if */
#endif
                    px += sprintf(px, ")");
                    info(chan, PRT_SHOWNUMBERS, STATE_RING, s);

                    if (sound)
                      ringer(chan, RING_RING);

                    p += (l * 3);
                    break;


        case 0x04 : /* Bearer capability E-DSS1 */

                    clearchan(chan, 0);

                    pd = p + (l * 3);
                    l1 = l;

                    sxp = 0;

                    if (--l1 < 0) {
                      p = pd;
                      goto escape;
                    } /* if */

                    c = strtol(p += 3, NIL, 16);       /* Octet 3 */

                    px = sx[sxp];
                    *px = 0;
                    sn[sxp] = c;

#ifdef Q931
                    if (!q931dmp)
#endif
                        px += sprintf(px, "BEARER: ");

                    /* Mapping from E-DSS1 Bearer capability to 1TR6 Service Indicator: */

                    switch (c & 0x1f) {
                      case 0x00 : px += sprintf(px, "Speech");                           /* "CCITT Sprache" */
                                  call[chan].si1 = 1;
                                  call[chan].si11 = 1;
                                  strcpy(call[chan].service, "Speech");
                                  break;

                      case 0x08 : px += sprintf(px, "Unrestricted digital information"); /* "uneingeschr„nkte digitale Information" */
                                  call[chan].si1 = 7;
                                  call[chan].si11 = 0;
                                  strcpy(call[chan].service, "Data");
                                  break;

                      case 0x09 : px += sprintf(px, "Restricted digital information");  /* "eingeschr„nkte digitale Information" */
                                  call[chan].si1 = 2;
                                  call[chan].si11 = 0;
                                  strcpy(call[chan].service, "Fax G3");
                                  break;

                      case 0x10 : px += sprintf(px, "3.1 kHz audio");                   /* "3,1 kHz audio" */
                                  call[chan].si1 = 1;
                                  call[chan].si11 = 0;
                                  strcpy(call[chan].service, "3.1 kHz audio");
                                  break;

                      case 0x11 : px += sprintf(px, "Unrestricted digital information with tones/announcements"); /* "uneingeschr„nkte digitale Ton-Inform." */
                                  call[chan].si1 = 3;
                                  call[chan].si11 = 0;
                                  break;

                      case 0x18 : px += sprintf(px, "Video");                           /* "Video" */
                                  call[chan].si1 = 4;
                                  call[chan].si11 = 0;
                                  strcpy(call[chan].service, "Fax G4");
                                  break;

                        default : px += sprintf(px, "Service %d", c & 0x1f);
                                  sprintf(call[chan].service, "Service %d", c & 0x1f);
                                  break;
                    } /* switch */

                    switch (c & 0x60) {
                      case 0x00 : px += sprintf(px, ", CCITT standardized coding");        break;
                      case 0x20 : px += sprintf(px, ", ISO/IEC");                          break;
                      case 0x40 : px += sprintf(px, ", National standard");                break;
                      case 0x60 : px += sprintf(px, ", Standard defined for the network"); break;
                    } /* switch */

                    if (--l1 < 0) {
                      p = pd;
                      goto escape;
                    } /* if */

                    c = strtol(p += 3, NIL, 16);       /* Octet 4 */
                    px = sx[++sxp];
                    *px = 0;
                    sn[sxp] = c;

                    switch (c & 0x1f) {
                      case 0x10 : px += sprintf(px, "64 kbit/s");     break;
                      case 0x11 : px += sprintf(px, "2 * 64 kbit/s"); break;
                      case 0x13 : px += sprintf(px, "384 kbit/s");    break;
                      case 0x15 : px += sprintf(px, "1536 kbit/s");   break;
                      case 0x17 : px += sprintf(px, "1920 kbit/s");   break;

                      case 0x18 : oc3 = strtol(p += 3, NIL, 16); /* Octet 4.1 */
                                  px += sprintf(px, ", %d kbit/s", 64 * oc3 & 0x7f);
                                  break;

                    } /* switch */

                    switch (c & 0x60) {
                      case 0x00 : px += sprintf(px, ", Circuit mode"); break;
                      case 0x40 : px += sprintf(px, ", Packet mode");  break;
                    } /* switch */

                    if (--l1 < 0) {
                      p = pd;
                      goto escape;
                    } /* if */

                    c = strtol(p += 3, NIL, 16);

                    if ((c & 0x60) == 0x20) { /* User information layer 1 */
                      int ch = ' ';

                      do {
                        switch (ch) {
                          case ' ' : px = sx[++sxp]; /* Octet 5 */
                                     *px = 0;
                                     sn[sxp] = c;

                                     switch (c & 0x1f) {
                                       case 0x01 : px += sprintf(px, "CCITT standardized rate adaption V.110/X.30");               break;
                                       case 0x02 : px += sprintf(px, "G.711 u-law");                                               break;
                                       case 0x03 : px += sprintf(px, "G.711 A-law");                                               break;
                                       case 0x04 : px += sprintf(px, "G.721 32 kbit/s ADPCM (I.460)");                             break;
                                       case 0x05 : px += sprintf(px, "H.221/H.242");                                               break;
                                       case 0x07 : px += sprintf(px, "Non-CCITT standardized rate adaption");                      break;
                                       case 0x08 : px += sprintf(px, "CCITT standardized rate adaption V.120");                    break;
                                       case 0x09 : px += sprintf(px, "CCITT standardized rate adaption X.31, HDLC flag stuffing"); break;
                                     } /* switch */

                                     break;

                          case 'a' : px = sx[++sxp]; /* Octet 5a */
                                     *px = 0;
                                     sn[sxp] = c;

                                     switch (c & 0x1f) {
                                       case 0x01 : px += sprintf(px, "0.6 kbit/s");       break;
                                       case 0x02 : px += sprintf(px, "1.2 kbit/s");       break;
                                       case 0x03 : px += sprintf(px, "2.4 kbit/s");       break;
                                       case 0x04 : px += sprintf(px, "3.6 kbit/s");       break;
                                       case 0x05 : px += sprintf(px, "4.8 kbit/s");       break;
                                       case 0x06 : px += sprintf(px, "7.2 kbit/s");       break;
                                       case 0x07 : px += sprintf(px, "8 kbit/s");         break;
                                       case 0x08 : px += sprintf(px, "9.6 kbit/s");       break;
                                       case 0x09 : px += sprintf(px, "14.4 kbit/s");      break;
                                       case 0x0a : px += sprintf(px, "16 kbit/s");        break;
                                       case 0x0b : px += sprintf(px, "19.2 kbit/s");      break;
                                       case 0x0c : px += sprintf(px, "32 kbit/s");        break;
                                       case 0x0e : px += sprintf(px, "48 kbit/s");        break;
                                       case 0x0f : px += sprintf(px, "56 kbit/s");        break;
                                       case 0x15 : px += sprintf(px, "0.1345 kbit/s");    break;
                                       case 0x16 : px += sprintf(px, "0.100 kbit/s");     break;
                                       case 0x17 : px += sprintf(px, "0.075/1.2 kbit/s"); break;
                                       case 0x18 : px += sprintf(px, "1.2/0.075 kbit/s"); break;
                                       case 0x19 : px += sprintf(px, "0.050 kbit/s");     break;
                                       case 0x1a : px += sprintf(px, "0.075 kbit/s");     break;
                                       case 0x1b : px += sprintf(px, "0.110 kbit/s");     break;
                                       case 0x1c : px += sprintf(px, "0.150 kbit/s");     break;
                                       case 0x1d : px += sprintf(px, "0.200 kbit/s");     break;
                                       case 0x1e : px += sprintf(px, "0.300 kbit/s");     break;
                                       case 0x1f : px += sprintf(px, "12 kbit/s");        break;
                                     } /* switch */

                                     switch (c & 0x40) {
                                       case 0x00 : px += sprintf(px, ", Synchronous");    break;
                                       case 0x40 : px += sprintf(px, ", Asynchronous");   break;
                                     } /* switch */

                                     switch (c & 0x20) {
                                       case 0x00 : px += sprintf(px, ", In-band negotiation not possible"); break;
                                       case 0x20 : px += sprintf(px, ", In-band negotiation possible");     break;
                                     } /* switch */

                                     break;

                          case 'b' : px = sx[++sxp]; /* Octet 5b */
                                     *px = 0;
                                     sn[sxp] = c;

                                     switch (c & 0x60) {
                                       case 0x20 : px += sprintf(px, "8 kbit/s");  break;
                                       case 0x40 : px += sprintf(px, "16 kbit/s"); break;
                                       case 0x60 : px += sprintf(px, "32 kbit/s"); break;
                                     } /* switch */

                                     break;
                        } /* switch */

                        ch = (ch == ' ') ? 'a' : ch + 1;

                        if (--l1 < 0) {
                          p = pd;
                          goto escape;
                        } /* if */

                        c = strtol(p += 3, NIL, 16);
                      } while (!(c & 0x80));
                    } /* if */

                    if ((c & 0x60) == 0x40) { /* User information layer 2 */
                      px = sx[++sxp];
                      *px = 0;
                      sn[sxp] = c;

                      switch (c & 0x1f) {
                        case 0x02 : px += sprintf(px, "Q.931/I.441");        break;
                        case 0x06 : px += sprintf(px, "X.25, packet layer"); break;
                      } /* switch */

                      if (--l1 < 0) {
                        p = pd;
                        goto escape;
                      } /* if */

                      c = strtol(p += 3, NIL, 16);
                    } /* if */

                    if ((c & 0x60) == 0x60) { /* User information layer 3 */
                      px = sx[++sxp];
                      *px = 0;
                      sn[sxp] = c;

                      switch (c & 0x1f) {
                        case 0x02 : px += sprintf(px, "Q.931/I.451");        break;
                        case 0x06 : px += sprintf(px, "X.25, packet layer"); break;
                      } /* switch */

                    } /* if */

escape:             for (c = 0; c <= sxp; c++)
#ifdef Q931
                      if (q931dmp)
                        Q931dump(TYPE_STRING, sn[c], sx[c], version);
                      else
#endif
                      if (*sx[c])
                        info(chan, PRT_SHOWBEARER, STATE_RING, sx[c]);

                    p = pd;

                    break;


        case 0x18 : /* Channel identification */
                    c = strtol(p + 3, NIL, 16);

                    sxp = 0;
                    px = sx[sxp];
                    *px = 0;
                    sn[sxp] = c;

#ifdef Q931
                    if (!q931dmp)
#endif
                      px += sprintf(px, "CHANNEL: ");

                    switch (c) {
                      case 0x80 : px += sprintf(px, "BRI, none requested");
                                  call[chan].knock = 1;			  break;
                      case 0x81 : px += sprintf(px, "BRI, B1 requested"); break;
                      case 0x82 : px += sprintf(px, "BRI, B2 requested"); break;
                      case 0x83 : px += sprintf(px, "BRI, any channel");  break;
                      case 0x89 : px += sprintf(px, "BRI, B1 needed");    break;
                      case 0x8a : px += sprintf(px, "BRI, B2 needed");    break;
                      case 0x84 : px += sprintf(px, "BRI, D requested");  break;
                      case 0x8c : px += sprintf(px, "BRI, D needed");     break;
                      case 0xa0 : px += sprintf(px, "PRI, no channel");   break;
                      case 0xa1 : px += sprintf(px, "PRI, channel to be indicated later");
									  break;
                      case 0xa3 : px += sprintf(px, "PRI, indicated channel requested");
									  break;
                      case 0xa9 : px += sprintf(px, "PRI, indicated channel needed");
									  break;
                      case 0xac : px += sprintf(px, "PRI, D needed");     break;
                      case 0xe0 : px += sprintf(px, "no channel");        break;
                      case 0xe1 : px += sprintf(px, "channel to be indicated later");
									  break;
                      case 0xe3 : px += sprintf(px, "any channel");       break;
                      case 0xe9 : px += sprintf(px, "Nur der nachst. angegeb. Kanal ist akzeptabel"); break;
                    } /* switch */

#ifdef Q931
                    if (q931dmp)
                      Q931dump(TYPE_STRING, sn[0], sx[0], version);
                    else
#endif
                      info(chan, PRT_SHOWBEARER, STATE_RING, sx[0]);

                    if (c == 0x8a)
                      call[chan].channel = 2;
                      /* Jetzt eine 1 fuer Kanal 1 und 2 fuer die 2.
                         0 heisst unbekannt. chan muss dann spaeter
                         auf channel - 1 gesetzt werden.
                         Beim Parken bleibt der Kanal belegt (bei mir jedenfalls)
                         und neue Verbindungen kriegen vom Amt den anderen */
                    else if (c == 0x89)
                      call[chan].channel = 1;
                    else
                      call[chan].channel = 0;
                    p += (l * 3);
                    break;


        case 0x1e : /* Progress indicator */
                    sxp = 0;

                    px = sx[sxp];
                    *px = 0;

                    c = strtol(p + 3, NIL, 16);
                    sn[sxp] = c;

#ifdef Q931
                    if (!q931dmp)
#endif
                      px += sprintf(px, "PROGRESS: ");

                    switch (c) {
                      case 0x80 : px += sprintf(px, "Location: User");               break;
                      case 0x81 : px += sprintf(px, "Location: Local:private net");  break;
                      case 0x82 : px += sprintf(px, "Location: Local:public net");   break;
                      case 0x84 : px += sprintf(px, "Location: Remote:public net");  break;
                      case 0x85 : px += sprintf(px, "Location: Remote:private net"); break;
                      case 0x8a : px += sprintf(px, "Location: Interworking");       break;
                    } /* switch */

                    if (l > 1) {
                      px = sx[++sxp];
                      *px = 0;

#ifdef Q931
                      if (!q931dmp)
#endif
                        px += sprintf(px, "PROGRESS: ");

                      c = strtol(p + 6, NIL, 16);
                      sn[sxp] = c;

                      switch (c) {
                        case 0x81 : px += sprintf(px, "Der Ruf verlaeuft nicht vom Anfang bis zum Ende im ISDN"); break;
                        case 0x82 : px += sprintf(px, "Zieladresse ist kein ISDN-Anschluss");                     break;
                        case 0x83 : px += sprintf(px, "(Ab)Sendeadresse ist kein ISDN-Anschluss");                break;
                        case 0x84 : px += sprintf(px, "Ruf ist zum ISDN zurueckgekehrt");                         break;
                        case 0x88 : px += sprintf(px, "Inband Information available");                            break;
                      } /* switch */
                    } /* if */

                    for (c = 0; c <= sxp; c++)
#ifdef Q931
                      if (q931dmp)
                        Q931dump(TYPE_STRING, sn[c], sx[c], version);
                      else
#endif
                      if (*sx[c])
                        info(chan, PRT_SHOWBEARER, STATE_RING, sx[c]);

                    p += (l * 3);
                    break;


        case 0x27 : /* Notification indicator */
                    sxp = 0;
                    px = sx[sxp];
                    *px = 0;

                    c = strtol(p + 3, NIL, 16);
                    sn[sxp] = c;

#ifdef Q931
                    if (!q931dmp)
#endif
                      px += sprintf(px, "NOTIFICATION: ");

                    switch (c) {
                      case 0x80 : px += sprintf(px, "Nutzer legt auf");                                        break;
                      case 0x81 : px += sprintf(px, "Nutzer nimmt wieder auf");                                break;
                      case 0x82 : px += sprintf(px, "Wechsel des Uebermittlungsdienstes");                     break;
                      case 0x83 : px += sprintf(px, "Discriminator for extension to ASN.1 encoded component"); break;
                      case 0x84 : px += sprintf(px, "Call completion delay");                                  break;
                      case 0xc2 : px += sprintf(px, "Conference established");                                 break;
                      case 0xc3 : px += sprintf(px, "Conference disconnected");                                break;
                      case 0xc4 : px += sprintf(px, "Other party added");                                      break;
                      case 0xc5 : px += sprintf(px, "Isolated");                                               break;
                      case 0xc6 : px += sprintf(px, "Reattached");                                             break;
                      case 0xc7 : px += sprintf(px, "Other party isolated");                                   break;
                      case 0xc8 : px += sprintf(px, "Other party reattached");                                 break;
                      case 0xc9 : px += sprintf(px, "Other party split");                                      break;
                      case 0xca : px += sprintf(px, "Other party disconnected");                               break;
                      case 0xcb : px += sprintf(px, "Conference floating");                                    break;
                      case 0xcf : px += sprintf(px, "Conference floating, served user preemted");              break;
                      case 0xcc : px += sprintf(px, "Conference disconnected, preemtion");                     break;
                      case 0xf9 : px += sprintf(px, "Remote hold");                                            break;
                      case 0xfa : px += sprintf(px, "Remote retrieval");                                       break;
                      case 0xe0 : px += sprintf(px, "Call is a waiting call");                                 break;
                      case 0xfb : px += sprintf(px, "Call is diverting");                                      break;
                      case 0xe8 : px += sprintf(px, "Diversion activated");                                    break;
                      case 0xee : px += sprintf(px, "Reverse charging");                                       break;
                    } /* switch */

#ifdef Q931
                    if (q931dmp)
                      Q931dump(TYPE_STRING, sn[0], sx[0], version);
                    else
#endif
                      info(chan, PRT_SHOWNUMBERS, STATE_RING, sx[0]);

                    p += (l * 3);
                    break;


        case 0x7d : /* High layer compatibility */
                    if (l > 1) {
                      sxp = 0;
                      px = sx[sxp];
                      *px = 0;

                      c = strtol(p + 3, NIL, 16);
                      sn[sxp] = c;

#ifdef Q931
                      if (!q931dmp)
#endif
                        px += sprintf(px, "HLC: ");

                      switch (c) {
                        case 0x91 : px += sprintf(px, "CCITT");    break;
                        case 0xb1 : px += sprintf(px, "Reserv.");  break;
                        case 0xd1 : px += sprintf(px, "national"); break;
                        case 0xf1 : px += sprintf(px, "Eigendef"); break;
                      } /* switch */

                      px = sx[++sxp];
                      *px = 0;

#ifdef Q931
                      if (!q931dmp)
#endif
                        px += sprintf(px, "HLC: ");

                      c = strtol(p + 6, NIL, 16);
                      sn[sxp] = c;

                      switch (c) {
                        case 0x81 : px += sprintf(px, "Telefonie");                                        break;
                        case 0x84 : px += sprintf(px, "Fax Gr.2/3 (F.182)");                               break;
                        case 0xa1 : px += sprintf(px, "Fax Gr.4 (F.184)");                                 break;
                        case 0xa4 : px += sprintf(px, "Teletex service,basic and mixed-mode");             break;
                        case 0xa8 : px += sprintf(px, "Teletex service,basic and processab.-mode of Op."); break;
                        case 0xb1 : px += sprintf(px, "Teletex service,basic mode of operation");          break;
                        case 0xb2 : px += sprintf(px, "Syntax based Videotex");                            break;
                        case 0xb3 : px += sprintf(px, "International Videotex interworking via gateway");  break;
                        case 0xb5 : px += sprintf(px, "Telex service");                                    break;
                        case 0xb8 : px += sprintf(px, "Message Handling Systems (MHS)(X.400)");            break;
                        case 0xc1 : px += sprintf(px, "OSI application (X.200)");                          break;
                        case 0xde :
                        case 0x5e : px += sprintf(px, "Reserviert fuer Wartung");                          break;
                        case 0xdf :
                        case 0x5f : px += sprintf(px, "Reserviert fuer Management");                       break;
                        case 0xe0 : px += sprintf(px, "Audio visual");                                     break;
                          default : px += sprintf(px, "unknown: %d", c);                                   break;
                      } /* switch */

                      if ((c == 0x5e) || (c == 0x5f)) {
                        px = sx[++sxp];
                        *px = 0;

#ifdef Q931
                        if (!q931dmp)
#endif
                          px += sprintf(px, "HLC: ");

                        c = strtol(p + 9, NIL, 16);
                        sn[sxp] = c;

                        switch (c) {
                          case 0x81 : px += sprintf(px, "Telefonie G.711");                                                  break;
                          case 0x84 : px += sprintf(px, "Fax Gr.4 (T.62)");                                                  break;
                          case 0xa1 : px += sprintf(px, "Document Appl. Profile for Fax Gr4 (T.503)");                       break;
                          case 0xa4 : px += sprintf(px, "Doc.Appl.Prof.for formatted Mixed-Mode(T501)");                     break;
                          case 0xa8 : px += sprintf(px, "Doc.Appl.Prof.for Processable-form (T.502)");                       break;
                          case 0xb1 : px += sprintf(px, "Teletex (T.62)");                                                   break;
                          case 0xb2 : px += sprintf(px, "Doc.App.Prof. for Videotex interworking between Gateways (T.504)"); break;
                          case 0xb5 : px += sprintf(px, "Telex");                                                            break;
                          case 0xb8 : px += sprintf(px, "Message Handling Systems (MHS)(X.400)");                            break;
                          case 0xc1 : px += sprintf(px, "OSI application (X.200)");                                          break;
                        } /* case */
                      } /* if */

                      for (c = 0; c <= sxp; c++)
#ifdef Q931
                        if (q931dmp)
                          Q931dump(TYPE_STRING, sn[c], sx[c], version);
                        else
#endif
                        if (*sx[c])
                          info(chan, PRT_SHOWNUMBERS, STATE_RING, sx[c]);

                    } /* if */

                    p += (l * 3);
                    break;


        default   : {
                      register char *p1, *p2;
                      register int  i;
UNKNOWN_ELEMENT:      p1 = p; p2 = s;

                      for (i = 0; i < l; i++)
                        p2 += sprintf(p2, "%02x ", (int)strtol(p1 += 3, NIL, 16));

                      p2 += sprintf(p2, "\"");
                      p1 = p;

                      for (i = 0; i < l; i++) {
                        c = (int)strtol(p1 += 3, NIL, 16);
                        p2 += sprintf(p2, "%c", isgraph(c) ? c : ' ');
                      } /* for */

                      p2 += sprintf(p2, "\"");

                      if (allflags & PRT_DEBUG_DECODE)
                        print_msg(PRT_DEBUG_DECODE, " DEBUG> %s: ELEMENT=0x%02x :%s\n", st + 4, element, s);
#ifdef Q931
                      if (q931dmp)
                        Q931dump(TYPE_STRING, -4, s, version);
#endif
                    } /* if */

                    p += (l * 3);
                    break;
      } /* switch */

    }
#ifdef Q931
    else if (q931dmp) {
      if (version == VERSION_1TR6) {
        switch ((element >> 4) & 7) {
          case 1 : sprintf(s, "%02x ---> Shift %d (cs=%d, cs_fest=%d)", element, element & 0xf, element & 7, element & 8);
                   break;

          case 3 : sprintf(s, "%02x ---> Congestion level %d", element, element & 0xf);
                   break;

          case 2 : if (element == 0xa0)
                     sprintf(s, "%02x ---> More data", element);
                   else if (element == 0xa1)
                     sprintf(s, "%02x ---> Sending complete", element);
                   break;

         default : sprintf(s, "%02x ---> Reserved %d", element, element);
                   break;
        } /* switch */

        Q931dump(TYPE_STRING, -3, s, version);
      }
      else if (version == VERSION_EDSS1) {
        switch ((element >> 4) & 7) {
          case 1 : sprintf(s, "%02x ---> Shift %d", element, element & 0xf);
                   break;

          case 3 : sprintf(s, "%02x ---> Congestion level %d", element, element & 0xf);
                   break;

          case 5 : sprintf(s, "%02x ---> Repeat indicator %d", element, element & 0xf);
                   break;

          case 2 : if (element == 0x90)
                     sprintf(s, "%02x ---> Umschaltung in eine andere Codegruppe %d\n", element, element);
                   if (element == 0xa0)
                     sprintf(s, "%02x ---> More data", element);
                   else if (element == 0xa1)
                     sprintf(s, "%02x ---> Sending complete", element);
                   break;

         default : sprintf(s, "%02x ---> Reserved %d\n", element, element);
                   break;
        } /* switch */

        Q931dump(TYPE_STRING, -3, s, version);
      } /* else */
    } /* else */
#endif
  } /* while */
} /* decode */

/* --------------------------------------------------------------------------
   call reference:


    1 ..  63 DIALIN's    - ist dabei 8. Bit gesetzt, meine Antwort an VSt
                           (cref=1 : VSt->User // cref=129 : User->VSt)

   64 .. 127 DIALOUT's   - ist dabei 8. Bit gesetzt, Antwort der VSt an mich
                           (cref=64 : User->VSt // cref=192 : VSt->User)

   kommt ein SETUP ACKNOWLEDGE mit cref > 128, beginnt ein DIALOUT (!)
   _nicht_ mit der Teles-Karte

   kommt ein CALL PROCEEDING mit cref > 191, beginnt ein DIALOUT
   mit der 2. Teles-Karte

   folgt danach sofort ein SETUP, ist das ein Selbstanruf!


   DIALOUT's erhalten vom Teles-Treiber staendig eine um jeweils 1
   erhoehte call references
   War die letzte cref also < 127, und die naechste = 64, bedeutet dies
   einen Reload des Teles-Treibers!
-------------------------------------------------------------------------- */


void dotrace(void)
{
  register int  i;
  auto     char s[BUFSIZ];


  print_msg(PRT_NORMAL, ">>>>>>> TRACE (CR=next, q=quit, d=dump, g=go):");
  fgets(s, BUFSIZ, stdin);

  if (*s == 'q')
    exit(0);
  else if (*s == 'g')
    trace = 0;
  else if (*s == 'd') {

    print_msg(PRT_NORMAL, "chan=%d\n", chan);

    for (i = 0; i < MAXCHAN; i++) {
      if (call[i].state) {
        print_msg(PRT_NORMAL, "call[%d]:", i);
        print_msg(PRT_NORMAL, "state=%d, cref=%d, dialin=%d, cause=%d\n",
          call[i].state, call[i].cref, call[i].dialin, call[i].cause);
        print_msg(PRT_NORMAL, "\taoce=%d, channel=%d, dialog=%d, bearer=%d\n",
          call[i].aoce, call[i].channel, call[i].dialog, call[i].bearer);
        print_msg(PRT_NORMAL, "\tnum[0]=\"%s\", num[1]=\"%s\"\n",
          call[i].num[0], call[i].num[1]);
        print_msg(PRT_NORMAL, "\tvnum[0]=\"%s\", vnum[1]=\"%s\"\n",
          call[i].vnum[0], call[i].vnum[1]);
        print_msg(PRT_NORMAL, "\tconfentry[0]=%d,  confentry[1]=%d\n",
          call[i].confentry[0], call[i].confentry[1]);
        print_msg(PRT_NORMAL, "\ttime=%d, connect=%d, disconnect=%d, duration=%d\n",
          (int)call[i].time, (int)call[i].connect, (int)call[i].disconnect, (int)call[i].duration);
      } /* if */
    } /* for */

    dotrace();

  } /* if */
} /* dotrace */


static int b2c(register int b)
{
  register int i;


  for (i = 0; i < chans; i++)
    if ((call[i].bchan == b) && call[i].dialog)
      return(i);

  return(-1);
} /* b2c */


/* NET_DV since 'chargeint' field exists */
#define	NETDV_CHARGEINT		0x02

static void huptime(int chan, int bchan, int setup)
{
  register int                c = call[chan].confentry[OTHER];
  auto     isdn_net_ioctl_cfg cfg;
  auto     int                oldchargeint = 0, newchargeint = 0;
  auto     int                oldhuptimeout, newhuptimeout;
  auto     char               sx[BUFSIZ], why[BUFSIZ];
#if LCR
  auto	   char		      n[1024], n1[1024];
  auto	   union 	      p {
                	        isdn_net_ioctl_phone phone;
                		char n[1024];
  			      } ph;
#endif


  if (hupctrl && (c > -1) && (*known[c]->interface > '@') && expensive(bchan)) {
    memset(&cfg, 0, sizeof(cfg)); /* clear in case of older kernel */

    strcpy(cfg.name, known[c]->interface);

    if (ioctl(sockets[ISDNCTRL].descriptor, IIOCNETGCF, &cfg) >= 0) {
#if NET_DV >= NETDV_CHARGEINT
      if (net_dv >= NETDV_CHARGEINT)
        call[chan].chargeint = oldchargeint = cfg.chargeint;
#endif
      call[chan].huptimeout = oldhuptimeout = cfg.onhtime;

#if LCR
      if (setup) {
        strcpy(ph.phone.name, known[c]->interface);
        ph.phone.outgoing = 1;

        if (ioctl(sockets[ISDNCTRL].descriptor, IIOCNETGNM, &ph.phone) >= 0) {
          strcpy(n, ph.n);
          sprintf(sx, "@LCR: SETUP to %s detected", n);
          info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);

          if (memcmp(n, "010", 3) {
            sprintf(sx, "@LCR: HANGUP %s", known[c]->interface);
            info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);

            if (ioctl(sockets[ISDNCTRL].descriptor, IIOCNETHUP, known[c]->interface) >= 0) { /* HANGUP */
	      ph.phone.outgoing = 1;
	      strcpy(ph.phone.name, known[c]->interface);
	      strcpy(ph.phone.phone, n);

              sprintf(sx, "@LCR: DELPHONE %s", n);
              info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);

	      if (ioctl(sockets[ISDNCTRL].descriptor, IIOCNETDNM, &ph.phone) >= 0) { /* DELPHONE */
                sprintf(n1, "01019%s", n);
	        ph.phone.outgoing = 1;
	        strcpy(ph.phone.name, known[c]->interface);
	        strcpy(ph.phone.phone, n1);

                sprintf(sx, "@LCR: ADDPHONE %s", n1);
                info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);

	        if (ioctl(sockets[ISDNCTRL].descriptor, IIOCNETANM, &ph.phone) >= 0) { /* ADDPHONE */

                  sprintf(sx, "@LCR: DIAL");
            	  info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);

		  if (ioctl(sockets[ISDNCTRL].descriptor, IIOCNETDIL, known[c]->interface) >= 0) { /* DIAL */
                    sprintf(sx, "@LCR: DONE!");
            	    info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);
                  } /* if */
	        } /* if */
	      } /* if */
	    } /* if */
          } /* if */
        } /* if */
      } /* if */
#endif

      if (!oldhuptimeout) {
        sprintf(sx, "HUPTIMEOUT %s is *disabled* - unchanged", known[c]->interface);
        info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);
      	return;
      } /* if */

      newchargeint = taktlaenge(chan, why);

#ifdef ISDN_DE
      if (call[chan].provider == 19) { /* Mobilcom 60/60 Takt */
        newchargeint = 60;
      	sprintf(why, "via %s", Providername(call[chan].provider));
      }
      else if (call[chan].provider == 24) { /* TelePasswort 1s Takt */
        newchargeint = 1;
      	sprintf(why, "via %s", Providername(call[chan].provider));
      }
      else if (call[chan].provider == 70) { /* Arcor 1s Takt */
        newchargeint = 1;
      	sprintf(why, "via %s", Providername(call[chan].provider));
      }
      else if (call[chan].provider == 30) { /* TelDaFax 1s Takt */
        newchargeint = 1;
      	sprintf(why, "via %s", Providername(call[chan].provider));
      }
      else if (call[chan].provider == 13) { /* Tele 2 1s Takt */
        newchargeint = 1;
      	sprintf(why, "via %s", Providername(call[chan].provider));
      }
      else if (call[chan].provider == 24) { /* TelePassport 1/1 Takt */
        newchargeint = 1;
      	sprintf(why, "via %s", Providername(call[chan].provider));
      } /* else */
#endif

#if NET_DV >= NETDV_CHARGEINT
      if (net_dv >= NETDV_CHARGEINT) {
        if (hup1 && hup2)
          newhuptimeout = (newchargeint < 20) ? hup1 : hup2;
        else
          newhuptimeout = oldhuptimeout;
      }
      else
#endif
        /* for old kernels/kernel headers use old behaviour: hangup is charge
         * time minus -h param */
        if (hup1) {
          newhuptimeout = newchargeint - hup1;
          oldchargeint = newchargeint;
        }
        else
          newhuptimeout = oldhuptimeout;

      if (oldchargeint != newchargeint || oldhuptimeout != newhuptimeout) {
#if NET_DV >= NETDV_CHARGEINT
        if (net_dv >= NETDV_CHARGEINT)
          call[chan].chargeint = cfg.chargeint = newchargeint;
#endif
        call[chan].huptimeout = cfg.onhtime = newhuptimeout;

        if (ioctl(sockets[ISDNCTRL].descriptor, IIOCNETSCF, &cfg) >= 0) {
          sprintf(sx, "CHARGEINT %s %d (was %d) - %s",
            known[c]->interface, newchargeint, oldchargeint, why);

          info(chan, PRT_INFO, STATE_HUPTIMEOUT, sx);

            sprintf(sx, "HUPTIMEOUT %s %d (was %d)",
              known[c]->interface, newhuptimeout, oldhuptimeout);

            info(chan, PRT_INFO, STATE_HUPTIMEOUT, sx);
          } /* if */
      }
      else {
        sprintf(sx, "CHARGEINT %s still %d - %s", known[c]->interface,
          oldchargeint, why);
        info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);

        sprintf(sx, "HUPTIMEOUT %s still %d", known[c]->interface, oldhuptimeout);
        info(chan, PRT_SHOWNUMBERS, STATE_HUPTIMEOUT, sx);
      } /* else */
    } /* if */
  } /* if */
} /* huptime */


static void oops(int where)
{
  auto   char              s[BUFSIZ];
  auto   isdn_ioctl_struct ioctl_s;
  auto   int               cmd;
  static int               loop = 0;


  if (!replay) {
    strcpy(ioctl_s.drvid, ifo[0].id);
    ioctl_s.arg = 4;
    cmd = 1;

    if ((++loop == 2) || (ioctl(sockets[ISDNCTRL].descriptor, IIOCDRVCTL + cmd, &ioctl_s) < 0)) {
      info(0, PRT_ERR, STATE_AOCD, "FATAL: Please enable D-Channel logging with:");
      sprintf(s, "FATAL: \"hisaxctrl %s 1 4\"", ifo[0].id);
      info(0, PRT_ERR, STATE_AOCD, s);
      sprintf(s, "FATAL: and restart isdnlog! (#%d)\007", where);
      info(0, PRT_ERR, STATE_AOCD, s);

      Exit(34);
    } /* if */
  } /* if */

  sprintf(s, "WARNING \"hisaxctrl %s 1 4\" called! (#%d)", ifo[0].id, where);
  info(0, PRT_ERR, STATE_AOCD, s);

} /* if */


static void processbytes()
{
  register int     bchan, chan, change = 0;
  auto     char    sx[BUFSIZ], sy[BUFSIZ];
  auto     time_t  DiffTime = (time_t)0;
  auto     int     hup = 0, eh = 0;
#if SHOWTICKS
  auto     double  tack;
#endif
#if RATE_PER_SAMPLE
  auto     double  DiffTime2;
#endif


  for (bchan = 0; bchan < chans; bchan++)
    if (((ifo[bchan].u & ISDN_USAGE_MASK) == ISDN_USAGE_NET) ||
        ((ifo[bchan].u & ISDN_USAGE_MASK) == ISDN_USAGE_MODEM)) {

#if FUTURE
      if (!hexSeen)
        oops(1);
#endif

      if ((chan = b2c(bchan)) != -1) {

        *sy = 0;

        if (io[bchan].i > call[chan].ibytes) {
          call[chan].libytes = call[chan].ibytes;
          call[chan].ibytes = io[bchan].i;
          change++;
        } /* if */

        if (io[bchan].o > call[chan].obytes) {
          call[chan].lobytes = call[chan].obytes;
          call[chan].obytes = io[bchan].o;
          change++;
        } /* if */

        if (change) {
          call[chan].traffic = call[chan].aoce;

          if (!hexSeen)
            oops(3);
        } /* if */

        if (fullhour) /* zu jeder vollen Stunde HANGUP-Timer neu setzen (aendern sich um: 9:00, 12:00, 18:00, 21:00, 2:00, 5:00 Uhr) */
          huptime(chan, bchan, 0);

        DiffTime = cur_time - call[chan].connect;

        if (call[chan].chargeint && DiffTime) {
          hup = (int)(call[chan].chargeint - (DiffTime % call[chan].chargeint) - 2);

          if (hup < 0)
            hup = 0;

          eh = (DiffTime / (time_t)call[chan].chargeint) + 1;

          if (ifo[bchan].u & ISDN_USAGE_OUTGOING)
            sprintf(sy, "  H#%d=%3ds", eh, hup);
        } /* if */

        if (DiffTime) {
          call[chan].ibps = (double)(call[chan].ibytes / (double)(DiffTime));
          call[chan].obps = (double)(call[chan].obytes / (double)(DiffTime));
        }
        else
          call[chan].ibps = call[chan].obps = 0.0;

        if (change && (call[chan].ibytes + call[chan].obytes)) {

          sprintf(sx, "I=%s %s/s  O=%s %s/s%s",
            double2byte((double)call[chan].ibytes),
            double2byte((double)call[chan].ibps),
            double2byte((double)call[chan].obytes),
            double2byte((double)call[chan].obps),
            sy);
#if SHOWTICKS
          if ((message & PRT_SHOWTICKS) && (tack = call[chan].tick - (double)(cur_time - call[chan].connect)) > 0.0)
            sprintf(sy, " C=%s", double2clock(tack) + 4);
          else
            *sy = 0;

          sprintf(sx, "I=%s %s/s  O=%s %s/s%s",
            double2byte((double)call[chan].ibytes),
            double2byte((double)call[chan].ibps),
            double2byte((double)call[chan].obytes),
            double2byte((double)call[chan].obps),
            sy);
#endif

          info(chan, PRT_SHOWBYTE, STATE_BYTE, sx);

#if RATE_PER_SAMPLE
          if ((DiffTime2 = ((double)(tt - tto) / (double)CLK_TCK))) {
            auto long   ibytes = call[chan].ibytes - call[chan].libytes;
            auto long   obytes = call[chan].obytes - call[chan].lobytes;
            auto double ibps = (double)ibytes / (double)DiffTime2;
            auto double obps = (double)obytes / (double)DiffTime2;


            sprintf(sx, "I=%s %s/s  O=%s %s/s (%4.4gs)",
              double2byte(ibytes),
              double2byte(ibps),
              double2byte(obytes),
              double2byte(obps),
              (double)DiffTime2);

            info(chan, PRT_SHOWBYTE, STATE_BYTE, sx);
          } /* if */
#endif
        }
        else if (DiffTime) {
          sprintf(sx, "I=%s %s/s  O=%s %s/s%s",
            double2byte((double)call[chan].ibytes),
            double2byte((double)call[chan].ibps),
            double2byte((double)call[chan].obytes),
            double2byte((double)call[chan].obps),
            sy);

            info(chan, PRT_SHOWBYTE, STATE_BYTE, sx);
        } /* else */
      } /* if */
    } /* if */
} /* processbytes */


static void processinfo(char *s)
{
  register char *p;
  register int   j, k, chan, version;
  auto     char  sx[BUFSIZ];


  if (verbose & VERBOSE_INFO)
    print_msg(PRT_LOG, "%s\n", s);


  if (!memcmp(s, "idmap:", 6)) {
    j = sscanf(s + 7, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
      ifo[ 0].id, ifo[ 1].id, ifo[ 2].id, ifo[ 3].id,
      ifo[ 4].id, ifo[ 5].id, ifo[ 6].id, ifo[ 7].id,
      ifo[ 8].id, ifo[ 9].id, ifo[10].id, ifo[11].id,
      ifo[12].id, ifo[13].id, ifo[14].id, ifo[15].id, ifo[16].id);

    if (!newcps && (j == 17)) {
      newcps = 1;

      for (chans = 0; chans < 17; chans++)
        if (!strcmp(ifo[chans].id, "-"))
          break;

#ifdef Q931
      if (!q931dmp) {
#endif
        print_msg(PRT_NORMAL, "(ISDN subsystem with ISDN_MAX_CHANNELS > 16 detected - %d active channels, %d MSN/SI entries)\n", chans, mymsns);
        if (dual)
          print_msg(PRT_NORMAL, "(watching \"%s\" and \"%s\")\n", isdnctrl, isdnctrl2);
#ifdef Q931
      } /* if */
#endif

      /*
       * Ab "ISDN subsystem Rev: 1.21/1.20/1.14/1.10/1.6" gibt's den ioctl(IIOCGETDVR)
       *
       * Letzte Version davor war "ISDN subsystem Rev: 1.18/1.18/1.13/1.9/1.6"
       */

      if (!replay)
        if ((version = ioctl(sockets[ISDNINFO].descriptor, IIOCGETDVR)) != -EINVAL) {
#ifdef NET_DV
          int my_net_dv = NET_DV;
#else
          int my_net_dv = 0;
#endif

          tty_dv = version & 0xff;
          version = version >> 8;
          net_dv = version & 0xff;
          version = version >> 8;
          inf_dv = version & 0xff;

          print_msg(PRT_NORMAL, "(Data versions: iprofd=0x%02x  net_cfg=0x%02x  /dev/isdninfo=0x%02x)\n", tty_dv, net_dv, inf_dv);
          if (/* Abort if kernel version is greater, since struct has probably
               * become larger and would overwrite our stack */
              net_dv > my_net_dv ||
              /* version 0x03 is special, because it changed a field in the
               * middle of the struct and thus is compatible only to itself */
              ((my_net_dv == 0x03 || net_dv == 0x03) && my_net_dv != net_dv)) {
            print_msg(PRT_ERR, "isdn_net_ioctl_cfg version mismatch "
                      "(kernel 0x%02x, isdnlog 0x%02x)\n",
                      net_dv, my_net_dv);
            Exit(99);
          }
        } /* if */

      if (chans > 2) /* coming soon ;-) */
        chans = 2;
    } /* if */
  }
  else if (!memcmp(s, "chmap:", 6))
    sscanf(s + 7, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      &ifo[ 0].ch, &ifo[ 1].ch, &ifo[ 2].ch, &ifo[ 3].ch,
      &ifo[ 4].ch, &ifo[ 5].ch, &ifo[ 6].ch, &ifo[ 7].ch,
      &ifo[ 8].ch, &ifo[ 9].ch, &ifo[10].ch, &ifo[11].ch,
      &ifo[12].ch, &ifo[13].ch, &ifo[14].ch, &ifo[15].ch);
  else if (!memcmp(s, "drmap:", 6))
    sscanf(s + 7, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      &ifo[ 0].dr, &ifo[ 1].dr, &ifo[ 2].dr, &ifo[ 3].dr,
      &ifo[ 4].dr, &ifo[ 5].dr, &ifo[ 6].dr, &ifo[ 7].dr,
      &ifo[ 8].dr, &ifo[ 9].dr, &ifo[10].dr, &ifo[11].dr,
      &ifo[12].dr, &ifo[13].dr, &ifo[14].dr, &ifo[15].dr);
  else if (!memcmp(s, "usage:", 6))
    sscanf(s + 7, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      &ifo[ 0].u, &ifo[ 1].u, &ifo[ 2].u, &ifo[ 3].u,
      &ifo[ 4].u, &ifo[ 5].u, &ifo[ 6].u, &ifo[ 7].u,
      &ifo[ 8].u, &ifo[ 9].u, &ifo[10].u, &ifo[11].u,
      &ifo[12].u, &ifo[13].u, &ifo[14].u, &ifo[15].u);
  else if (!memcmp(s, "flags:", 6))
    sscanf(s + 7, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      &ifo[ 0].f, &ifo[ 1].f, &ifo[ 2].f, &ifo[ 3].f,
      &ifo[ 4].f, &ifo[ 5].f, &ifo[ 6].f, &ifo[ 7].f,
      &ifo[ 8].f, &ifo[ 9].f, &ifo[10].f, &ifo[11].f,
      &ifo[12].f, &ifo[13].f, &ifo[14].f, &ifo[15].f);
  else if (!memcmp(s, "phone:", 6)) {
    sscanf(s + 7, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
      ifo[ 0].n, ifo[ 1].n, ifo[ 2].n, ifo[ 3].n,
      ifo[ 4].n, ifo[ 5].n, ifo[ 6].n, ifo[ 7].n,
      ifo[ 8].n, ifo[ 9].n, ifo[10].n, ifo[11].n,
      ifo[12].n, ifo[13].n, ifo[14].n, ifo[15].n);

    for (j = 0; j < chans; j++)
      if (ifo[j].u & ISDN_USAGE_MASK) {

#if FUTURE
        if (!hexSeen)
          oops(2);
#endif

        for (chan = 0; chan < MAXCHAN; chan++)
          if (memcmp(ifo[j].n, "???", 3) && !strcmp(ifo[j].n, call[chan].onum[OTHER])) {
            call[chan].bchan = j;

            strcpy(call[chan].id, ifo[j].id);

            if (!(ifo[j].u & ISDN_USAGE_MASK)) /* no connection */
              strcpy(call[chan].usage, (ifo[j].u & ISDN_USAGE_EXCLUSIVE) ? "Exclusive" : "Offline");
            else {
              switch (ifo[j].u & ISDN_USAGE_MASK) {
                case ISDN_USAGE_RAW   : sprintf(call[chan].usage, "%s %s", (ifo[j].u & ISDN_USAGE_OUTGOING) ? "Outgoing" : "Incoming", "Raw");
                                        break;

                case ISDN_USAGE_MODEM : sprintf(call[chan].usage, "%s %s", (ifo[j].u & ISDN_USAGE_OUTGOING) ? "Outgoing" : "Incoming", "Modem");
                                        break;

                case ISDN_USAGE_NET   : sprintf(call[chan].usage, "%s %s", (ifo[j].u & ISDN_USAGE_OUTGOING) ? "Outgoing" : "Incoming", "Net");
                                        break;

                case ISDN_USAGE_VOICE : sprintf(call[chan].usage, "%s %s", (ifo[j].u & ISDN_USAGE_OUTGOING) ? "Outgoing" : "Incoming", "Voice");
                                        break;

                case ISDN_USAGE_FAX   : sprintf(call[chan].usage, "%s %s", (ifo[j].u & ISDN_USAGE_OUTGOING) ? "Outgoing" : "Incoming", "Fax");
                                        break;
              } /* switch */
            } /* else */

            huptime(chan, j, 1); /* bei Verbindungsbeginn HANGUP-Timer neu setzen */
          } /* if */
      } /* if */

    if (imon) {
      print_msg(PRT_SHOWIMON, "\n+ %s -----------------------------------------\n", st + 4);

      for (j = 0; j < chans; j++) {

        p = sx;

        p += sprintf(p, "| %s#%d : ", ifo[j].id, j & 1);

        if (!(ifo[j].u & ISDN_USAGE_MASK)) /* no connection */
          p += sprintf(p, (ifo[j].u & ISDN_USAGE_EXCLUSIVE) ? "exclusive" : "free");
        else {
          p += sprintf(p, "%s\t", (ifo[j].u & ISDN_USAGE_OUTGOING) ? "outgoing" : "incoming");

          switch (ifo[j].u & ISDN_USAGE_MASK) {
            case ISDN_USAGE_RAW   : p += sprintf(p, "raw device");
                                    break;

            case ISDN_USAGE_MODEM : p += sprintf(p, "tty emulation");
                                    break;

            case ISDN_USAGE_NET   : p += sprintf(p, "IP interface");
                                    break;

            case ISDN_USAGE_VOICE : p += sprintf(p, "Voice");
                                    break;

            case ISDN_USAGE_FAX   : p += sprintf(p, "Fax");
                                    break;
          } /* switch */

          p += sprintf(p, "\t%s", ifo[j].n);

          if ((chan = b2c(j)) != -1) {
            k = call[chan].dialin ? CALLING : CALLED;

            p += sprintf(p, " (%s/%s, %s)",
                   call[chan].vorwahl[k],
                   call[chan].rufnummer[k],
                   call[chan].area[k]);

          } /* if */
        } /* else */

        print_msg(PRT_SHOWIMON, "%s\n", sx);

      } /* for */
    } /* if */
  }
  else if (!memcmp(s, "ibytes:", 7))
    sscanf(s + 8, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
      &io[ 0].i, &io[ 1].i, &io[ 2].i, &io[ 3].i,
      &io[ 4].i, &io[ 5].i, &io[ 6].i, &io[ 7].i,
      &io[ 8].i, &io[ 9].i, &io[10].i, &io[11].i,
      &io[12].i, &io[13].i, &io[14].i, &io[15].i);
  else if (!memcmp(s, "obytes:", 7)) {
    sscanf(s + 8, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
      &io[ 0].o, &io[ 1].o, &io[ 2].o, &io[ 3].o,
      &io[ 4].o, &io[ 5].o, &io[ 6].o, &io[ 7].o,
      &io[ 8].o, &io[ 9].o, &io[10].o, &io[11].o,
      &io[12].o, &io[13].o, &io[14].o, &io[15].o);

    processbytes();
  } /* else */
} /* processinfo */


void clearchan(int chan, int total)
{
  register int i;


  if (total) {
    memset((char *)&call[chan], 0, sizeof(CALL));
    call[chan].tei = BROADCAST;
  }
  else
    for (i = 0; i < MAXMSNS; i++)
      *call[chan].onum[i] =
      *call[chan].num[i] = 0;

  call[chan].bchan = -1;

  call[chan].cause = -1;
  call[chan].loc = -1;
  call[chan].aoce = -1;

  call[chan].provider = -1;

  for (i = 0; i < MAXMSNS; i++) {
    strcpy(call[chan].vnum[i], "?");

    call[chan].confentry[i] = -1;
    call[chan].sondernummer[i] = -1;
    call[chan].intern[i] = 0;
  } /* for */
} /* clearchan */


static void how_expensive(int chan)
{
  register int    c, zone = -1, zone2 = -1, pro = -1, pro2 = -1;
  auto	   int	  dur = (int)(call[chan].disconnect - call[chan].connect);
  auto     double pay2 = -1.0, pay3, onesec, cheap;
  auto     char   sx[BUFSIZ];
  extern   double pay(time_t ts, int dauer, int tarifz, int pro);
  auto 	   struct tm *tm;


  if (OUTGOING && (dur > 0) && *call[chan].num[CALLED]) {

    tm = localtime(&call[chan].connect);

    if (call[chan].sondernummer[CALLED] != -1) {
      switch (SN[call[chan].sondernummer[CALLED]].tarif) {
        case -1 :
#ifdef ISDN_DE
	          if (!strcmp(call[chan].num[CALLED] + 3, "11833")) /* Sonderbedingung Auskunft Inland */
                    dur -= 30;
#endif
                  pay2 = SN[call[chan].sondernummer[CALLED]].grund1 * currency_factor;
                  pay2 += (dur / SN[call[chan].sondernummer[CALLED]].takt1) * currency_factor;
                  break;

        case  0 : pay2 = 0.0;
                  break;

        case  1 : zone = 1;
                  break;
      } /* switch */
    } /* if */

    if (zone == -1) {
      zone2 = area_diff(NULL, call[chan].num[CALLED]);

      if ((c = call[chan].confentry[OTHER]) > -1)
        zone = known[c]->zone;

      if ((zone == -1) && (zone2 > 0)) {
        sprintf(sx, "WARNING: Assuming ZONE %d", zone2);
        zone = zone2;
        info(chan, PRT_SHOWHANGUP, STATE_HANGUP, sx);
      } /* if */

      if ((zone != -1) && (zone2 != -1) && (zone != zone2)) {
        sprintf(sx, "WARNING: Wrong ZONE (%d), assuming %d", zone, zone2);
        zone = zone2;

        if (call[chan].sondernummer[CALLED] == -1)
          info(chan, PRT_SHOWHANGUP, STATE_HANGUP, sx);
      } /* if */
    } /* if */

    if (pay2 == -1.0) {
      if (call[chan].aoce > 0) /* Gebuehrentakt AOC-E kam (Komfortanschluss, via Telekom */
        call[chan].pay = call[chan].aoce * currency_factor;
      else {
        if (zone > 0) {
#ifdef ISDN_DE
          if (zone == 1)
            pro2 = 33;           /* CityCall :: Telekom */
          else if (zone == 2)
            pro2 = 70;	   	 /* RegioCall :: Arcor */
          else if (zone == 3) {
#if 0
    	    if ((tm->tm_wday > 0) && (tm->tm_wday < 6))
    	      wochentag;
    	    else
              wochenende;
#endif
            if ((tm->tm_hour > 20) || (tm->tm_hour < 5))
              pro2 = 30;         /* zw. 21:00 Uhr und 5:00 Uhr TelDaFax */
            else
              pro2 = 19;	 /* Mobilcom */
          } /* else */

          switch (zone) { /* map "isdnlog" to "gebuehr.c" Zones */
            case 2 : zone = 3;
                     break;
            case 3 : zone = 4;
                     break;
          } /* switch */

          pro = call[chan].provider;

          if (pro == -1)
            pro = preselect;

          if (pro) {
            call[chan].pay = pay(call[chan].connect, dur, zone, pro);

	    if (call[chan].pay == -1.0) { /* Unknown Tarif */
              if (pro == 23) { /* tesion )) -- quick hack for SL "Baden-Württemberg Tarif" */
                if ((tm->tm_hour > 20) || (tm->tm_hour < 9))
                  pay3 = 0.14;
                else
                  pay3 = 0.21;

            	call[chan].pay = (pay3 / 60.0) * dur;
              }
              else
            	call[chan].pay = 0;

            } /* if */
          } /* if */

          if (pro != pro2) {
            pay3 = pay(call[chan].connect, dur, zone, pro2);

            cheap = call[chan].pay - pay3;

            if (cheap > 0) {
              sprintf(sx, "WARNING: Provider %s DM %s cheaper!",
                Providername(pro2),
              	double2str(cheap, 5, 2, DEB));
              	info(chan, PRT_SHOWHANGUP, STATE_HANGUP, sx);
            } /* if */
          } /* if */
#endif
        } /* if */
      } /* else */

#ifdef ISDN_DE
      if ((dur > 600) && (zone > 1) && ((call[chan].aoce > 0) || (pro == 33))) {
        onesec = call[chan].pay / dur;
        pay2 = (dur - 600) * onesec * 0.30;

        sprintf(sx, "10plus DM %s - DM %s = DM %s",
          double2str(call[chan].pay, 6, 2, DEB),
          double2str(pay2, 6, 2, DEB),
          double2str(call[chan].pay - pay2, 6, 2, DEB));

        call[chan].pay -= pay2;

        info(chan, PRT_SHOWHANGUP, STATE_HANGUP, sx);
      } /* if */
#endif
    }
    else
      call[chan].pay = pay2;
  } /* if */
} /* how_expensive */


static void dumpme()
{
  register int  chan;
  auto	   char s[BUFSIZ];


  for (chan = 0; chan < MAXCHAN; chan++) {
    sprintf(s, "^CHAN[%d]: %s -> %s\n",
      chan,
      call[chan].vnum[0],
      call[chan].vnum[CALLED]);

    print_msg(PRT_SHOWNUMBERS, "%s", s);
  } /* for */
} /* dumpme */


/* mode :: 0 = Add new entry, 1 = change existing entry, 2 = Terminate entry, 3 = dump */
static void addlist(int chan, int type, int mode)
{

#define MAXLIST 1000

  typedef struct {
    int	    state;
    char   *vnum[2];
    int	    si;
    time_t  connect;
    time_t  disconnect;
    int	    cause;
    int	    uid;
  } LIST;

  static      LIST  list[MAXLIST];
  static      int   lp = -1;
  register    int   i;
  register    char *p;
  auto struct tm   *tm;
  auto 	      char  s[BUFSIZ], s1[BUFSIZ];


  if (((chan == -1) || call[chan].dialin)) {
    if (mode == 0) {

      if (++lp == MAXLIST)
        lp = 0;

      list[lp].state = SETUP;
      list[lp].vnum[CALLING] = strdup(call[chan].vnum[CALLING]);
      list[lp].vnum[CALLED] = strdup(call[chan].vnum[CALLED]);
      list[lp].si = call[chan].si1;
      list[lp].connect = call[chan].connect;
      list[lp].uid = call[chan].uid;
    }
    else if ((mode == 1) || (mode == 2)) {
      for (i = lp; i >= 0; i--) {
        if (call[chan].uid == list[i].uid) {
          switch (mode) {
            case 1 : list[i].state = CONNECT;
            	     break;

            case 2 : list[i].cause = call[chan].cause;
                     list[i].state = RELEASE;
          	     list[i].disconnect = call[chan].disconnect;
          	     break;
          } /* switch */

          break;
        } /* if */
      } /* if */
    }
    else if (mode == 3) {
      for (i = 0; i <= lp; i++) {
        tm = localtime(&list[i].connect);
      	strftime(s1, 64, "%a %b %d %X", tm);

        if (!list[i].disconnect)
          list[i].disconnect = cur_time;

        switch (list[i].si) {
           case 1 : p = "Speech"; break;
           case 2 : p = "Fax G3"; break;
           case 3 : p = "Data";	  break;
           case 4 : p = "Fax G4"; break;
	   case 7 : p = "Data";	  break;
          default : p = "";       break;
        } /* switch */

      	sprintf(s, "%s %s(%s) -> %s %ds %s",
          s1,
          list[i].vnum[0],
          p,
          list[i].vnum[1],
          (int)(list[i].disconnect - list[i].connect),
          qmsg(TYPE_CAUSE, VERSION_EDSS1, list[i].cause));

        print_msg(PRT_SHOWNUMBERS, "%s\n", s);
      } /* for */
    } /* else */
  } /* if */
} /* addlist */


static void processctrl(int card, char *s)
{
  register char       *ps = s, *p;
  register int         i, c;
  register int         wegchan; /* fuer gemakelte */
  auto     int         dialin, type = 0, cref = -1, creflen, version;
  static   int         tei = BROADCAST, sapi = 0, net = 1, firsttime = 1;
  auto     char        sx[BUFSIZ], s2[BUFSIZ];
  static   char        last[BUFSIZ];
  auto     int         isAVMB1 = 0;


  hexSeen = 1;

#ifdef Q931
  if (q931dmp) {
    register int bcast = (strtol(ps + 8, NIL, 16) >> 1) == 0x7f;

    if (replaydev)
      fprintf(stdout, "\n\n-----[ %d ]-------------------------------------------------------------------\n\n", ++lfd);
    else
      fprintf(stdout, "\n\n-----[ %d ]------[ %s ]------------------------------------------\n\n", ++lfd, st + 4);

    if (bcast) {
      s[13] = 0;

      if (replaydev)
        fprintf(stdout, "r[%d]: %s    %s\n\n", card, s + 5, s + 14);
      else
        fprintf(stdout, "[%s] r[%d]: %s    %s\n\n", st + 4, card, s + 5, s + 14);

      s[13] = ' ';
    }
    else {
      if (replaydev)
        fprintf(stdout, "r[%d]: %s\n\n", card, s + 5);
      else
        fprintf(stdout, "[%s] r[%d]: %s\n\n", st + 4, card, s + 5);
    } /* else */
  } /* if */
#endif

  if (verbose & VERBOSE_CTRL)
    print_msg(PRT_LOG, "%s\n", s);

  if (!memcmp(ps, "D2", 2)) { /* AVMB1 */
    if (firsttime) {
      firsttime = 0;
      print_msg (PRT_NORMAL, "(AVM B1 driver detected (D2))\n");
    }
    memcpy(ps, "HEX: ", 5);
  }
  if (!memcmp(ps, "HEX: ", 5)) { /* new HiSax Driver */

    if (((verbose & VERBOSE_HEX) && !(verbose & VERBOSE_CTRL)) || stdoutput)
      print_msg(PRT_LOG, "%2d %s\n", card, s);

    if (firsttime) {
      firsttime = 0;

#ifdef Q931
      if (!q931dmp)
#endif
        print_msg(PRT_NORMAL, "(HiSax driver detected)\n");

      HiSax = 1;
      strcpy(last, s);
    }
    else {
      if (!strcmp(last, s)) {
#ifdef Q931
        if (!q931dmp)
#endif
        return;
      }
      else
        strcpy(last, s);
    } /* else */

#ifdef Q931
    if (q931dmp) {
      register char *s1;
#if 0
      register char *s2;
#endif
      register int i = strtol(ps + 5, NIL, 16);
      register int j = strtol(ps + 8, NIL, 16);
      register int k = strtol(ps + 11, NIL, 16);
      register int l = strtol(ps + 14, NIL, 16);
      register int sapi = i >> 2;
      register int cr = (i >> 1) & 1;
      register int ea2 = i & 1;
      register int tei = j >> 1;
      register int bcast = 0;
      register int ea3 = j & 1;


#if 0
      switch (sapi) {
        case  0 : s1 = "Signalisierungsblock"; break;
        case 16 : s1 = "Paketdatenblock";      break;
        case 63 : s1 = "TEI-Verwaltungsblock"; break;
        default : s1 = "UNKNOWN sapi";         break;
      } /* switch */

      if (tei == BROADCAST) {
        s2 = "Broadcast";
        bcast = 1;
      }
      else if (tei < 64)
        s2 = "feste TEI";
      else
        s2 = "zugeteilte TEI";

      fprintf(stdout, "%02x  SAPI=%d    C/R=%d  E/A=%d [%s]\n",
        i, sapi, cr, ea2, s1);
      fprintf(stdout, "%02x  TEI=%d     E/A=%d [%s]\n",
        j, tei, ea3, s2);
#else
      fprintf(stdout, "%02x  SAPI=%d    C/R=%d  E/A=%d\n",
        i, sapi, cr, ea2);
      fprintf(stdout, "%02x  TEI=%d     E/A=%d\n",
        j, tei, ea3);
#endif

      if (!(k & 1)) { /* I-Block */
        if (bcast)
          fprintf(stdout, "%02x  I-B  N=%d\n", k, k >> 1);
        else
          fprintf(stdout, "%02x  I-B  N=%d  %02x: N(R)=%d  P=%d\n",
            k, k >> 1, l, l >> 1, l & 1);
      }
      else if ((k & 3) == 1) { /* S-Block */
        switch (k) {
          case 01 : s1 = "RR";              break;
          case 05 : s1 = "RNR";             break;
          case 07 : s1 = "REJ";             break;
          default : s1 = "UNKNOWN S-Block"; break;
        } /* switch */

        if (bcast)
          fprintf(stdout, "%02x  %s\n", k, s1);
        else
          fprintf(stdout, "%02x  %s  %02x: N(R)=%d  P/F=%d\n",
            k, s1, l, l >> 1, l & 1);
      }
      else { /* U-Format */
        switch (k) {
          case 0x7f : s1 = "SABME P=1"; break;
          case 0x6f : s1 = "SABME P=0"; break;
          case 0x0f : s1 = "DM    F=0"; break;
          case 0x1f : s1 = "DM    F=1"; break;
          case 0x53 : s1 = "DISC  P=1"; break;
          case 0x43 : s1 = "DISC  P=0"; break;
          case 0x73 : s1 = "UA    F=1"; break;
          case 0x63 : s1 = "UA    F=0"; break;
          case 0x93 : s1 = "FRMR  F=1"; break;
          case 0x83 : s1 = "FRMR  F=0"; break;
          case 0x13 : s1 = "UI    P=1"; break;
          case 0x03 : s1 = "UI    P=0"; break;
          default   : s1 = "UNKNOWN U-Block"; break;
        } /* switch */

        fprintf(stdout, "%02x  %s\n", k, s1);
      } /* else */
    } /* if */
#endif

    if (!*(ps + 13) || !*(ps + 16))
      return;

    i = strtol(ps += 5, NIL, 16) >> 1;
    net = i & 1;
    sapi = i >> 1;

    if (sapi == 63) /* AK:07-Nov-98 -- future expansion */
      return;

    tei = strtol(ps += 3, NIL, 16) >> 1;

    ps += (tei == BROADCAST) ? 1 : 4;
  }

  else  if (!memcmp(ps, "D3", 2)) { /* AVMB1 */

    if (firsttime) {
      firsttime = 0;
      print_msg (PRT_NORMAL, "(AVM B1 driver detected (D3))\n");
    }

    if (*(ps + 2) == '<')  /* this is our "direction flag" */
      net = 1;
    else
      net = 0;

    tei = 65;  /* we can't get a tei, so fake it */
    isAVMB1 = 1;

    ps[0] = 'h'; ps[1] = 'e'; ps[2] = 'x';  /* rewrite for the others */
  } /* AVM B1 */
  else { /* Old Teles Driver */

    /* Tei wird gelesen und bleibt bis zum Ende des naechsten hex: stehen.
       Der naechste hex: -Durchlauf hat also die korrekte tei. */

    if (!memcmp(ps, "Q.931 frame network->user tei ", 30)) {
      tei = strtol(ps += 30, NIL, 10);
      net = 1;
    }
    else if (!memcmp(ps, "Q.931 frame user->network tei ", 30)) {
      tei = strtol(ps += 30, NIL, 10);
      net = 0;
    }
    else if (!memcmp(ps, "Q.931 frame network->user with tei ", 35)) {
      tei = strtol(ps += 35, NIL, 10);
      net = 1;
    }
    else if (!memcmp(ps, "Q.931 frame network->user", 25)) {
      net = 1;
      tei = BROADCAST;
    } /* else */
  } /* else */

  if (!memcmp(ps, "hex: ", 5) || !memcmp(s, "HEX: ", 5)) {
    i = strtol(ps += 5, NIL, 16);

    switch (i) {
      case 0x40 :
      case 0x41 : version = VERSION_1TR6;
      	   	  break;

      case 0x08 : version = VERSION_EDSS1;
      	   	  break;

      default   : version = VERSION_UNKNOWN;
      		  sprintf(sx, "Unexpected discriminator 0x%02x -- ignored!", i);
          	  info(chan, PRT_SHOWNUMBERS, STATE_RING, sx);
		  return;
    } /* switch */

#ifdef Q931
    if (q931dmp) {
      register int crl = strtol(ps + 3, NIL, 16);
      register int crw = strtol(ps + 6, NIL, 16);


      if (crl) {
#if 0
        register int   dir = crw >> 7;
        register int   cr  = crw & 0x7f;
        register char *s1, *s2;


        if (cr < 64) {
          s1 = "Dialin";
          s2 = dir ? "User->VSt" : "VSt->User";
        }
        else {
          s1 = "Dialout";
          s2 = dir ? "VSt->User" : "User->VSt";
        } /* else */

        fprintf(stdout, "%02x  %s, PD=%02x  %02x: CRL=%d  %02x: CRW=%d  %s [%s, %s]\n",
          i, (version == VERSION_EDSS1) ? "E-DSS1" : "1TR6", i, crl,
          crl, crw, crw & 0x7f, (crw > 127) ? "Zielseite" : "Ursprungsseite", s1, s2);
#else
        fprintf(stdout, "%02x  %s, PD=%02x  %02x: CRL=%d  %02x: CRW=%d  %s\n",
          i, (version == VERSION_EDSS1) ? "E-DSS1" : "1TR6", i, crl,
          crl, crw, crw & 0x7f, (crw > 127) ? "Zielseite" : "Ursprungsseite");
#endif
      }
      else
        fprintf(stdout, "%02x  %s, PD=%02x  %02x: CRL=%d\n",
          i, (version == VERSION_EDSS1) ? "E-DSS1" : "1TR6", i, crl,
          crl);
    } /* if */
#endif

    if (bilingual && version == VERSION_1TR6) {
      print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: OOPS! 1TR6 Frame? Ignored!\n", st + 4);
      goto endhex;
    } /* if */

    creflen = strtol(ps += 3, NIL, 16);

    if (creflen)
      cref = strtol(ps += 3, NIL, 16);
    else
      cref = -1;

    type = strtol(ps += 3, NIL, 16);

    if (!isAVMB1)
      dialin = (tei == BROADCAST); /* dialin (Broadcast), alle anderen haben schon eine Tei! */
    else
      dialin = (cref & 0x80);  /* first (SETUP) tells us who initiates the connection */

    /* dialin = (cref & 0x7f) < 64; */

    cref = (net) ? cref : cref ^ 0x80; /* cref immer aus Sicht des Amts */

#ifdef Q931
    if (q931dmp)
      Q931dump(TYPE_MESSAGE, type, NULL, version);
#endif
    if (allflags & PRT_DEBUG_DIAG)
      diag(cref, tei, sapi, dialin, net, type, version);

    /* leider laesst sich kein switch nehmen, da decode
       innerhalb von SETUP/A_ACK aufgerufen werden muss, sonst
       aber erst nach feststellen von chan
       Daher GOTO (urgs...) an das Ende vom if hex:.. */

    if (type == SETUP) { /* neuen Kanal, ev. dummy, wenn keiner da ist */
      chan = 5; /* den nehmen wir _nur_ dafuer! */
      clearchan(chan, 1);
      call[chan].dialin = dialin;
      call[chan].tei = tei;
      call[chan].card = card;
      call[chan].uid = ++uid;
      decode(chan, ps, type, version, tei);

      if (call[chan].channel) { /* Aha, Kanal war dabei, dann nehmen wir den gleich */
        chan = call[chan].channel - 1;

        print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: Chan auf %d gesetzt\n", st + 4, chan);

        /* nicht --channel, channel muss unveraendert bleiben! */
        memcpy((char *)&call[chan], (char *)&call[5], sizeof(CALL));
        Change_Channel(5, chan);
        clearchan(5, 1);
      } /* if */

      call[chan].cref = (dialin) ? cref : (cref | 0x80); /* immer die cref, die _vom_ Amt kommt/kommen sollte */
      call[chan].dialin = dialin;
      call[chan].tei = tei;
      call[chan].card = card;
      call[chan].connect = cur_time;
      call[chan].duration = tt;
      call[chan].state = type;

      print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: START CHAN#%d tei %d cref %d %d %s %s->\n",
        st + 4, chan, tei, cref, call[chan].cref,
        call[chan].dialin ? " IN" : "OUT",
        net ? "NET" : "USR");

      addlist(chan, type, 0);

      goto endhex;
    } /* if SETUP */

/* AK:13-Feb-97 ::
   Bei Rausrufen mit Creatix a/b kommt im
   SETUP : Channel identification : BRI, beliebiger Kanal
   und im
   SETUP ACKNOWLEDGE : Channel identification : BRI, B1 gefordert

   Bei Rausrufen mit Europa-10 dagegen:
   SETUP : --
   SETUP ACKNOWLEDGE : Channel identification : BRI, B1 gefordert

*/

    if ((type == SETUP_ACKNOWLEDGE) || (type == CALL_PROCEEDING)) {
      /* Kann sein, dass ein SETUP vorher kam, suchen wir mal, denkbar:
           a) SETUP in 5 (eig. rausruf): decode auf 5, dann copy nach channel
           b) nichts (rausruf fremd): decode auf 5, copy nach channel */

      chan = 5;

      if ((call[5].cref != cref) || (call[5].tei != tei)) {
        /* bei C_PROC/S_ACK ist cref _immer_ > 128 */
        /* keiner da, also leeren */
        if (isAVMB1 && (call[chan].state == SETUP))  /* direction already set for AVMB1 */
          dialin = call[chan].dialin;
        clearchan(chan, 1);
        call[chan].dialin = dialin;
        call[chan].tei = tei;
        call[chan].cref = cref;
      	call[chan].card = card;
      } /* if */

      decode(chan, ps, type, version, tei);

      /* dumpme(); */

      if (call[chan].channel) { /* jetzt muesste einer da sein */

        chan = call[chan].channel - 1;

        /* nicht --channel, channel muss unveraendert bleiben! */
        memcpy((char *)&call[chan], (char *)&call[5], sizeof(CALL));
        Change_Channel(5, chan);
	addlist(chan, type, 1);
        clearchan(5, 1);
      }
      else
        print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: OOPS, C_PROC/S_ACK ohne channel? tei %d\n",
          st + 4, tei);

      call[chan].connect = cur_time;
      call[chan].duration = tt;
      call[chan].state = type;

      print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: START CHAN#%d tei %d %s %s->\n",
        st + 4, chan, tei,
        call[chan].dialin ? " IN" : "OUT",
        net ? "NET" : "USR");
      goto endhex;
    } /* if C_PROC || S_ACK */

    if (type == AOCD_1TR6) {
      decode(chan, ps, type, version, tei);
      goto endhex;
    } /* if AOCD_1TR6 */

    /* Beim Makeln kommt Geb. Info nur mit Cref und Tei, die
       cref muessen wir dann in chan 2/3 suchen */

    /* Bei geparkten Gespr. kommen die waehrend des Parkens
       aufgelaufenen Gebuehren beim Wiederholen. */

    if ((cref != call[0].cref) && (cref != call[1].cref) &&
        (cref != call[2].cref) && (cref != call[3].cref)) {

      decode(6, ps, type, version, tei);

      /* Mit falscher cref kommt hier keiner rein, koennte
         ein RELEASE auf bereits freiem Kanal sein */
      goto endhex;
    } /* if */

    /* So, wenn wir hier ankommen, haben wir auf jeden Fall einen
       Kanal (0, 1, 2 oder 3) und eine cref. Die tei folgt evtl. erst beim
       Connect (Reinruf). Suchen wir den Kanal: */

    /* crefs absuchen. Gibt's die mehrmals, tei absuchen, dann haben wir
       ihn.
       Es kann aber sein, dass cref stimmt, aber noch keine tei da war
       (Reinruf). Dann ist aber die cref eindeutig (hoffentlich)!
       finden wir einen Kanal mit passender cref, der keine
       tei hat, haben wir ihn. Hat er eine, und sie stimmt,
       ebenso. Sonst weitersuchen. Geparkte Kanaele ignorieren
       bis zum RESUME, oder sie werden bei neuem SETUP_ACK. ueber-
       schrieben, wenn wir wen im Parken verhungen lassen haben */

    chan = -1;

    for (i = 0; ((i < 4) &&
      ((call[i].cref != cref) ||
        ((call[i].state == SUSPEND) && (type != RESUME_ACKNOWLEDGE)) ||
        ((call[i].tei != BROADCAST) && (call[i].tei != tei)))); i++);
      chan = i;

    print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: Kanal %d\n", st + 4, chan);

    /* auch wenn hier schon eine tei bei ist, erst beim connect hat
       ein reingerufener Kanal eine gueltige tei */

    decode(chan, ps, type, version, tei);

    switch (type) {

      case ALERTING            :
      case CALL_PROCEEDING     :
#ifdef Q931
       	   		         if (!q931dmp)
#endif
      	   		         if (dual && *call[chan].digits) {
                      	       	   strcpy(call[chan].onum[CALLED], call[chan].digits);
                                   buildnumber(call[chan].digits, call[chan].oc3, -1, call[chan].num[CALLED], version, &call[chan].provider, &call[chan].sondernummer[CALLED], &call[chan].intern[CALLED], call[chan].dialin, CALLED);

                      		   strcpy(call[chan].vnum[CALLED], vnum(chan, CALLED));
      	   		       	 } /* if */
                                 break;

      case CONNECT             :
      case CONNECT_ACKNOWLEDGE :

        /* Bei Rufen an die Teles kommt CONNECT und CONN.ACKN., eins reicht uns */
        if (call[chan].state == CONNECT)
          goto doppelt;

        call[chan].state = CONNECT;
        call[chan].tei = tei;
        call[chan].dialog++; /* es hat connect gegeben */
        call[chan].connect = cur_time;
        call[chan].duration = tt;
      	call[chan].card = card;

        if (message & PRT_SHOWNUMBERS)
          info(chan, PRT_SHOWCONNECT, STATE_CONNECT, "CONNECT");
        else {
          sprintf(sx, "CONNECT (%s)", call[chan].service);
          info(chan, PRT_SHOWCONNECT, STATE_CONNECT, sx);
        } /* else */

        if (OUTGOING) {
          auto	 char s[BUFSIZ], sx[BUFSIZ];

      	  if ((call[chan].cint = taktlaenge(chan, s)) > 1) {
            call[chan].cinth    = hour;
            call[chan].nextcint = call[chan].connect + (int)call[chan].cint;
            call[chan].ctakt    = 1;

            sprintf(sx, "NEXT CHARGEINT IN %s (%s)", double2clock(call[chan].cint), s);
          info(chan, PRT_SHOWCONNECT, STATE_CONNECT, sx);
        } /* if */
        } /* if */

        if (sound)
          ringer(chan, RING_CONNECT);

doppelt:break;

      case SUSPEND_ACKNOWLEDGE :
        call[chan].state = SUSPEND;
        info(chan, PRT_SHOWHANGUP, STATE_HANGUP, "PARK");
        break;

      case RESUME_ACKNOWLEDGE :
        call[chan].state = CONNECT;
        info(chan, PRT_SHOWCONNECT, STATE_CONNECT, "RESUME");
        break;

      case MAKEL_ACKNOWLEDGE :
        wegchan = (call[2].state) ? 3 : 2;
        memcpy((char *)&call[wegchan], (char *)&call[chan], sizeof(CALL));
        Change_Channel(chan, wegchan);
	addlist(wegchan, type, 1);
        clearchan(chan, 1);
        call[wegchan].state = MAKEL_ACKNOWLEDGE;
        info(wegchan, PRT_SHOWHANGUP, STATE_HANGUP, "MAKEL");
        break;

      case MAKEL_RESUME_ACK :
        if (call[chan].channel) { /* muesste einer da sein */
          memcpy((char *)&call[call[chan].channel - 1], (char *)&call[chan], sizeof(CALL));
          call[call[chan].channel - 1].channel = chan; /* den alten merken */
          Change_Channel(chan, call[chan].channel - 1);
          chan = call[chan].channel - 1; /* chan setzen */
	  addlist(chan, type, 1);
          clearchan(call[chan].channel, 1);
          call[chan].channel = chan + 1; /* in Ordnung bringen */
          call[chan].state = CONNECT;
        } /* if */

        info(chan, PRT_SHOWCONNECT, STATE_CONNECT, "MAKELRESUME");
        break;


      case DISCONNECT          :

        if (!call[chan].state) /* Keine Infos -> Weg damit */
          break;

        call[chan].disconnect = cur_time;

        if (replay)
          call[chan].duration = (tt - call[chan].duration) * 100;
        else
          call[chan].duration = tt - call[chan].duration;

        call[chan].state = DISCONNECT;

        break;


      case RELEASE             :
      case RELEASE_COMPLETE    :

        if (!net) /* wir nehmen nur RELEASE vom Amt */
          break;

        if (!call[chan].state) /* Keine Infos -> Weg damit */
          break;

        /* Wenn's keinen CONNECT gab, hat's auch nichts gekostet.
           Falls der RELEASE aber ein Rufablehnen war und der
           CONNECT noch folgt, wird dafuer jetzt chan auf
           4 gepackt, um die schoenen Daten in 0/1/ev.4 nicht
           zu zerstoeren. Wir erkennen das an fehlender tei. */

        if (call[chan].tei == BROADCAST) {
          memcpy((char *)&call[4], (char *)&call[chan], sizeof(CALL));
          Change_Channel(chan, 4);
          chan = 4;
	  addlist(chan, type, 1);
          call[chan].tei = tei;
      	  call[chan].card = card;
        } /* if */

        if (!call[chan].disconnect) {
          call[chan].disconnect = cur_time;

          if (replay)
            call[chan].duration = (tt - call[chan].duration) * 100;
          else
            call[chan].duration = tt - call[chan].duration;
        } /* if */

        if (!call[chan].dialog) {
          call[chan].duration = 0;
          call[chan].disconnect = call[chan].connect;

          print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: CHAN#%d genullt (dialin=%d, state=%d, tei=%d, cref=%d)\n",
            st + 4, chan, call[chan].dialin, call[chan].state, call[chan].tei, call[chan].cref);

          print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: OOPS! DURATION=0\n", st + 4);

          if (OUTGOING) {
            print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: OOPS! AOCE=0\n", st + 4);
          } /* if */
        } /* if kein connect */

        if (allflags & PRT_DEBUG_BUGS) {
          strcpy(sx, ctime(&call[chan].connect));
          sx[19] = 0;

          print_msg(PRT_DEBUG_BUGS, " DEBUG> %s: LOG CHAN#%d(%s : DIAL%s : %s -> %s : %d s (%d s) : %d EH):%s\n\n",
            st + 4, chan,
            sx + 4,
            call[chan].dialin ? "IN" : "OUT",
            call[chan].num[CALLING],
            call[chan].num[CALLED],
            (int)(call[chan].disconnect - call[chan].connect),
            (int)call[chan].duration,
            call[chan].aoce,
            qmsg(TYPE_CAUSE, version, call[chan].cause));
        } /* if */

        how_expensive(chan);

#ifdef Q931
       	if (!q931dmp)
#endif
          logger(chan);

	addlist(chan, type, 2);

        if (call[chan].dialog || any) {
          if (call[chan].ibytes + call[chan].obytes) {
            sprintf(s2, " I=%s O=%s",
              double2byte((double)call[chan].ibytes),
              double2byte((double)call[chan].obytes));
          }
          else
            *s2 = 0;

          if (call[chan].dialin)
            sprintf(sx, "HANGUP (%s%s)",
              double2clock((double)(call[chan].disconnect - call[chan].connect)), s2);
          else {
            if (call[chan].aoce > 0)
              sprintf(sx, "HANGUP (%d EH %s %s %s%s)",
                call[chan].aoce,
                currency,
                double2str(call[chan].pay, 6, 2, DEB),
                double2clock((double)(call[chan].disconnect - call[chan].connect)), s2);
            else if (call[chan].pay)
              sprintf(sx, "HANGUP (%s %s %s%s)",
                currency,
                double2str(call[chan].pay, 6, 2, DEB),
                double2clock((double)(call[chan].disconnect - call[chan].connect)), s2);
            else
              sprintf(sx, "HANGUP (%s%s) %s (%s)",
                double2clock((double)(call[chan].disconnect - call[chan].connect)), s2,
                qmsg(TYPE_CAUSE, version, call[chan].cause),
            	location(call[chan].loc));
          } /* else */

          if (!memcmp(sx, "HANGUP (        )", 17))
            sx[6] = 0;

          if ((call[chan].cause != 0x10) && (call[chan].cause != 0x1f)) { /* "Normal call clearing", "Normal, unspecified" */
            strcat(sx, " ");
            strcat(sx, qmsg(TYPE_CAUSE, version, call[chan].cause));
            if ((p = location(call[chan].loc))) {
              strcat(sx, " (");
              strcat(sx, location(call[chan].loc));
              strcat(sx, ")");
            } /* if */
          } /* if */

          info(chan, PRT_SHOWHANGUP, STATE_HANGUP, sx);

          if (OUTGOING && ((c = call[chan].confentry[OTHER]) > -1)) {
	    if (chargemax != 0.0) {
            sprintf(sx, "CHARGEMAX total=%s %s today=%s %s remaining=%s %s",
              currency,
              double2str(known[c]->scharge + known[c]->charge, 7, 2, DEB),
              currency,
              double2str(known[c]->charge, 6, 2, DEB),
              currency,
              double2str((chargemax - known[c]->charge), 6, 2, DEB));
            info(chan, PRT_SHOWCHARGEMAX, STATE_HANGUP, sx);
	    } /* if */

            if (connectmax != 0.0) {
              if (connectmaxmode == 1)
								known[c]->online += ((int)((call[chan].disconnect - call[chan].connect + 59) / 60.0)) * 60.0;
              else
                known[c]->online += call[chan].disconnect - call[chan].connect;

              sprintf(sx, "CONNECTMAX total=%s month=%s remaining=%s",
                double2clock(known[c]->sonline + known[c]->online),
                double2clock(known[c]->online),
                double2clock(connectmax - known[c]->online));
              info(chan, PRT_SHOWCHARGEMAX, STATE_HANGUP, sx);
            } /* if */

            if (bytemax != 0.0) {
              auto double byte;

              switch (bytemaxmode & 25) {
                case  8 : byte = call[chan].obytes;
                          break;

                case 16 : byte = call[chan].ibytes + call[chan].obytes;
                          break;

                default : byte = call[chan].ibytes;
                          break;
              } /* switch */

              switch (bytemaxmode & 3) {
                case 0 : known[c]->bytes += byte;
                         break;
                case 1 : known[c]->bytes += byte;
                         break;
                case 2 : known[c]->bytes += byte;
                         break;
              } /* switch */

              sprintf(sx, "BYTEMAX total=%s month=%s remaining=%s",
                double2byte((double)(known[c]->sbytes + known[c]->bytes)),
                double2byte((double)(known[c]->bytes)),
                double2byte((double)(bytemax - known[c]->bytes)));
              info(chan, PRT_SHOWCHARGEMAX, STATE_HANGUP, sx);
            } /* if */
          } /* if */

          if (sound)
            ringer(chan, RING_HANGUP);
        } /* if */

        clearchan(chan, 1);

        break;

    } /* switch */

endhex:
    tei = BROADCAST; /* Wenn nach einer tei-Zeile keine hex:-Zeile kommt, tei ungueltig machen! */

    if ((type == SETUP) && !replay) { /* fetch additional info from "/dev/isdninfo" */
      static void moreinfo(); /* soviel zu Objektorientiertem Denken ;-) */
      moreinfo();
    } /* if */

  } /* if */
} /* processctrl */


void processrate()
{
  register char  *p;
  register int    j;
  auto     char   sx[BUFSIZ];
  auto     double s;


  if (!ioctl(sockets[ISDNINFO].descriptor, IIOCGETCPS, &io)) {

    if (verbose & VERBOSE_RATE) {
      p = sx;
      s = 0L;

      for (j = 0; j < chans; j++) {
        p += sprintf(p, "%ld ", io[j].i);
        s += io[j].i;
      } /* for */

      if (s > 0L)
        print_msg(PRT_LOG, "ibytes:\t%s\n", sx);

      p = sx;
      s = 0L;

      for (j = 0; j < chans; j++) {
        p += sprintf(p, "%ld ", io[j].o);
        s += io[j].o;
      } /* for */

      if (s > 0L)
        print_msg(PRT_LOG, "obytes:\t%s\n", sx);
    } /* if */

    processbytes();
  } /* if */
} /* processrate */


int morectrl(int card)
{
  register char      *p, *p1, *p2, *p3;
  static   char       s[MAXCARDS][BIGBUFSIZ];
  static   char      *ps[MAXCARDS] = { s[0], s[1] };
  auto     int        n = 0;
  auto     struct tm *tm;


  if ((n = read(sockets[card ? ISDNCTRL2 : ISDNCTRL].descriptor, ps[card], BUFSIZ)) > 0) {

    now();
    ps[card] += n;

    *ps[card] = 0;

    p1 = s[card];

    while ((p = p2 = strchr(p1, '\n'))) {
      *p = 0;

      while (*--p == ' ')
        *p = 0;
retry:
      if (replay) {

        if (replaydev)
          p3 = p1;
        else {
          cur_time = tt = atom(p1 + 4);

          if (cur_time == (time_t)-1) {
            now();
            replaydev++;
            goto retry;
          } /* if */

          set_time_str();

          tm = localtime(&cur_time);
          p3 = p1 + 26;

        } /* if */

        if (!memcmp(p3, "idmap:", 6) ||
            !memcmp(p3, "chmap:", 6) ||
            !memcmp(p3, "drmap:", 6) ||
            !memcmp(p3, "usage:", 6) ||
            !memcmp(p3, "flags:", 6) ||
            !memcmp(p3, "phone:", 6) ||
            !memcmp(p3, "ibytes:", 7) ||
            !memcmp(p3, "obytes:", 7))
          processinfo(p3);
        else if (!memcmp(p3, "HEX: ", 5) ||
                 !memcmp(p3, "hex: ", 5) ||
/*               !memcmp(p3, "D2<: ", 5) ||   Layer 2 not yet evaluated */
/*               !memcmp(p3, "D2>: ", 5) ||   Layer 2 not yet evaluated */
                 !memcmp(p3, "D3<: ", 5) ||
                 !memcmp(p3, "D3>: ", 5))
          processctrl(0, p3);
        else if (!memcmp(p3 + 3, "HEX: ", 5))
          processctrl(atoi(p3), p3 + 3);
      }
      else
        processctrl(card, p1);

      p1 = p2 + 1;
    } /* while */

    if (p1 < ps[card]) {
      n = ps[card] - p1;
      memmove(s, p1, n);
      ps[card] = s[card] + n;
    }
    else
      ps[card] = s[card];

    return(1);
  }
  else {
    alarm(0);
    return(0);
  } /* else */
} /* morectrl */


void moreinfo()
{
  register char *p, *p1, *p2;
  static   char  s[BIGBUFSIZ * 2];
  static   char *ps = s;
  auto     int   n;


  if ((n = read(sockets[ISDNINFO].descriptor, ps, BIGBUFSIZ)) > 0) {
    now();
    ps += n;

    *ps = 0;

    p1 = s;

    while ((p = p2 = strchr(p1, '\n'))) {
      *p = 0;

      while (*--p == ' ')
        *p = 0;

      processinfo(p1);

      p1 = p2 + 1;
    } /* while */

    if (p1 < ps) {
      n = ps - p1;
      memmove(s, p1, n);
      ps = s + n;
    }
    else
      ps = s;
  } /* if */
} /* moreinfo */

/*****************************************************************************/

void morekbd()
{
  auto char  s[BIGBUFSIZ * 2];
  auto char *ps = s;
  auto int   n, chan;


  if ((n = read(sockets[STDIN].descriptor, ps, BIGBUFSIZ)) > 0) {
    ps += n;

    *ps = 0;

    switch (*s) {
      case 'l' : print_msg(PRT_SHOWNUMBERS, "Recent caller's:\n");
      	       	 addlist(-1, SETUP, 3);
                 break;

      case 'h' : print_msg(PRT_SHOWNUMBERS, "\n\t*** s)tatus, l)ist, u)p, d)own ***\n");
      	       	 break;

      case 'u' : /* huptime(0, 0); */
      	       	 break;

      case 'd' : /* huptime(0, 0); */
      	       	 break;

      case 's' : now();

      	         print_msg(PRT_SHOWNUMBERS, "\n\t*** %s\n", stl);

      	         for (chan = 0; chan < MAXCHAN; chan++) {
      	       	   if (call[chan].bchan == -1)
                     sprintf(s, "\t*** BCHAN#%d : FREE ***\n", chan + 1);
      	       	   else {
                     sprintf(s, "\t*** BCHAN#%d : %d %s %s %s ***\n",
                       chan + 1,
                       call[chan].bchan,
                       call[chan].vnum[0],
                       call[chan].dialin ? "<-" : "->",
                       call[chan].vnum[1]);
      	       	   } /* else */

      	       	   print_msg(PRT_SHOWNUMBERS, "%s", s);
      	       	 } /* for */
                 break;
    } /* switch */

  } /* if */
} /* morekbd */

/*****************************************************************************/

void processcint()
{
  register int    chan;
  auto	   char   s[BUFSIZ], sx[BUFSIZ];
  auto	   double  newcint;
  auto	   double exp;
  auto	   int	  dur;
  extern   double pay(time_t ts, int dauer, int tarifz, int pro);


  for (chan = 0; chan < 2; chan++) {
    if (OUTGOING && (call[chan].cint > 1)) {
      if (call[chan].nextcint == cur_time) {

        dur = cur_time - call[chan].connect;

        if (call[chan].cinth != hour) { /* Moeglicherweise Taktwechsel */

      	  newcint = taktlaenge(chan, s);
	  if (newcint != call[chan].cint) {
          call[chan].cint = newcint;
	    sprintf(sx, "NEXT CHARGEINT IN %s (%s)", double2clock((double)call[chan].cint), s);
          info(chan, PRT_SHOWCONNECT, STATE_CONNECT, sx);
	  }
        } /* if */

        call[chan].cinth = hour;
        call[chan].ctakt++;

        if (1 /* message & PRT_SHOWTICKS */) {

#ifdef ISDN_DE
          if ((call[chan].provider == -1) || (call[chan].provider == 33))
            exp = call[chan].ctakt * currency_factor;
          else
            /* call pay() with duration + 1 to get the charge for the _next_ chargeint! */
            exp = pay(call[chan].connect, dur + 1, call[chan].zone, call[chan].provider);
#else
	  exp = call[chan].ctakt * currency_factor;
#endif
          sprintf(sx, "START %d.CHARGEINT %s %s (%s)",
            call[chan].ctakt,
            currency,
            double2str(exp, 6, 2, DEB),
            double2clock((double)dur));

          info(chan, PRT_SHOWCONNECT, STATE_CONNECT, sx);
        } /* if */

        call[chan].nextcint += (int)call[chan].cint;
      } /* if */
    } /* if */
  } /* for */
} /* processcint */
