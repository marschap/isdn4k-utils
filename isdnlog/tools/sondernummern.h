/* $Id: sondernummern.h,v 1.1 1999/03/24 19:39:05 akool Exp $
 *
 * Gebuehrenberechnung fuer Sonderrufnummern
 *
 * Copyright 1999 by Mario Jou/3en (mario.joussen@post.rwth-aachen.de)
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
 * $Log: sondernummern.h,v $
 * Revision 1.1  1999/03/24 19:39:05  akool
 * - isdnlog Version 3.10
 * - moved "sondernnummern.c" from isdnlog/ to tools/
 * - "holiday.c" and "rate.c" integrated
 * - NetCologne rates from Oliver Flimm <flimm@ph-cip.uni-koeln.de>
 * - corrected UUnet and T-Online rates
 *
 */

#ifndef _SONDERNUMMERN_H_
#define _SONDERNUMMERN_H_

#define SO_FAIL      -3
#define SO_UNKNOWN   -2
#define SO_CITYCALL  -1
#define SO_FREE       0
#define SO_CALCULATE  1

typedef struct {
  int    provider; /* Provider */
  char  *number;   /* Telefonnummer */
  int    tarif;    /* 0 = free, -1 = CityCall, -2 = unknown, 1 = calculate */
  int    tday;     /* 0 = alle Tage, 1 = Wochentag, 2 = Wochenende */
  int    tbegin;   /* Tarifanfang */
  int    tend;     /* Tarifende */
  double grund;    /* Grundtarif */
  double takt;     /* Zeittakt */
  char  *info;     /* Kurzbeschreibung */
} SonderNummern;

extern SonderNummern *SN;
extern int nSN;
extern int interns0;

void   exitSondernummern(void);
int    initSondernummern(char *fn, char **msg);
int    is_sondernummer(char *number, int provider);
char  *sondernummername(int index);
char  *sondernum(int index);
int    sondertarif(int index);
double sonderpreis(time_t connect, int duration, int index);
double sondertaktlaenge(time_t connect, int index, int *next);

#endif
