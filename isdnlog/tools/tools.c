/* $Id: tools.c,v 1.17 1999/01/10 15:24:31 akool Exp $
 *
 * ISDN accounting for isdn4linux. (Utilities)
 *
 * Copyright 1995, 1999 by Andreas Kool (akool@isdn4linux.de)
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
 * $Log: tools.c,v $
 * Revision 1.17  1999/01/10 15:24:31  akool
 *  - "message = 0" bug fixed (many thanks to
 *    Sebastian Kanthak <sebastian.kanthak@muehlheim.de>)
 *  - CITYWEEKEND via config-file possible
 *  - fixes from Michael Reinelt <reinelt@eunet.at>
 *  - fix a typo in the README from Sascha Ziemann <szi@aibon.ping.de>
 *  - Charge for .at optimized by Michael Reinelt <reinelt@eunet.at>
 *  - first alpha-Version of the new chargeinfo-Database
 *    ATTENTION: This version requires the following manual steps:
 *      cp /usr/src/isdn4k-utils/isdnlog/tarif.dat /usr/lib/isdn
 *      cp /usr/src/isdn4k-utils/isdnlog/samples/tarif.conf /etc/isdn
 *
 * Revision 1.16  1998/12/09 20:40:19  akool
 *  - new option "-0x:y" for leading zero stripping on internal S0-Bus
 *  - new option "-o" to suppress causes of other ISDN-Equipment
 *  - more support for the internal S0-bus
 *  - Patches from Jochen Erwied <mack@Joker.E.Ruhr.DE>, fixes TelDaFax Tarif
 *  - workaround from Sebastian Kanthak <sebastian.kanthak@muehlheim.de>
 *  - new CHARGEINT chapter in the README from
 *    "Georg v.Zezschwitz" <gvz@popocate.hamburg.pop.de>
 *
 * Revision 1.15  1998/11/24 20:53:07  akool
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
 * Revision 1.14  1998/09/26 18:30:14  akool
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
 * Revision 1.13  1998/06/21 11:53:23  akool
 * First step to let isdnlog generate his own AOCD messages
 *
 * Revision 1.12  1998/06/07 21:09:57  akool
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
 * Revision 1.11  1998/05/06 14:43:27  paul
 * Assumption about country codes always being 2 digits long fixed for the
 * USA case (caused strncpy to be called with length -1; ouch).
 *
 * Revision 1.10  1998/04/09 19:15:45  akool
 *  - CityPlus Implementation from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 *  - dont change huptimeout, if disabled (via isdnctrl huptimeout isdnX 0)
 *  - Support for more Providers (TelePassport, Tele 2, TelDaFax)
 *
 * Revision 1.9  1998/03/08 11:43:16  luethje
 * I4L-Meeting Wuerzburg final Edition, golden code - Service Pack number One
 *
 * Revision 1.8  1997/05/11 22:41:43  luethje
 * README completed
 * changed the E-mail address for the switch -V
 *
 * Revision 1.7  1997/04/16 22:23:04  luethje
 * some bugfixes, README completed
 *
 * Revision 1.6  1997/04/08 21:56:59  luethje
 * Create the file isdn.conf
 * some bug fixes for pid and lock file
 * make the prefix of the code in `isdn.conf' variable
 *
 * Revision 1.5  1997/04/06 21:17:46  luethje
 * Bugfix von Andreas Jaeger.
 *
 * Revision 1.4  1997/04/03 22:40:21  luethje
 * some bugfixes.
 *
 * Revision 1.3  1997/03/31 22:15:32  akool
 * added support for the new glibc 2.0.x (aka libc 6.0)
 * changed "HOWTO" to reflect the current stage of development
 *
 * Revision 1.2  1997/03/29 09:24:33  akool
 * CLIP presentation enhanced, new ILABEL/OLABEL operators
 *
 * Revision 1.1  1997/03/16 20:59:24  luethje
 * Added the source code isdnlog. isdnlog is not working yet.
 * A workaround for that problem:
 * copy lib/policy.h into the root directory of isdn4k-utils.
 *
 * Revision 2.6.26  1997/01/19  22:23:43  akool
 * Weitere well-known number's hinzugefuegt
 *
 * Revision 2.6.24  1997/01/15  19:13:43  akool
 * neue AreaCode Lib 0.99 integriert
 *
 * Revision 2.6.20  1997/01/05  20:06:43  akool
 * atom() erkennt nun "non isdnlog" "/tmp/isdnctrl0" Output's
 *
 * Revision 2.6.19  1997/01/05  19:39:43  akool
 * LIBAREA Support added
 *
 * Revision 2.40    1996/06/16  10:06:43  akool
 * double2byte(), time2str() added
 *
 * Revision 2.3.26  1996/05/05  12:09:16  akool
 * known.interface added
 *
 * Revision 2.3.15  1996/04/22  21:10:16  akool
 *
 * Revision 2.3.4  1996/04/05  11:12:16  sl
 * confdir()
 *
 * Revision 2.2.5  1996/03/25  19:41:16  akool
 * 1TR6 causes implemented
 *
 * Revision 2.23  1996/03/14  20:29:16  akool
 * Neue Routine i2a()
 *
 * Revision 2.17  1996/02/25  19:14:16  akool
 * Soft-Error in atom() abgefangen
 *
 * Revision 2.06  1996/02/07  18:49:16  akool
 * AVON-Handling implementiert
 *
 * Revision 2.01  1996/01/20  12:11:16  akool
 * Um Einlesen der neuen isdnlog.conf Felder erweitert
 * discardconfig() implementiert
 *
 * Revision 2.00  1996/01/10  20:11:16  akool
 *
 */

