/* $Id: rate.h,v 1.1 1999/03/14 12:16:43 akool Exp $
 *
 * Tarifdatenbank
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
 * $Log: rate.h,v $
 * Revision 1.1  1999/03/14 12:16:43  akool
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

#ifndef _RATE_H_
#define _RATE_H_

void  exitRate(void);
int   initRate(char *conf, char *dat, char *msg);
char *Providername(int prefix);
char *Zonename(int prefix, int zone);

#endif
