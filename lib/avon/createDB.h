/* $Id: createDB.h,v 1.1 1997/03/03 04:25:46 fritz Exp $
 *
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
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
 */

#ifndef _CREATEDB_H_
#define _CREATEDB_H_

#ifdef linux
#	include <ndbm.h>
#else
#	include "/usr/ucbinclude/ndbm.h"
/*#	include <libgen.h>*/
#endif

#ifdef DBMALLOC
#include "dbmalloc.h"
#endif

#ifdef _CREATEDB_C_
#define _EXTERN
#define SET_NULL = NULL
#else
#define _EXTERN extern
#define SET_NULL
#endif

_EXTERN datum    key, data;
_EXTERN DBM     *dbm SET_NULL;

_EXTERN void set_print_fkt_for_avon(int (*new_print_msg)(const char *, ...));
_EXTERN int createDB(char *fn, int force);
_EXTERN int openDB(char *fn);
_EXTERN void closeDB();
_EXTERN void insertDB(char *num, char *area);
_EXTERN void readAVON(char *fn);

#undef SET_NULL
#undef _EXTERN

#endif
