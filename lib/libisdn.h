
/* $Id: libisdn.h,v 1.1 1997/03/03 04:23:16 fritz Exp $
 *
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1997 by Stefan Luethje (luethje@sl-gw.lake.de)
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

#ifndef _LIB_H_
#define _LIB_H_

#include <linux/limits.h>

#include "policy.h"
#include "conffile.h"

#ifdef LIBAREA
# include "areacode/areacode.h"
#else
# include "avon/createDB.h"
#endif

/****************************************************************************/

extern char *basename __P((__const char *__name));

/****************************************************************************/

#define CONFDIR_VAR "ISDN"
#define C_SLASH '/'
/* #define C_SLASH '\\' */

#ifndef TMPDIR
# define TMPDIR P_tmpdir
#endif

#ifndef I4LCONFDIR
# define I4LCONFDIR "/etc/isdn"
#endif

#ifndef RUNDIR
# define RUNDIR "/var/run"
#endif

#ifndef CONFFILE
# define CONFFILE "isdn.conf"
#endif

#ifndef USERCONFFILE
#	define USERCONFFILE "~/.isdn"
#endif

#ifndef S_COUNTRY_PREFIX
# define S_COUNTRY_PREFIX    "+" /* "00" */
#endif

#ifndef S_AREA_PREFIX
# define S_AREA_PREFIX  "0"
#endif

#ifndef AVON
# define AVON  "avon"
#endif

/****************************************************************************/

#define CONF_SEC_GLOBAL  "GLOBAL"
#define CONF_ENT_COUNTRY "COUNTRYCODE"
#define CONF_ENT_AREA    "AREACODE"
#define CONF_ENT_AREALIB "AREALIB"
#define CONF_ENT_AVONLIB "AVON"

/****************************************************************************/

#define C_NUM_DELIM  ','

/****************************************************************************/

#define SHORT_STRING_SIZE      256
#define LONG_STRING_SIZE      1024
#define BUF_SIZE              4096

/****************************************************************************/

#define F_IGNORE_CASE		1024

/****************************************************************************/

#define C_NO_ERROR   1
#define C_NO_WARN    2
#define C_NO_EXPAND  4

/****************************************************************************/

#ifdef _ISDNTOOLS_C_
#define _EXTERN
#define SET_NULL = ""
#else
#define _EXTERN extern
#define SET_NULL
#endif

_EXTERN char    *mycountry SET_NULL;
_EXTERN char    *myarea    SET_NULL;

_EXTERN void set_print_fkt_for_lib(int (*new_print_msg)(const char *, ...));
_EXTERN int num_match(char *Pattern, char *number);
_EXTERN char *expand_number(char *s);
_EXTERN char *expand_file(char *s);
_EXTERN char *confdir(void);
_EXTERN int create_runfile(const char* progname);
_EXTERN int delete_runfile(const char* progname);
_EXTERN int Set_Codes(section* Section);
_EXTERN char *get_areacode(char *code, int *Len, int flag);

#undef SET_NULL
#undef _EXTERN

/****************************************************************************/

#endif