/****************************************************************************/

#define  _TOOLS_C_

/****************************************************************************/

#include "tools.h"

/****************************************************************************/

/*static char *cclass(register char *p, register int sub);*/

/****************************************************************************/

char Months[][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
       	    	     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/****************************************************************************/

static int  cnf;

/****************************************************************************/

void set_print_fct_for_tools(int (*new_print_msg)(const char *, ...))
{
	_print_msg = new_print_msg;
	set_print_fct_for_lib(_print_msg);
}

/****************************************************************************/

time_t atom(register char *p)
{
  register char     *p1 = p;
  auto 	   struct tm tm;


#ifdef DEBUG_1
  if (strlen(p) < 20) {
    _print_msg(PRT_DEBUG_GENERAL, " DEBUG> Huch? atom(``%s'')\n", p);
    return((time_t)0);
  } /* if */
#endif

  tm.tm_mon = 0;

  while ((tm.tm_mon < 12) && memcmp(p1, Months[tm.tm_mon], 3)) tm.tm_mon++;

  if (tm.tm_mon == 12)
    return((time_t)-1);

  p1 += 4;
  p = p1 + 2;

  *p = 0;

  day = tm.tm_mday = atoi(p1);

  p1 += 3;
  p = p1 + 2;
  *p = 0;
  tm.tm_hour = atoi(p1);

  p1 = ++p;
  p += 2;
  *p = 0;
  tm.tm_min = atoi(p1);

  p1 = ++p;
  p += 2;
  *p = 0;
  tm.tm_sec = atoi(p1);

  p1 = ++p;
  p += 4;
  *p = 0;

  tm.tm_year = atoi(p1 + 2);

#ifdef DEBUG_1
  if (tm.tm_year < 1995) {
    _print_msg(PRT_DEBUG_GENERAL, " DEBUG> Huch? atom(): year=%d\n", tm.tm_year);
    return((time_t)0);
  } /* if */
#endif

  tm.tm_wday = tm.tm_yday;
  tm.tm_isdst = -1;

  return(mktime(&tm));
} /* atom */

/****************************************************************************/

char *num2nam(char *num, int si)
{
  register int i, n;


  if (*num) {
    for (n = 0; n < 2; n++) {
      for (i = 0; i < knowns; i++) {
        if (((known[i]->si == si) || n) && (!num_match(known[i]->num, num))) {
          if (++retnum == MAXRET)
            retnum = 0;

          cnf = i;
          return(strcpy(retstr[retnum], known[i]->who));
        } /* if */
      } /* for */
    } /* for */
  } /* if */

  cnf = -1;
  return("");
} /* num2nam */

/****************************************************************************/

#if defined __GLIBC__ && __GLIBC__ >= 2
char *double2str(double n, int l, int d, int flags)
{
  if (++retnum == MAXRET)
    retnum = 0;

  sprintf(retstr[retnum], "%*.*f", l, d, n);
  return(retstr[retnum]);
} /* double2str */
#else
char *double2str(double n, int l, int d, int flags)
{
  register char *p, *ps, *pd, *px;
  auto     int   decpt, sign, dec, dp;
  auto     char  buf[BUFSIZ];
  static   char  proto[] = "                   0,000000000";


  if (++retnum == MAXRET)
    retnum = 0;

  p = retstr[retnum] + l + 1;
  *p = 0;

  dec = d ? d : -1;
  dp = l - dec;

  *buf = '0';
  memcpy(buf + 1, ecvt(n, DIGITS, &decpt, &sign), DIGITS);

  ps = buf;
  px = ps + decpt + d;

  if (px >= buf) {
    int rfound = 0;
    pd = px + 1;

    if (*pd > '4') {
      pd++;
      rfound++;
    } /* if */

    if (rfound) {
      while (pd > px)
	if (*pd >= '5') {
	  pd--;
	  while (*pd == '9')
	    *pd-- = '0';
	  *pd += 1;
	}
	else
	  pd--;
    } /* if */

    if (*buf == '1')
      decpt++;
    else
      ps++;

    if ((dp < 2 + sign) || ((decpt ? decpt : 1) + sign) >= dp) {
      memset(retstr[retnum] + 1, '*', *retstr[retnum] = l);
      return(retstr[retnum] + 1);
    } /* if */

  } /* if */

  memcpy(retstr[retnum] + 1, proto + 21 - l + dec, *retstr[retnum] = l);

  if (!((decpt < 0) && ((dec + decpt) <= 0))) {
    pd = retstr[retnum] + dp - decpt;

    if (sign) {
      if (decpt > 0)
	*(pd - 1) = '-';
      else
	*(retstr[retnum] + dp - 2) = '-';
    } /* if */

    while (decpt-- > 0)
      *pd++ = *ps++;

    pd++; /* skip comma */

    while (d-- > 0)
      *pd++ = *ps++;
  } /* if */

  retstr[retnum][l + 1] = 0;

  if (flags & DEB) {
    p = retstr[retnum] + 1;

    while (*p == ' ')
      p++;

    return(p);
  } /* if */

  return(retstr[retnum] + 1);

} /* double2str */
#endif

/****************************************************************************/

char *double2byte(double bytes)
{
  static   char   mode[4] = " KMG";
  register int    m = 0;


  if (++retnum == MAXRET)
    retnum = 0;

  while (bytes > 999.9) {
    bytes /= 1024.0;
    m++;
  } /* while */

  sprintf(retstr[retnum], "%s%cb", double2str(bytes, 5, 1, 0), mode[m]);

  return(retstr[retnum]);
} /* double2byte */

/****************************************************************************/

char *time2str(time_t sec)
{
  static   char   mode[3] = "smh";
  register int    m = 0;
  auto     double s = (double)sec;


  if (++retnum == MAXRET)
    retnum = 0;

  while (s > 59.9) {
    s /= 60.0;
    m++;
  } /* while */

  sprintf(retstr[retnum], "%s%c", double2str(s, 4, 1, 0), mode[m]);

  return(retstr[retnum]);
} /* time2str */

/****************************************************************************/

char *double2clock(double n)
{
  auto int x, h, m, s;


  if (++retnum == MAXRET)
    retnum = 0;


  if (n <= 0.0)
    sprintf(retstr[retnum], "        ");
  else {
#if 0
    x = floor(n);
#else
    x = (int)n;
#endif

    h = (int)(x / 60 / 60);
    x %= 60 * 60;
    m = (int)(x / 60);
    s = (int)(x % 60);

#if 0
    sprintf(retstr[retnum], "%2d:%02d:%02d.%02d", h, m, s,
                                                  (int)((n - x) * 100));
#else
    sprintf(retstr[retnum], "%2d:%02d:%02d", h, m, s);
#endif
  } /* else */

  return(retstr[retnum]);
} /* double2clock */

/****************************************************************************/

char *vnum(int chan, int who)
{
  register int    l = strlen(call[chan].num[who]), got = 0;
  register int    flag = C_NO_WARN | C_NO_EXPAND;
  auto     char  *ptr;
  auto	   int    ll, lx;
  auto	   int 	  prefix = strlen(countryprefix);
  auto	   int 	  cc_len = 2;   /* country code length defaults to 2 */


  if (++retnum == MAXRET)
    retnum = 0;

  *call[chan].vorwahl[who] =
  *call[chan].rufnummer[who] =
  *call[chan].alias[who] =
  *call[chan].area[who] = 0;
  call[chan].confentry[who] = -1;

  if (!l) {       /* keine Meldung von der Vst (Calling party number fehlt) */
    sprintf(retstr[retnum], "%c", C_UNKNOWN);
    return(retstr[retnum]);
  } /* if */

  strcpy(call[chan].alias[who], num2nam(call[chan].num[who], call[chan].si1));

  if (cnf > -1) {                    /* Alias gefunden! */
    call[chan].confentry[who] = cnf;
    strcpy(retstr[retnum], call[chan].alias[who]);
  } /* if */

#ifdef Q931
  if (q931dmp)
    flag |= C_NO_ERROR;
#endif

  if ((call[chan].sondernummer[who] != -1) || call[chan].intern[who]) {
    strcpy(call[chan].rufnummer[who], call[chan].num[who]);

    if (cnf > -1)
      strcpy(retstr[retnum], call[chan].alias[who]);
    else if (call[chan].sondernummer[who] != -1)
      strcpy(retstr[retnum], SN[call[chan].sondernummer[who]].sinfo);
    else
      sprintf(retstr[retnum], "TN %s", call[chan].num[who]);

    return(retstr[retnum]);
  }
  else {
  if ((ptr = get_areacode(call[chan].num[who], &ll, flag)) != 0) {
    strcpy(call[chan].area[who], ptr);
    l = ll;
    got++;
  } /* if */
  } /* else */

  if (l > 1) {
    if (call[chan].num[who][prefix] == '1')
      cc_len = 1; /* USA is only country with country code length 1 */
    /*
     * there should be code for country codes > 2 in length,
     * but that at least doesn't cause a possible strncpy(x, y, -1) call!
     */
    lx = cc_len + prefix;

    if (lx > 0)
      strncpy(call[chan].areacode[who], call[chan].num[who], lx);

    lx = l - cc_len - prefix;

    if (lx > 0)
      strncpy(call[chan].vorwahl[who], call[chan].num[who] + cc_len + prefix, lx);

    strcpy(call[chan].rufnummer[who], call[chan].num[who] + l);
  } /* if */

  if (cnf > -1)
    strcpy(retstr[retnum], call[chan].alias[who]);
  else if (l > 1)
    sprintf(retstr[retnum], "%s %s/%s, %s",
      call[chan].areacode[who],
      call[chan].vorwahl[who],
      call[chan].rufnummer[who],
      call[chan].area[who]);
  else
    strcpy(retstr[retnum], call[chan].num[who]);

  return(retstr[retnum]);
} /* vnum */

/****************************************************************************/

char *i2a(int n, int l, int base)
{
  static   char  Digits[] = "0123456789abcdef";
  register char *p;
  register int	 dot = 0;


  if (++retnum == MAXRET)
    retnum = 0;

  p = retstr[retnum] + RETSIZE - 1;
  *p = 0;

  while (n || (l > 0)) {
    if (n) {
      *--p = Digits[n % base];
      n /= base;
    }
    else
      *--p = '0';

    l--;

    dot++;

    if (!(dot % 8))
      *--p = ' ';
    else if (!(dot % 4))
      *--p = '.';
  } /* while */

  return(((*p == ' ') || (*p == '.')) ? p + 1 : p);
} /* i2a */

/****************************************************************************/

static char *itoa(register unsigned int num, register char *p, register int radix, int dots)
{
  register int   i, j = 0;
  register char *q = p + MAXDIG;


  do {
    i = (int)(num % radix);
    i += '0';

    if (i > '9')
      i += 'A' - '0' - 10;

    *--q = i;

    if (dots)
      if (!(++j % 3))
	*--q = '.';

  } while ((num = num / radix));

  if (*q == '.')
    q++;

  i = p + MAXDIG - q;

  do
    *p++ = *q++;
  while (--i);

  return(p);
} /* itoa */

/****************************************************************************/

/*
static char *ltoa(register unsigned long num, register char *p, register int radix, int dots)
{
  register int   i, j = 0;
  register char *q = p + MAXDIG;


  do {
    i = (int)(num % radix);
    i += '0';

    if (i > '9')
      i += 'A' - '0' - 10;

    *--q = i;

    if (dots)
      if (!(++j % 3))
        *--q = '.';

  } while ((num = num / radix));

  if (*q == '.')
    q++;

  i = p + MAXDIG - q;

  do
    *p++ = *q++;
  while (--i);

  return(p);
} 
*/

/****************************************************************************/

int iprintf(char *obuf, int chan, register char *fmt, ...)
{
  register char     *p, *s;
  register int       c, i, who;
  register short int width, ndigit;
  register int       ndfnd, ljust, zfill, lflag;
  register int	     unknown = !*call[chan].digits && !*call[chan].onum[OTHER];
  register char     *op = obuf;
  auto     char      buf[MAXDIG + 1]; /* +1 for sign */
  auto	   char	     sx[BUFSIZ];
  static   char      nul[] = "(null)";
  auto     va_list   ap;


  va_start(ap, fmt);

  for (;;) {
    c = *fmt++;

    if (!c) {
      va_end(ap);

      *op = 0;

      return((int)(op - obuf));
    } /* if */

    if (c != '%') {
      *op++ = c;
      continue;
    } /* if */

    p = s = buf;

    ljust = 0;

    if (*fmt == '-') {
      fmt++;
      ljust++;
    } /* if */

    zfill = ' ';

    if (*fmt == '0') {
      fmt++;
      zfill = '0';
    } /* if */

    for (width = 0;;) {
      c = *fmt++;

      if (isdigit(c))
	c -= '0';
      else if (c == '*')
	c = GETARG(int);
      else
	break;

      width *= 10;
      width += c;
    } /* for */

    ndfnd = ndigit = 0;

    if (c == '.') {
      for (;;) {
	c = *fmt++;

	if (isdigit(c))
	  c -= '0';
	else if (c == '*')
	  c = GETARG(int);
	else
	  break;

	ndigit *= 10;
	ndigit += c;
	ndfnd++;
      } /* for */
    } /* if */

    lflag = 0;

    if (tolower(c) == 'l') {
      lflag++;

      if (*fmt)
	c = *fmt++;
    } /* if */

    who = OTHER;

    switch (c) {
      case 's' : zfill = ' ';

	         if ((s = GETARG(char *)) == NULL)
	           s = nul;

	         if (!ndigit)
	           ndigit = 32767;

	         for (p = s; *p && --ndigit >= 0; p++);

	         break;

      case 'k' : p = itoa(call[chan].card, p, 10, 0);
      	       	 break;

      case 't' : p = itoa(call[chan].tei, p, 10, 0);
      	       	 break;

      case 'C' : p = itoa(call[chan].cref, p, 10, 0);
      	       	 break;

      case 'B' : p = itoa(chan, p, 10, 0);
      	       	 break;

      case 'A' : s = sx;
      	         if (*call[chan].onum[CLIP])
      	       	   sprintf(sx, " alias %s", call[chan].vnum[CLIP]);
      		 else
                   *sx = 0;
                 p = s + strlen(s);
                 break;

      case 'z' : p = itoa(area_diff(NULL, call[chan].num[OTHER]), p, 10, 0);
      	       	 break;

      case 'Z' : s = sx;
      	         if (*call[chan].num[OTHER])
      	       	   sprintf(sx, " %s", area_diff_string(NULL, call[chan].num[OTHER]));
      	         else
                   *sx = 0;
                 p = s + strlen(s);
                 break;

      case 'n' : who = ME;    goto go;
      case 'c' : who = CLIP;  goto go;
      case 'N' :
go:   	         if (!ndigit)
	           ndigit = 32767;

      		 if (*fmt) {
                   switch (*fmt++) {
                     case '0' : s = call[chan].onum[who];      break;
                     case '1' : s = call[chan].num[who];       break;
                     case '2' : s = call[chan].vnum[who];      break;
                     case '3' : s = call[chan].vorwahl[who];   break;
                     case '4' : s = call[chan].rufnummer[who]; break;
                     case '5' : s = call[chan].alias[who];     break;
                     case '6' : s = call[chan].area[who];      break;
                     case '7' : s = call[chan].areacode[who];  break;
                      default : s = nul; 		       break;
                   } /* switch */

                   p = s + strlen(s);
      		 } /* if */
                 break;

      case 'I' : switch (chan) {
     	       	   case 0 : s = "";   p = s;     break;
     		   case 1 : s = "  "; p = s + 2; break;
    		  default : s = "* "; p = s + 2; break;
     		 } /* switch */
		 break;

      case 'a' : s = idate; p = s + 3;
      	       	 break;

      case 'b' : s = idate + 3; p = s + 3;
      	       	 break;

      case 'e' : s = idate + 6; p = s + 2;
                 break;

      case 'T' : s = idate + 8; p = s + 8;
                 break;

      case ' ' :
      case '(' :
      case ')' :
      case '/' : sprintf(sx, "%c", unknown ? 0 : c);
      		 s = sx;
      	       	 p = s + strlen(s);
		 break;

      case 'p' : s = sx;
      	         if (call[chan].provider != -1) {

      		   if (call[chan].provider < 100)
      	       	   sprintf(sx, "010%02d", call[chan].provider);
      		   else
		     sprintf(sx, "010%03d", call[chan].provider - 100);
      	         }
      		 else
                   *sx = 0;
                 p = s + strlen(s);
                 break;

      case 'P' : s = sx;
      	         if (call[chan].provider != -1)
      	       	   sprintf(sx, " via %s", Providername(call[chan].provider));
      		 else
                   *sx = 0;
                 p = s + strlen(s);
                 break;

      default  : *p++ = c;
	         break;
    } /* switch */

    i = p - s;

    if ((width -= i) < 0)
      width = 0;

    if (!ljust)
      width = -width;

    if (width < 0) {
      if ((*s == '-') && (zfill == '0')) {
	*op++ = *s++;
	i--;
      } /* if */

      do
	*op++ = zfill;
      while (++width);
    } /* if */

    while (--i >= 0)
      *op++ = *s++;

    while (width) {
      *op++ = zfill;
      width--;
    } /* while */
  } /* for */

} /* iprintf */

/****************************************************************************/

int print_version(char *myname)
{
	_print_msg("%s Version %s, Copyright (C) 1995, 1996, 1997, 1998, 1999\n",myname,VERSION);
	/*
	_print_msg("                                   Andreas Kool (akool@isdn4linux.de)\n");
	_print_msg("                               and Stefan Luethje (luethje@sl-gw.lake.de)\n\n");
	*/
	_print_msg("                                   Andreas Kool and Stefan Luethje\n");
	_print_msg("                                   (i4l-isdnlog@franken.de)\n\n");
	_print_msg("%s comes with ABSOLUTELY NO WARRANTY; for details see COPYING.\n", myname);
	_print_msg("This is free software, and you are welcome to redistribute it\n");
	_print_msg("under certain conditions; see COPYING for details.\n");
	return 0;
}

/****************************************************************************/

