/* $Id: tools.h,v 1.24 1998/12/09 20:40:27 akool Exp $
 *
 * ISDN accounting for isdn4linux.
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Log: tools.h,v $
 * Revision 1.24  1998/12/09 20:40:27  akool
 *  - new option "-0x:y" for leading zero stripping on internal S0-Bus
 *  - new option "-o" to suppress causes of other ISDN-Equipment
 *  - more support for the internal S0-bus
 *  - Patches from Jochen Erwied <mack@Joker.E.Ruhr.DE>, fixes TelDaFax Tarif
 *  - workaround from Sebastian Kanthak <sebastian.kanthak@muehlheim.de>
 *  - new CHARGEINT chapter in the README from
 *    "Georg v.Zezschwitz" <gvz@popocate.hamburg.pop.de>
 *
 * Revision 1.23  1998/11/24 20:53:10  akool
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
 * Revision 1.22  1998/11/01 08:50:35  akool
 *  - fixed "configure.in" problem with NATION_*
 *  - DESTDIR fixes (many thanks to Michael Reinelt <reinelt@eunet.at>)
 *  - isdnrep: Outgoing calls ordered by Zone/Provider/MSN corrected
 *  - new Switch "-i" -> running on internal S0-Bus
 *  - more providers
 *  - "sonderrufnummern.dat" extended (Frag Fred, Telegate ...)
 *  - added AVM-B1 to the documentation
 *  - removed the word "Teles" from the whole documentation ;-)
 *
 * Revision 1.21  1998/10/18 20:13:44  luethje
 * isdnlog: Added the switch -K
 *
 * Revision 1.20  1998/09/26 18:30:18  akool
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
 * Revision 1.19  1998/06/21 11:53:27  akool
 * First step to let isdnlog generate his own AOCD messages
 *
 * Revision 1.18  1998/06/07 21:10:02  akool
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
 * Revision 1.17  1998/03/08 11:43:18  luethje
 * I4L-Meeting Wuerzburg final Edition, golden code - Service Pack number One
 *
 * Revision 1.16  1997/05/29 17:07:30  akool
 * 1TR6 fix
 * suppress some noisy messages (Bearer, Channel, Progress) - can be reenabled with log-level 0x1000
 * fix from Bodo Bellut (bodo@garfield.ping.de)
 * fix from Ingo Schneider (schneidi@informatik.tu-muenchen.de)
 * limited support for Info-Element 0x76 (Redirection number)
 *
 * Revision 1.15  1997/05/25 19:41:16  luethje
 * isdnlog:  close all files and open again after kill -HUP
 * isdnrep:  support vbox version 2.0
 * isdnconf: changes by Roderich Schupp <roderich@syntec.m.EUnet.de>
 * conffile: ignore spaces at the end of a line
 *
 * Revision 1.14  1997/05/15 22:21:49  luethje
 * New feature: isdnrep can transmit via HTTP fax files and vbox files.
 *
 * Revision 1.13  1997/05/09 23:31:00  luethje
 * isdnlog: new switch -O
 * isdnrep: new format %S
 * bugfix in handle_runfiles()
 *
 * Revision 1.12  1997/05/04 20:20:05  luethje
 * README completed
 * isdnrep finished
 * interval-bug fixed
 *
 * Revision 1.11  1997/04/20 22:52:36  luethje
 * isdnrep has new features:
 *   -variable format string
 *   -can create html output (option -w1 or ln -s isdnrep isdnrep.cgi)
 *    idea and design from Dirk Staneker (dirk.staneker@student.uni-tuebingen.de)
 * bugfix of processor.c from akool
 *
 * Revision 1.10  1997/04/15 22:37:13  luethje
 * allows the character `"' in the program argument like the shell.
 * some bugfixes.
 *
 * Revision 1.9  1997/04/03 22:40:21  luethje
 * some bugfixes.
 *
 * Revision 1.8  1997/03/31 22:43:18  luethje
 * Improved performance of the isdnrep, made some changes of README
 *
 * Revision 1.7  1997/03/29 09:24:34  akool
 * CLIP presentation enhanced, new ILABEL/OLABEL operators
 *
 * Revision 1.6  1997/03/23 23:12:05  luethje
 * improved performance
 *
 * Revision 1.5  1997/03/23 21:04:10  luethje
 * some bugfixes
 *
 * Revision 1.4  1997/03/20 22:42:41  akool
 * Some minor enhancements.
 *
 * Revision 1.3  1997/03/20 00:19:18  luethje
 * inserted the line #include <errno.h> in avmb1/avmcapictrl.c and imon/imon.c,
 * some bugfixes, new structure in isdnlog/isdnrep/isdnrep.c.
 *
 * Revision 1.2  1997/03/17 23:21:08  luethje
 * README completed, new funktion Compare_Sections() written, "GNU_SOURCE 1"
 * added to tools.h and a sample file added.
 *
 * Revision 1.1  1997/03/16 20:59:25  luethje
 * Added the source code isdnlog. isdnlog is not working yet.
 * A workaround for that problem:
 * copy lib/policy.h into the root directory of isdn4k-utils.
 *
 * Revision 2.6.36  1997/02/10  09:30:43  akool
 * MAXCARDS implemented
 *
 * Revision 2.6.25  1997/01/17  23:30:43  akool
 * City Weekend Tarif implemented (Thanks to Oliver Schoett <schoett@muc.de>)
 *
 * Revision 2.6.20  1997/01/05  20:05:43  akool
 * neue "AreaCode" Release implemented
 *
 * Revision 2.6.15  1997/01/02  19:51:43  akool
 * CHARGEMAX erweitert
 * CONNECTMAX implementiert
 *
 * Revision 2.40    1996/06/19  17:45:43  akool
 * double2byte(), time2str() added
 *
 * Revision 2.3.26  1996/05/05  12:07:43  akool
 * known.interface added
 *
 * Revision 2.3.23  1996/04/28  12:16:43  akool
 * confdir()
 *
 * Revision 2.2.5  1996/03/25  19:17:43  akool
 * 1TR6 causes implemented
 *
 * Revision 2.23  1996/03/24  12:11:43  akool
 * Explicit decl. of basename() - new "unistd.h" dont have one
 *
 * Revision 2.15  1996/02/21  20:14:43  akool
 *
 * Revision 2.12  1996/02/13  20:08:43  root
 * Nu geht's (oder?)
 *
 * Revision 2.12  1996/02/13  20:08:43  root
 * Nu geht's (oder?)
 *
 * Revision 1.2  1996/02/13  20:05:28  root
 * so nun gehts
 *
 * Revision 1.1  1996/02/13  14:28:14  root
 * Initial revision
 *
 * Revision 2.05  1995/02/11  17:10:16  akool
 *
 */

