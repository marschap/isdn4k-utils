/* $Id: tools_at.c,v 1.2 1998/11/24 20:53:13 akool Exp $
 *
 * ISDN accounting for isdn4linux. (Utilities)
 *
 * Copyright 1995, 1998 by Andreas Kool (akool@isdn4linux.de)
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
 * $Log: tools_at.c,v $
 * Revision 1.2  1998/11/24 20:53:13  akool
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
 * Revision 1.1  1998/09/26 18:30:21  akool
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
    default : return("PTA");
  } /* switch */
} /* Providername */

/****************************************************************************/

char *t2tz(int zeit)
{
  switch (zeit) {
    case  0 : return("Tageszeit 1");    break;
    case  1 : return("Tageszeit 2");    break;
    case  2 : return("Sparzeit");       break;
    case  3 : return("Supersparzeit");  break;
    default : return("");
  } /* switch */
} /* t2tz */

/****************************************************************************/

char *z2s(int zone)
{
  switch (zone) {
  case  1 : return("Regionalzone"); break;
  case  2 : return("Fernzone 1"); break;
  case  3 : return("Fernzone 2"); break;
  case  4 : return("Online"); break;
  case  5 : return("Mobilfunk"); break;
  case  6 : return("Ausland Zone 1"); break;
  case  7 : return("Ausland Zone 2"); break;
  case  8 : return("Ausland Zone 3"); break;
  case  9 : return("Ausland Zone 4"); break;
  case 10 : return("Ausland Zone 5"); break;
  case 11 : return("Ausland Zone 6"); break;
  case 12 : return("Ausland Zone 7"); break;
  case 13 : return("Ausland Zone 8"); break;
  case 14 : return("Ausland Zone 9"); break;
  case 15 : return("Ausland Zone 10"); break;
  case 16 : return("Ausland Zone 11"); break;
  case 17 : return("Ausland Zone 12"); break;
  case 18 : return("Ausland Zone 13"); break;
  case 19 : return("Ausland Zone 14"); break;
  case 20 : return("Ausland Zone 15"); break;
  case 21 : return("Handvermittelter Verkehr"); break;
  case 22 : return("Grenznahverkehr");break;
  case 23 : return("Teleinfo 04570"); break;
  case 24 : return("Teleinfo 04500"); break;
  case 25 : return("Telebusiness 04590"); break;
  case 26 : return("Teleinfo 04580"); break;
  case 27 : return("Businessline 0711x"); break;
  case 28 : return("Businessline 0713x"); break;
  case 29 : return("Businessline 0714x"); break;
  case 30 : return("Votingline 0717x"); break;
  default : return("");
  } /* switch */
} /* z2s */

/****************************************************************************/

