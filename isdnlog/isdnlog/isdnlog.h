/* $Id: isdnlog.h,v 1.14 1998/12/09 20:39:30 akool Exp $
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
 * $Log: isdnlog.h,v $
 * Revision 1.14  1998/12/09 20:39:30  akool
 *  - new option "-0x:y" for leading zero stripping on internal S0-Bus
 *  - new option "-o" to suppress causes of other ISDN-Equipment
 *  - more support for the internal S0-bus
 *  - Patches from Jochen Erwied <mack@Joker.E.Ruhr.DE>, fixes TelDaFax Tarif
 *  - workaround from Sebastian Kanthak <sebastian.kanthak@muehlheim.de>
 *  - new CHARGEINT chapter in the README from
 *    "Georg v.Zezschwitz" <gvz@popocate.hamburg.pop.de>
 *
 * Revision 1.13  1998/11/24 20:51:35  akool
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
 * Revision 1.12  1998/11/01 08:49:47  akool
 *  - fixed "configure.in" problem with NATION_*
 *  - DESTDIR fixes (many thanks to Michael Reinelt <reinelt@eunet.at>)
 *  - isdnrep: Outgoing calls ordered by Zone/Provider/MSN corrected
 *  - new Switch "-i" -> running on internal S0-Bus
 *  - more providers
 *  - "sonderrufnummern.dat" extended (Frag Fred, Telegate ...)
 *  - added AVM-B1 to the documentation
 *  - removed the word "Teles" from the whole documentation ;-)
 *
 * Revision 1.11  1998/10/18 20:13:37  luethje
 * isdnlog: Added the switch -K
 *
 * Revision 1.10  1998/09/26 18:29:10  akool
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
 * Revision 1.9  1998/06/21 11:52:47  akool
 * First step to let isdnlog generate his own AOCD messages
 *
 * Revision 1.8  1998/06/07 21:08:34  akool
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
 * Revision 1.7  1997/05/25 19:41:02  luethje
 * isdnlog:  close all files and open again after kill -HUP
 * isdnrep:  support vbox version 2.0
 * isdnconf: changes by Roderich Schupp <roderich@syntec.m.EUnet.de>
 * conffile: ignore spaces at the end of a line
 *
 * Revision 1.6  1997/05/09 23:30:50  luethje
 * isdnlog: new switch -O
 * isdnrep: new format %S
 * bugfix in handle_runfiles()
 *
 * Revision 1.5  1997/04/08 00:02:17  luethje
 * Bugfix: isdnlog is running again ;-)
 * isdnlog creates now a file like /var/lock/LCK..isdnctrl0
 * README completed
 * Added some values (countrycode, areacode, lock dir and lock file) to
 * the global menu
 *
 * Revision 1.4  1997/04/03 22:34:50  luethje
 * splitt the files callerid.conf and ~/.isdn.
 *
 * Revision 1.3  1997/03/23 23:11:54  luethje
 * improved performance
 *
 * Revision 1.2  1997/03/23 20:55:41  luethje
 * some bugfixes
 *
 * Revision 1.1  1997/03/16 20:58:43  luethje
 * Added the source code isdnlog. isdnlog is not working yet.
 * A workaround for that problem:
 * copy lib/policy.h into the root directory of isdn4k-utils.
 *
 * Revision 2.6.36  1997/02/11  18:21:43  akool
 * isdnctrl2 implemented
 *
 * Revision 2.6.30  1997/01/28  16:49:43  akool
 * LOG_MAJOR_VERSION auf 3 erhoeht
 *
 * Revision 2.6.26  1997/01/18  12:56:43  akool
 * huptimeofs neu definiert
 *
 * Revision 2.3.4  1996/04/05  11:30:43  akool
 * New ibytes/obytes ioctl() from Fritz implemented
 * USERFILE
 *
 * Revision 2.23  1996/03/14  18:07:43  akool
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

#ifndef _ISDNLOG_H_
#define _ISDNLOG_H_

/****************************************************************************/

#define PUBLIC extern

/****************************************************************************/

#include <tools.h>
#include "socket.h"

/****************************************************************************/

#define MAX_CALLS_IN_QUEUE    100
#define MAX_PRINTS_IN_QUEUE   500