/****************************************************************************/

#ifndef _TOOLS_H_
#define _TOOLS_H_

/****************************************************************************/

#define _GNU_SOURCE 1

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <math.h>
#include <syslog.h>
#include <sys/ioctl.h>
#ifdef linux
#include <sys/kd.h>
#include <linux/isdn.h>
#else
#include <libgen.h>
#endif
#ifdef DBMALLOC
#include "dbmalloc.h"
#endif

/****************************************************************************/

#include "policy.h"
#include "libisdn.h"

/****************************************************************************/

#ifndef OLDCONFFILE
#	define OLDCONFFILE "isdnlog.conf"
#endif

#ifndef RELOADCMD
#	define RELOADCMD "reload"
#endif

#ifndef STOPCMD
#	define STOPCMD "stop"
#endif

#ifndef REBOOTCMD
#	define REBOOTCMD "/sbin/reboot"
#endif

#ifndef LOGFILE
#	define LOGFILE "/sbin/reboot"
#endif

/****************************************************************************/

#define LOG_VERSION "3.1"

/****************************************************************************/

#undef  min
#undef  max
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#define	abs(x)		(((x) < 0) ? -(x) : (x))

/****************************************************************************/

#define MAXDIG 128
#define GETARG(typ)     va_arg(ap, typ)

/****************************************************************************/

#define NIL  (char **)NULL

/****************************************************************************/

#define NUMSIZE      20
#define	FNSIZE	     64
#define RETSIZE     128
#define MAXRET	      5
#define MAXZONES      6
#define MAXCHAN       7
#define MAXCARDS      2

#define DIGITS 	     17
#define DEB           1

#define MAXUNKNOWN   50
#define MAXCONNECTS  50

/****************************************************************************/

