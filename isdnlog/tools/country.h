/* $Id: country.h,v 1.1 1999/05/27 18:19:58 akool Exp $
 *
 * L�nderdatenbank
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
 * $Log: country.h,v $
 * Revision 1.1  1999/05/27 18:19:58  akool
 * first release of the new country decoding module
 *
 *
 */

#ifndef _COUNTRY_H_
#define _COUNTRY_H_

typedef struct {
  char  *Name;
  char **Alias;
  int   nAlias;
  char **Code;
  int   nCode;
} COUNTRY;

int  initCountry(char *path, char **msg);
void exitCountry(void);

#endif