/****************************************************************************/

#define BROADCAST   0x7f

/****************************************************************************/

#define ISDNCTRL    0
#define ISDNCTRL2   1
#define ISDNINFO    2
#define	STDIN	    3
#define IN_PORT     4

#define FIRST_DESCR 4

/****************************************************************************/

#define PRT_SHOWNUMBERS	  	0x10
#define PRT_SHOWAOCD   	  	0x20
#define PRT_SHOWCONNECT	  	0x40
#define PRT_SHOWHANGUP 	  	0x80
#define PRT_SHOWCAUSE  	       0x100
#define PRT_SHOWTIME   	       0x200
#define PRT_SHOWBYTE   	       0x400
#define	PRT_SHOWIMON	       0x800
#define PRT_SHOWBEARER	      0x1000
#define	PRT_SHOWTICKS	      0x2000
#define PRT_SHOWCHARGEMAX     0x4000
#define PRT_DEBUG_GENERAL     0x8000
#define PRT_DEBUG_DIAG       0x10000
#define PRT_DEBUG_INFO       0x20000
#define PRT_DEBUG_EXEC       0x40000
#define PRT_DEBUG_BUGS       0x80000
#define PRT_DEBUG_DECODE    0x100000
#define PRT_DEBUG_RING      0x200000
#define PRT_DEBUG_CS	    0x200000
#define PRT_DEBUG_PROT      0x800000
#define PRT_NOTHING        0x1000000

#define IS_DEBUG(VALUE) (VALUE >= PRT_DEBUG_GENERAL && VALUE < PRT_NOTHING)

/****************************************************************************/

#define AOC_INITIAL             0
#define AOCDCurrency            1
#define AOCDChargingUnit        2
#define AOCECurrency            3
#define AOCEChargingUnit        4
#define AOCDCurrencyInfo        5
#define AOCECurrencyInfo        6
#define AOCDChargingUnitInfo    7
#define AOCEChargingUnitInfo    8
#define RecordedCurrency       	9
#define TypeOfChargingInfo     10
#define Currency               11
#define Amount                 12
#define CurrencyAmount         13
#define Multiplier             14
#define RecordedUnitsList      15
#define RecordedUnits          16
#define NumberOfUnits          17

/****************************************************************************/

#define VERBOSE_HEX	    1  /* only "HEX:" messages from /dev/isdnctrl */
#define VERBOSE_CTRL	    2  /* any message from /dev/isdnctrl */
#define	VERBOSE_INFO	    4  /* any message from /dev/isdninfo */
#define VERBOSE_RATE	    8  /* any message from ioctl(IIOCGETCPS) */

/****************************************************************************/

#ifndef USERFILE
#define USERFILE "isdnlog.users"
#endif

/****************************************************************************/

typedef struct _interval {
	int        event;
	int        chan;
	info_args *infoarg;
	time_t     next_start;
	struct _interval *next;
} interval;

/****************************************************************************/

#ifdef _ISDNLOG_C_
#define _EXTERN
socket_queue *sockets = NULL;
#else
#define _EXTERN extern
extern socket_queue *sockets;
#endif

_EXTERN FILE   *flog;    /* /var/adm/isdn.log          */
_EXTERN FILE   *fcons;   /* /dev/ttyX      (or stderr) */
_EXTERN FILE   *fprot;   /* /tmp/isdnctrl0 	       */
_EXTERN FILE   *fout;    /* outfile 	       */