#define CALLING       0
#define CALLED        1
#define DATETIME      2

/****************************************************************************/

#define DIALOUT	      0
#define DIALIN	      1

/****************************************************************************/

#define ALERTING               0x01
#define CALL_PROCEEDING	       0x02
#define SETUP                  0x05
#define SETUP_ACKNOWLEDGE      0x0d
#define SUSPEND                0x25
#define SUSPEND_ACKNOWLEDGE    0x2d
#define RESUME                 0x26
#define RESUME_ACKNOWLEDGE     0x2e
#define CONNECT                0x07
#define CONNECT_ACKNOWLEDGE    0x0f
#define FACILITY               0x62
#define NOTIFY                 0x6e
#define STATUS                 0x7d
#define MAKEL_ACKNOWLEDGE      0x28
#define MAKEL_RESUME_ACK       0x33
#define DISCONNECT             0x45
#define RELEASE                0x4d
#define RELEASE_COMPLETE       0x5a
#define INFORMATION	       0x7b
#define AOCD_1TR6              0x6d

/****************************************************************************/

#define AOC_UNITS	        0
#define AOC_AMOUNT              1

/****************************************************************************/

#define	RING_INCOMING    1
#define RING_OUTGOING    2
#define	RING_RING        4
#define RING_CONNECT     8
#define RING_BUSY       16
#define RING_AOCD       32
#define RING_ERROR      64
#define RING_HANGUP    128
#define	RING_KILL      256
#define RING_SPEAK     512
#define RING_PROVIDER 1024
#define RING_LOOP     2048
#define RING_UNIQUE   4096
#define RING_INTERVAL 8192

/****************************************************************************/

#define STATE_RING		  1 /* "Telefonklingeln" ... jemand ruft an, oder man selbst ruft raus */
#define STATE_CONNECT	  	  2 /* Verbindung */
#define STATE_HANGUP	  	  3 /* Verbindung beendet */
#define STATE_AOCD		100 /* Gebuehrenimpuls _waehrend_ der Verbindung */
#define	STATE_CAUSE		101 /* Aussergewoehnliche Cause-Meldungen von der VSt */
#define STATE_TIME		102 /* Uhrzeit-Meldung von der VSt */
#define STATE_BYTE		103 /* Durchsatz-Meldung von Frank (Byte/s/B-Kanal) */
#define	STATE_HUPTIMEOUT	104 /* Wechsel des hangup-Timer's  */

/****************************************************************************/

#define AOC_OTHER          -999999L

/****************************************************************************/

#define QCMASK  	       0377
#define QUOTE   	       0200
#define QMASK      (QCMASK &~QUOTE)
#define NOT                     '!'

#define	AVON	             "avon"
#define INFO	    "/dev/isdninfo"

#define	BIGBUFSIZ              2048

/****************************************************************************/

#define VAR_START      "START"
#define VAR_MYMSNS     "MYMSNS"
#define VAR_MYCOUNTRY  "MYAREA"
#define VAR_MYAREA     "MYPREFIX"
#define VAR_CURRENCY   "CURRENCY"
#define VAR_ILABEL     "ILABEL"
#define VAR_OLABEL     "OLABEL"
#define VAR_CHARGEMAX  "CHARGEMAX"
#define VAR_CONNECTMAX "CONNECTMAX"
#define VAR_BYTEMAX    "BYTEMAX"

/****************************************************************************/

#define VERSION_UNKNOWN       0
#define	VERSION_EDSS1	      1
#define	VERSION_1TR6	      2

#define DEF_NUM_MSN 3

/****************************************************************************/

#define  OTHER (call[chan].dialin ? CALLING : CALLED)
#define  ME    (call[chan].dialin ? CALLED : CALLING)
#define	 CLIP  2
#define	 REDIR 3
#define  _OTHER(call) (call->dialin ? CALLING : CALLED)
#define  _ME(call)    (call->dialin ? CALLED : CALLING)

#define	 MAXMSNS  (REDIR + 1)

/****************************************************************************/

#define SHORT_STRING_SIZE      256
#define LONG_STRING_SIZE      1024
#define BUF_SIZE              4096

/****************************************************************************/

/* Keywords for parameter file */

