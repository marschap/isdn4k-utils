/* $Id: holiday.h,v 1.4 1999/04/14 13:17:21 akool Exp $
 *
 * Feiertagsberechnung
 *
 * Copyright 1999 by Michael Reinelt (reinelt@eunet.at)
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
 * $Log: holiday.h,v $
 * Revision 1.4  1999/04/14 13:17:21  akool
 * isdnlog Version 3.14
 *
 * - "make install" now install's "rate-xx.dat", "rate.conf" and "ausland.dat"
 * - "holiday-xx.dat" Version 1.1
 * - many rate fixes (Thanks again to Michael Reinelt <reinelt@eunet.at>)
 *
 * Revision 1.3  1999/04/10 16:36:35  akool
 * isdnlog Version 3.13
 *
 * WARNING: This is pre-ALPHA-dont-ever-use-Code!
 * 	 "tarif.dat" (aka "rate-xx.dat"): the next generation!
 *
 * You have to do the following to test this version:
 *   cp /usr/src/isdn4k-utils/isdnlog/holiday-de.dat /etc/isdn
 *   cp /usr/src/isdn4k-utils/isdnlog/rate-de.dat /usr/lib/isdn
 *   cp /usr/src/isdn4k-utils/isdnlog/samples/rate.conf.de /etc/isdn/rate.conf
 *
 * After that, add the following entries to your "/etc/isdn/isdn.conf" or
 * "/etc/isdn/callerid.conf" file:
 *
 * [ISDNLOG]
 * SPECIALNUMBERS = /usr/lib/isdn/sonderrufnummern.dat
 * HOLIDAYS       = /usr/lib/isdn/holiday-de.dat
 * RATEFILE       = /usr/lib/isdn/rate-de.dat
 * RATECONF       = /etc/isdn/rate.conf
 *
 * Please replace any "de" with your country code ("at", "ch", "nl")
 *
 * Good luck (Andreas Kool and Michael Reinelt)
 *
 * Revision 1.2  1999/03/16 17:38:07  akool
 * - isdnlog Version 3.07
 * - Michael Reinelt's patch as of 16Mar99 06:58:58
 * - fix a fix from yesterday with sondernummern
 * - ignore "" COLP/CLIP messages
 * - dont show a LCR-Hint, if same price
 *
 * Revision 1.1  1999/03/14 12:16:41  akool
 * - isdnlog Version 3.04
 * - general cleanup
 * - new layout for "rate-xx.dat" and "holiday-xx.dat" files from
 *     Michael Reinelt <reinelt@eunet.at>
 *     unused by now - it's a work-in-progress !
 * - bugfix for Wolfgang Siefert <siefert@wiwi.uni-frankfurt.de>
 *     The Agfeo AS 40 (Software release 2.1b) uses AOC_AMOUNT, not AOC_UNITS
 * - bugfix for Ralf G. R. Bergs <rabe@RWTH-Aachen.DE> - 0800/xxx numbers
 *     are free of charge ;-)
 * - tarif.dat V 1.08 - new mobil-rates DTAG
 *
 */

#ifndef _HOLIDAY_H_
#define _HOLIDAY_H_

#define MONDAY    1
#define THUESDAY  2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6
#define SUNDAY    7
#define WORKDAY   8
#define WEEKEND   9
#define HOLIDAY  10
#define EVERYDAY 11

typedef unsigned long bitfield;

int  initHoliday(char *path, char **msg);
void exitHoliday(void);
int  isDay(struct tm *tm, bitfield mask, char **name);

#endif