_EXTERN int     first_descr;
_EXTERN int     chan;
_EXTERN int     message;
_EXTERN int     syslogmessage;
_EXTERN int     xinfo;
_EXTERN int     sound;
_EXTERN int     trace;
_EXTERN int     isdaemon;
_EXTERN int     imon;
_EXTERN int     port;
_EXTERN int     wakeup;
_EXTERN	int	fullhour;
_EXTERN	int	tty_dv;
_EXTERN	int	net_dv;
_EXTERN	int	inf_dv;
_EXTERN clock_t tt;
_EXTERN clock_t tto;
_EXTERN char    st[FNSIZE];
_EXTERN char    stl[FNSIZE];
_EXTERN int     settime;
_EXTERN int     replay;
_EXTERN int     replaydev;
_EXTERN int     verbose;
_EXTERN int     synclog;
_EXTERN int     any;
_EXTERN int     stdoutput;
_EXTERN int     allflags;
_EXTERN int     newcps;
_EXTERN int     chans;
_EXTERN int     bilingual;
_EXTERN int  	hupctrl;
_EXTERN int  	hup1;
_EXTERN int  	hup2;
_EXTERN int     trim;
_EXTERN int     trimi;
_EXTERN int     trimo;
_EXTERN int     mcalls;
_EXTERN int     xlog;
_EXTERN char   *myname, *myshortname;
_EXTERN int     newline;
_EXTERN int	width;
_EXTERN	int	watchdog;
_EXTERN char   *isdnctrl;
_EXTERN	char    isdnctrl2[FNSIZE];
_EXTERN	char   *outfile;
_EXTERN	char    tmpout[PATH_MAX];
_EXTERN int     readkeyboard;
_EXTERN	int     interns0;
_EXTERN	int	preselect;
_EXTERN	int	other;
_EXTERN IFO     ifo[ISDN_MAX_CHANNELS];
_EXTERN IO      io[ISDN_MAX_CHANNELS];

#undef _EXTERN

/****************************************************************************/

#ifdef _PROCESSOR_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN void dotrace(void);
_EXTERN int  morectrl(int card);
_EXTERN void moreinfo(void);
_EXTERN void morekbd(void);
_EXTERN void processcint(void);
_EXTERN void processrate(void);
_EXTERN void clearchan(int chan, int total);

#undef _EXTERN

/****************************************************************************/

#ifdef _FUNCTIONS_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

#define Exit(a) _Exit(__FILE__,__LINE__,a)

_EXTERN void _Exit(char *File, int Line, int RetCode);
_EXTERN int  print_msg(int Level, const char *fmt, ...);
_EXTERN int  Change_Channel(int old_channel, int new_channel);
_EXTERN void set_time_str(void);
_EXTERN void now(void);
_EXTERN void logger(int chan);
_EXTERN int  ringer(int chan, int event);
_EXTERN void initSondernummern(void);
_EXTERN int  is_sondernummer(char *num);

#undef _EXTERN

/****************************************************************************/

#ifdef _SERVER_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN int message_from_server(CALL *call, int chan);
_EXTERN int eval_message(int sock);
_EXTERN int start_server(int port);
_EXTERN int listening(int port);
_EXTERN int print_from_server(char *String);
_EXTERN int change_channel(int old_chan, int new_chan);
_EXTERN int disconnect_client(int sock);

#undef _EXTERN

/****************************************************************************/

#ifdef _START_PROG_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN int Ring(info_args *, char *[], int, int);
_EXTERN void  Alarm(void);
_EXTERN int   CheckTime(char *);
_EXTERN int   Print_Cmd_Output( int sock );
_EXTERN int Get_Sock_From_Call_And_Info_Args( int chan, info_args *Ptr, int Cnt );
_EXTERN int Get_Sock_From_Call( int chan, int Cnt );
_EXTERN int Get_Sock_From_Info_Args( info_args *Ptr, int Cnt );
_EXTERN int Condition_Changed( int condition, int flag );
_EXTERN const char *Set_Ringer_Flags( int condtion, int InOut );
_EXTERN int Start_Ring(int chan, info_args *infoarg, int event, int intervalflag);
_EXTERN int Start_Process(int chan, info_args *infoarg, int event);
_EXTERN int New_Interval(int chan, info_args *infoarg, int event);
_EXTERN int Del_Interval(int chan, info_args *infoarg);
_EXTERN struct timeval *Get_Interval(int Sec);
_EXTERN int Change_Channel_Ring( int old_channel, int new_channel);
_EXTERN int Start_Interval(void);

#undef _EXTERN

/****************************************************************************/

#ifdef _USER_ACCESS_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN int read_user_access( void );
_EXTERN int write_user_access( void );
_EXTERN int user_has_access(char *User, char *Host);
_EXTERN int User_Get_Message(char *User, char *Host, char* mymsn, int Flag);
_EXTERN const char *userfile(void);

#undef _EXTERN

/****************************************************************************/

#endif /* _ISDNLOG_H_ */