#define CONF_SEC_OPT     "OPTIONS"
#define CONF_ENT_DEV     "DEVICE"
#define CONF_ENT_LOG     "LOG"
#define CONF_ENT_FLUSH   "FLUSH"
#define CONF_ENT_PORT    "PORT"
#define CONF_ENT_STDOUT  "STDOUT"
#define CONF_ENT_SYSLOG  "SYSLOG"
#define CONF_ENT_XISDN   "XISDN"
#define CONF_ENT_TIME    "TIME"
#define CONF_ENT_CON     "CONSOLE"
#define CONF_ENT_START   "START"
#define CONF_ENT_THRU    "THRUPUT"
#define CONF_ENT_DAEMON  "DAEMON"
#define CONF_ENT_PIPE    "PIPE"
#define CONF_ENT_BI      "BILINGUAL"
#define CONF_ENT_MON     "MONITOR"
#define CONF_ENT_HANGUP  "HANGUP"
#define CONF_ENT_CALLS   "CALLS"
#define CONF_ENT_XLOG    "XLOG"
#define CONF_ENT_NL			 "NEWLINE"
#define CONF_ENT_WIDTH	 "WIDTH"
#define CONF_ENT_WD			 "WATCHDOG"
#define CONF_ENT_CW			 "CITYWEEKEND"
#define CONF_ENT_AMT		 "AMT"
#define CONF_ENT_DUAL		 "DUAL"
#define CONF_ENT_Q931		 "Q931DUMP"
#define CONF_ENT_OUTFILE "OUTFILE"
#define CONF_ENT_KEYBOARD "KEYBOARD"
#define CONF_ENT_INTERNS0 "INTERNS0"
#define CONF_ENT_PRESELECT "PRESELECTED"
#define	CONF_ENT_TRIM	   "TRIM"
#define	CONF_ENT_OTHER	   "OTHER"

/****************************************************************************/

/* Keywords for isdn.conf */

#define CONF_SEC_ISDNLOG  "ISDNLOG"
#define CONF_ENT_CHARGE   "CHARGEMAX"
#define CONF_ENT_CONNECT  "CONNECTMAX"
#define CONF_ENT_BYTE     "BYTEMAX"
#define CONF_ENT_CURR     "CURRENCY"
#define CONF_ENT_ILABEL   "ILABEL"
#define CONF_ENT_OLABEL   "OLABEL"
#define CONF_ENT_RELOAD   "RELOADCMD"
#define CONF_ENT_STOP     "STOPCMD"
#define CONF_ENT_REBOOT   "REBOOTCMD"
#define CONF_ENT_LOGFILE  "LOGFILE"

#define CONF_SEC_START    "START"
#define CONF_SEC_FLAG     "FLAG"
#define CONF_ENT_FLAGS    "FLAGS"
#define CONF_ENT_PROG     "PROGRAM"
#define CONF_ENT_USER     "USER"
#define CONF_ENT_GROUP    "GROUP"
#define CONF_ENT_INTVAL   "INTERVAL"
#define CONF_ENT_TIME     "TIME"

#define CONF_ENT_REPFMT   "REPFMT"

#define CONF_ENT_CALLFILE "CALLFILE"
#define CONF_ENT_CALLFMT  "CALLFMT"

#define CONF_ENT_VBOXVER  "VBOXVERSION"
#define CONF_ENT_VBOXPATH "VBOXPATH"
#define CONF_ENT_VBOXCMD1 "VBOXCMD1"
#define CONF_ENT_VBOXCMD2 "VBOXCMD2"
#define CONF_ENT_MGTYVER  "MGETTYVERSION"
#define CONF_ENT_MGTYPATH "MGETTYPATH"
#define CONF_ENT_MGTYCMD  "MGETTYCMD"

/****************************************************************************/

#define PRT_ERR                1
#define PRT_WARN               2
#define PRT_INFO               4
#define PRT_PROG_OUT           4
#define PRT_NORMAL             4
#define PRT_LOG                8

/****************************************************************************/

#define NO_MSN       -1

#define C_FLAG_DELIM '|'

/****************************************************************************/

#define C_UNKNOWN '?'
#define S_UNKNOWN "UNKNOWN"

/****************************************************************************/

#define S_QUOTES "\\$@;,#"

/****************************************************************************/

