/* $Id: tools.h,v 1.14 1997/05/15 22:21:49 luethje Exp $
 *
 * ISDN accounting for isdn4linux.
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
 * $Log: tools.h,v $
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

#define LOG_VERSION "3.0"

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
#define  _OTHER(call) (call->dialin ? CALLING : CALLED)
#define  _ME(call)    (call->dialin ? CALLED : CALLING)

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

#define CONF_ENT_VBOXVER  "VBOXVERSION"
#define CONF_ENT_VBOXPATH "VBOXPATH"
#define CONF_ENT_VBOXCMD  "VBOXCMD"
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
  int	  aoce;
  int	  traffic;
  int	  channel;
  int	  dialog;
  int     bearer;
  int	  si1;     /* Service Indicator entsprechend i4l convention */
  int	  si11;	   /* if (si1 == 1) :: 0 = Telefon analog / 1 = Telefon digital */
  char    onum[3][NUMSIZE];
  int	  screening;
  char    num[3][NUMSIZE];
  char    vnum[3][256];
  char    id[32];
  char	  usage[16];
  int	  confentry[3];
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
  char	  areacode[3][NUMSIZE];
  char	  vorwahl[3][NUMSIZE];
  char	  rufnummer[3][NUMSIZE];
  char	  alias[3][NUMSIZE];
  char	  area[3][128];
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
  int	  aoc;
  int 	  card;
  int	  knock; 
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

PUBLIC KNOWN    start_procs;
PUBLIC KNOWN  **known;
PUBLIC int      mymsns;
PUBLIC int      knowns;
PUBLIC int	currency_mode;
PUBLIC double   currency_factor;
PUBLIC double   chargemax;
PUBLIC double   connectmax;
PUBLIC double   bytemax;
PUBLIC int   	connectmaxmode;
PUBLIC int   	bytemaxmode;
PUBLIC char    *currency;
PUBLIC int      day;
PUBLIC int      month;
PUBLIC int      retnum;
PUBLIC int      ln;
PUBLIC char     retstr[MAXRET + 1][RETSIZE];
PUBLIC char     Months[][4];
PUBLIC time_t   cur_time;
PUBLIC section *conf_dat;
PUBLIC char     ilabel[256];
PUBLIC char    	olabel[256];
PUBLIC char    	idate[256];
PUBLIC CALL    	call[MAXCHAN];
#ifdef Q931
PUBLIC int     	q931dmp;
#endif
PUBLIC int     	CityWeekend;
PUBLIC int	dual;
PUBLIC char    	mlabel[BUFSIZ];
PUBLIC char    *amtsholung;

/****************************************************************************/

extern int   optind, errno;
extern char *optarg;

/****************************************************************************/

#ifdef _TOOLS_C_
#define _EXTERN
#define _EXTERN

_EXTERN char* reloadcmd = RELOADCMD;
_EXTERN char* stopcmd   = STOPCMD;
_EXTERN char* rebootcmd = REBOOTCMD;
_EXTERN char* logfile   = LOGFILE;
_EXTERN int  (*_print_msg)(const char *, ...) = printf;
_EXTERN int   use_new_config = 1;
_EXTERN char ***lineformats = NULL;
_EXTERN int   vboxversion   = 0;
_EXTERN char *vboxpath      = NULL;
_EXTERN char *vboxcommand   = NULL;
_EXTERN int   mgettyversion = 0;
_EXTERN char *mgettypath    = NULL;
_EXTERN char *mgettycommand = NULL;

#else
#define _EXTERN extern

_EXTERN char* reloadcmd;
_EXTERN char* stopcmd;
_EXTERN char* rebootcmd;
_EXTERN char* logfile;
_EXTERN int  (*_print_msg)(const char *, ...);
_EXTERN int   use_new_config;
_EXTERN char ***lineformats;
_EXTERN int   vboxversion;
_EXTERN char *vboxpath;
_EXTERN char *vboxcommand;
_EXTERN int   mgettyversion;
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

