/* $Id: tools_de.c,v 1.4 1998/11/17 00:37:57 akool Exp $
 *
 * ISDN accounting for isdn4linux. (Utilities)
 *
 * Copyright 1995, 1998 by Andreas Kool (akool@Kool.f.UUnet.de)
 *
 * splitting into nations 1998 by Michael Reinelt (reinelt@eunet.at)
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
 * $Log: tools_de.c,v $
 * Revision 1.4  1998/11/17 00:37:57  akool
 *  - fix new Option "-i" (Internal-S0-Bus)
 *  - more Providers (Nikoma, First Telecom, Mox)
 *  - isdnrep-Bugfix from reinhard.karcher@dpk.berlin.fido.de (Reinhard Karcher)
 *  - Configure.help completed
 *
 * Revision 1.3  1998/11/05 19:10:27  akool
 *  - Support for all the new L2 frames from HiSax 3.0d (RR, UA, SABME and
 *    tei management)
 *  - CityWeekend reimplemented
 *    Many thanks to Rainer Gallersdoerfer <gallersd@informatik.rwth-aachen.de>
 *    for the tip
 *  - more providers
 *  - general clean-up
 *
 * Revision 1.2  1998/11/01 08:50:41  akool
 *  - fixed "configure.in" problem with NATION_*
 *  - DESTDIR fixes (many thanks to Michael Reinelt <reinelt@eunet.at>)
 *  - isdnrep: Outgoing calls ordered by Zone/Provider/MSN corrected
 *  - new Switch "-i" -> running on internal S0-Bus
 *  - more providers
 *  - "sonderrufnummern.dat" extended (Frag Fred, Telegate ...)
 *  - added AVM-B1 to the documentation
 *  - removed the word "Teles" from the whole documentation ;-)
 *
 * Revision 1.1  1998/09/26 18:30:24  akool
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
 *
 */

#include <isdnlog.h>

char *Providername(int number)
{
  switch (number) {
    case 01 : return("Alpha");
    case 11 : return("o.tel.o");
    case 13 : return("Tele 2");
    case 14 : return("EWE Tel");
    case 15 : return("RSL COM");
    case 18 : return("Debitel");
    case 19 : return("MobilCom");
    case 20 : return("ISIS");
    case 22 : return("NetCologne");
    case 23 : return("tesion ))");
    case 24 : return("TelePassport");
    case 25 : return("Citykom Muenster");
    case 28 : return("COLT/Telebridge/Global Comm./Nikoma");
    case 30 : return("TelDaFax");
    case 33 : return("Telekom");
    case 36 : return("Hutchison Telekom");
    case 39 : return("First Telecom");
    case 40 : return("Plusnet");
    case 41 : return("HanseNet");
    case 43 : return("KielNet");
    case 44 : return("VEW Telnet");
    case 46 : return("KomTel");
    case 49 : return("ACC Telecom");
    case 50 : return("Talkline");
    case 55 : return("Esprit Telecom");
    case 57 : return("Mox");
    case 63 : return("KDD-Conos");
    case 66 : return("Interoute");
    case 70 : return("Arcor");
    case 72 : return("d2privat");
    case 79 : return("Viatel");
    case 85 : return("WESTCom");
    case 87 : return("HamCom");
    case 88 : return("WorldCom");
    case 90 : return("VIAG Interkom");
    case 98 : return("STAR Telecom/C@llas/Mox");
    case 99 : return("ECONOphone/Telco");
    default : return("UNKNOWN Provider");
  } /* switch */
} /* Providername */

/****************************************************************************/

char *t2tz(int zeit)
{
  switch (zeit) {
    case  0 : return("Vormittag");    break;
    case  1 : return("Nachmittag");   break;
    case  2 : return("Freizeit");     break;
    case  3 : return("Mondschein");   break;
    case  4 : return("Nacht");        break;
    case  5 : return("Standard");     break;
    case  6 : return("Spartarif");    break;
    case  7 : return("City Weekend"); break;
    case  8 : return("City Plus");    break;
    case  9 : return("Feiertag");     break;
    default : return("");             break;
  } /* switch */
} /* t2tz */

/****************************************************************************/

char *z2s(int zone)
{
  switch (zone) {
    case  1 : return("CityCall");     break;
    case  2 : return("RegioCall");    break;
    case  3 : return("GermanCall");   break;
    case  4 : return("GermanCall");   break;
    case  5 : return("GlobalCall");   break;
    case  6 : return("Vis1");         break;
    case  7 : return("Vis2");         break;
    case  8 : return("Vis3");         break;
    case  9 : return("Welt1");        break;
    case 10 : return("Welt2");        break;
    case 11 : return("Welt3");        break;
    case 12 : return("T-Online");     break;
    case 13 : return("KONF");         break;
    case 14 : return("Inmar");        break;
    case 15 : return("C-Box");        break;
    case 16 : return("T-Box");        break;
    case 21 : return("City");         break; /* City Plus */
    default : return("");             break;
  } /* switch */
} /* z2s */

/****************************************************************************/