#define TYPE_STRING   0
#define TYPE_MESSAGE  1
#define TYPE_ELEMENT  2
#define TYPE_CAUSE    3
#define TYPE_SERVICE  4

/****************************************************************************/

typedef struct {
  int	  state;
  int     cref;
  int     tei;
  int	  dialin;
  int	  cause;
  int	  loc;
  int	  aoce;
  int	  traffic;
  int	  channel;
  int	  dialog;
  int     bearer;
  int	  si1;     /* Service Indicator entsprechend i4l convention */
  int	  si11;	   /* if (si1 == 1) :: 0 = Telefon analog / 1 = Telefon digital */
  char    onum[MAXMSNS][NUMSIZE];
  int	  screening;
  char    num[MAXMSNS][NUMSIZE];
  char    vnum[MAXMSNS][256];
  int	  provider;
  int	  sondernummer[MAXMSNS];
  int	  intern[MAXMSNS];
  char    id[32];
  char	  usage[16];
  int	  confentry[MAXMSNS];
  time_t  time;
  time_t  connect;
  time_t  t_duration;
  time_t  disconnect;
  clock_t duration;
  int     cur_event;
  long	  ibytes;
  long	  obytes;
  long	  libytes;
  long	  lobytes;
  double  ibps;
  double  obps;
  char	  areacode[MAXMSNS][NUMSIZE];
  char	  vorwahl[MAXMSNS][NUMSIZE];
  char	  rufnummer[MAXMSNS][NUMSIZE];
  char	  alias[MAXMSNS][NUMSIZE];
  char	  area[MAXMSNS][128];
  char	  money[64];
  char	  currency[32];
  char    msg[128];
  int     stat;
  int	  version;
  int	  bchan;
  double  tick;
  int	  chargeint;
  int     huptimeout;
  char	  service[32];
  double  pay;
  char	  digits[NUMSIZE];
  int	  oc3;
  int	  takteChargeInt;
  int 	  card;
  int	  knock; 
  time_t  nextcint;
  float	  cint;
  int     cinth;
  int	  ctakt;
  int	  zone;
  int	  uid;
} CALL;

/****************************************************************************/

typedef struct {
  int	  flag;
  char *time;
  char *infoarg;
  int   interval;
  char *user;
  char *group;
/*  char *service; */
} info_args;

/****************************************************************************/

typedef struct {
  char   *num;
  char	 *who;
  int	  zone;
  int	  flags;
  int     si;
  char	 *interface;
  info_args **infoargs;
  /* above from "isdnlog.conf" */
  int	  usage[2];
  double  dur[2];
  int     eh;
  double  dm;
  double  charge;
  double  rcharge;
  double  scharge;
  int	  day;
  int	  month;
  double  online;
  double  sonline;
  double  bytes;
  double  sbytes;
  double  ibytes[2];
  double  obytes[2];
} KNOWN;

/****************************************************************************/

typedef struct {
  int    in;
  int    out;
  int    eh;
  int    err;
  double din;
  double dout;
  double dm;
  long	 ibytes;
  long	 obytes;
} sum_calls;

/****************************************************************************/

typedef struct {
  int    eh;
  int    cause;
  time_t t;
  int    dir;
  double duration;
  double dm;
  char   num[2][NUMSIZE];
  char   who[2][NUMSIZE];
  long	 ibytes;
  long	 obytes;
  char   version[10];
  int	 si;
  int	 si1;
  double currency_factor;
  char	 currency[32];
  double pay;
  int	 provider;
} one_call;

/****************************************************************************/

typedef struct {
  unsigned long i;
  unsigned long o;
} IO;

/****************************************************************************/

typedef struct {
  char id[20];
  int  ch;
  int  dr;
  int  u;
  int  f;
  char n[20];
} IFO;

/****************************************************************************/

typedef struct {
  char  *msn;      /* Telefonnummer */
  char  *sinfo;    /* Kurzbeschreibung */
  int    tarif;    /* 0 = free, 1 = CityCall, -1 = see grund1 .. takt2 */
  double grund1;   /* Grundtarif Werktage 9-18 Uhr */
  double grund2;   /* Grundtarif uebrige Zeit */
  double takt1;	   /* Zeittakt Werktage 9-18 Uhr */
  double takt2;	   /* Zeittakt uebrige Zeit */
} SonderNummern;

/****************************************************************************/

#ifdef _TOOLS_C_
#define _EXTERN
#else
#define _EXTERN extern

_EXTERN char     Months[][4];

#endif /* _TOOLS_C_ */

_EXTERN KNOWN    start_procs;
_EXTERN KNOWN  **known;
_EXTERN int      mymsns;
_EXTERN int      knowns;
_EXTERN int	currency_mode;
_EXTERN double   currency_factor;
_EXTERN double   chargemax;
_EXTERN double   connectmax;
_EXTERN double   bytemax;
_EXTERN int   	connectmaxmode;
_EXTERN int   	bytemaxmode;
_EXTERN char    *currency;
_EXTERN int	 hour;
_EXTERN int      day;
_EXTERN int      month;
_EXTERN int      retnum;
_EXTERN int      ln;
_EXTERN char     retstr[MAXRET + 1][RETSIZE];
_EXTERN time_t   cur_time;
_EXTERN section *conf_dat;
_EXTERN char     ilabel[256];
_EXTERN char    	olabel[256];
_EXTERN char    	idate[256];
_EXTERN CALL    	call[MAXCHAN];
#ifdef Q931
_EXTERN int     	q931dmp;
#endif
_EXTERN int     	CityWeekend;
_EXTERN int	dual;
_EXTERN char    	mlabel[BUFSIZ];
_EXTERN char    *amtsholung;
_EXTERN SonderNummern *SN;
_EXTERN int	      nSN;
#undef _EXTERN

/****************************************************************************/

extern int   optind, errno;
extern char *optarg;

/****************************************************************************/

#ifdef _TOOLS_C_
#define _EXTERN

_EXTERN char* reloadcmd = RELOADCMD;
_EXTERN char* stopcmd   = STOPCMD;
_EXTERN char* rebootcmd = REBOOTCMD;
_EXTERN char* logfile   = LOGFILE;
_EXTERN char* callfile  = NULL;
_EXTERN char* callfmt   = NULL;
_EXTERN int  (*_print_msg)(const char *, ...) = printf;
_EXTERN int   use_new_config = 1;
_EXTERN char ***lineformats = NULL;
_EXTERN char *vboxpath      = NULL;
_EXTERN char *vboxcommand1  = NULL;
_EXTERN char *vboxcommand2  = NULL;
_EXTERN char *mgettypath    = NULL;
_EXTERN char *mgettycommand = NULL;

#else
#define _EXTERN extern

_EXTERN char* reloadcmd;
_EXTERN char* stopcmd;
_EXTERN char* rebootcmd;
_EXTERN char* logfile;
_EXTERN char* callfile;
_EXTERN char* callfmt;
_EXTERN int  (*_print_msg)(const char *, ...);
_EXTERN int   use_new_config;
_EXTERN char ***lineformats;
_EXTERN char *vboxpath;
_EXTERN char *vboxcommand1;
_EXTERN char *vboxcommand2;
_EXTERN char *mgettypath;
_EXTERN char *mgettycommand;

#endif

_EXTERN void set_print_fct_for_tools(int (*new_print_msg)(const char *, ...));
_EXTERN int    print_version(char *myname);
_EXTERN time_t atom(register char *p);
_EXTERN char  *num2nam(char *num, int si);
_EXTERN char  *double2str(double n, int l, int d, int flags);
_EXTERN char  *double2byte(double bytes);
_EXTERN char  *time2str(time_t sec);
_EXTERN char  *double2clock(double n);
_EXTERN char  *vnum(int chan, int who);
_EXTERN char  *i2a(int n, int l, int base);
_EXTERN char  *Providername(int number);
_EXTERN int    iprintf(char *obuf, int chan, register char *fmt, ...);
_EXTERN char  *qmsg(int type, int version, int val);
_EXTERN char  *Myname;

#undef _EXTERN

/****************************************************************************/

#ifdef _ISDNCONF_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN int    readconfig(char *myname);
_EXTERN void   setDefaults(void);
_EXTERN void   discardconfig(void);
_EXTERN char  *t2tz(int zeit);
_EXTERN char  *z2s(int zone);
_EXTERN section *read_isdnconf(section **_conf_dat);

#undef _EXTERN

/****************************************************************************/

#endif /* _TOOLS_H_ */

